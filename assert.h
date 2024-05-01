#pragma once
#include <filesystem>

enum class AssertBehavior
{
    None,
    Hang,
    Popup,
    Throw,
    Terminate
};

#define USE_RELEASEBUILD_ASSERT // To be passed to compiler
#define NDEBUG

#if !defined(NDEBUG) || !defined(USE_RELEASEBUILD_ASSERT)

#define _LIBCPP_UCRT_INCLUDE(x) <../include/x> // Custom path for Visual Studio includes

#include _LIBCPP_UCRT_INCLUDE(assert.h)

#else

#include <source_location>

extern AssertBehavior assertBehavior;
extern bool logAsserts;

#define assert(_Expression) (void)((static_cast<bool>(_Expression)) || \
                                   (_f15assert(__CRT_STRINGIZE(#_Expression), std::source_location::current()), 0))

void _f15assert(const char *_Message, const std::source_location &loc);

#endif // !defined(NDEBUG) || !defined(USE_RELEASEBUILD_ASSERT)
