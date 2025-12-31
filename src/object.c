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
#include <zlib.h>  // For deflate()

#include "utils.h"

char* hash_object_to_disk(const char* filename);
void sha1_to_hex(const unsigned char* binary_hash, char* hex_buffer);
void create_hex_hash(unsigned char* data, size_t size, char* hex_buffer);
int write_loose_object(unsigned char* data, size_t size, char* hex_hash);

int cmd_hash_object(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s hash-object <filename>\n", argv[0]);
    return 1;
  }

  const char* filename = argv[2];

  char* hex_hash = hash_object_to_disk(filename);
  if (!hex_hash) {
    fprintf(stderr, "Failed hashing.");
    return 1;
  }

  return 0;
}

char* hash_object_to_disk(const char* filename) {
  static char hex_hash[41];  // Static buffer to return
  size_t file_length;
  char* file_content = read_file_content(filename, &file_length);

  if (!file_content) return NULL;

  // 1. Create Header
  size_t header_length = snprintf(NULL, 0, "blob %lu", file_length) + 1;
  size_t full_length = header_length + file_length;
  unsigned char* full_data = malloc(full_length);

  sprintf((char*)full_data, "blob %lu", file_length);
  memcpy(full_data + header_length, file_content, file_length);

  // 2. Hash
  create_hex_hash(full_data, full_length, hex_hash);

  // 3. Write
  write_loose_object(full_data, full_length, hex_hash);

  free(full_data);
  free(file_content);

  return hex_hash;
}

// Convert binary hash to readable hex
void sha1_to_hex(const unsigned char* binary_hash, char* hex_buffer) {
  for (int i = 0; i < 20; i++) {
    sprintf(hex_buffer + i * 2, "%02x", binary_hash[i]);
  }

  hex_buffer[40] = '\0';
}

// Create hex hash from data
void create_hex_hash(unsigned char* data, size_t size, char* hex_buffer) {
  unsigned char binary_hash[SHA_DIGEST_LENGTH];  // SHA_DIGEST_LENGTH is 20
  SHA1(data, size, binary_hash);

  // Convert to hex
  sha1_to_hex(binary_hash, hex_buffer);
  printf("Hex hash: %s\n", hex_buffer);
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
  fwrite(compressed_data, 1, compressed_length, fp);
  fclose(fp);

  free(compressed_data);
  return 0;
}