#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <cstdint>
#include <string>
#include <vector>

namespace encryption {

// Vigenère extendido sobre bytes (mod 256)
std::vector<uint8_t> vigenere_encrypt(const std::vector<uint8_t>& data,
                                      const std::string& key);

std::vector<uint8_t> vigenere_decrypt(const std::vector<uint8_t>& data,
                                      const std::string& key);

} // namespace encryption

#endif // ENCRYPTION_H