/*
Declaration of helper functions, allowing main.c and object.c to use said functions
*/

// Since both main.c and object.c use utils.h, the compiler may think we are defining things twice
// This conditional sets the flag UTILS_H to check if the function definitions are already mounted
#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>  // Defines size_t

// Function prototypes
char* read_file_content(const char* path, size_t* out_len);

#endif