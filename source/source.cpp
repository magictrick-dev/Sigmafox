#include <source.h>

SourceFileBuffer::
SourceFileBuffer(char *source_buffer)
    : buffer(source_buffer)
{
    
    // Get the size of the buffer.
    this->size = 0;
    while (buffer[this->size++] != '\0');

    return;
}

