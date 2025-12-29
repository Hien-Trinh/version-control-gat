/*
Implements gat hash-object.
Hashing: reads data, wrap it in a header, calculate the SHA-1 hash
Compressing: using zlib
Storing: takes the hash (e.g. a74fe9...), create a directory a7/ (first 2 chars),
                                          write the file 4fe9... (remaining chars)
*/