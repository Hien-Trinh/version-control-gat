/*
Hashing: reads data, wrap it in a header, calculate the SHA-1 hash
Compressing: using zlib
Storing: takes the hash (e.g. a74fe9...), create a directory a7/ (first 2 chars),
                                          write the file 4fe9... (remaining chars)
*/

#include "object.h"

#include <openssl/sha.h>  // For SHA1()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>  // For mkdir()
#include <zlib.h>      // For deflate()

#include "utils.h"

// Convert binary hash to readable hex
void sha1_to_hex(const unsigned char* binary_hash, char* hex_buffer) {
  for (int i = 0; i < 20; i++) {
    sprintf(hex_buffer + i * 2, "%02x", binary_hash[i]);
  }

  hex_buffer[40] = '\0';
}

// Compress and write to disk
int write_loose_object(const void* data, size_t length, const char* hex_hash) {
  // 1. Create path
  char path[256];

  // Directory path is the first 2 chars
  sprintf(path, ".gat/objects/%.2s", hex_hash);
  create_dir(path);

  // Filename is the remaining chars
  sprintf(path, ".gat/objects/%.2s/%s", hex_hash, hex_hash + 2);

  // 2. Compress using zlib
  unsigned long compressed_length = compressBound(length);  // Calculates the upper bound for compressed length
  unsigned char* compressed_data = malloc(compressed_length);
  if (compressed_data == NULL) {
    fprintf(stderr, "Failed to allocate memory for compression");
    return -1;
  }

  // zlib compress()
  int result = compress(compressed_data, &compressed_length, data, length);
  if (result != Z_OK) {  // Z_OK = 0
    fprintf(stderr, "Failed to compress");
    return -1;
  }

  // Write to file, binary mode
  FILE* fp = fopen(path, "wb");
  fwrite(compressed_data, 0, compressed_length, fp);
  fclose(fp);

  free(compressed_data);
  return 0;
}

// Create object blob
int cmd_hash_object(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s hash-object <filename>\n", argv[0]);
    return 1;
  }

  const char* filename = argv[2];

  // 1. Applying header
  // Read input file
  size_t file_length;
  char* file_content = read_file_content(filename, &file_length);

  if (file_content == NULL) {
    fprintf(stderr, "Error reading file: %s\n", filename);
    return 1;
  }

  // Create header: "blob <file_length>\0"
  size_t header_length = snprintf(NULL, 0, "blob %lu", file_length) + 1;
  size_t full_length = header_length + file_length;

  unsigned char* full_data = malloc(full_length);

  // Add header to full_data
  sprintf((char*)full_data, "blob %lu", file_length);

  // Add file_content after the null-terminator
  memcpy(full_data + header_length, file_content, file_length);

  // 2. Hashing
  unsigned char binary_hash[SHA_DIGEST_LENGTH];  // SHA_DIGEST_LENGTH is 20
  SHA1(full_data, full_length, binary_hash);

  // Convert to hex
  char hex_hash[41];
  sha1_to_hex(binary_hash, hex_hash);
  printf("Hex hash: %s\n", hex_hash);

  // 3. Write to disk
  if (write_loose_object(full_data, full_length, hex_hash) != 0) {
    fprintf(stderr, "Error writing to disk");
    return 1;
  }

  free(full_data);
  free(file_content);  // from read_file_content()
  return 0;
}
