#include "encryption.h"

namespace encryption {

std::vector<uint8_t> vigenere_encrypt(const std::vector<uint8_t>& data,
                                      const std::string& key) {
    std::vector<uint8_t> out;
    out.reserve(data.size());

    if (key.empty()) {
        // Si no hay clave, se deja igual
        out = data;
        return out;
    }

    const size_t keyLen = key.size();
    for (size_t i = 0; i < data.size(); ++i) {
        uint8_t k = static_cast<uint8_t>(key[i % keyLen]);
        uint8_t c = static_cast<uint8_t>((data[i] + k) % 256);
        out.push_back(c);
    }
    return out;
}

std::vector<uint8_t> vigenere_decrypt(const std::vector<uint8_t>& data,
                                      const std::string& key) {
    std::vector<uint8_t> out;
    out.reserve(data.size());

    if (key.empty()) {
        out = data;
        return out;
    }

    const size_t keyLen = key.size();
    for (size_t i = 0; i < data.size(); ++i) {
        uint8_t k = static_cast<uint8_t>(key[i % keyLen]);
        int value = static_cast<int>(data[i]) - static_cast<int>(k);
        if (value < 0) value += 256;
        out.push_back(static_cast<uint8_t>(value));
    }
    return out;
}

} // namespace encryption