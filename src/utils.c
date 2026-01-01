/*
Implements helper functions
 */

#include "utils.h"

#include <openssl/sha.h>  // For SHA1()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>  // For mkdir()
#include <unistd.h>    // Contains getcwd()
#include <zlib.h>      // For deflate()

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

    printf("Initialized empty directory in %s\n", path);
    return 0;

  } else {
    printf("Reinitialized existing directory in %s\n", path);
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
  size_t bytes_read = fread(buffer, 1, file_size, fp);
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

// Compress and write to disk
int write_loose_object(unsigned char* data, size_t size, char* hex_hash) {
  // 1. Create path
  char path[256];

  // Directory path is the first 2 chars
  sprintf(path, ".gat/objects/%.2s", hex_hash);
  create_dir(path);

  // Filename is the remaining chars
  sprintf(path, ".gat/objects/%.2s/%s", hex_hash, hex_hash + 2);

  // 2. Compress using zlib
  unsigned long compressed_length = compressBound(size);  // Calculates the upper bound for compressed blob.size
  unsigned char* compressed_data = malloc(compressed_length);
  if (compressed_data == NULL) {
    fprintf(stderr, "Failed to allocate memory for compression");
    return -1;
  }

  // zlib compress()
  int result = compress(compressed_data, &compressed_length, data, size);
  if (result != Z_OK) {  // Z_OK = 0
    fprintf(stderr, "Failed to compress");
    return -1;
  }

  // Write to file, binary mode
  FILE* fp = fopen(path, "wb");
  fwrite(compressed_data, compressed_length, 1, fp);
  fclose(fp);

  free(compressed_data);
  return 0;
}

// Convert binary hash to readable hex
void sha1_to_hex(const unsigned char* binary_hash, char* hex_buffer) {
  for (int i = 0; i < 20; i++) {
    sprintf(hex_buffer + i * 2, "%02x", binary_hash[i]);
  }

  hex_buffer[40] = '\0';
}

// Helper: Converts a single hex char (0-9, a-f) to its integer value (0-15)
int hex_char_to_int(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return -1;
}

// Converts "a94..." (40 chars) -> binary buffer (20 bytes)
void hex_to_sha1(const char* hex_hash, unsigned char* binary_buffer) {
  for (int i = 0; i < 20; i++) {
    // We take two hex chars (e.g., "a" and "9") to make one byte
    int low = hex_char_to_int(hex_hash[i * 2]);
    int high = hex_char_to_int(hex_hash[i * 2 + 1]);

    // Just in case
    if (low == -1 || high == -1) {
      fprintf(stderr, "Invalid hex input");
    }

    // Combine them: (0xa << 4) | 0x9  ==  0xa0 | 0x09  ==  0xa9
    binary_buffer[i] = (unsigned char)((high << 4) | low);
  }
}

// Create hex hash from data
void create_hex_hash(unsigned char* data, size_t size, char* hex_buffer) {
  unsigned char binary_hash[SHA_DIGEST_LENGTH];  // SHA_DIGEST_LENGTH is 20
  SHA1(data, size, binary_hash);

  // Convert to hex
  sha1_to_hex(binary_hash, hex_buffer);
  printf("Hex hash: %s\n", hex_buffer);
}