
#include <stdio.h>
#include <stdlib.h>

#include "priority_queue.h"

static void swapNodes(Node **a, Node **b)
{
    Node *temp = *a;
    *a = *b;
    *b = temp;
}

MinHeap *createMinHeap(int capacity)
{
    MinHeap *heap = malloc(sizeof(MinHeap));

    if (heap == NULL)
        return NULL;

    heap->nodes = malloc(capacity * sizeof(Node *));

    if (heap->nodes == NULL) {
        free(heap);
        return NULL;
    }

    heap->size = 0;
    heap->capacity = capacity;

    return heap;
}

void insertMinHeap(MinHeap *heap, Node *node)
{
    if (heap == NULL || node == NULL)
        return;

    if (heap->size == heap->capacity)
        return;

    int i = heap->size;

    heap->nodes[i] = node;
    heap->size++;

    while (i > 0) {
        int parent = (i - 1) / 2;

        if (heap->nodes[parent]->frequency <=
            heap->nodes[i]->frequency)
            break;

        swapNodes(&heap->nodes[parent], &heap->nodes[i]);

        i = parent;
    }
}

Node *extractMin(MinHeap *heap)
{
    if (heap == NULL || heap->size == 0)
        return NULL;

    Node *minimum = heap->nodes[0];

    heap->size--;

    if (heap->size > 0) {
        heap->nodes[0] = heap->nodes[heap->size];

        int i = 0;

        while (1) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            int smallest = i;

            if (left < heap->size &&
                heap->nodes[left]->frequency <
                heap->nodes[smallest]->frequency) {
                smallest = left;
            }

            if (right < heap->size &&
                heap->nodes[right]->frequency <
                heap->nodes[smallest]->frequency) {
                smallest = right;
            }

            if (smallest == i)
                break;

            swapNodes(
                &heap->nodes[i],
                &heap->nodes[smallest]
            );

            i = smallest;
        }
    }

    return minimum;
}

void freeMinHeap(MinHeap *heap)
{
    if (heap == NULL)
        return;

    free(heap->nodes);
    free(heap);
}
