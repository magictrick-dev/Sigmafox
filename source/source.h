#ifndef SIGMAFOX_SOURCE_H
#define SIGMAFOX_SOURCE_H
#include <core.h>

class SourceFileBuffer
{
    public:
        SourceFileBuffer(char *source_buffer);

    protected:
        char * buffer;
};

#endif
