
#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "huffman.h"

typedef struct {
    Node **nodes;
    int size;
    int capacity;
} MinHeap;

MinHeap *createMinHeap(int capacity);

void insertMinHeap(MinHeap *heap, Node *node);

Node *extractMin(MinHeap *heap);

void freeMinHeap(MinHeap *heap);

#endif
