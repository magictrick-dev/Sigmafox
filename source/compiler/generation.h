#ifndef SOURCE_COMPILER_GENERATION_H
#define SOURCE_COMPILER_GENERATION_H

typedef struct source_layout
{
    char *source_buffer;   
    uint64_t size;
    uint64_t offset;
} file_layout;

void source_layout_write(source_layout *layout, char *contents);
void source_layout_merge(source_layout *from, source_layout *to);

#endif
