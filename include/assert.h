#pragma once

#if defined(NDEBUG) && defined(F15ASSERT)

#include <filesystem>
#include <source_location>

#define assert(expression) (void)((static_cast<bool>(expression)) ||                                   \
                                  (F15Assert::f15assert(#expression, std::source_location::current()), \
                                   0))

namespace F15Assert
{
    enum class Behavior
    {
        Ignore,
        Hang,
        Popup,
        Throw,
        Terminate
    };

    void ParseConfig(std::filesystem::path iniFile);
    void f15assert(const char *expression, const std::source_location &);
}

#else

// Use system assert.h (safety net)
// Should not get here if this assert.h is only in the additional include directories
// when building ReleaseWithDebug
#include <C:/msys64/ucrt64/include/assert.h>

#endif
