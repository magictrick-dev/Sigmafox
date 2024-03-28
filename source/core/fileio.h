#ifndef SIGMFA_CORE_FILEIO_H
#define SIGMFA_CORE_FILEIO_H
#include <core/primitives.h>

// --- File IO Functions -------------------------------------------------------
//
// The following functions provide synchronous file operations. The implementations
// are usually OS-defined, but the behavior is guaranteed to be consistent across
// platforms.
//
//      sigmafox_file_size(path)
//      sigmafox_file_read(path, buffer, rsize, bsize);
//      sigmafox_file_read_text(path, buffer, rsize, bsize);
//      sigmafox_file_write(path, buffer, wsize);
//
// File size returns zero if the file size doesn't exist and so it should not
// be used to check if a file exists since a file may exist with a size of zero.
//
// File read text is designed specifically for files that are plain-text and will
// appropriately null-terminal one byte over the last character that was read,
// but the implementation uses the normal file read to pull the file into memory.
//

bool                    sigmafox_file_exists(const char *path);
size_t                  sigmafox_file_size(const char *path);
SF_MAY_ASSERT bool      sigmafox_file_read(const char *path, void *buffer, size_t read_size, size_t buffer_size);
SF_MAY_ASSERT bool      sigmafox_file_read_text(const char *path, void *buffer, size_t read_size, size_t buffer_size);
SF_MAY_ASSERT bool      sigmafox_file_write(const char *path, void *buffer, size_t write_size);
SF_MAY_ASSERT void      sigmafox_file_get_full_path(const char *path, char *path_buffer, size_t buffer_size);

#endif
