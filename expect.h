#ifndef NDEBUG

#include <iostream>
#include <sstream>
#include <source_location>
#include "windows.h"

using std::source_location;

// Release build version for desktop and trainers
inline void expect(bool cond, const source_location &loc = source_location::current())
{
    std::stringstream oss;
    oss << loc.file_name() << ":|" << loc.line() << "| " << loc.function_name();

#ifndef MESSAGEBOX // For desktop and/or in-house trainers
    MessageBoxA(NULL, oss.str().c_str(), "Assertion failed", MB_ICONERROR);
#endif

    // For desktop and trainers
    // Any fast file I/O method may be used here
    std::cout << oss.str() << std::endl;
}

#else // Debug build version for desktop only

#include <cassert>
#define expect(cond) assert(cond);

#endif