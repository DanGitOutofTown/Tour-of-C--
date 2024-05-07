#pragma once

#if defined(NDEBUG) && defined(F15ASSERT)

#include <source_location>

#define assert(expression) (void)((static_cast<bool>(expression)) || \
                                   (F15Assert::f15assert(#expression, std::source_location::current()), \
                                    0))

namespace F15Assert
{

enum class Behavior {None, Hang, Popup, Throw, Terminate};

extern Behavior behavior;
extern bool enableBehavior;
extern bool logAsserts;

void f15assert(const char *expression, const std::source_location &);

}

#else

#define _LIBCPP_UCRT_INCLUDE(x) <C:/msys64/ucrt64/include/x> // Custom path for Visual Studio includes

#include _LIBCPP_UCRT_INCLUDE(assert.h)

#endif
