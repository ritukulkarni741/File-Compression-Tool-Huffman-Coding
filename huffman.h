
#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BYTE_RANGE 256

typedef struct Node {
    unsigned char data;
    uint64_t frequency;

    struct Node *left;
    struct Node *right;

} Node;

Node *createNode(unsigned char data, uint64_t frequency);

Node *buildHuffmanTree(uint64_t frequencies[]);

void generateCodes(
    Node *root,
    char *code,
    int depth,
    char *codes[]
);

void freeHuffmanTree(Node *root);

#endif