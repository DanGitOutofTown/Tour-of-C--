#include <fstream>
#include <string>
#include <array>
#include <mutex>
#include <chrono>
#include <ctime>

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
        bool firstpass{true};
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

    void Init(std::filesystem::path file)
    {
        errFile = file;
    }

    void LogError(std::string_view errMsg, const std::source_location loc)
    {
        if (errFile.empty())
            return;

        const std::lock_guard<std::mutex> lock(logLock);

        const std::string &locStr = std::string(loc.file_name()) + ":|" + std::to_string(loc.line()) +
                                    "| " + loc.function_name();

        std::ofstream ofs;

        if (numMsgs < maxMsgs && !MsgLogged(locStr))
        {
            if (firstpass)
            {
                if (std::filesystem::exists(errFile))
                {
                    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

                    constexpr int maxDateTimeLen{20};
                    char dateTimeBuf[maxDateTimeLen] = {0};

                    std::strftime(dateTimeBuf, sizeof(dateTimeBuf), "_%y-%m-%d_%H-%M-%S", std::localtime(&now));

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
                firstpass = false;
            }
            else
            {
                ofs.open(errFile, std::ios::app);
            }

            if (ofs)
            {
                ofs << locStr << ": " << errMsg << std::endl;
                ofs.close();
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
