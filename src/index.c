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

index_entry_t create_index_entry(const char* path, const char* hex_hash, struct stat* st);

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
  index_entry_t entry = create_index_entry(filename, hex_hash, &st);

  return 0;
}

index_entry_t create_index_entry(const char* path, const char* hex_hash, struct stat* st) {
  index_entry_t entry;

  // Clear memory
  memset(&entry, 0, sizeof(index_entry_t));

  // Fill in metadata
  entry.ctime_sec = htonl((uint32_t)st->st_ctimespec.tv_sec);
  entry.ctime_nano = htonl((uint32_t)st->st_ctimespec.tv_nsec);
  entry.mtime_sec = htonl((uint32_t)st->st_mtimespec.tv_sec);
  entry.mtime_nano = htonl((uint32_t)st->st_mtimespec.tv_nsec);
  entry.dev = htonl((uint32_t)st->st_dev);
  entry.ino = htonl((uint32_t)st->st_ino);
  entry.mode = htonl((uint32_t)st->st_mode);
  entry.uid = htonl((uint32_t)st->st_uid);
  entry.gid = htonl((uint32_t)st->st_gid);
  entry.file_size = htonl((uint32_t)st->st_size);

  // Convert Hex Hash back to Binary
  hex_to_sha1(hex_hash, entry.sha1);

  // Flags
  size_t path_length = strlen(path);
  if (path_length > 0xFFF) path_length = 0xFFF;
  entry.flags = htons((uint16_t)path_length);

  return entry;
}