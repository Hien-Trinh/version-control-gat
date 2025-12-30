/*
Blueprint for gat objects and how the code should handle them in memory
*/

#include <stddef.h>

int cmd_hash_object(int argc, char* argv[]);

typedef enum {
  OBJ_COMMIT,
  OBJ_TREE,
  OBJ_BLOB,
  OBJ_TAG
} object_type;

typedef struct {
  object_type type;
  size_t size;
  void* data;     // Generic pointer (could be Tree or Blob object)
  char hash[41];  // 40 chars of SHA-1 + 1 terminator
} gat_object;

/*
How its used:
1. Create the struct
gat_object obj;

2. Set the type
obj.type = OBJ_BLOB;

3. Load data
obj.data = read_file("test.txt", &obj.size);

4. Calculate Hash
sha1_hash(obj.data, obj.size, obj.hash);

Now 'obj' contains everything needed to write to the .gat folder
*/