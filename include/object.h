/*
Blueprint for gat objects and how the code should handle them in memory
*/

#include <stddef.h>
#include <stdint.h>

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

// Git Index Entry Structure (aligned to match binary format on disk)
// We use __attribute__((packed)) to stop C from adding padding bytes.
typedef struct __attribute__((packed)) {
  uint32_t ctime_sec;      // Creation time (seconds)
  uint32_t ctime_nano;     // Creation time (nanoseconds)
  uint32_t mtime_sec;      // Modification time (seconds)
  uint32_t mtime_nano;     // Modification time (nanoseconds)
  uint32_t dev;            // Device ID
  uint32_t ino;            // Inode
  uint32_t mode;           // File mode (permissions)
  uint32_t uid;            // User ID
  uint32_t gid;            // Group ID
  uint32_t file_size;      // File size
  unsigned char sha1[20];  // The hash of the blob
  uint16_t flags;          // Flags (length of filename)
                           // The filename follows immediately after this struct in memory
} index_entry_t;

int cmd_hash_object(int argc, char* argv[]);
char* hash_object_to_disk(const char* filename);