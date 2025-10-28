#include "bitstream_utils.h"
#include <cassert>

void BitWriter::put_bit(int bit) {
    
    bit = (bit != 0) ? 1 : 0;

    if (bit) {
        current_ |= static_cast<uint8_t>(1u << bitpos_);
    }
    bitpos_++;

    if (bitpos_ == 8) {
        bytes_.push_back(current_);
        current_ = 0;
        bitpos_ = 0;
    }
}

void BitWriter::put_bits(uint32_t value, int nbits) {
    assert(nbits >= 0 && nbits <= 32);

    for (int i = 0; i < nbits; ++i) {
        int b = (value >> i) & 1u;
        put_bit(b);
    }
}

void BitWriter::flush_zero() {
    if (bitpos_ != 0) {
        bytes_.push_back(current_);
        current_ = 0;
        bitpos_ = 0;
    }
}

void BitWriter::reset() {
    bytes_.clear();
    current_ = 0;
    bitpos_ = 0;
}


// ----------------- BitReader -----------------

BitReader::BitReader(const std::vector<uint8_t>& bytes)
    : bytes_(bytes), idx_(0), current_(0), bitpos_(8) {
}

bool BitReader::get_bit(int& out_bit) {
    if (bitpos_ == 8) {
        if (idx_ >= bytes_.size()) {
            return false;
        }
        current_ = bytes_[idx_++];
        bitpos_ = 0;
    }

    out_bit = (current_ >> bitpos_) & 1u;
    bitpos_++;
    return true;
}

bool BitReader::get_bits(uint32_t& out_value, int nbits) {
    assert(nbits >= 0 && nbits <= 32);
    out_value = 0;
    int bit;
    for (int i = 0; i < nbits; ++i) {
        if (!get_bit(bit)) {
            return false;
        }
        if (bit) out_value |= (1u << i);
    }
    return true;
}

bool BitReader::has_more() const {
    // Quedan bits si:
    // - el índice de byte aún no llegó al final
    // - hay bits sin consumir en el 'current_' (bitpos_ < 8).
    return (idx_ < bytes_.size()) || (bitpos_ < 8);
}

void BitReader::reset() {
    idx_ = 0;
    current_ = 0;
    bitpos_ = 8;
}