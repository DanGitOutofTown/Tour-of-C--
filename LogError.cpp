#include <fstream>
#include <array>
#include <mutex>
#include <chrono>
#include <ctime>

#include "LogError.h"

bool logErrors = true;
std::filesystem::path errorLogFile = "error_logs/error.log";

// Maximum number of unique assert message that can be logged
constexpr int maxErrorMsgs{5};

std::array<std::string, maxErrorMsgs> errorMsgLog;
int numErrorMsgs{0};
std::mutex errorLogLock;
bool errorLogFirstpass{true};

bool MsgLogged(const std::string &msg)
{
    for (auto m : errorMsgLog)
    {
        if (m == msg)
        {
            return true;
        }
    }

    return false;
}

void LogError(const std::string& errMsg, const std::source_location& loc)
{
    const std::string locStr = std::string(loc.file_name()) + ":|" + std::to_string(loc.line()) +
                               "| " + loc.function_name();

    const std::lock_guard<std::mutex> lock(errorLogLock);

    std::ofstream logFile;

    if (numErrorMsgs < maxErrorMsgs && !MsgLogged(locStr))
    {
        if (errorLogFirstpass)
        {
            if (std::filesystem::exists(errorLogFile))
            {
                std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

                constexpr int maxDateTimeLen{20};
                char dateTimeBuf[maxDateTimeLen] = {0};

                std::strftime(dateTimeBuf, sizeof(dateTimeBuf), "_%y-%m-%d_%H-%M-%S", std::localtime(&now));

                std::filesystem::path parent = errorLogFile.parent_path().string();
                if (!parent.empty())
                {
                    parent += "/";
                }
                std::filesystem::path dest = parent.string() + errorLogFile.stem().string() +
                                             dateTimeBuf + errorLogFile.extension().string();

                std::filesystem::rename(errorLogFile, dest);
            }
            else if (!std::filesystem::exists(errorLogFile.parent_path()) &&
                     !std::filesystem::create_directories(errorLogFile.parent_path()))
            {
                throw std::runtime_error(locStr + ": " + "Can't create directory " +
                                         errorLogFile.parent_path().string());
            }

            logFile.open(errorLogFile);
            errorLogFirstpass = false;
        }
        else
        {
            logFile.open(errorLogFile, std::ios::app);
        }

        if (logFile)
        {
            logFile << locStr << ": " << errMsg << std::endl;
            logFile.close();
        }
        else
        {
            throw std::runtime_error(locStr + ": " + "Can't open " + errorLogFile.string() + " for write");
        }

        errorMsgLog[numErrorMsgs] = locStr;
        numErrorMsgs++;
    }
    else if (numErrorMsgs == maxErrorMsgs)
    {
        logFile.open(errorLogFile, std::ios::app);

        std::string msg = "Max messages of " + std::to_string(maxErrorMsgs) + " exceeded, logging stopped";

        if (logFile)
        {
            logFile << msg << std::endl;
            logFile.close();
        }
        else
        {
            throw std::runtime_error(locStr + ": " + "Can't open " + errorLogFile.string() + " for append");
        }

        logErrors = false;
    }
}
