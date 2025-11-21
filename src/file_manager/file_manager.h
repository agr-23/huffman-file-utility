#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <cstdint>
#include <string>
#include <vector>

namespace file_manager {

// Verifica si la ruta existe y es directorio
bool is_directory(const std::string& path);

// Verifica si la ruta existe y es archivo regular
bool is_regular_file(const std::string& path);

// Lista archivos regulares (no recursivo) en un directorio
std::vector<std::string> list_files_in_directory(const std::string& dirpath);

// Lee un archivo completo usando open/read (binario)
std::vector<uint8_t> read_file(const std::string& path);

// Escribe un archivo completo usando open/write (binario)
void write_file(const std::string& path, const std::vector<uint8_t>& data);

// Obtiene el nombre base de una ruta
std::string basename(const std::string& path);

// Une directorio + nombre de archivo
std::string join_paths(const std::string& dir, const std::string& name);

} // namespace file_manager

#endif // FILE_MANAGER_H