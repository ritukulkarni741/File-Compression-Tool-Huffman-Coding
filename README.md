
# Huffman File Compression Tool

## About the Project

A lossless file compression and decompression tool
implemented in C using Huffman Coding.

The program reduces file size by assigning variable-length
binary codes based on byte frequencies.

## Features

- Lossless compression
- Huffman Coding algorithm
- Binary file support
- Min Heap priority queue
- Dynamic memory allocation
- Bit-level encoding and decoding
- Compression statistics
- File restoration

## Technologies

- C Programming
- C11
- Data Structures
- Binary Trees
- Priority Queues
- Greedy Algorithms
- File Handling
- Bitwise Operations

## Project Structure

- main.c: Command-line interface
- huffman.c: Huffman Tree construction
- priority_queue.c: Min Heap implementation
- bit_io.c: Bit-level input/output
- file_handler.c: Compression and decompression


## Usage

### Compress

huffman.exe compress input.txt output.huff

### Decompress

huffman.exe decompress output.huff restored.txt

## Testing

Compare the original and restored files
to verify lossless compression.

## Author

Ritu N Kulkarni
