
# Secure and Efficient File Management Utility (GSEA)

## Overview
GSEA is a C++ command-line application designed for lossless file compression and symmetric encryption using custom-built algorithms.  
The project demonstrates systems programming principles by using POSIX system calls for all file I/O and leveraging multithreading for parallel processing of directories.

GSEA supports:
- Compression and decompression using a custom Huffman implementation.
- Encryption and decryption using an extended Vigenère cipher operating on bytes.
- Concurrent processing: one thread per file when a directory is provided as input.
- Bit-level operations with custom BitReader/BitWriter utilities.

This project is part of the Final Practical Exam for Operating Systems.

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

Compile from the `src/` directory:

```

make

```

This generates the executable:

```

./huffenc

```

---

## Command-Line Usage

```

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
```

./huffenc -c --comp-alg huffman 
-i tests/single_text.md -o tests/out1

```

### Decompress it
```

./huffenc -d --comp-alg huffman 
-i tests/out1 -o tests/out1_dec

```

### Compress + encrypt a directory concurrently
```

./huffenc -c -e -k secret 
--comp-alg huffman --enc-alg vigenere 
-i tests/dir_test -o tests/out_dir1

```

### Decrypt + decompress
```

./huffenc -u -d -k secret 
--comp-alg huffman --enc-alg vigenere 
-i tests/out_dir1 -o tests/out_dir1_rec

```

---

## Automated Test Suite

A full validation script is included:

```

tests/run_tests.sh

```

It performs:
- Text file compression/decompression tests  
- Binary file tests  
- Directory pipeline tests (c + e + u + d)  
- Non-recursive directory validation  
- Edge cases: empty file, 1-byte file  

Run with:

```

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
Academic use for Operating Systems Final Exam.