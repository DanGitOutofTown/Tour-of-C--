#include <fstream>
#include <sstream>
#include <array>
#include <mutex>
#include <chrono>
#include <ctime>

#include "windows.h"

#include "LogError.h"

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
        bool firstPass{true};

        // Set default configuration
        // To be overridden only through ParseConfig()
        std::string srvrSktName;
        int srvrPort{0};
        int clientPort{0};

        int srvrSkt{0};

        enum class PopupBehavior
        {
            Local,
            Remote,
            Off,
        };

        PopupBehavior behavior = PopupBehavior::Local;

        std::string clientSktName;
        struct SrvrMsg
        {
            char caption[64];
            char clientSktName[64];
            char errBuf[256];
        } srvrMsg;

        void ParseConfig(std::filesystem::path iniFile)
        {
            // override defaults above
            ;
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

    void Init(std::filesystem::path file, std::string clientSktName, std::filesystem::path iniFile)
    {
        errFile = file;
        ParseConfig(iniFile);
        if (!srvrSktName.empty())
        {
            // open socket to server
            srvrSkt = 9999;
            // preset clientPort
            clientPort = 1111;
            strcpy(srvrMsg.clientSktName, clientSktName.c_str());
        }
    }

    void PopupError(std::string errMsg, std::string caption)
    {
        if (behavior == PopupBehavior::Local)
        {
            MessageBoxA(NULL, errMsg.c_str(), caption.c_str(), MB_ICONERROR);
        }
        else if (behavior == PopupBehavior::Remote)
        {
            strcpy(srvrMsg.caption, caption.c_str());
            strcpy(srvrMsg.errBuf, errMsg.c_str());
            // send errMsg to server app
            // wait for acknowledge from server app
        }
    }

    void LogError(std::string errMsg, std::string caption, const std::source_location loc)
    {
        if (errFile.empty())
            return;

        const std::lock_guard<std::mutex> lock(logLock);

        const std::string& locStr = std::string(loc.file_name()) + ":|" + std::to_string(loc.line()) +
                                    "| " + loc.function_name();

        std::ofstream ofs;

        if (numMsgs < maxMsgs && !MsgLogged(locStr))
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
                else if (!std::filesystem::exists(errFile.parent_path()) &&
                         !std::filesystem::create_directories(errFile.parent_path()))
                {
                    throw std::runtime_error(locStr + ": " + "Can't create directory " +
                                             errFile.parent_path().string());
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
                std::ostringstream oss;
                oss << locStr << ": " << caption << ": " << errMsg << std::endl;
                ofs << oss.str();
                ofs.close();
                PopupError(oss.str(), caption);
            }
            else
            {
                throw std::runtime_error(locStr + ": " + "Can't open " + errFile.string() + " for write");
            }

            msgLog[numMsgs] = locStr;
            numMsgs++;
        }
        else if (numMsgs == maxMsgs)
        {
            ofs.open(errFile, std::ios::app);

            std::string msg = "Max messages of " + std::to_string(maxMsgs) + " exceeded, logging stopped";

            if (ofs)
            {
                ofs << msg << std::endl;
                ofs.close();
            }
            else
            {
                throw std::runtime_error(locStr + ": " + "Can't open " + errFile.string() + " for append");
            }

            numMsgs++; // no more logging
        }
    }
}
