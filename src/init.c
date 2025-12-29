/*
Implements gat init. Create the directories and default HEAD.
*/

// Initialize .gat/ and its subdirectories
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