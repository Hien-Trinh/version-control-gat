/*
Implements gat init. Create the directories and default HEAD.
*/

#include <stdio.h>

#include "utils.h"

// Initialize .gat/ and its subdirectories
int cmd_init(int argc, char* argv[]) {
  // Check additional arguments
  if (argc > 2) {
    fprintf(stderr, "Initializing Gat repository in subdirectory %s not available", argv[2]);
    return 1;
  }

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