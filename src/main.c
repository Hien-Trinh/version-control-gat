/*
CLI entrypoint for 'gat', a mini version control system
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cmd_init(int argc, char* argv[]);
// int cmd_add(int argc, char* argv[]);
// int cmd_commit(int argc, char* argv[]);

int main(int argc, char* argv[]) {
  // Print command list if no argument passed
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <command> [<args>]\n", argv[0]);
    return 1;
  }

  const char* command = argv[1];

  // Case 1: init
  if (strcmp(command, "init") == 0) {
    return cmd_init(argc, argv);
  }

  // Case 2: staging
  else if (strcmp(command, "add") == 0) {
    // TODO: implement cmd_add() in index.c
    // return cmd_add(argc, argv);
    return 1;
  }

  // Case 3: commit
  else if (strcmp(command, "commit") == 0) {
    // TODO: implement cmd_commit() in commit.c
    // return cmd_commit(argc, argv);
    return 1;
  }

  // Unknown command
  else {
    fprintf(stderr, "Unknown command: %s\n", command);
    return 1;
  }

  return 0;
}