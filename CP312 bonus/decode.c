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

void read_tree_file(char *filename, T_Node **root) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening tree file %s\n", filename);
        exit(1);
    }

    int freq[256] = {0};
    char ch[MAX];
    int frequency;
    while (fscanf(fp, "%s %d\n", ch, &frequency) == 2) {
        if (strcmp(ch, "SPACE") == 0) {
            freq[' '] = frequency;
        } else if (strcmp(ch, "NEWLINE") == 0) {
            freq['\n'] = frequency;
        } else {
            freq[(unsigned char)ch[0]] = frequency;
        }
    }
    fclose(fp);

    *root = build_huffman_tree(freq);
    printf("Huffman tree read from file successfully.\n");
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

int main() {
    char encoded_filename[MAX];
    char tree_filename[MAX];

    printf("Enter the name of the encoded file: ");
    scanf("%s", encoded_filename);

    printf("Enter the name of the tree file: ");
    scanf("%s", tree_filename);

    T_Node *root = NULL;
    read_tree_file(tree_filename, &root);
    if (root == NULL) {
        printf("Error reading Huffman tree.\n");
        return 1;
    }

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