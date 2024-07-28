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
// lower case letters, digits, period, comma, space and newline characters
// Input files: encoded.bin, tree.txt
// Output file: result.txt
// "#" represents NULL in tree, and "*" represents internal nodes of the tree
// gcc -o decode decode.c heap.c

// Imports
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heap.h"   // Importing heap.h header file

// Constants
#define MAX 1024

// Function to read a binary file
void read_binary_file(char *filename, char *encoded_text) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {   // If file cannot be opened
        printf("Error opening file %s\n", filename);
        exit(1);
    }

    // Read the binary file bit by bit
    unsigned char buffer;   // Buffer to store each byte read from file
    int bit_count = 0;  
    // Read each byte from file and store each bit
    while (fread(&buffer, sizeof(unsigned char), 1, fp)) {
        for (int i = 7; i >= 0; i--) {      
            encoded_text[bit_count++] = (buffer & (1 << i)) ? '1' : '0';  
        }
    }
    encoded_text[bit_count] = '\0';
    fclose(fp);
    printf("Encoded text read from file successfully.\n");
}

// Function to decode the encoded text
void decode_text(T_Node *root, char *encoded_text, char *decoded_text) {
    T_Node *current = root;
    int i = 0, j = 0;
    // Traverse the Huffman tree to decode the encoded text
    while (encoded_text[i] != '\0') {
        if (encoded_text[i] == '0') {   // Move to left child if bit is 0
            if (current->left == NULL) {    // If left child is NULL
                printf("Error: Current left node is NULL at index %d\n", i);
                exit(1);
            }
            current = current->left;
        } else {    // Move to right child if bit is 1
            if (current->right == NULL) {   // If right child is NULL
                printf("Error: Current right node is NULL at index %d\n", i);
                exit(1);
            }
            current = current->right;
        }

        // If current node is a leaf node, add character to decoded text
        if (current->left == NULL && current->right == NULL) {
            decoded_text[j++] = current->ch;
            current = root;
        }
        i++;
    }
    // Add null character at the end of decoded text
    decoded_text[j] = '\0';
}

// Function to deserialize the Huffman tree
T_Node* deserialize_tree(FILE *fp) {
    char ch[10];
    // Read character from file
    if (fscanf(fp, "%s", ch) != 1 || strcmp(ch, "#") == 0) {
        return NULL;
    }

    // Create a new node
    T_Node *node = (T_Node *)malloc(sizeof(T_Node));
    if (node == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    // Set character of node
    if (strcmp(ch, "SPACE") == 0) {
        node->ch = ' ';
    } else if (strcmp(ch, "NEWLINE") == 0) {
        node->ch = '\n';
    } else {
        node->ch = ch[0];
    }

    // Deserialize left and right children
    node->left = deserialize_tree(fp);
    node->right = deserialize_tree(fp);
    return node;
}

// Function to read the Huffman tree from a file and return the root node
T_Node* read_tree_file(char *filename) {
    // Open file for reading
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening tree file %s\n", filename);
        exit(1);
    }
    // Deserialize the tree
    T_Node *root = deserialize_tree(fp);
    fclose(fp);
    return root;
}

// Function to write the decoded text to a file
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

// Main function
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

    char *encoded_text = (char *) malloc(MAX * sizeof(char));  // Fixed allocation size issue
    if (encoded_text == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }
    read_binary_file(encoded_filename, encoded_text);

    char *decoded_text = (char *) malloc(MAX * sizeof(char));  // Fixed allocation size issue
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
