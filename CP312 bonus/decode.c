/*
 -------------------------------------
 File:    decode.c
 Project: Huffman Encoding
 -------------------------------------
 Author:  Tyler Rizzi
 ID:      169022274
 Email:   rizz2274@mylaurier.ca
 Version  2024-07-26
 -------------------------------------
 */

// Program to decode a message from a .txt file using Variable Length Huffman encoding algorithm

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heap.h"

#define MAX 1024

void read_binary_file(char *filename, char *encoded_text) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }

    unsigned char buffer;
    int bit_count = 0;
    while (fread(&buffer, sizeof(unsigned char), 1, fp)) {
        for (int i = 7; i >= 0; i--) {
            encoded_text[bit_count++] = (buffer & (1 << i)) ? '1' : '0';
        }
    }
    encoded_text[bit_count] = '\0';
    fclose(fp);
    printf("Encoded text read from file successfully.\n");
}

void decode_text(T_Node *root, char *encoded_text, char *decoded_text) {
    T_Node *current = root;
    int i = 0, j = 0;
    while (encoded_text[i] != '\0') {
        if (encoded_text[i] == '0') {
            current = current->left;
        } else {
            current = current->right;
        }

        if (current->left == NULL && current->right == NULL) {
            decoded_text[j++] = current->ch;
            current = root;
        }
        i++;
    }
    decoded_text[j] = '\0';
}

T_Node* deserialize_tree(FILE *fp) {
    char ch;
    if (fscanf(fp, " %c", &ch) != 1 || ch == '#') {
        return NULL;
    }

    T_Node *node = (T_Node *)malloc(sizeof(T_Node));
    if (node == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    node->ch = ch;
    node->left = deserialize_tree(fp);
    node->right = deserialize_tree(fp);
    return node;
}

T_Node* read_tree_file(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening tree file %s\n", filename);
        exit(1);
    }
    T_Node *root = deserialize_tree(fp);
    fclose(fp);
    return root;
}

void write_decoded_file(char *filename, char *decoded_text) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }

    fprintf(fp, "%s", decoded_text);
    fclose(fp);
    printf("Decoded text written to file successfully.\n");
}

void print_tree(T_Node *root, int depth) {
    if (root == NULL) {
        for (int i = 0; i < depth; i++) printf("  ");
        printf("#\n");
        return;
    }
    for (int i = 0; i < depth; i++) printf("  ");
    printf("%c\n", root->ch);
    print_tree(root->left, depth + 1);
    print_tree(root->right, depth + 1);
}

int main() {
    char encoded_filename[MAX];
    char tree_filename[MAX];

    printf("Enter the name of the encoded file: ");
    scanf("%s", encoded_filename);

    printf("Enter the name of the tree file: ");
    scanf("%s", tree_filename);

    T_Node *root = read_tree_file(tree_filename);  // Read the Huffman tree from the file
    if (root == NULL) {
        printf("Error reading Huffman tree.\n");
        return 1;
    }

    // Debug print the deserialized tree
    printf("Deserialized Huffman Tree:\n");
    print_tree(root, 0);

    char *encoded_text = (char *) malloc(MAX * sizeof(char));
    if (encoded_text == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }
    read_binary_file(encoded_filename, encoded_text);

    char *decoded_text = (char *) malloc(MAX * sizeof(char));
    if (decoded_text == NULL) {
        printf("Memory allocation failed\n");
        free(encoded_text);
        return 1;
    }

    decode_text(root, encoded_text, decoded_text);

    write_decoded_file("result.txt", decoded_text);

    free(encoded_text);
    free(decoded_text);

    printf("Decompression completed.\n");

    return 0;
}