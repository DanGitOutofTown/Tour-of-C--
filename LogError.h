#pragma once

#include <filesystem>
#include <string>
#include <source_location>

namespace ErrorLogger
{
    void Init(std::filesystem::path file, std::string clientSktName, std::filesystem::path iniFile);
    void LogError(std::string errMsg, std::string caption = "Error", const std::source_location loc = std::source_location::current());
    void PopupError(std::string errMsg, std::string caption);
}