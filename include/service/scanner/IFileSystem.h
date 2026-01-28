#ifndef IFILESYSTEM_H
#define IFILESYSTEM_H

#include <string>
#include <vector>

/**
 * @brief Interface for file system operations.
 * Allows decoupling FileScanner from specific OS implementations (std::filesystem).
 */
class IFileSystem {
public:
    virtual ~IFileSystem() = default;

    // Checks if a specific path exists
    virtual bool exists(const std::string& path) const = 0;

    // Checks if the path refers to a directory
    virtual bool isDirectory(const std::string& path) const = 0;

    // Returns a list of full paths representing the contents of the directory
    virtual std::vector<std::string> listEntries(const std::string& path) const = 0;
};

#endif // IFILESYSTEM_H
