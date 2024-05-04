#pragma once

#include <filesystem>
#include <string_view>
#include <source_location>

extern bool logErrors;
extern std::filesystem::path errorLogFile;

void LogError(std::string_view errMsg, const std::source_location& loc = std::source_location::current());