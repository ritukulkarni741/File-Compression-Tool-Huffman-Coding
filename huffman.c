
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "huffman.h"
#include "priority_queue.h"

Node *createNode(unsigned char data, uint64_t frequency)
{
    Node *node = malloc(sizeof(Node));

    if (node == NULL)
        return NULL;

    node->data = data;
    node->frequency = frequency;
    node->left = NULL;
    node->right = NULL;

    return node;
}

Node *buildHuffmanTree(uint64_t frequencies[])
{
    MinHeap *heap = createMinHeap(BYTE_RANGE);

    if (heap == NULL)
        return NULL;

    for (int i = 0; i < BYTE_RANGE; i++) {

        if (frequencies[i] > 0) {

            Node *node = createNode(
                (unsigned char)i,
                frequencies[i]
            );

            if (node == NULL) {
                while (heap->size > 0)
                    freeHuffmanTree(extractMin(heap));

                freeMinHeap(heap);
                return NULL;
            }

            insertMinHeap(heap, node);
        }
    }

    if (heap->size == 0) {
        freeMinHeap(heap);
        return NULL;
    }

    while (heap->size > 1) {

        Node *left = extractMin(heap);
        Node *right = extractMin(heap);

        if (UINT64_MAX - left->frequency < right->frequency) {
            freeHuffmanTree(left);
            freeHuffmanTree(right);

            while (heap->size > 0)
                freeHuffmanTree(extractMin(heap));

            freeMinHeap(heap);
            return NULL;
        }

        Node *parent = createNode(
            0,
            left->frequency + right->frequency
        );

        if (parent == NULL) {
            freeHuffmanTree(left);
            freeHuffmanTree(right);

            while (heap->size > 0)
                freeHuffmanTree(extractMin(heap));

            freeMinHeap(heap);
            return NULL;
        }

        parent->left = left;
        parent->right = right;

        insertMinHeap(heap, parent);
    }

    Node *root = extractMin(heap);

    freeMinHeap(heap);

    return root;
}

void generateCodes(
    Node *root,
    char *code,
    int depth,
    char *codes[]
)
{
    if (root == NULL)
        return;

    if (root->left == NULL && root->right == NULL) {

        if (depth == 0) {
            code[0] = '0';
            depth = 1;
        }

        code[depth] = '\0';

        codes[root->data] = malloc((depth + 1) * sizeof(char));

        if (codes[root->data] != NULL) {
            for (int i = 0; i <= depth; i++)
                codes[root->data][i] = code[i];
        }

        return;
    }

    code[depth] = '0';

    generateCodes(root->left, code, depth + 1, codes);

    code[depth] = '1';

    generateCodes(root->right, code, depth + 1, codes);
}

void freeHuffmanTree(Node *root)
{
    if (root == NULL)
        return;

    freeHuffmanTree(root->left);
    freeHuffmanTree(root->right);

    free(root);
}
