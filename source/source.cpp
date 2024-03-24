#include <source.h>

SourceFileBuffer::
SourceFileBuffer(char *source_buffer)
    : buffer(source_buffer)
{
    return;
}

#if 0
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <source.h>

bool
source_file_open(int *descriptor, const char *file_path)
{
    
    // Attempt to open the file.
    int fd = open(file_path, O_RDONLY);
    if (fd == -1)
        return false;

    // Sets the descriptor and returns true.
    *descriptor = fd;
    return true;
}

void
source_file_close(int descriptor)
{
    close(descriptor);
    return;
}

char* source_file::
load_source(const char *file_path)
{
    
    // Attempt to open the source file. This should always succeed since
    // the CLI parser step will check if the file exists first, but the
    // assertion is in place to catch the edge-case.
    int fd = -1;
    bool open_status = source_file_open(&fd, file_path);
    POSY_ASSERT(open_status);
    if (open_status == false)
        return NULL;

    // Get the file size.
    struct stat file_stats = {};
    fstat(fd, &file_stats);
    u64 file_size = file_stats.st_size;

    char *buffer = (char*)smalloc(file_size + 1); // +1 for null-terminator.
    read(fd, buffer, file_size);
    buffer[file_size] = '\0';

    // Close the source file.
    source_file_close(fd);

    // Return the buffer.
    return buffer;

}

void source_file::
free_source(char *buffer)
{
    smalloc_free(buffer);
    return;
}

source_file::
source_file()
{
    return;
}

source_file::
source_file(char *buffer)
{
    this->set_buffer(buffer);
    return;
}

void source_file::
set_buffer(char *buffer)
{

    // Set the buffer pointer.
    this->buffer_ptr = buffer;

    // Determine the length of the buffer.
    u64 length = 0;
    while (buffer[length] != '\0')
        length++;
    this->buffer_size = length;

    return;

}
#endif
