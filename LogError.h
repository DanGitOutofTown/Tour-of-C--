#pragma once

#include <filesystem>
#include <string>
#include <source_location>

namespace ErrorLogger
{
    void Init(const std::filesystem::path& errLogFile, const std::string& clientSktName,
              const std::filesystem::path& iniFile);
    
    void LogError(const std::string& errMsg, const std::string& caption = "Error",
                  const std::source_location& loc = std::source_location::current());
    
    void PopupError(const std::string& errMsg, const std::string& caption);
}