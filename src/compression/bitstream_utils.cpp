#include "bitstream_utils.h"

namespace compression {

BitWriter::BitWriter(std::vector<uint8_t>& buf)
    : buffer(buf), currentByte(0), bitPos(0) {}

void BitWriter::writeBit(int bit) {
    if (bit) {
        currentByte |= static_cast<uint8_t>(1u << bitPos);
    }
    bitPos++;
    if (bitPos == 8) {
        buffer.push_back(currentByte);
        currentByte = 0;
        bitPos = 0;
    }
}

void BitWriter::writeBits(uint32_t value, int bitCount) {
    for (int i = 0; i < bitCount; ++i) {
        int bit = (value >> i) & 1u; // LSB-first
        writeBit(bit);
    }
}

void BitWriter::flush() {
    if (bitPos > 0) {
        buffer.push_back(currentByte);
        currentByte = 0;
        bitPos = 0;
    }
}

BitReader::BitReader(const std::vector<uint8_t>& buf, size_t offset)
    : buffer(buf), index(offset), bitPos(0), atEnd(false) {
    if (index >= buffer.size()) {
        atEnd = true;
    }
}

int BitReader::readBit() {
    if (atEnd) return -1;
    if (index >= buffer.size()) {
        atEnd = true;
        return -1;
    }

    uint8_t byte = buffer[index];
    int bit = (byte >> bitPos) & 1u;
    bitPos++;
    if (bitPos == 8) {
        bitPos = 0;
        index++;
        if (index >= buffer.size()) {
            atEnd = true;
        }
    }
    return bit;
}

bool BitReader::eof() const {
    return atEnd;
}

} // namespace compression