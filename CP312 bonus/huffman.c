/*
 -------------------------------------
 File:    huffman.c
 Project: Huffman Encoding
 -------------------------------------
 Author:  Tyler Rizzi
 ID:      169022274
 Email:   rizz2274@mylaurier.ca
 Version  2024-07-26
 -------------------------------------
 */

// Program to encode a message from a .txt file using Variable Length Huffman encoding algorithm 
// lower case letters, digits, period, comma, space and newline characters
// Input file: script.txt
// Output files: encoded.bin, tree.txt
// "#" represents NULL in tree, and "*" represents internal nodes of the tree
// gcc -o huffman huffman.c heap.c

// Imports
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "heap.h"   // Importing heap.h header file

// Constants
#define MAX 1024

// Code structure
typedef struct {
    char ch;
    char code[MAX];
} Code;

// Function to read a file
char* read_file(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {   // If file cannot be opened
        printf("Error opening file %s\n", filename);
        exit(1);
    }

    fseek(fp, 0, SEEK_END); // Move file pointer to end of file
    long file_size = ftell(fp); // Get the position of the file pointer
    fseek(fp, 0, SEEK_SET); // Move file pointer back to beginning of file

    char *text = (char *)malloc((file_size + 1) * sizeof(char));    // Allocate memory for text
    if (text == NULL) { // If memory allocation fails
        printf("Memory allocation failed\n");
        exit(1);
    }

    fread(text, sizeof(char), file_size, fp);   // Read file contents into text
    text[file_size] = '\0'; // Add null terminator to end of text
    fclose(fp); 

    printf("File read successfully.\n");
    return text;    
}

// Function to find frequency of characters in text
void find_frequency(char *text, int freq[]) {
    for (int i = 0; text[i] != '\0'; i++) {   // Loop through text
        char ch = text[i];  
        if (isalpha(ch)) {  // If character is a letter
            ch = tolower(ch);
        }
        if (isalpha(ch) || isdigit(ch) || ch == ' ' || ch == '.' || ch == ',' || ch == '\n') {  // If character is a letter, digit, space, period, comma or newline
            freq[(unsigned char)ch]++;  // Increment frequency of character
        }
    }
}

// Function to build Huffman tree
T_Node* build_huffman_tree(int freq[]) {
    // Create a min heap and insert all characters of text into it
    MinHeap *minHeap = create_min_heap(256);
    // Insert characters with non-zero frequency into min heap
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            T_Node *node = create_node(i, freq[i]);
            insert_min_heap(minHeap, node);
        }
    }
    build_min_heap(minHeap);

    // Build Huffman tree
    while (minHeap->size != 1) {
        T_Node *left = extract_min(minHeap);
        T_Node *right = extract_min(minHeap);
        T_Node *top = create_node('*', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insert_min_heap(minHeap, top);
    }

    return extract_min(minHeap);
}

// Function to encode a character
void encode_character(T_Node *root, char ch, char *code, int index, char *result) {
    // If root is NULL, return
    if (root == NULL) {
        return;
    }
    if (root->left == NULL && root->right == NULL) { // If leaf node
        if (root->ch == ch) {
            code[index] = '\0';
            strcat(result, code);
        }
        return;
    }
    // Traverse left and right subtrees
    code[index] = '0';
    encode_character(root->left, ch, code, index + 1, result);
    code[index] = '1';
    encode_character(root->right, ch, code, index + 1, result);
}

// Function to encode text
char* encode_text(char *text, T_Node *root) {
    char *encoded_text = (char *)malloc(MAX * sizeof(char));  // Allocate memory for encoded text
    char code[MAX];  // Temporary array to store Huffman code
    if (encoded_text == NULL) { // If memory allocation fails
        printf("Memory allocation failed\n");
        exit(1);
    }
    encoded_text[0] = '\0';  // Initialize encoded text to empty string
    // Encode each character in text
    for (int i = 0; text[i] != '\0'; i++) {
        char ch = text[i];
        // Convert letter to lowercase if needed
        if (isalpha(ch)) {
            ch = tolower(ch);
        }
        // Encode character if it is a letter, digit, space, period, comma or newline
        if (isalpha(ch) || isdigit(ch) || ch == ' ' || ch == '.' || ch == ',' || ch == '\n') {
            encode_character(root, ch, code, 0, encoded_text);
        }
    }
    printf("Text encoded successfully.\n");
    return encoded_text;
}

// Function to write encoded text to a binary file
void write_binary_file(char *filename, char *encoded_text) {
    // Open file for writing in binary mode
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }
    
    unsigned char mask = 0;
    int bit_count = 0;
    
    // Write encoded text to file
    for (int i = 0; encoded_text[i] != '\0'; i++) {
        // Set bit in mask based on encoded text
        if (encoded_text[i] == '1') {
            mask |= (1 << (7 - bit_count)); // Set bit to 1 in mask
        }
        bit_count++;
        
        // Write mask to file if it is full
        if (bit_count == 8) {
            fwrite(&mask, sizeof(unsigned char), 1, fp);    // Write mask to file
            mask = 0;
            bit_count = 0;
        }
    }
    
    // Write remaining bits to file
    if (bit_count > 0) {
        fwrite(&mask, sizeof(unsigned char), 1, fp);
    }

    fclose(fp);
    printf("Encoded text written to file successfully.\n");
}

// Function to serialize Huffman tree
// Serialize tree in preorder traversal
void serialize_tree(FILE *fp, T_Node *node) {
    if (node == NULL) { // If node is NULL, write # as placeholder
        fprintf(fp, "# ");
        return;
    }
    // Use SPACE for space character and NEWLINE for newline character
    if (node->ch == ' ') {
        fprintf(fp, "SPACE ");
    } else if (node->ch == '\n') {
        fprintf(fp, "NEWLINE ");
    } else {
        fprintf(fp, "%c ", node->ch);
    }
    serialize_tree(fp, node->left);
    serialize_tree(fp, node->right);
}

// Function to write Huffman tree to a file
void write_tree_file(char *filename, T_Node *root) {
    // Open file for regular writing
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }
    // Serialize tree and write to file
    serialize_tree(fp, root);
    fclose(fp);
    printf("Huffman tree written to file successfully.\n");
}

// Main function
int main() {
    char filename[MAX];

    printf("Enter the name of the file: ");
    scanf("%s", filename);

    char *text = read_file(filename);
    if (text == NULL) {
        printf("Error reading file.\n");
        return 1;
    }

    int freq[256] = {0};
    find_frequency(text, freq);

    T_Node *root = build_huffman_tree(freq);
    if (root == NULL) {
        printf("Error building Huffman tree.\n");
        free(text);
        return 1;
    }

    char *encoded_text = encode_text(text, root);
    if (encoded_text == NULL) {
        printf("Error encoding text.\n");
        free(text);
        return 1;
    }

    write_binary_file("encoded.bin", encoded_text); // Write encoded text to a binary file
    write_tree_file("tree.txt", root);  // Save the Huffman tree to a file for decoding

    free(text); // Free memory allocated for text
    free(encoded_text); // Free memory allocated for encoded text

    printf("Compression completed.\n"); // Print success message

    return 0;
}
