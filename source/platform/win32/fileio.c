#include <platform/fileio.h>
#include <windows.h>

b32
fileio_file_exists(cc64 path)
{

    DWORD file_attr = GetFileAttributesA(path);
    return (file_attr != INVALID_FILE_ATTRIBUTES && 
            !(file_attr & FILE_ATTRIBUTE_DIRECTORY));

}

u64 
fileio_file_size(cc64 path)
{

    // Attempt to open the file.
    HANDLE file_handle = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (file_handle == INVALID_HANDLE_VALUE)
        return 0;

    // Get the file size.
    LARGE_INTEGER lifsize = {0};
    GetFileSizeEx(file_handle, &lifsize);
    u64 file_size = (u64)lifsize.QuadPart;

    CloseHandle(file_handle);
    return file_size;

}

b32
fileio_file_read(cc64 path, void *buffer, u64 rsize, u64 bsize)
{

    assert(rsize <= bsize);

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
    assert(rsize == bytes_read);
    CloseHandle(file_handle);

    return true;

}

b32
fileio_file_write(cc64 path, void *buffer, u64 wsize)
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
    assert(wsize == bytes_written);
    CloseHandle(file_handle);

    return true;

}

void
fileio_file_get_full_path(cc64 path, c64 path_buffer, u64 buffer_size)
{

    DWORD path_length = GetFullPathNameA(path, buffer_size, path_buffer, NULL);
    assert(path_length != 0);
    path_buffer[path_length] = '\0';
    return;

}

