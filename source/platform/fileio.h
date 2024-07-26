#ifndef SIGMAFOX_PLATFORM_FILEIO_H
#define SIGMAFOX_PLATFORM_FILEIO_H
#include <core/definitions.h>

// --- File IO Functions -------------------------------------------------------
//
// The following functions provide synchronous file operations.
//
// File size returns zero if the file size doesn't exist, however it should not
// be used to check if a file exists since a file may exist with a size of zero.
// Use the appropriate file exists function to determine if a file actually exists.
//

b32 fileio_file_exists(cc64 path);
u64 fileio_file_size(cc64 path);
b32 fileio_file_read(cc64 path, void *buffer, u64 read_size, u64 buffer_size);
b32 fileio_file_write(cc64 path, void *buffer, u64 write_size);
void fileio_file_get_full_path(cc64 path, c64 path_buffer, u64 buffer_size);

#endif
