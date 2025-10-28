#pragma once
#include <vector>
#include <cstdint>
#include <cstddef>

class BitWriter {
public:

    void put_bit(int bit);

    void put_bits(uint32_t value, int nbits);

    void flush_zero();

    const std::vector<uint8_t>& data() const { return bytes_; }

    void reset();

private:
    std::vector<uint8_t> bytes_;
    uint8_t current_ {0};
    int bitpos_ {0};
};

class BitReader {
public:
    explicit BitReader(const std::vector<uint8_t>& bytes);

    bool get_bit(int& out_bit);

    bool get_bits(uint32_t& out_value, int nbits);

    bool has_more() const;

    void reset();

private:
    const std::vector<uint8_t>& bytes_;
    size_t idx_ {0};
    uint8_t current_ {0};
    int bitpos_ {8};
};