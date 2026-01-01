/*
Declaration of helper functions, allowing main.c and object.c to use said functions
*/

// Since both main.c and object.c use utils.h, the compiler may think we are defining things twice
// This conditional sets the flag UTILS_H to check if the function definitions are already mounted
#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>  // Defines size_t

// Directory
int create_dir(const char* path);
int create_default_head();

// File read/write
char* read_file_content(const char* path, size_t* out_len);
int write_loose_object(unsigned char* data, size_t size, char* hex_hash);

// Hashing
void sha1_to_hex(const unsigned char* binary_hash, char* hex_buffer);
void hex_to_sha1(const char* hex_hash, unsigned char* binary_buffer);
void create_hex_hash(unsigned char* data, size_t size, char* hex_buffer);

#endif