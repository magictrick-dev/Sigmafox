#include <utilities/path.h>
#include <platform/filesystem.h>

using namespace Sigmafox;

Filepath::
Filepath()
{
    this->zero_initialize();
}

Filepath::
Filepath(u64 initial_size)
{

    this->zero_initialize();
    if (initial_size > 0)
    {
        u64 required_size = this->find_best_fit(initial_size);
        this->buffer_ptr        = (char*)SF_MEMORY_ALLOC(required_size);
        this->buffer_capacity   = required_size;
        this->buffer_length     = 0;
        this->buffer_ptr[0]     = '\0';
    }

}

Filepath::
Filepath(ccptr path)
{

    this->zero_initialize();

    u64 string_length = strlen(path);
    u64 required_size = this->find_best_fit(string_length + 1);
    this->buffer_ptr        = (char*)SF_MEMORY_ALLOC(required_size);
    this->buffer_capacity   = required_size;
    this->buffer_length     = string_length;
    strcpy(this->buffer_ptr, path);

}

Filepath::
Filepath(const std::string& path)
{

    this->zero_initialize();

    u64 string_length = path.length();
    u64 required_size = this->find_best_fit(string_length + 1);
    this->buffer_ptr        = (char*)SF_MEMORY_ALLOC(required_size);
    this->buffer_capacity   = required_size;
    this->buffer_length     = string_length;
    strcpy(this->buffer_ptr, path.c_str());

}

Filepath::
Filepath(const Filepath& other)
{

    this->zero_initialize();
    this->buffer_ptr = (char*)SF_MEMORY_ALLOC(other.buffer_capacity);
    this->buffer_length = other.buffer_length;
    this->buffer_capacity = other.buffer_capacity;
    strcpy(this->buffer_ptr, other.buffer_ptr);

}

Filepath& Filepath::
operator=(const Filepath& rhs)
{
    
    this->release();
    this->zero_initialize();
    this->buffer_ptr = (char*)SF_MEMORY_ALLOC(rhs.buffer_capacity);
    this->buffer_length = rhs.buffer_length;
    this->buffer_capacity = rhs.buffer_capacity;
    strcpy(this->buffer_ptr, rhs.buffer_ptr);
    return *this;

}

Filepath& Filepath::
operator+=(const Filepath& rhs)
{
    
    u64 required_size = rhs.buffer_length + this->buffer_length;
    if (required_size >= this->buffer_capacity - 1)
        this->resize(required_size + 1);
    strcpy(this->buffer_ptr + this->buffer_length, rhs.buffer_ptr);
    this->buffer_length = strlen(this->buffer_ptr);
    return *this;

}

Filepath& Filepath::
operator+=(ccptr rhs)
{

    u64 right_length = strlen(rhs);
    u64 required_size = right_length + this->buffer_length;
    if (required_size >= this->buffer_capacity - 1)
        this->resize(required_size + 1);
    strcpy(this->buffer_ptr + this->buffer_length, rhs);
    this->buffer_length = strlen(this->buffer_ptr);
    return *this;

}

Filepath& Filepath::
operator+=(const std::string& rhs)
{
    
    u64 right_length = rhs.length();
    u64 required_size = right_length + this->buffer_length;
    if (required_size >= this->buffer_capacity - 1)
        this->resize(required_size + 1);
    strcpy(this->buffer_ptr + this->buffer_length, rhs.c_str());
    this->buffer_length = strlen(this->buffer_ptr);
    return *this;

}

bool Filepath::
operator==(const Filepath& rhs) const
{

    bool result = strcmp(this->buffer_ptr, rhs.buffer_ptr) == 0;
    return result;

}

Filepath::
~Filepath()
{
    this->release();
}

void Filepath::
zero_initialize()
{
    this->buffer_ptr = nullptr;
    this->buffer_length = 0;
    this->buffer_capacity = 0;
}

void Filepath::
release()
{
    if (this->buffer_ptr != nullptr)
        SF_MEMORY_FREE(this->buffer_ptr);
}

void Filepath::
resize(u64 request_size)
{
    
    u64 best_fit = this->find_best_fit(request_size);
    if (best_fit <= this->buffer_capacity) return;

    char* new_buffer = (char*)SF_MEMORY_ALLOC(best_fit);
    strcpy(new_buffer, this->buffer_ptr);
    SF_MEMORY_FREE(this->buffer_ptr);
    this->buffer_ptr = new_buffer;
    this->buffer_capacity = best_fit;
    return;

}

u64 Filepath::
find_best_fit(u64 request_size) const
{

    u64 result = ((request_size / 32) + (request_size % 32 != 0)) * 32;
    return result;

}

u64 Filepath::
size() const
{
    return this->buffer_length;
}

u64 Filepath::
capacity() const
{
    return this->buffer_capacity;
}

ccptr Filepath::
c_str() const
{
    return this->buffer_ptr;
}

Filepath& Filepath::
canonicalize()
{

    char buffer[260];
    file_canonicalize_path(260, buffer, this->c_str());
        
    this->release();
    this->zero_initialize();
    u64 string_length = strlen(buffer);
    u64 required_size = this->find_best_fit(string_length + 1);
    this->buffer_ptr        = (char*)SF_MEMORY_ALLOC(required_size);
    this->buffer_capacity   = required_size;
    this->buffer_length     = string_length;
    strcpy(this->buffer_ptr, buffer);

    return *this;

}

bool Filepath::
is_valid_directory() const
{

    bool result = (file_is_directory(this->c_str()));
    return result;

}

bool Filepath::
is_valid_file() const
{

    bool result = (file_is_file(this->c_str()));
    return result;

}

bool Filepath::
is_valid() const
{

    bool result = (this->is_valid_directory() || this->is_valid_file());
    return result;

}

Filepath Filepath::
cwd()
{
    
    char buffer[260];
    u32 size = file_current_working_directory(260, buffer);
    Filepath path(buffer);
    return path;

}

Filepath Filepath::
root_directory() const
{

    std::string path = this->c_str();
    u64 index = path.length() - 1;
    while (path[index--] != '\\') path.pop_back();
    return Filepath(path);

}
