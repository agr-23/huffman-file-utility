#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <cstdint>
#include <vector>

namespace compression {

// Comprime usando Huffman con formato:
// [4 bytes 'HFUF']
// [4 bytes tamaño original LE]
// [256 * 4 bytes frecuencias LE]
// [bitstream LSB-first]
std::vector<uint8_t> huffman_compress(const std::vector<uint8_t>& input);

// Descomprime el formato anterior
std::vector<uint8_t> huffman_decompress(const std::vector<uint8_t>& input);

} // namespace compression

#endif // HUFFMAN_H
