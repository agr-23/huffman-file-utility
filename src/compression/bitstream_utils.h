#ifndef BITSTREAM_UTILS_H
#define BITSTREAM_UTILS_H

#include <cstdint>
#include <vector>

namespace compression {

class BitWriter {
public:
    BitWriter(std::vector<uint8_t>& buffer);

    // Escribe un bit (0 o 1)
    void writeBit(int bit);

    // Escribe una secuencia de bits a partir de un valor entero (LSB-first)
    void writeBits(uint32_t value, int bitCount);

    // Vacía el byte actual si está parcialmente lleno
    void flush();

private:
    std::vector<uint8_t>& buffer;
    uint8_t currentByte;
    int bitPos; // número de bits ya usados (0-7), LSB-first
};

class BitReader {
public:
    BitReader(const std::vector<uint8_t>& buffer, size_t offset);

    // Lee un bit, retorna -1 si se llega al final
    int readBit();

    bool eof() const;

private:
    const std::vector<uint8_t>& buffer;
    size_t index;   // índice actual en buffer
    int bitPos;     // bit actual dentro del byte (0-7), LSB-first
    bool atEnd;
};

} // namespace compression

#endif // BITSTREAM_UTILS_H