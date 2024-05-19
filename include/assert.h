#pragma once

#if defined(NDEBUG) && defined(RELEASE_ASSERT)

#include <filesystem>
#include <source_location>

#define assert(expression) (void)((static_cast<bool>(expression)) ||                                   \
                                  (ReleaseAssert::release_assert(#expression, std::source_location::current()), \
                                   0))

namespace ReleaseAssert
{
    enum class Behavior
    {
        Ignore,
        Hang,
        Popup,
        Throw,
        Terminate
    };

    void ParseConfig(const std::filesystem::path& iniFile);
    void release_assert(const char *expression, const std::source_location& loc);
}

#else

// Use system assert.h (safety net)
// Should not get here if this assert.h is only in the additional include directories
// when building ReleaseWithDebug
#include <C:/msys64/ucrt64/include/assert.h>

#endif
