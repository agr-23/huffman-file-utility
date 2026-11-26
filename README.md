
# huffman-file-utility

## Overview
GSEA (the course name for this tool) is a C++ command-line application designed for lossless file compression and symmetric encryption using custom-built algorithms.  
The project demonstrates systems programming principles by using POSIX system calls for all file I/O and leveraging multithreading for parallel processing of directories.

GSEA supports:
- Compression and decompression using a custom Huffman implementation.
- Encryption and decryption using an extended Vigenère cipher operating on bytes.
- Concurrent processing: one thread per file when a directory is provided as input.
- Bit-level operations with custom BitReader/BitWriter utilities.

Final practical exam for the Operating Systems course at EAFIT.

The Vigenère cipher is a classroom exercise in byte-level transforms. It is not secure encryption and should not be used to protect real data.

---

## Features

### Compression
- Fully custom Huffman coding implementation.
- Lossless, bit-precise reconstruction.
- Custom binary format (magic header, frequency table, bitstream).

### Encryption
- Symmetric encryption using extended Vigenère (mod 256).
- Reversible with the same key.

### Concurrent Execution
- Automatic multithreading when processing directories.
- No shared buffers between threads, avoiding race conditions.

### POSIX-Based File Handling
GSEA uses:
- `open`, `read`, `write`, `close`
- `opendir`, `readdir`, `stat`

No use of high-level I/O (`stdio.h`).

---

## Build Instructions

Compile from the `src/` directory (all commands below run from there):

```sh
make
```

This generates the executable:

```sh
./huffenc
```

---

## Command-Line Usage

```sh
./huffenc [operations] -i <input> -o <output> [options]
```

### Operations
- `-c` : Compress  
- `-d` : Decompress  
- `-e` : Encrypt  
- `-u` : Decrypt  

Operations are applied in this order:
```
Compress → Encrypt → Decrypt → Decompress
```

### Options
- `--comp-alg huffman`
- `--enc-alg vigenere`
- `-k <key>` : Key for encryption/decryption
- `-i <path>` : File or directory
- `-o <path>` : Output file or directory

---

## Examples

### Compress a text file
```sh
./huffenc -c --comp-alg huffman \
  -i tests/single_text.md -o tests/out1
```

### Decompress it
```sh
./huffenc -d --comp-alg huffman \
  -i tests/out1 -o tests/out1_dec
```

### Compress + encrypt a directory concurrently

When the input is a directory, the output directory must already exist.

```sh
mkdir -p tests/out_dir1
./huffenc -c -e -k secret \
  --comp-alg huffman --enc-alg vigenere \
  -i tests/dir_test -o tests/out_dir1
```

### Decrypt + decompress
```sh
mkdir -p tests/out_dir1_rec
./huffenc -u -d -k secret \
  --comp-alg huffman --enc-alg vigenere \
  -i tests/out_dir1 -o tests/out_dir1_rec
```

---

## Automated Test Suite

A full validation script is included:

```sh
tests/run_tests.sh
```

It performs:
- Text file compression/decompression tests  
- Binary file tests  
- Directory pipeline tests (c + e + u + d)  
- Non-recursive directory validation  
- Edge cases: empty file, 1-byte file  

Run with:

```sh
./tests/run_tests.sh
```

---

## Project Structure

```
src/
├── main.cpp
├── compression/
│    ├── huffman.cpp
│    └── bitstream_utils.cpp
├── encryption/
│    └── encryption.cpp
├── file_manager/
│    └── file_manager.cpp
├── tests/
│    ├── run_tests.sh
│    └── (test files)
├── Makefile
```

---

## License

MIT, see [LICENSE](LICENSE).