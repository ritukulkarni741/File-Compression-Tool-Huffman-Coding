
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#include "file_handler.h"
#include "huffman.h"
#include "bit_io.h"

#define HEADER_MAGIC "HUF1"

static int writeUint64(FILE *file, uint64_t value)
{
    for (int i = 0; i < 8; i++) {

        unsigned char byte = (unsigned char)(value & 0xFF);

        if (fputc(byte, file) == EOF)
            return 0;

        value >>= 8;
    }

    return 1;
}

static int readUint64(FILE *file, uint64_t *value)
{
    uint64_t result = 0;

    for (int i = 0; i < 8; i++) {

        int byte = fgetc(file);

        if (byte == EOF)
            return 0;

        result |= (uint64_t)(unsigned char)byte << (8 * i);
    }

    *value = result;

    return 1;
}

int compressFile(const char *inputPath, const char *outputPath)
{
    if (inputPath == NULL || outputPath == NULL)
        return 0;

    if (strcmp(inputPath, outputPath) == 0) {
        printf("Error: Input and output paths must be different.\n");
        return 0;
    }

    FILE *input = fopen(inputPath, "rb");

    if (input == NULL) {
        perror("Error opening input file");
        return 0;
    }

    uint64_t frequencies[BYTE_RANGE] = {0};
    uint64_t originalSize = 0;

    int ch;

    while ((ch = fgetc(input)) != EOF) {

        unsigned char byte = (unsigned char)ch;

        if (frequencies[byte] == UINT64_MAX ||
            originalSize == UINT64_MAX) {

            printf("Error: Input file is too large.\n");
            fclose(input);
            return 0;
        }

        frequencies[byte]++;
        originalSize++;
    }

    if (ferror(input)) {
        printf("Error reading input file.\n");
        fclose(input);
        return 0;
    }

    Node *root = buildHuffmanTree(frequencies);

    if (originalSize > 0 && root == NULL) {
        printf("Error building Huffman Tree.\n");
        fclose(input);
        return 0;
    }

    char *codes[BYTE_RANGE] = {0};
    char codeBuffer[BYTE_RANGE];

    if (root != NULL)
        generateCodes(root, codeBuffer, 0, codes);

    for (int i = 0; i < BYTE_RANGE; i++) {

        if (frequencies[i] > 0 && codes[i] == NULL) {

            printf("Error generating Huffman codes.\n");

            for (int j = 0; j < BYTE_RANGE; j++)
                free(codes[j]);

            freeHuffmanTree(root);
            fclose(input);

            return 0;
        }
    }

    FILE *output = fopen(outputPath, "wb");

    if (output == NULL) {
        perror("Error opening output file");

        for (int i = 0; i < BYTE_RANGE; i++)
            free(codes[i]);

        freeHuffmanTree(root);
        fclose(input);

        return 0;
    }

    int success = 1;

    if (fwrite(HEADER_MAGIC, 1, 4, output) != 4)
        success = 0;

    for (int i = 0; i < BYTE_RANGE && success; i++) {

        if (!writeUint64(output, frequencies[i]))
            success = 0;
    }

    if (fseek(input, 0, SEEK_SET) != 0)
        success = 0;

    BitWriter writer;

    initBitWriter(&writer, output);

    while (success && (ch = fgetc(input)) != EOF) {

        unsigned char byte = (unsigned char)ch;

        for (int i = 0; codes[byte][i] != '\0'; i++) {

            if (!writeBit(&writer, codes[byte][i] - '0')) {
                success = 0;
                break;
            }
        }
    }

    if (ferror(input))
        success = 0;

    if (success && !flushBitWriter(&writer))
        success = 0;

    if (fflush(output) != 0)
        success = 0;

    long compressedSize = -1;

    if (success) {
        compressedSize = ftell(output);

        if (compressedSize < 0)
            success = 0;
    }

    if (fclose(input) != 0)
        success = 0;

    if (fclose(output) != 0)
        success = 0;

    for (int i = 0; i < BYTE_RANGE; i++)
        free(codes[i]);

    freeHuffmanTree(root);

    if (!success) {
        printf("Compression failed.\n");
        return 0;
    }

    printf("\nCompression successful!\n");
    printf("Original size: %" PRIu64 " bytes\n", originalSize);
    printf("Compressed size: %ld bytes\n", compressedSize);

    if (originalSize > 0) {

        double ratio = (double)originalSize / compressedSize;

        double saving =
            (1.0 - (double)compressedSize / originalSize) * 100.0;

        printf("Compression ratio: %.2f:1\n", ratio);
        printf("Space saving: %.2f%%\n", saving);
    }

    return 1;
}

int decompressFile(const char *inputPath, const char *outputPath)
{
    if (inputPath == NULL || outputPath == NULL)
        return 0;

    if (strcmp(inputPath, outputPath) == 0) {
        printf("Error: Input and output paths must be different.\n");
        return 0;
    }

    FILE *input = fopen(inputPath, "rb");

    if (input == NULL) {
        perror("Error opening compressed file");
        return 0;
    }

    char magic[4];

    if (fread(magic, 1, 4, input) != 4 ||
        memcmp(magic, HEADER_MAGIC, 4) != 0) {

        printf("Error: Invalid compressed file format.\n");
        fclose(input);
        return 0;
    }

    uint64_t frequencies[BYTE_RANGE] = {0};
    uint64_t originalSize = 0;

    for (int i = 0; i < BYTE_RANGE; i++) {

        if (!readUint64(input, &frequencies[i])) {
            printf("Error: Incomplete frequency table.\n");
            fclose(input);
            return 0;
        }

        if (UINT64_MAX - originalSize < frequencies[i]) {
            printf("Error: Invalid file size information.\n");
            fclose(input);
            return 0;
        }

        originalSize += frequencies[i];
    }

    Node *root = buildHuffmanTree(frequencies);

    if (originalSize > 0 && root == NULL) {
        printf("Error reconstructing Huffman Tree.\n");
        fclose(input);
        return 0;
    }

    FILE *output = fopen(outputPath, "wb");

    if (output == NULL) {
        perror("Error opening restored file");

        freeHuffmanTree(root);
        fclose(input);

        return 0;
    }

    BitReader reader;

    initBitReader(&reader, input);

    int success = 1;

    for (uint64_t i = 0; i < originalSize; i++) {

        Node *current = root;

        if (current == NULL) {
            success = 0;
            break;
        }

        if (current->left == NULL && current->right == NULL) {

            int bit = readBit(&reader);

            if (bit != 0) {
                success = 0;
                break;
            }

        } else {

            while (current->left != NULL ||
                   current->right != NULL) {

                int bit = readBit(&reader);

                if (bit == -1) {
                    success = 0;
                    break;
                }

                current = (bit == 0) ?
                          current->left : current->right;

                if (current == NULL) {
                    success = 0;
                    break;
                }
            }
        }

        if (!success)
            break;

        if (fputc(current->data, output) == EOF) {
            success = 0;
            break;
        }
    }

    if (reader.error || ferror(input) || ferror(output))
        success = 0;

    if (fclose(input) != 0)
        success = 0;

    if (fclose(output) != 0)
        success = 0;

    freeHuffmanTree(root);

    if (!success) {
        printf("Decompression failed. The compressed data may be damaged.\n");
        return 0;
    }

    printf("\nDecompression successful!\n");
    printf("Restored size: %" PRIu64 " bytes\n", originalSize);

    return 1;
}
