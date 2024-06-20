#ifndef SIGMAFOX_PLATFORM_FILEIO_H
#define SIGMAFOX_PLATFORM_FILEIO_H

// --- File IO Functions -------------------------------------------------------
//
// The following functions provide synchronous file operations.
//
// File size returns zero if the file size doesn't exist, however it should not
// be used to check if a file exists since a file may exist with a size of zero.
// Use the appropriate file exists function to determine if a file actually exists.
//

bool    fileio_file_exists(const char *path);
size_t  fileio_file_size(const char *path);
bool    fileio_file_read(const char *path, void *buffer, size_t read_size, size_t buffer_size);
bool    fileio_file_write(const char *path, void *buffer, size_t write_size);
void    fileio_file_get_full_path(const char *path, char *path_buffer, size_t buffer_size);

#endif
