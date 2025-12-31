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
