#include <fstream>
#include <sstream>
#include <vector>
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
        //***** Begin default configuration *****//
        // To be overridden only through ParseConfig()
 
        bool enableLogging = false;
        bool enablePopups = false;
        bool enableUniqueMsgs = true;

        enum class PopupLocation
        {
            Local,
            Remote
        };
        PopupLocation location = PopupLocation::Local;

        int maxErrMsgs{10};
        int maxPopups{5}; // must be <= maxErrMsgs
        int srvrPort{0};
        int clientPort{0};

        std::string srvrSktName;
    
        //***** End default configuration *****//

        // passed into Init()
        std::filesystem::path errFile;
        std::string clientSktName;

        std::vector<std::string> ErrMsgs;
        int numErrMsgs{0};
        int numPopups{0};

        std::mutex logLock;
        std::mutex popupLock;

        ClientToSrvrMsgBuf srvrMsgBuf;
        SrvrResponse srvrResp;
        int srvrSkt{0};

        bool logErrorFirstPass{true};

        void ParseConfig(const std::filesystem::path& iniFile)
        {
            if (!iniFile.empty())
            {
                ; // parse and override defaults
            }
       }
    }

    inline bool MsgLogged(std::string_view msg)
    {
        if (!enableUniqueMsgs)
            return false;

        for (const auto &m : ErrMsgs)
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
        if (!errLogFile.empty())
        {
            // Have a log file so enable logging and popups
            // in case we don't have an iniFile
            enableLogging = true;
            enablePopups = true;
            errFile = errLogFile;
        }

        ParseConfig(iniFile);

        if (!enablePopups)
            return;

        if (location == PopupLocation::Remote)
        {
            if (!srvrSktName.empty() && srvrPort > 0 && clientPort > 0)
            {
                // assign srvrSkt by opening socket to server
                // preset clientPort

                strncpy(srvrMsgBuf.clientSktName, clientSktName.c_str(), clientSktNameBufSz);
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
        if (!enablePopups || numPopups >= maxPopups)
            return;
            
        const std::lock_guard<std::mutex> lock(popupLock);

        std::string instructions =
            "\nError is now logged, but if possible please leave this"
            "\nmessage up and contact a software engineer to debug the issue."
            "\n\nHit Abort to kill client process to allow for trainer reset."
            "\n\nHit Ignore to continue. Training may be compromised."
            "\n\nHit Retry to disable popups and continue. Training may be compromised.";

        if (enableLogging)
        {
            instructions += "\n\nError is logged to " + std::filesystem::absolute(errFile).string() +
                " and will not be overwritten when trainer is reset or new errors occur. "
                "Please report error at earliest convenience. Thank you.";
        }

        if (location == PopupLocation::Local)
        {
            auto result = MessageBoxA(NULL, (errMsg + instructions).c_str(),
                                      caption.c_str(), MB_ICONERROR | MB_ABORTRETRYIGNORE | MB_DEFBUTTON3);
            
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
            
            numPopups++;
        }
        else if (location == PopupLocation::Remote)
        {
            strncpy(srvrMsgBuf.caption, caption.c_str(), captionBufSz);
            strncpy(srvrMsgBuf.errMsg, errMsg.c_str(), errMsgBufSz);
            strncpy(srvrMsgBuf.instructions, instructions.c_str(), instructionsBufSz);

            // send msgBuf to server app using UMP
            // wait for response from server app
            
            switch (srvrResp)
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
            
            numPopups++;
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

        if (numErrMsgs < maxErrMsgs)
        {
            if (logErrorFirstPass)
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
                logErrorFirstPass = false;
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

            ErrMsgs.push_back(locStr);
            numErrMsgs++;
        }
        else if (numErrMsgs == maxErrMsgs)
        {
            ofs.open(errFile, std::ios::app);

            std::string msg = "Max messages of " + std::to_string(maxErrMsgs) +
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
            enablePopups = false;
        }
    }
}
