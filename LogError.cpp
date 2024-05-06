#include <fstream>
#include <string>
#include <array>
#include <mutex>
#include <chrono>
#include <ctime>

#include "LogError.h"

namespace ErrorLogger
{

bool logErrors = true;
std::filesystem::path errFile = "error_logs/error.log";

// Maximum number of unique assert message that can be logged
constexpr int maxErrorMsgs{5};

std::array<std::string, maxErrorMsgs> errorMsgLog;
int numErrorMsgs{0};
std::mutex errorLogLock;
bool errorLogFirstpass{true};

inline bool MsgLogged(std::string_view msg)
{
    for (const auto& m : errorMsgLog)
    {
        if (m == msg)
        {
            return true;
        }
    }

    return false;
}

void LogError(std::string_view errMsg, const std::source_location loc)
{
    if (!logErrors)
        return;

    const std::lock_guard<std::mutex> lock(errorLogLock);

    const std::string& locStr = std::string(loc.file_name()) + ":|" + std::to_string(loc.line()) +
                                            "| " + loc.function_name();

    std::ofstream ofs;

    if (numErrorMsgs < maxErrorMsgs && !MsgLogged(locStr))
    {
        if (errorLogFirstpass)
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
            errorLogFirstpass = false;
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

        errorMsgLog[numErrorMsgs] = locStr;
        numErrorMsgs++;
    }
    else if (numErrorMsgs == maxErrorMsgs)
    {
        ofs.open(errFile, std::ios::app);

        std::string msg = "Max messages of " + std::to_string(maxErrorMsgs) + " exceeded, logging stopped";

        if (ofs)
        {
            ofs << msg << std::endl;
            ofs.close();
        }
        else
        {
            throw std::runtime_error(locStr + ": " + "Can't open " + errFile.string() + " for append");
        }

        logErrors = false;
    }
}

}
