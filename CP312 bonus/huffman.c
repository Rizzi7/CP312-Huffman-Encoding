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
// Output files: encoded.bin, freq.txt
// Imports
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "heap.h"   // Importing heap.h

// Constants
#define MAX 1024

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

// Function to build dictionary 
void build_dict(T_Node *root, char code[], int index, char *dict[]) {
    if (root == NULL) { 
        return;
    }
    if (root->left == NULL && root->right == NULL) {    // If leaf node
        code[index] = '\0'; // Add null terminator to end of code
        dict[(int) root->ch] = (char *) malloc((strlen(code) + 1) * sizeof(char));  // Allocate memory for dictionary entry
        if (dict[(int) root->ch] == NULL) { // If memory allocation fails
            printf("Memory allocation failed\n");
            exit(1);
        }
        strcpy(dict[(int) root->ch], code);
        return;
    }
    code[index] = '0';
    build_dict(root->left, code, index + 1, dict);
    code[index] = '1';
    build_dict(root->right, code, index + 1, dict);
}

char* encode_text(char *text, char *dict[]) {
    char *encoded_text = (char *) malloc(MAX * sizeof(char));
    if (encoded_text == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    encoded_text[0] = '\0';
    for (int i = 0; text[i] != '\0'; i++) {
        char ch = text[i];
        if (isalpha(ch)) {
            ch = tolower(ch);
        }
        if (isalpha(ch) || isdigit(ch) || ch == ' ' || ch == '.' || ch == ',' || ch == '\n') {
            strcat(encoded_text, dict[(int) ch]);
        }
    }
    printf("Text encoded successfully.\n");
    return encoded_text;
}

void write_binary_file(char *filename, char *encoded_text) {
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }
    
    unsigned char mask = 0;
    int bit_count = 0;
    
    for (int i = 0; encoded_text[i] != '\0'; i++) {
        if (encoded_text[i] == '1') {
            mask |= (1 << (7 - bit_count));
        }
        bit_count++;
        
        if (bit_count == 8) {
            fwrite(&mask, sizeof(unsigned char), 1, fp);
            mask = 0;
            bit_count = 0;
        }
    }
    
    if (bit_count > 0) {
        fwrite(&mask, sizeof(unsigned char), 1, fp);
    }

    fclose(fp);
    printf("Encoded text written to file successfully.\n");
}

void serialize_tree(T_Node *root, FILE *fp) {
    if (root == NULL) {
        fprintf(fp, "# ");
        return;
    }
    fprintf(fp, "%c ", root->ch);
    serialize_tree(root->left, fp);
    serialize_tree(root->right, fp);
}

void write_tree_file(char *filename, T_Node *root) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }
    serialize_tree(root, fp);
    fclose(fp);
    printf("Huffman tree written to file successfully.\n");
}

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

    char *dict[256] = {NULL};
    char code[MAX];
    build_dict(root, code, 0, dict);

    printf("Dictionary built successfully. Contents:\n");

    char *encoded_text = encode_text(text, dict);
    if (encoded_text == NULL) {
        printf("Error encoding text.\n");
        free(text);
        for (int i = 0; i < 256; i++) {
            if (dict[i] != NULL) {
                free(dict[i]);
            }
        }
        return 1;
    }

    write_binary_file("encoded.bin", encoded_text);
    write_tree_file("tree.txt", root);  // Save the Huffman tree to a file

    free(text);
    free(encoded_text);

    for (int i = 0; i < 256; i++) {
        if (dict[i] != NULL) {
            free(dict[i]);
        }
    }

    printf("Compression completed.\n");

    return 0;
}