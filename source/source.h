#ifndef SIGMAFOX_SOURCE_H
#define SIGMAFOX_SOURCE_H
#include <core.h>

class SourceFileBuffer
{
    public:
        SourceFileBuffer(char *source_buffer);

    protected:
        size_t  size;
        char   *buffer;
};



#endif
