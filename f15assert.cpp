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

// To be controlled via f15assert.ini w/ installer selection
bool enableBehavior = false; // default

// May also be controlled via f15assert.ini w/ installer selection
Behavior behavior = Behavior::Popup; // default

bool logAsserts = true; // default
bool firstPass = true;

void ParseConfig()
{
    // parse f15assert.ini
}

void f15assert(const char *expression, const std::source_location& loc)
{
    if (firstPass)
    {
        ParseConfig();
        firstPass = false;
    }

    if (logAsserts)
    {
        ErrorLogger::LogError("Assert: " + std::string(expression), loc);
    }

    if (!enableBehavior || behavior == Behavior::None)
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
