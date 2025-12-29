/*
Declaration of helper functions, allowing main.c and object.c to use said functions
*/

#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>  // Defines size_t

// Function prototypes
char* read_file_content(const char* path, size_t* out_len);

#endif