#pragma once

namespace Expect
{
enum ExpectEnum
{
    ExpectNone,
    ExpectHang,
    ExpectLog,
    ExpectBoth
};

#ifdef EXPECT_MAIN
ExpectEnum expectConfig = ExpectNone;
#else
extern ExpectEnum expectConfig;
#endif
}

#ifndef NDEBUG

#include <iostream>
#include <sstream>
#include <source_location>
#include <chrono>
#include <thread>

namespace Expect
{
    // Release build version for desktop and trainers
    inline void expect(bool cond, const std::source_location& loc = std::source_location::current())
    {
        std::stringstream oss;
        oss << loc.file_name() << ":|" << loc.line() << "| " << loc.function_name();

        if (!cond)
        {
            if (expectConfig == ExpectLog || expectConfig == ExpectBoth)
            {
                // For in-house trainers, fielded trainers, and/or desktop
                // Need to use thread-safe I/O here that preferrably supports throttling
                std::cout << oss.str() << std::endl;
            }
            if (expectConfig == ExpectHang || expectConfig == ExpectBoth)
            {
                // For desktop and/or in-house trainers
                for (auto debug = false;;)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    if (debug) // attach debugger here and set debug to true
                    {
                        break;
                    }
                }
            }
        }
    }
}
#else // Debug build version for desktop only

#include <cassert>
#define expect(cond) assert(cond);

#endif