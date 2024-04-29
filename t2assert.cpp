#include <iostream>
#include <fstream>
#include <string>
#include <source_location>
#include <thread>
#include <exception>
#include <array>
#include <mutex>
#include <chrono>
#include <ctime>

#include "assert.h"

// Maximum number of unique assert message that can be logged
constexpr int maxAssertMsgs{5};

std::array<std::string, maxAssertMsgs> assertMsgLog;
int numAssertMsgs{0};
std::mutex assertLogLock;
bool assertFirstpass{true};

bool MsgLogged(const std::string &msg)
{
    for (auto m : assertMsgLog)
    {
        if (m == msg)
        {
            return true;
        }
    }

    return false;
}

void _t2assert(const char *_Message, const std::source_location &loc)
{
    if (logAsserts)
    {
        const std::lock_guard<std::mutex> lock(assertLogLock);

        const std::string locStr = std::string(loc.file_name()) + ":|" + std::to_string(loc.line()) +
                                   "| " + loc.function_name();

        std::ofstream logFile;

        if (numAssertMsgs < maxAssertMsgs && !MsgLogged(locStr))
        {
            if (assertFirstpass)
            {
                if (std::filesystem::exists(assertLogFile))
                {
                    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

                    constexpr int maxDateTimeLen{20};
                    char dateTimeBuf[maxDateTimeLen] = {0};

                    std::strftime(dateTimeBuf, sizeof(dateTimeBuf), "_%y-%m-%d_%H-%M-%S", std::localtime(&now));

                    std::filesystem::path parent = assertLogFile.parent_path().string();
                    if (!parent.empty())
                    {
                        parent += "/";
                    }
                    std::filesystem::path dest = parent.string() + assertLogFile.stem().string() +
                                                 dateTimeBuf + assertLogFile.extension().string();

                    std::filesystem::rename(assertLogFile, dest);
                }
                else if (!std::filesystem::exists(assertLogFile.parent_path()) &&
                         !std::filesystem::create_directories(assertLogFile.parent_path()))
                {
                    throw std::runtime_error(locStr + ": " + "Can't create directory " +
                                             assertLogFile.parent_path().string());
                }

                logFile.open(assertLogFile);
                assertFirstpass = false;
            }
            else
            {
                logFile.open(assertLogFile, std::ios::app);
            }

            if (logFile)
            {
                logFile << locStr << ": " << _Message << std::endl;
                logFile.close();
            }
            else
            {
                throw std::runtime_error(locStr + ": " + "Can't open " + assertLogFile.string() + " for write");
            }

            assertMsgLog[numAssertMsgs] = locStr;
            numAssertMsgs++;
        }
        else if (numAssertMsgs == maxAssertMsgs)
        {
            logFile.open(assertLogFile, std::ios::app);

            std::string msg = "Max messages of " + std::to_string(maxAssertMsgs) + " exceeded, logging stopped";

            if (logFile)
            {
                logFile << msg << std::endl;
                logFile.close();
            }
            else
            {
                throw std::runtime_error(locStr + ": " + "Can't open " + assertLogFile.string() + " for append");
            }

            logAsserts = false;
        }
    }

    if (assertBehavior == AssertBehavior::None)
    {
        return;
    }

    if (assertBehavior == AssertBehavior::Throw)
    {
        throw std::runtime_error(_Message);
    }

    if (assertBehavior == AssertBehavior::Terminate)
    {
        std::terminate();
    }

    if (assertBehavior == AssertBehavior::Hang)
    {
        for (;;)
        {
            // Attach debugger here
            std::this_thread::yield();
        }
    }
}