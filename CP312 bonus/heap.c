/*
 -------------------------------------
 File:    heap.c
 Project: heap implementations file
 -------------------------------------
 Author:  Tyler Rizzi
 ID:      169022274
 Email:   rizz2274@mylaurier.ca
 Version  2024-07-26
 -------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include "heap.h"

MinHeap* create_min_heap(int capacity) {
    MinHeap* minHeap = (MinHeap*) malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (T_Node**) malloc(minHeap->capacity * sizeof(T_Node*));
    return minHeap;
}

void min_heapify(MinHeap *minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        T_Node *temp = minHeap->array[smallest];
        minHeap->array[smallest] = minHeap->array[idx];
        minHeap->array[idx] = temp;
        min_heapify(minHeap, smallest);
    }
}

T_Node* extract_min(MinHeap *minHeap) {
    if (minHeap->size == 0)
        return NULL;
    T_Node *temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    min_heapify(minHeap, 0);
    return temp;
}

void insert_min_heap(MinHeap *minHeap, T_Node *node) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && node->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = node;
}

void build_min_heap(MinHeap *minHeap) {
    int n = minHeap->size - 1;
    for (int i = (n - 1) / 2; i >= 0; --i)
        min_heapify(minHeap, i);
}

T_Node* create_node(char ch, unsigned freq) {
    T_Node* node = (T_Node*) malloc(sizeof(T_Node));
    node->ch = ch;
    node->freq = freq;
    node->left = node->right = NULL;
    return node;
}
