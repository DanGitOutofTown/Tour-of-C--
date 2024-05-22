#if defined(NDEBUG) && defined(RELEASE_ASSERT)

#include <iostream>
#include <string>
#include <thread>
#include <exception>

#include "windows.h"

#include "assert.h"
#include "LogError.h"

namespace ReleaseAssert
{
    namespace
    {
        // Set default configuration
        // To be overridden only through ParseConfig()
        bool logAsserts = true;
        Behavior behavior = Behavior::Popup;
    }

    void ParseConfig(const std::filesystem::path& iniFile)
    {
        ;  // parse and override defaults above
    }

    void release_assert(const char *expression, const std::source_location& loc)
    {
        if (logAsserts)
        {
            // Will only log if ErrorLogger::Init() has been called in main executable
            // To be replaced with TrainerLog::Msg:Error which in turn will call LogError()
            // and also output to LogFileViewer file.
            ErrorLogger::LogError(std::string(expression), "Assertion Failed", loc);
        }

        if (behavior == Behavior::Ignore)
        {
            return;
        }

        if (behavior == Behavior::Popup && !logAsserts)
        {
            const std::string& locStr = std::string(loc.file_name()) + ":|" + std::to_string(loc.line()) +
                                        "| " + loc.function_name();

            std::ostringstream oss;
            oss << locStr << ": " << std::string(expression) << std::endl;

            ErrorLogger::PopupError(oss.str(), "Assertion Failed");
            
            return;
        }

        if (behavior == Behavior::Throw)
        {
            throw std::runtime_error(expression);
        }

        if (behavior == Behavior::Terminate)
        {
            std::terminate();
        }

        if (behavior == Behavior::Hang)
        {
            for (;;)
            {
                // Attach debugger here
                std::this_thread::yield();
            }
        }
    }
}

#endif
