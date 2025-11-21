#include "file_manager.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <stdexcept>

namespace file_manager {

bool is_directory(const std::string& path) {
    struct stat st{};
    if (stat(path.c_str(), &st) != 0) return false;
    return S_ISDIR(st.st_mode);
}

bool is_regular_file(const std::string& path) {
    struct stat st{};
    if (stat(path.c_str(), &st) != 0) return false;
    return S_ISREG(st.st_mode);
}

std::vector<std::string> list_files_in_directory(const std::string& dirpath) {
    std::vector<std::string> files;

    DIR* dir = opendir(dirpath.c_str());
    if (!dir) {
        return files;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        // Ignorar . y ..
        if (std::strcmp(entry->d_name, ".") == 0 ||
            std::strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        std::string fullPath = dirpath + "/" + entry->d_name;

        struct stat st{};
        if (stat(fullPath.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
            files.push_back(fullPath);
        }
    }
    closedir(dir);
    return files;
}

std::vector<uint8_t> read_file(const std::string& path) {
    std::vector<uint8_t> data;

    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        throw std::runtime_error("Error al abrir archivo para lectura: " + path);
    }

    const size_t BUF_SIZE = 32 * 1024;
    uint8_t buffer[BUF_SIZE];

    while (true) {
        ssize_t bytesRead = read(fd, buffer, BUF_SIZE);
        if (bytesRead < 0) {
            close(fd);
            throw std::runtime_error("Error al leer archivo: " + path);
        }
        if (bytesRead == 0) break;
        data.insert(data.end(), buffer, buffer + bytesRead);
    }

    close(fd);
    return data;
}

void write_file(const std::string& path, const std::vector<uint8_t>& data) {
    int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        throw std::runtime_error("Error al abrir archivo para escritura: " + path);
    }

    const uint8_t* ptr = data.data();
    size_t remaining = data.size();

    while (remaining > 0) {
        ssize_t written = write(fd, ptr, remaining);
        if (written < 0) {
            close(fd);
            throw std::runtime_error("Error al escribir archivo: " + path);
        }
        remaining -= written;
        ptr += written;
    }

    close(fd);
}

std::string basename(const std::string& path) {
    if (path.empty()) return std::string();

    // Buscar última '/'
    size_t pos = path.find_last_of('/');
    if (pos == std::string::npos) return path;
    if (pos + 1 >= path.size()) return std::string();
    return path.substr(pos + 1);
}

std::string join_paths(const std::string& dir, const std::string& name) {
    if (dir.empty()) return name;
    if (!dir.empty() && dir.back() == '/') {
        return dir + name;
    }
    return dir + "/" + name;
}

} // namespace file_manager