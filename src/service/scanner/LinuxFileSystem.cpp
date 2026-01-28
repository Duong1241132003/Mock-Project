#include "service/scanner/LinuxFileSystem.h"
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

bool LinuxFileSystem::exists(const std::string& path) const {
    std::error_code ec;
    return fs::exists(path, ec);
}

bool LinuxFileSystem::isDirectory(const std::string& path) const {
    std::error_code ec;
    return fs::is_directory(path, ec);
}

std::vector<std::string> LinuxFileSystem::listEntries(const std::string& path) const {
    std::vector<std::string> entries;
    std::error_code ec;
    
    // Use directory_iterator for non-recursive listing (Scanner handles recursion)
    if (fs::exists(path, ec) && fs::is_directory(path, ec)) {
        for (const auto& entry : fs::directory_iterator(path, ec)) {
            entries.push_back(entry.path().string());
        }
    }
    return entries;
}