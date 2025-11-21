#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <cstring>
#include <stdexcept>

#include "file_manager/file_manager.h"
#include "compression/huffman.h"
#include "encryption/encryption.h"

struct JobConfig {
    std::string input_path;
    std::string output_path;

    bool do_compress = false;
    bool do_decompress = false;
    bool do_encrypt = false;
    bool do_decrypt = false;

    std::string key;
};

void print_usage(const char* prog) {
    std::cerr << "Uso:\n"
              << "  " << prog << " [-c] [-d] [-e] [-u] "
              << "--comp-alg huffman --enc-alg vigenere "
              << "-i <entrada> -o <salida> [-k <clave>]\n\n"
              << "  -c : comprimir\n"
              << "  -d : descomprimir\n"
              << "  -e : encriptar\n"
              << "  -u : desencriptar\n"
              << "  -i : ruta de archivo o directorio de entrada\n"
              << "  -o : ruta de archivo o directorio de salida\n"
              << "  -k : clave para Vigenere (encriptar/desencriptar)\n";
}

void process_file_job(JobConfig cfg) {
    try {
        std::vector<uint8_t> data = file_manager::read_file(cfg.input_path);

        // Pipeline: -c -> -e -> -u -> -d
        if (cfg.do_compress) {
            data = compression::huffman_compress(data);
        }
        if (cfg.do_encrypt) {
            data = encryption::vigenere_encrypt(data, cfg.key);
        }
        if (cfg.do_decrypt) {
            data = encryption::vigenere_decrypt(data, cfg.key);
        }
        if (cfg.do_decompress) {
            data = compression::huffman_decompress(data);
        }

        file_manager::write_file(cfg.output_path, data);

        std::cout << "Procesado: " << cfg.input_path
                  << " -> " << cfg.output_path << "\n";
    } catch (const std::exception& ex) {
        std::cerr << "Error procesando " << cfg.input_path
                  << ": " << ex.what() << "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        print_usage(argv[0]);
        return 1;
    }

    JobConfig baseCfg;
    std::string compAlg;
    std::string encAlg;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-c") == 0) {
            baseCfg.do_compress = true;
        } else if (std::strcmp(argv[i], "-d") == 0) {
            baseCfg.do_decompress = true;
        } else if (std::strcmp(argv[i], "-e") == 0) {
            baseCfg.do_encrypt = true;
        } else if (std::strcmp(argv[i], "-u") == 0) {
            baseCfg.do_decrypt = true;
        } else if (std::strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            baseCfg.input_path = argv[++i];
        } else if (std::strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            baseCfg.output_path = argv[++i];
        } else if (std::strcmp(argv[i], "-k") == 0 && i + 1 < argc) {
            baseCfg.key = argv[++i];
        } else if (std::strcmp(argv[i], "--comp-alg") == 0 && i + 1 < argc) {
            compAlg = argv[++i];
        } else if (std::strcmp(argv[i], "--enc-alg") == 0 && i + 1 < argc) {
            encAlg = argv[++i];
        } else {
            std::cerr << "Argumento desconocido o incompleto: " << argv[i] << "\n";
            print_usage(argv[0]);
            return 1;
        }
    }

    // Validaciones simples
    if (baseCfg.input_path.empty() || baseCfg.output_path.empty()) {
        std::cerr << "Debe especificar -i y -o\n";
        return 1;
    }

    if (baseCfg.do_encrypt || baseCfg.do_decrypt) {
        if (baseCfg.key.empty()) {
            std::cerr << "Debe especificar -k <clave> para encriptar/desencriptar.\n";
            return 1;
        }
    }

    // Validar algoritmos (solo huffman + vigenere)
    if (!compAlg.empty() && compAlg != "huffman") {
        std::cerr << "Solo se soporta --comp-alg huffman\n";
        return 1;
    }
    if (!encAlg.empty() && encAlg != "vigenere") {
        std::cerr << "Solo se soporta --enc-alg vigenere\n";
        return 1;
    }

    // Detectar si entrada es archivo o directorio
    bool inputIsDir = file_manager::is_directory(baseCfg.input_path);
    bool inputIsFile = file_manager::is_regular_file(baseCfg.input_path);

    if (!inputIsDir && !inputIsFile) {
        std::cerr << "La ruta de entrada no es archivo ni directorio válido: "
                  << baseCfg.input_path << "\n";
        return 1;
    }

    std::vector<std::thread> workers;

    if (inputIsFile) {
        // Un solo archivo
        JobConfig cfg = baseCfg;

        // Si la salida es un directorio, usar mismo nombre
        if (file_manager::is_directory(baseCfg.output_path)) {
            std::string name = file_manager::basename(baseCfg.input_path);
            cfg.output_path = file_manager::join_paths(baseCfg.output_path, name);
        }

        workers.emplace_back(process_file_job, cfg);
    } else {
        // Directorio: salida debe ser directorio también
        if (!file_manager::is_directory(baseCfg.output_path)) {
            std::cerr << "Cuando la entrada es directorio, la salida debe ser un directorio existente.\n";
            return 1;
        }

        std::vector<std::string> files =
            file_manager::list_files_in_directory(baseCfg.input_path);

        if (files.empty()) {
            std::cerr << "El directorio de entrada no contiene archivos regulares.\n";
            return 1;
        }

        for (const auto& inPath : files) {
            JobConfig cfg = baseCfg;
            cfg.input_path = inPath;
            std::string name = file_manager::basename(inPath);
            cfg.output_path = file_manager::join_paths(baseCfg.output_path, name);

            workers.emplace_back(process_file_job, cfg);
        }
    }

    // Esperar todos los hilos
    for (auto& t : workers) {
        if (t.joinable()) t.join();
    }

    return 0;
}