#pragma once

#include <filesystem>
#include <string>
#include <source_location>

extern bool logErrors;
extern std::filesystem::path errorLogFile;

void LogError(const std::string &errMsg, const std::source_location &loc = std::source_location::current());