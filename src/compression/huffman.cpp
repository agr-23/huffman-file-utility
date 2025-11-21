#include "huffman.h"
#include "bitstream_utils.h"

#include <cstdint>
#include <vector>
#include <queue>
#include <array>
#include <stdexcept>

namespace compression {

namespace {

struct Node {
    uint8_t symbol;
    uint64_t freq;
    Node* left;
    Node* right;

    Node(uint8_t s, uint64_t f) : symbol(s), freq(f), left(nullptr), right(nullptr) {}
    Node(Node* l, Node* r) : symbol(0), freq(l->freq + r->freq), left(l), right(r) {}

    bool isLeaf() const { return left == nullptr && right == nullptr; }
};

struct NodeCompare {
    bool operator()(const Node* a, const Node* b) const {
        return a->freq > b->freq; // min-heap
    }
};

void freeTree(Node* node) {
    if (!node) return;
    freeTree(node->left);
    freeTree(node->right);
    delete node;
}

void buildCodes(Node* node, std::vector<std::vector<uint8_t>>& codes, std::vector<uint8_t>& current) {
    if (!node) return;
    if (node->isLeaf()) {
        if (current.empty()) {
            // Caso borde: solo un símbolo en todo el archivo
            current.push_back(0);
        }
        codes[node->symbol] = current;
        return;
    }
    // izquierda = bit 0
    current.push_back(0);
    buildCodes(node->left, codes, current);
    current.pop_back();

    // derecha = bit 1
    current.push_back(1);
    buildCodes(node->right, codes, current);
    current.pop_back();
}

void write_uint32_le(std::vector<uint8_t>& out, uint32_t value) {
    out.push_back(static_cast<uint8_t>(value & 0xFF));
    out.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    out.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    out.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
}

uint32_t read_uint32_le(const std::vector<uint8_t>& in, size_t offset) {
    if (offset + 4 > in.size()) throw std::runtime_error("Huffman: cabecera corrupta");
    uint32_t b0 = in[offset];
    uint32_t b1 = in[offset + 1];
    uint32_t b2 = in[offset + 2];
    uint32_t b3 = in[offset + 3];
    return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
}

} // namespace

std::vector<uint8_t> huffman_compress(const std::vector<uint8_t>& input) {
    std::vector<uint8_t> output;

    // Cabecera mágica
    output.push_back('H');
    output.push_back('F');
    output.push_back('U');
    output.push_back('F');

    // Tamaño original
    uint32_t originalSize = static_cast<uint32_t>(input.size());
    write_uint32_le(output, originalSize);

    // Tabla de frecuencias
    std::array<uint64_t, 256> freq{};
    freq.fill(0);

    for (uint8_t b : input) {
        freq[b]++;
    }

    for (int i = 0; i < 256; ++i) {
        uint32_t f = static_cast<uint32_t>(freq[i]);
        write_uint32_le(output, f);
    }

    // Caso especial: archivo vacío
    if (input.empty()) {
        return output;
    }

    // Construir árbol
    std::priority_queue<Node*, std::vector<Node*>, NodeCompare> pq;
    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            pq.push(new Node(static_cast<uint8_t>(i), freq[i]));
        }
    }

    if (pq.empty()) {
        return output;
    }

    while (pq.size() > 1) {
        Node* a = pq.top(); pq.pop();
        Node* b = pq.top(); pq.pop();
        pq.push(new Node(a, b));
    }

    Node* root = pq.top();

    // Construir códigos
    std::vector<std::vector<uint8_t>> codes(256);
    std::vector<uint8_t> current;
    buildCodes(root, codes, current);

    // Codificar los datos
    BitWriter writer(output);
    for (uint8_t b : input) {
        const auto& code = codes[b];
        for (uint8_t bit : code) {
            writer.writeBit(bit);
        }
    }
    writer.flush();

    freeTree(root);
    return output;
}

std::vector<uint8_t> huffman_decompress(const std::vector<uint8_t>& input) {
    std::vector<uint8_t> output;

    if (input.size() < 4 + 4 + 256 * 4) {
        // Formato inválido
        return output;
    }

    // Verificar magia
    if (!(input[0] == 'H' && input[1] == 'F' && input[2] == 'U' && input[3] == 'F')) {
        // No es nuestro formato
        return output;
    }

    size_t offset = 4;
    uint32_t originalSize = read_uint32_le(input, offset);
    offset += 4;

    // Leer frecuencias
    std::array<uint64_t, 256> freq{};
    freq.fill(0);
    for (int i = 0; i < 256; ++i) {
        uint32_t f = read_uint32_le(input, offset);
        offset += 4;
        freq[i] = f;
    }

    if (originalSize == 0) {
        return output;
    }

    // Construir árbol desde las frecuencias
    std::priority_queue<Node*, std::vector<Node*>, NodeCompare> pq;
    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            pq.push(new Node(static_cast<uint8_t>(i), freq[i]));
        }
    }

    if (pq.empty()) {
        return output;
    }

    while (pq.size() > 1) {
        Node* a = pq.top(); pq.pop();
        Node* b = pq.top(); pq.pop();
        pq.push(new Node(a, b));
    }

    Node* root = pq.top();

    // Caso borde: solo un símbolo
    if (root->isLeaf()) {
        output.resize(originalSize, root->symbol);
        freeTree(root);
        return output;
    }

    // Decodificar bit a bit
    output.reserve(originalSize);
    BitReader reader(input, offset);

    while (output.size() < originalSize && !reader.eof()) {
        Node* node = root;
        while (!node->isLeaf()) {
            int bit = reader.readBit();
            if (bit == -1) break;
            node = (bit == 0) ? node->left : node->right;
            if (!node) break;
        }
        if (!node || !node->isLeaf()) {
            break; // datos corruptos
        }
        output.push_back(node->symbol);
    }

    freeTree(root);
    return output;
}

} // namespace compression