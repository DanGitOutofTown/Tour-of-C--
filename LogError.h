#pragma once

#include <filesystem>
#include <string_view>
#include <source_location>

namespace ErrorLogger
{
    void Init(std::filesystem::path file);
    void LogError(std::string_view errMsg, const std::source_location loc = std::source_location::current());
}