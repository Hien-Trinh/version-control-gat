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

gat_object* create_object(const char* filename, object_type object_type);
int write_loose_object(gat_object* blob);

int cmd_hash_object(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s hash-object <filename>\n", argv[0]);
    return 1;
  }

  const char* filename = argv[2];

  gat_object* blob = create_object(filename, OBJ_BLOB);

  // 3. Write to disk
  // if (write_loose_object(blob) != 0) {
  //   fprintf(stderr, "Error writing to disk");
  //   return 1;
  // }

  free(blob->data);
  free(blob);
  return 0;
}

// Create object blob
gat_object* create_object(const char* filename, object_type object_type) {
  // 0. Create blob
  gat_object* blob = malloc(sizeof(gat_object));
  blob->type = object_type;

  // 1. Applying header
  // Read input file
  size_t file_length;
  char* file_content = read_file_content(filename, &file_length);

  if (file_content == NULL) {
    fprintf(stderr, "Error reading file: %s\n", filename);
    free(blob);
    return NULL;
  }

  // Create header: "blob <file_length>\0"
  size_t header_length = snprintf(NULL, 0, "blob %lu", file_length) + 1;
  blob->size = header_length + file_length;
  blob->data = (unsigned char*)malloc(blob->size);

  // Add header to blob.data
  sprintf((char*)blob->data, "blob %lu", file_length);

  // Add file_content after the null-terminator
  memcpy(blob->data + header_length, file_content, file_length);

  // 2. Hashing
  unsigned char binary_hash[SHA_DIGEST_LENGTH];  // SHA_DIGEST_LENGTH is 20
  SHA1(blob->data, blob->size, binary_hash);

  // Convert to hex
  sha1_to_hex(binary_hash, blob->hash);
  printf("Hex hash: %s\n", blob->hash);

  free(file_content);  // from read_file_content()
  return blob;
}

// Compress and write to disk
int write_loose_object(gat_object* blob) {
  // 1. Create path
  char path[256];

  // Directory path is the first 2 chars
  sprintf(path, ".gat/objects/%.2s", blob->hash);
  create_dir(path);

  // Filename is the remaining chars
  sprintf(path, ".gat/objects/%.2s/%s", blob->hash, blob->hash + 2);

  // 2. Compress using zlib
  unsigned long compressed_length = compressBound(blob->size);  // Calculates the upper bound for compressed blob.size
  unsigned char* compressed_data = malloc(compressed_length);
  if (compressed_data == NULL) {
    fprintf(stderr, "Failed to allocate memory for compression");
    return -1;
  }

  // zlib compress()
  int result = compress(compressed_data, &compressed_length, blob->data, blob->size);
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