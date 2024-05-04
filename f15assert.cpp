#include <iostream>
#include <string>
#include <thread>
#include <exception>

#include "windows.h"

#include "assert.h"
#include "LogError.h"

namespace F15Assert
{
Behavior behavior = Behavior::None;
bool logAsserts = true;

void f15assert(const char *_Message, const std::source_location& loc)
{
    if (logAsserts)
    {
        LogError("Assert: " + std::string(_Message), loc);
    }

    if (behavior == Behavior::None)
    {
        return;
    }

    if (behavior == Behavior::Popup)
    {
        MessageBoxA(NULL, _Message, "Assertion failed", MB_ICONERROR);
        return;
    }

    if (behavior == Behavior::Throw)
    {
        throw std::runtime_error(_Message);
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
