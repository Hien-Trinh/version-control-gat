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
  fprintf(fp, ref_content);

  fclose(fp);

  return 0;
}

int cmd_init(int argc, char* argv[]) {
  // Root directory
  if (create_dir(".gat") != 0) return 1;

  // Subdirectories
  if (create_dir(".gat/objects") != 0) return 1;
  if (create_dir(".gat/refs") != 0) return 1;
  if (create_dir(".gat/refs/heads") != 0) return 1;

  // HEAD file
  if (create_default_head() != 0) return 1;

  return 0;
}