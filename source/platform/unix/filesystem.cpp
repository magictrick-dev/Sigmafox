#include <platform/filesystem.hpp>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
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

b32
file_create_directory(ccptr file_path)
{

    // Create a directory.
    if (mkdir(file_path, 0777) == -1)
    {
        return false;
    }

    return true;

}

b32
file_remove_directory(ccptr file_path)
{

    struct dirent *entry;
    struct stat statbuf;
    char full_path[1024];
    
    DIR *dir = opendir(path);
    if (!dir) 
    {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) 
    {
        // Skip "." and ".." directories
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        if (lstat(full_path, &statbuf) == -1) 
        {
            perror("lstat");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode))
        {
            // i love recursion haha
            remove_directory(full_path);
        } 
        else 
        {
            if (unlink(full_path) == -1) 
            {
                perror("unlink");
            }
        }
    }

    closedir(dir);

    // Remove the empty directory
    if (rmdir(path) == -1) 
    {
        perror("rmdir");
    }

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

ccptr
file_get_current_working_directory()
{

    // Get the current working directory.
    static char buffer[1024];
    static bool initialized = false;
    if (!initialized)
    {
        file_current_working_directory(1024, buffer);
        initialized = true;
    }

    return buffer;

}

ccptr
file_get_runtime_directory()
{

    // Get the runtime directory.
    static char buffer[1024];
    static bool initialized = false;
    if (!initialized)
    {
        file_runtime_directory(1024, buffer);
        initialized = true;
    }

    return buffer;

}