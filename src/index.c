/*
Implements gat add. Reads the .gat/index file into memory,
add new file's path and hash, sort the list, writes into disk.
*/

#include <arpa/inet.h>  // For htonl (Host TO Network Long) - Endianness!
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "object.h"
#include "utils.h"

index_entry_t create_index_entry(const char* path, const char* hex_hash, struct stat st);

int cmd_add(int argc, char* argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s add <filename>\n", argv[0]);
    return 1;
  }
  const char* filename = argv[2];

  // 1. Check file exists
  struct stat st = {0};
  if (stat(filename, &st) == -1) {
    fprintf(stderr, "Error file %s doesn't exist", filename);
    return 1;
  }

  // 2. Hash the object
  char* hex_hash = hash_object_to_disk(filename);

  // 3. Create the entry
  index_entry_t entry = create_index_entry(filename, hex_hash, st);

  return 0;
}

index_entry_t create_index_entry(const char* path, const char* hex_hash, struct stat st) {
  index_entry_t entry;
  return entry;
}