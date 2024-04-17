#include <core/fileio.h>
#include <core/debugging.h>

// --- Internal Implementation Definitions -------------------------------------
//
// The following definitions are used internally are called by the API. This is
// done so that operating system specific definitions can be written.
//

SF_INTERNAL bool    impl_file_exists(const char *path);
SF_INTERNAL size_t  impl_file_size(const char *path);
SF_INTERNAL bool    impl_file_read(const char *path, void *buffer, size_t rsize, size_t bsize);
SF_INTERNAL bool    impl_file_read_text(const char *path, void *buffer, size_t rsize, size_t bsize);
SF_INTERNAL bool    impl_file_write(const char *path, void *buffer, size_t wsize);
SF_INTERNAL void    impl_file_get_full_path(const char *path, char *path_buffer, size_t buffer_size);

bool                    
sigmafox_file_exists(const char *path)
{
    return impl_file_exists(path);
}

size_t
sigmafox_file_size(const char *path)
{
    return impl_file_size(path);
}

SF_MAY_ASSERT bool      
sigmafox_file_read(const char *path, void *buffer, size_t read_size, size_t buffer_size)
{
    return impl_file_read(path, buffer, read_size, buffer_size);
}

SF_MAY_ASSERT bool      
sigmafox_file_read_text(const char *path, void *buffer, size_t read_size, size_t buffer_size)
{
    return impl_file_read_text(path, buffer, read_size, buffer_size);
}

SF_MAY_ASSERT bool      
sigmafox_file_write(const char *path, void *buffer, size_t write_size)
{
    return impl_file_write(path, buffer, write_size);
}

SF_MAY_ASSERT void
sigmafox_file_get_full_path(const char *path, char *path_buffer, size_t buffer_size)
{
    impl_file_get_full_path(path, path_buffer, buffer_size);
}

// --- Win32 Definitions -------------------------------------------------------
//
// Below are the Windows definitions of the file IO operations. Most of the
// implementations are reliant on the Win32 runtime and thus require no dependency
// to the standard library.
//

#if defined(_WIN32)
#include <windows.h>

SF_INTERNAL bool
impl_file_exists(const char *path)
{

    DWORD file_attr = GetFileAttributesA(path);
    return (file_attr != INVALID_FILE_ATTRIBUTES && 
            !(file_attr & FILE_ATTRIBUTE_DIRECTORY));

}

SF_INTERNAL size_t
impl_file_size(const char *path)
{

    // Attempt to open the file.
    HANDLE file_handle = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (file_handle == INVALID_HANDLE_VALUE)
        return 0;

    // Get the file size.
    LARGE_INTEGER lifsize = {};
    GetFileSizeEx(file_handle, &lifsize);
    size_t file_size = (size_t)lifsize.QuadPart;

    CloseHandle(file_handle);
    return file_size;

}

SF_INTERNAL bool
impl_file_read(const char *path, void *buffer, size_t rsize, size_t bsize)
{

    SF_ASSERT(rsize <= bsize);

    // Attempt to open the file.
    HANDLE file_handle = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (file_handle == INVALID_HANDLE_VALUE)
        return false;
    
    // Now read the bytes into the buffer.
    DWORD bytes_read = 0;
    BOOL read_status = ReadFile(file_handle, buffer, (DWORD)rsize, &bytes_read, NULL);

    // TODO(Chris): The number of bytes read should match rsize; Windows will
    //              probably perform the read to our requested size, but we should
    //              throw this in a loop until we reach our request size. For now,
    //              through an assert in case this ever does happen.
    SF_ASSERT(rsize == bytes_read);
    CloseHandle(file_handle);

    return true;

}

SF_INTERNAL bool
impl_file_read_text(const char *path, void *buffer, size_t rsize, size_t bsize)
{

    // NOTE(Chris): Perhaps not implicit here, but the buffer needs to be at least
    //              one byte larger than the read size to accomodate the null-term.
    SF_ASSERT(bsize >= rsize + 1);

    // Perform a standard read as normal then chuck in the null-term.
    if (impl_file_read(path, buffer, rsize, bsize))
    {
        ((char*)buffer)[rsize] = '\0';
        return true;
    }
    else
    {
        return false;
    }


}

SF_INTERNAL bool
impl_file_write(const char *path, void *buffer, size_t wsize)
{

    HANDLE file_handle = CreateFileA(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,
            NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_handle == INVALID_HANDLE_VALUE)
        return false;

    DWORD bytes_written = 0;
    BOOL write_status = WriteFile(file_handle, buffer, (DWORD)wsize, &bytes_written, NULL);

    // TODO(Chris): The number of bytes written should match wsize; Windows will
    //              probable perform the write to our requested size, but we should
    //              throw this in a loop until we reach our request size.
    SF_ASSERT(wsize == bytes_written);
    CloseHandle(file_handle);

    return true;

}

SF_INTERNAL void
impl_file_get_full_path(const char *path, char *path_buffer, size_t buffer_size)
{

    DWORD path_length = GetFullPathNameA(path, buffer_size, path_buffer, NULL);
    SF_ASSERT(path_length != 0);
    path_buffer[path_length] = '\0';
    return;

}

#endif

