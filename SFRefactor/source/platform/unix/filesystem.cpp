#include <platform/filesystem.hpp>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <stdlib.h>

b32         
file_exists(ccptr file_path)
{

    // Check if the file exists.
    struct stat file_info;
    if (stat(file_path, &file_info) == -1)
    {
        return false;
    }

    return true;

}

b32         
file_ready(ccptr file_path)
{

    // Check if the file is ready for reading.
    int file = open(file_path, O_RDONLY);
    if (file == -1)
    {
        return false;
    }

    close(file);
    return true;

}

b32         
file_is_directory(ccptr file_path)
{

    // Check if the file is a directory.
    struct stat file_info;
    if (stat(file_path, &file_info) == -1)
    {
        return false;
    }

    if (S_ISDIR(file_info.st_mode))
    {
        return true;
    }

    return false;

}

b32         
file_is_file(ccptr file_path)
{

    // Check if the file is a file. 
    struct stat file_info;
    if (stat(file_path, &file_info) == -1)
    {
        return false;
    }

    if (S_ISREG(file_info.st_mode))
    {
        return true;
    }

    return false;

}

u64         
file_size(ccptr file_path)
{

    // Get the size of the file.
    struct stat file_info;
    if (stat(file_path, &file_info) == -1)
    {
        return 0;
    }

    return file_info.st_size;

}

u64         
file_read_all(ccptr file_path, vptr buffer, u64 buffer_size)
{

    // Read the entire file into the buffer.
    int file = open(file_path, O_RDONLY);
    if (file == -1)
    {
        return 0;
    }

    u64 size = file_size(file_path);
    SF_ASSERT(size <= buffer_size);

    u64 bytes_read = read(file, buffer, size);
    close(file);

    SF_ASSERT(bytes_read == size);

    return bytes_read;

}

u64         
file_write_all(ccptr file_path, vptr buffer, u64 buffer_size)
{

    // Write the entire buffer to the file.
    int file = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (file == -1)
    {
        return 0;
    }

    u64 bytes_written = write(file, buffer, buffer_size);
    close(file);

    return bytes_written;

}

u64         
file_current_working_directory(u32 buffer_size, cptr buffer)
{

    // Get the current working directory.
    char* cwd = getcwd(buffer, buffer_size);
    if (cwd == nullptr)
    {
        return 0;
    }

    return strlen(cwd);

}

void        
file_canonicalize_path(u32 buffer_size, cptr dest, ccptr path)
{

    // Canonicalize the path.
    char* canonical_path = realpath(path, dest);
    if (canonical_path == nullptr)
    {
        return;
    }

    SF_ASSERT(strlen(canonical_path) < buffer_size);

}
