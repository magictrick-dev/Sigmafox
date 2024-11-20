#include <windows.h>
#include <shlwapi.h>
#include <platform/filesystem.hpp>

b32         
file_exists(ccptr file_path)
{

    DWORD dwAttrib = GetFileAttributes(file_path);
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

    DWORD dwAttrib = GetFileAttributes(file_path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
        (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));

}

b32         
file_is_file(ccptr file_path)
{

    DWORD dwAttrib = GetFileAttributes(file_path);
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
file_copy_all(ccptr source, ccptr destination)
{

    SF_NO_IMPL("Copying is a rare procedure, implement when needed.");
    return 0;

}

u64         
file_last_write_time(ccptr file_path)
{

    HANDLE file_handle = CreateFileA(file_path, GENERIC_READ, FILE_SHARE_READ,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_handle == INVALID_HANDLE_VALUE)
        return 0;

    FILETIME last_write = {0};
    GetFileTime(file_handle, NULL, NULL, &last_write);

    LARGE_INTEGER quad_cast = {0};
    quad_cast.LowPart = last_write.dwLowDateTime;
    quad_cast.HighPart = last_write.dwHighDateTime;

    CloseHandle(file_handle);

    return ((u64)quad_cast.QuadPart);

}

vptr          
file_stream_handle_create(ccptr file_path, u32 open_context)
{

    // TODO(Chris): Set varying platform flags that controls the open context.
    //              For now, we always open exists.
    DWORD open_type = OPEN_EXISTING;

    HANDLE file_handle = CreateFileA(file_path, GENERIC_READ, FILE_SHARE_READ,
            NULL, open_type, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_handle == INVALID_HANDLE_VALUE) return NULL;

    return file_handle;

}

void        
file_stream_handle_close(vptr handle)
{

    if (handle != NULL) CloseHandle((HANDLE)handle);

}

void        
file_stream_reset_offset(vptr handle)
{

    SF_ASSERT(handle != NULL);

    LARGE_INTEGER input = {0};
    input.QuadPart = 0;

    BOOL move_ret = SetFilePointerEx((HANDLE)handle, input, NULL, FILE_BEGIN);
    SF_ASSERT(move_ret != 0);


}

void        
file_stream_set_offset_abs(vptr handle, u64 relative_offset)
{

    SF_ASSERT(handle != NULL);

    LARGE_INTEGER input = {0};
    input.QuadPart = relative_offset;
    BOOL move_ret = SetFilePointerEx((HANDLE)handle, input, NULL, FILE_BEGIN);
    SF_ASSERT(move_ret != 0);

}

void        
file_stream_set_offset_rel(vptr handle, u64 absolute_offset)
{

    SF_ASSERT(handle != NULL);

    LARGE_INTEGER input = {0};
    input.QuadPart = absolute_offset;
    BOOL move_ret = SetFilePointerEx((HANDLE)handle, input, NULL, FILE_CURRENT);
    SF_ASSERT(move_ret != 0);

}

u64          
file_stream_get_offset(vptr handle)
{

    SF_ASSERT(handle != NULL);

    LARGE_INTEGER input = {0};
    input.QuadPart = 0;

    LARGE_INTEGER output = {0};

    BOOL move_ret = SetFilePointerEx((HANDLE)handle, input, &output, FILE_CURRENT);
    SF_ASSERT(move_ret != 0);

    return (size_t)output.QuadPart;

}

u64          
file_stream_read(vptr handle, u64 read_size, vptr dest, u64 dest_size)
{

    SF_ASSERT(handle != NULL);
    SF_ASSERT(dest_size >= read_size);

    DWORD bytes_read;
    ReadFile((HANDLE)handle, dest, read_size, &bytes_read, NULL);
    return (size_t)bytes_read;

}

b32         
file_stream_is_eof(vptr handle)
{

    SF_ASSERT(handle != NULL);

    size_t current_offset = file_stream_get_offset(handle);
    
    LARGE_INTEGER input = {0};
    input.QuadPart = 0;

    LARGE_INTEGER output = {0};

    BOOL move_ret = SetFilePointerEx((HANDLE)handle, input, &output, FILE_END);
    SF_ASSERT(move_ret != 0);

    file_stream_set_offset_abs(handle, current_offset);

    if (current_offset < (size_t)output.QuadPart) return false;
    return true;

}

u64         
file_current_working_directory(u32 buffer_size, cptr buffer)
{

    SF_ENSURE_PTR(buffer);
    SF_ASSERT(buffer_size != 0);

    DWORD read_size = GetCurrentDirectory(buffer_size, buffer);
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
