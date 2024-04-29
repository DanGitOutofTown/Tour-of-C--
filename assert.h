#pragma once
#include <filesystem>

enum class AssertBehavior
{
    None,
    Hang,
    Throw,
    Terminate
};

#ifdef ASSERT_MAIN

AssertBehavior assertBehavior = AssertBehavior::None;
bool logAsserts = true;
std::filesystem::path assertLogFile;

#else

extern AssertBehavior assertBehavior;
extern bool logAsserts;
extern std::filesystem::path assertLogFile;

#endif

#define USE_RELEASEBUILD_ASSERT // To be passed to compiler
#define NDEBUG

#if !defined(NDEBUG) || !defined(USE_RELEASEBUILD_ASSERT)

#define _LIBCPP_UCRT_INCLUDE(x) <../include/x> // Custom path for Visual Studio includes

#include _LIBCPP_UCRT_INCLUDE(assert.h)

#else

#include <source_location>

#define assert(_Expression) (void)((static_cast<bool>(_Expression)) || \
                                   (_t2assert(__CRT_STRINGIZE(#_Expression), std::source_location::current()), 0))

void _t2assert(const char *_Message, const std::source_location &loc);

#endif // !defined(NDEBUG) || !defined(USE_RELEASEBUILD_ASSERT)
