/*
 -------------------------------------
 File:    heap.h
 Project: functions helper file
 -------------------------------------
 Author:  Tyler Rizzi
 ID:      169022274
 Email:   rizz2274@mylaurier.ca
 Version  2024-07-26
 -------------------------------------
 */

#ifndef HEAP_H
#define HEAP_H

typedef struct T_Node {
    char ch;
    unsigned freq;
    struct T_Node *left, *right;
} T_Node;

typedef struct {
    T_Node **array;
    unsigned size;
    unsigned capacity;
} MinHeap;

MinHeap* create_min_heap(int capacity);
void min_heapify(MinHeap *minHeap, int idx);
T_Node* extract_min(MinHeap *minHeap);
void insert_min_heap(MinHeap *minHeap, T_Node *node);
void build_min_heap(MinHeap *minHeap);
T_Node* create_node(char ch, unsigned freq);
T_Node* build_huffman_tree(int freq[]);

#endif // HEAP_H
