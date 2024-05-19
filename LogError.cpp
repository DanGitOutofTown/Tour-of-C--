#include <fstream>
#include <sstream>
#include <array>
#include <mutex>
#include <chrono>
#include <ctime>

#include "windows.h"

#include "LogError.h"
#include "ClientToSrvrMsgBuf.h"

namespace ErrorLogger
{
    namespace
    {
        std::filesystem::path errFile;

        // Maximum number of unique assert messages that can be logged
        constexpr int maxMsgs{10};

        std::array<std::string, maxMsgs> msgLog;
        int numMsgs{0};
        std::mutex logLock;
        std::mutex popupLock;
        bool firstPass{true};

        enum class PopupLocation
        {
            Local,
            Remote
        };

        ClientToSrvrMsgBuf msgBuf;
        SrvrResponse response;

        // Set default configuration
        // To be overridden only through ParseConfig()
        std::string srvrSktName;
        int srvrPort{0};
        int clientPort{0};
        bool enableLogging = true;
        bool enablePopups = true;
        PopupLocation location = PopupLocation::Local;

        int srvrSkt{0};

        std::string clientSktName;

        void ParseConfig(const std::filesystem::path& iniFile)
        {
            if (!iniFile.empty())
            {
                // parse and override defaults above
                ;
            }
       }
    }

    inline bool MsgLogged(std::string_view msg)
    {
        for (const auto &m : msgLog)
        {
            if (m == msg)
            {
                return true;
            }
        }

        return false;
    }

    void Init(const std::filesystem::path& errLogFile, const std::string& clientSktName,
              const std::filesystem::path& iniFile)
    {
        ParseConfig(iniFile);

        if (enableLogging && !errLogFile.empty())
        {
            errFile = errLogFile;
        }
        else
        {
            enableLogging = false;
        }

        if (!enablePopups)
            return;

        if (location == PopupLocation::Remote)
        {
            if (!srvrSktName.empty() && srvrPort > 0 && clientPort > 0)
            {
                // assign srvrSkt by opening socket to server
                // preset clientPort

                strncpy(msgBuf.clientSktName, clientSktName.c_str(), clientSktNameBufSz);
            }
            else
            {
                enablePopups = false;
                LogError("Communication with remote Popup Server not setup correctly, popups disabled.");
            }
        }
    }

    void PopupError(const std::string& errMsg, const std::string& caption)
    {
        if (!enablePopups)
            return;
            
        const std::lock_guard<std::mutex> lock(popupLock);

        if (location == PopupLocation::Local)
        {
            auto result = MessageBoxA(NULL, (errMsg + "\nHit Retry to disable popups").c_str(),
                                      caption.c_str(), MB_ICONERROR | MB_ABORTRETRYIGNORE);
            
            switch (result)
            {
                case IDABORT:
                    std::terminate();
                case IDRETRY:
                    enablePopups = false;
                    break;
                case IDIGNORE:
                    break;
                default:
                    break;
            }
        }
        else if (location == PopupLocation::Remote)
        {
            strncpy(msgBuf.caption, caption.c_str(), captionBufSz);
            strncpy(msgBuf.errMsg, errMsg.c_str(), errMsgBufSz);
            
            // send msgBuf to server app using UMP
            // wait for response from server app
            
            switch (response)
            {
                case SrvrResponse::Ignore:
                    break;
                case SrvrResponse::DisablePopups:
                    enablePopups = false;
                    break;
                case SrvrResponse::TerminateClient:
                    std::terminate();
                default:
                    break;
            }
        }
    }

    void LogError(const std::string& errMsg, const std::string& caption, const std::source_location& loc)
    {
        if (!(enableLogging || enablePopups))
            return;

        const std::string& locStr = std::string(loc.file_name()) + ":|" + std::to_string(loc.line()) +
                                    "| " + loc.function_name();

        std::ostringstream oss;
        oss << locStr << ": " << caption << ": " << errMsg << std::endl;

        if (MsgLogged(locStr))
            return;

        if (!enableLogging)
        {
            PopupError(oss.str(), caption);
            return;
        }

        const std::lock_guard<std::mutex> lock(logLock);

        std::ofstream ofs;

        if (numMsgs < maxMsgs)
        {
            if (firstPass)
            {
                if (std::filesystem::exists(errFile))
                {
                    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

                    constexpr int maxDateTimeLen{20};
                    char dateTimeBuf[maxDateTimeLen] = {0};

                    struct tm newtime{0};
                    localtime_s(&newtime, &now);

                    std::strftime(dateTimeBuf, sizeof(dateTimeBuf), "_%y-%m-%d_%H-%M-%S", &newtime);

                    std::filesystem::path parent = errFile.parent_path().string();
                    if (!parent.empty())
                    {
                        parent += "/";
                    }
                    std::filesystem::path dest = parent.string() + errFile.stem().string() +
                                                 dateTimeBuf + errFile.extension().string();

                    std::filesystem::rename(errFile, dest);
                }
                else if (enablePopups && !std::filesystem::exists(errFile.parent_path()) &&
                         !std::filesystem::create_directories(errFile.parent_path()))
                {
                    PopupError(locStr + ": " + "Can't create directory " +
                               errFile.parent_path().string() + ", logging stopped.", "Error");
                    enableLogging = false;
                }

                ofs.open(errFile);
                firstPass = false;
            }
            else
            {
                ofs.open(errFile, std::ios::app);
            }

            if (ofs)
            {
                ofs << oss.str();
                ofs.close();
                PopupError(oss.str(), caption);
            }
            else
            {
                PopupError(locStr + ": " + "Can't open " + errFile.string() +
                           " for write, logging stopped.", "Error");
                PopupError(oss.str(), caption);
                enableLogging = false;
            }

            msgLog[numMsgs] = locStr;
            numMsgs++;
        }
        else if (numMsgs == maxMsgs)
        {
            ofs.open(errFile, std::ios::app);

            std::string msg = "Max messages of " + std::to_string(maxMsgs) +
                              " exceeded, logging stopped";

            if (ofs)
            {
                ofs << msg << std::endl;
                ofs.close();
            }
            else
            {
                PopupError(locStr + ": " + "Can't open " + errFile.string() +
                           " for append, logging stopped.", "Error");
            }

            PopupError(oss.str(), caption);
            enableLogging = false;
        }
    }
}
