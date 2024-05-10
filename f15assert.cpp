#if defined(NDEBUG) && defined(F15ASSERT)

#include <iostream>
#include <string>
#include <thread>
#include <exception>

#include "windows.h"

#include "assert.h"
#include "LogError.h"

namespace F15Assert
{
    namespace
    {
        // To be contained in f15assert.ini
        bool logAsserts = true;
        bool enableBehavior = true;
        Behavior behavior = Behavior::Popup;
    }

    void ParseConfig(std::filesystem::path iniFile)
    {
        ; // parse f15assert.ini
    }

    void f15assert(const char *expression, const std::source_location& loc)
    {
        if (logAsserts)
        {
            ErrorLogger::LogError("Assert: " + std::string(expression), loc);
        }

        if (!enableBehavior || behavior == Behavior::Ignore)
        {
            return;
        }

        if (behavior == Behavior::Popup)
        {
            MessageBoxA(NULL, expression, "Assertion failed", MB_ICONERROR);
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
