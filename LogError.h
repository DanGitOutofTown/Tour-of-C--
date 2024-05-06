#pragma once

#include <filesystem>
#include <string_view>
#include <source_location>

namespace ErrorLogger
{

extern bool logErrors;
extern std::filesystem::path errFile;

void LogError(std::string_view errMsg, const std::source_location loc = std::source_location::current());

}