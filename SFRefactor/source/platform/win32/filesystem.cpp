#include <windows.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <platform/filesystem.hpp>

b32         
file_exists(ccptr file_path)
{

    DWORD dwAttrib = GetFileAttributesA(file_path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
        !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));

}

b32         
file_ready(ccptr file_path)
{

    HANDLE file_handle = CreateFileA(file_path, GENERIC_READ, FILE_SHARE_READ,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_handle == INVALID_HANDLE_VALUE)
        return false;
    else
    {
        CloseHandle(file_handle);
        return true;
    }

}

b32         
file_is_directory(ccptr file_path)
{

    DWORD dwAttrib = GetFileAttributesA(file_path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
        (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));

}

b32         
file_is_file(ccptr file_path)
{

    DWORD dwAttrib = GetFileAttributesA(file_path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
        !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));

}

u64          
file_size(ccptr file_path)
{

    HANDLE file_handle = CreateFileA(file_path, GENERIC_READ, FILE_SHARE_READ,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_handle == INVALID_HANDLE_VALUE)
        return 0;

    size_t file_size = GetFileSize(file_handle, NULL);
    CloseHandle(file_handle);

    return (size_t)file_size;

}

u64          
file_read_all(ccptr file_path, vptr buffer, u64 buffer_size)
{

    HANDLE file_handle = CreateFileA(file_path, GENERIC_READ, FILE_SHARE_READ,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_handle == INVALID_HANDLE_VALUE)
        return 0;

    size_t file_size = GetFileSize(file_handle, NULL);
    DWORD bytes_read;
    ReadFile(file_handle, buffer, file_size, &bytes_read, NULL);
    SF_ASSERT(bytes_read == file_size);
    CloseHandle(file_handle);

    return bytes_read;

}

u64          
file_write_all(ccptr file_path, vptr buffer, u64 buffer_size)
{

    HANDLE file_handle = CreateFileA(file_path, GENERIC_WRITE, FILE_SHARE_WRITE,
            NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_handle == INVALID_HANDLE_VALUE)
        return 0;

    DWORD bytes_written;
    WriteFile(file_handle, buffer, (DWORD)buffer_size, &bytes_written, NULL);

    SF_ASSERT((size_t)bytes_written == buffer_size);

    CloseHandle(file_handle);

    return bytes_written;

}

b32
file_create_directory(ccptr file_path)
{

    return CreateDirectoryA(file_path, NULL);

}

b32
file_remove_directory(ccptr file_path)
{

    char searchPath[MAX_PATH];
    _WIN32_FIND_DATAA findFileData = {0};
    HANDLE hFind;

    // Construct the search path
    snprintf(searchPath, MAX_PATH, "%s\\*", file_path);

    hFind = FindFirstFileA(searchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) 
    {
        printf("Error opening directory: %s\n", file_path);
        return false;
    }

    do 
    {

        // Skip "." and ".." directories
        if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0) 
            continue;

        char fullPath[MAX_PATH];
        snprintf(fullPath, MAX_PATH, "%s\\%s", file_path, findFileData.cFileName);

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
        {
            // Recursively delete subdirectory
            file_remove_directory(fullPath);
        } 
        else 
        {
            // Remove read-only attribute if needed
            SetFileAttributesA(fullPath, FILE_ATTRIBUTE_NORMAL);

            // Delete file
            if (!DeleteFileA(fullPath)) 
            {
                printf("Failed to delete file: %s\n", fullPath);
                return false;
            }
        }
    } 
    while (FindNextFileA(hFind, &findFileData));

    FindClose(hFind);

    // Remove the now-empty directory
    if (!RemoveDirectoryA(file_path)) 
    {
        printf("Failed to remove directory: %s\n", file_path);
        return false;
    }

    return true;

}

u64
file_runtime_directory(u32 buffer_size, cptr buffer)
{

    SF_ENSURE_PTR(buffer);
    SF_ASSERT(buffer_size != 0);

    DWORD read_size = GetModuleFileNameA(NULL, buffer, buffer_size);
    SF_ASSERT(read_size > 0);
    return (u32)read_size;

}

u64         
file_current_working_directory(u32 buffer_size, cptr buffer)
{

    SF_ENSURE_PTR(buffer);
    SF_ASSERT(buffer_size != 0);

    DWORD read_size = GetCurrentDirectoryA(buffer_size, buffer);
    SF_ASSERT(read_size > 0);
    return (u32)read_size;

}

void
file_canonicalize_path(u32 buffer_size, cptr dest, ccptr path)
{

    // NOTE(Chris): PathCanonicalizeA expects that our buffer size is
    //              greater than MAX_PATH constant set by Windows otherwise
    //              it will overrun and cause all sorts of issues. We enforce
    //              this constant here. (We also use this over their preferred
    //              function since the new API is absolute dogwater to work with.)
    SF_ENSURE_PTR(dest);
    SF_ENSURE_PTR(path);
    SF_ASSERT(buffer_size >= MAX_PATH);

    // Canonicalize doesn't respect '/', so we need to swap it to '\\' first and
    // store that in a temp-buffer.
    char temporary_buffer[MAX_PATH];
    u32 index = 0;
    while (path[index] != '\0')
    {
        if (path[index] == '/') temporary_buffer[index] = '\\';
        else temporary_buffer[index] = path[index];
        index++;
    }
    temporary_buffer[index] = '\0';

    // This will take the contents of temp, canonicalize it in destination.
    BOOL result = PathCanonicalizeA(dest, temporary_buffer);
    SF_ASSERT(result);

}

ccptr 
file_get_current_working_directory()
{

    // Cache the directory since it won't change.
    static char buffer[MAX_PATH];
    static bool initialized = false;
    if (!initialized)
    {
        file_current_working_directory(MAX_PATH, buffer);
        initialized = true;
    }
    return buffer;

}

ccptr
file_get_runtime_directory()
{

    // Cache the directory since it won't change.
    static char buffer[MAX_PATH];
    static bool initialized = false;
    if (!initialized)
    {
        file_runtime_directory(MAX_PATH, buffer);
        initialized = true;
    }
    return buffer;

}
