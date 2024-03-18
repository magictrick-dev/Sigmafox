#ifndef POSY_SOURCE_H
#define POSY_SOURCE_H
#include <core.h>

class source_file
{

    public:
                source_file();
                source_file(char *buffer);

        void    set_buffer(char *buffer);
    
    public:
        static char*    load_source(const char *file_path);
        static void     free_source(char *buffer);

    protected:
        char   *buffer_ptr;
        u64     buffer_size;

};

#endif
