#include <iostream>
#include <string>
#include <thread>
#include <exception>

#include "windows.h"

#include "assert.h"
#include "LogError.h"

AssertBehavior assertBehavior = AssertBehavior::None;
bool logAsserts = true;

void _f15assert(const char *_Message, const std::source_location& loc)
{
    if (logAsserts)
    {
        LogError("Assert: " + std::string(_Message), loc);
    }

    if (assertBehavior == AssertBehavior::None)
    {
        return;
    }

    if (assertBehavior == AssertBehavior::Popup)
    {
        MessageBoxA(NULL, _Message, "Assertion failed", MB_ICONERROR);
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
