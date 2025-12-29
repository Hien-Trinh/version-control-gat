/*
Implements helper functions
 */

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>  // Contains mkdir()
#include <unistd.h>    // Contains getcwd()

// Helper function to create a directory if it doesn't exist
int create_dir(const char* path) {
  // Check if directory exists using stat()
  struct stat st = {0};

  if (stat(path, &st) == -1) {
    // Call mkdir(path, mode) if no existing directory
    // 0755 is standard (Read/Write/Execute for owner, Read/Execute for others)
    int status = mkdir(path, 0755);

    if (status != 0) {
      perror("Error creating directory");
      return -1;
    }

    printf("Initialized empty Git repository in %s\n", path);
    return 0;

  } else {
    printf("Reinitialized existing Git repository in %s\n", path);
    return 0;
  }

  return 0;
}

// Helper function to create the default HEAD file
int create_default_head() {
  const char* head_path = ".gat/HEAD";
  const char* ref_content = "ref: refs/heads/main\n";

  // Open HEAD file in write mode
  FILE* fp = fopen(head_path, "w");

  if (fp == NULL) {
    fprintf(stderr, "Error creating HEAD file");
    return 1;
  }

  // Write content to HEAD file
  fprintf(fp, "%s", ref_content);

  fclose(fp);

  return 0;
}

char* read_file_content(const char* path, size_t* out_len) {
  // 1. Open file in read binary mode
  FILE* fp = fopen(path, "rb");

  if (fp == NULL) {
    fprintf(stderr, "Failed to open file");
    return NULL;
  }

  // 2. Calculate file size
  // Move pointer to the end of the file
  if (fseek(fp, 0, SEEK_END) != 0) {
    fprintf(stderr, "fseek failed");
    fclose(fp);
    return NULL;
  }

  // Get the current position (file size in bytes)
  long file_size = ftell(fp);
  if (file_size < 0) {
    fprintf(stderr, "ftell failed");
    fclose(fp);
    return NULL;
  }

  // Reset pointer to the start of the file
  rewind(fp);

  // 3. Read file to buffer
  // Allocate memory with size + 1 for a null terminator
  char* buffer = malloc(file_size + 1);
  if (buffer == NULL) {
    fprintf(stderr, "Memory allocation failed");
    fclose(fp);
    return NULL;
  }

  // fread returns the number of bytes read successfully
  size_t bytes_read = fread(buffer, 1, file_size, path);
  if (bytes_read != (size_t)file_size) {
    fprintf(stderr, "Error: Expected %ld bytes, read %lu\n", file_size, bytes_read);
    free(buffer);
    fclose(fp);
    return NULL;
  }

  // 4. Null-terminate
  buffer[file_size] = '\0';      // Null-terminate for safety
  *out_len = (size_t)file_size;  // Inform the caller how big the file is

  fclose(fp);
  return buffer;
}