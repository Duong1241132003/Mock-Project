#ifndef LINUXFILESYSTEM_H
#define LINUXFILESYSTEM_H

#include "service/scanner/IFileSystem.h"

/**
 * @brief Concrete implementation of IFileSystem using C++17 std::filesystem.
 * This class handles actual OS interactions.
 */
class LinuxFileSystem : public IFileSystem {
public:
    bool exists(const std::string& path) const override;
    bool isDirectory(const std::string& path) const override;
    std::vector<std::string> listEntries(const std::string& path) const override;
};

#endif // LINUXFILESYSTEM_H