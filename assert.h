#pragma once

#include <filesystem>
#include <source_location>

namespace F15Assert
{
enum class Behavior
{
    None,
    Hang,
    Popup,
    Throw,
    Terminate
};
}

#define USE_F15_ASSERT // To be passed to compiler
#define NDEBUG

#if !defined(NDEBUG) || !defined(USE_F15_ASSERT)

#define _LIBCPP_UCRT_INCLUDE(x) <../include/x> // Custom path for Visual Studio includes

#include _LIBCPP_UCRT_INCLUDE(assert.h)

#else

#include <source_location>

#define assert(_Expression) (void)((static_cast<bool>(_Expression)) || \
                                   (F15Assert::f15assert(__CRT_STRINGIZE(#_Expression), std::source_location::current()), 0))

namespace F15Assert
{
extern Behavior behavior;
extern bool logAsserts;

void f15assert(const char *_Message, const std::source_location&);
}

#endif // !defined(NDEBUG) || !defined(USE_RELEASEBUILD_ASSERT)
