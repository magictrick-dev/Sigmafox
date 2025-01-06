#ifndef SIGMAFOX_COMPILER_SYMBOLS_HPP
#define SIGMAFOX_COMPILER_SYMBOLS_HPP
#include <definitions.hpp>
#include <iostream>
#include <string>

// --- Symbol Definition -------------------------------------------------------
//
// You can freely modify the symbol to your liking without having to modify the
// symbol table setup since it has been abstracted enough to automatically default
// to do as you need.
//

enum class Symboltype
{
    SYMBOL_TYPE_UNDEFINED,
    SYMBOL_TYPE_VARIABLE,
    SYMBOL_TYPE_ARRAY,
    SYMBOL_TYPE_PROCEDURE,
    SYMBOL_TYPE_FUNCTION,
    SYMBOL_TYPE_PARAMETER,
};

class Symbol
{
    
    public:
        inline              Symbol();
        inline              Symbol(const std::string& identifier, Symboltype type, i32 arity);
        inline virtual     ~Symbol();

    public:
        std::string     identifier;
        Symboltype      type;
        i32             arity;

};

Symbol::
Symbol()
{
    this->type = Symboltype::SYMBOL_TYPE_UNDEFINED;
    this->arity = 0;
    this->identifier = "UNSET SYMBOL IDENTIFIER NAME!";
}

Symbol::
Symbol(const std::string &identifier, Symboltype type, i32 arity)
{

    this->identifier = identifier;
    this->type = type;
    this->arity = arity;

}

Symbol::
~Symbol()
{

}

inline std::ostream& 
operator<<(std::ostream& os, const Symbol& rhs)
{
    os << rhs.identifier << " " << rhs.arity;
    return os;
}

// --- FNV1A32 Hash ------------------------------------------------------------
//
// A 32-bit hashing algorithm. It's not the fastest hashing algorithm in existence,
// but it is pretty fast for what we need.
//

struct FNV1A32Hash
{

    static inline u32 hash(const char* string)
    {

        u64 length = strlen(string);
        u32 hash = 0x811C9DC5; // offset basis

        for (u64 byte_index = 0; byte_index < length; ++byte_index)
        {

            hash ^= string[byte_index];
            hash *= 0x01000193; // prime

        }

        return hash;

    }

    inline u32 operator()(const char* string) const
    {
        return FNV1A32Hash::hash(string);
    }

};

// --- FNV1A64 Hash ------------------------------------------------------------
//
// A 64-bit version of the previous version. They are generally roughly the
// same in performance, the 64-bit version provides slightly better avalanche
// characteristics over the 32-bit version at the cost of a couple extra cycles
// to process the 64-bit operations.
//

struct FNV1A64Hash
{

    static inline u64 hash(const char* string)
    {

        u64 length = strlen(string);
        u64 hash = 0xCBF29CE484222325; // offset basis

        for (u64 byte_index = 0; byte_index < length; ++byte_index)
        {

            hash ^= string[byte_index];
            hash *= 0xCBF29CE484222325; // prime

        }

        return hash;

    }

    inline u64 operator()(const char* string) const
    {
        return FNV1A64Hash::hash(string);
    }

};

// --- MurmurHash64A Hash ------------------------------------------------------
//
// The sort of industry-standard all arounder that provides excellent avalanche
// characteristics with minimal hit to performance.
//

template <u64 S = 0xFFFF'FFFF'FFFF'FFFF>
struct Murmur64A 
{

    static inline u64 hash(const char* string)
    {

        u64 len = strlen(string);

        const u64 m = 0xC6A4A7935BD1E995;
        const i32 r = 47;

        u64 h = S ^ (len * m);

        const u64* data = (const u64*)string;
        const u64* end = data + (len/8);

        while(data != end)
        {

            u64 k = *data++;

            k *= m;
            k ^= k >> r;
            k *= m;

            h ^= k;
            h *= m;

        }

        const u8* data2 = (const u8*)data;

        switch(len & 7) {
            case 7: h ^= (uint64_t)((uint64_t)data2[6] << (uint64_t)48);
            case 6: h ^= (uint64_t)((uint64_t)data2[5] << (uint64_t)40);
            case 5: h ^= (uint64_t)((uint64_t)data2[4] << (uint64_t)32);
            case 4: h ^= (uint64_t)((uint64_t)data2[3] << (uint64_t)24);
            case 3: h ^= (uint64_t)((uint64_t)data2[2] << (uint64_t)16);
            case 2: h ^= (uint64_t)((uint64_t)data2[1] << (uint64_t)8 );
            case 1: h ^= (uint64_t)((uint64_t)data2[0]                );
            h *= m;
        };

        h ^= h >> r;
        h *= m;
        h ^= h >> r;

        return h;

    }

    inline u64 operator()(const char* string) const
    {
        return Murmur64A::hash(string);
    }

};

// --- Symboltable Entry -------------------------------------------------------
//
// Allows the user to decouple some of book keeping semantics of the hashmap
// hoopla from the actual implementation of what constitutes a "symbol". Generally,
// we only refer to a symbol as a collection of reference properties, so we won't
// need to get too deep into the weeds about it, but nonetheless helps with future
// extensibility without requiring the user to care too much about the implementation
// of the symboltable's hash map setup.
//

template <typename Symboltype = Symbol>
class SymboltableEntry
{

    public:
        inline              SymboltableEntry();
        inline virtual     ~SymboltableEntry();

        inline bool                 is_active()     const;
        inline u64                  get_hash()      const;
        inline std::string          get_key()       const;
        inline const Symboltype&    get_value()     const;
        inline Symboltype&          get_value();

        inline void         set(Symboltype symbol, std::string key, u64 hash);
        inline void         unset();

    protected:
        Symboltype  symbol;
        std::string key;
        u64         hash;
        bool        active;

};

template <typename Symboltype>
SymboltableEntry<Symboltype>::
SymboltableEntry()
{

    this->key       = "";
    this->hash      = (u64)-1;
    this->active    = false;

}

template <typename Symboltype>
SymboltableEntry<Symboltype>::
~SymboltableEntry()
{

}

template <typename Symboltype>
void SymboltableEntry<Symboltype>::
set(Symboltype symbol, std::string key, u64 hash)
{

    this->key = key;
    this->hash = hash;
    this->symbol = symbol;
    this->active = true;

}

template <typename Symboltype>
void SymboltableEntry<Symboltype>::
unset()
{

    this->key = "";
    this->hash = (u64)-1;
    this->active = false;

}

template <typename Symboltype>
bool SymboltableEntry<Symboltype>::
is_active() const
{

    return this->active;

}

template <typename Symboltype>
u64 SymboltableEntry<Symboltype>::
get_hash() const
{

    return this->hash;

}

template <typename Symboltype>
std::string SymboltableEntry<Symboltype>::
get_key() const
{

    return this->key;

}

template <typename Symboltype>
const Symboltype& SymboltableEntry<Symboltype>::
get_value() const
{

    return this->symbol;

}

template <typename Symboltype>
Symboltype& SymboltableEntry<Symboltype>::
get_value()
{

    return this->symbol;

}

// --- Symboltable -------------------------------------------------------------
//
// The symbol table is built to store symbols that are encountered in the parser.
// It uses a custom hash map algorithm to build the table and dynamically resizes
// whenever it reaches a given load capacity. The defaults are an FNV1A hashing
// schema and 75% load factor.
//

template <typename Symboltype = Symbol, typename Hashfunction = FNV1A32Hash, float LF = 0.75f>
class Symboltable
{

    public:
        inline              Symboltable();
        inline              Symboltable(u64 initialize_size);
        inline              Symboltable(const Symboltable& rhs);
        inline virtual     ~Symboltable();

        inline Symboltable<Symbol, Hashfunction, LF>& operator=(const Symboltable& rhs);

        inline Symboltype&          operator[](std::string key);
        inline const Symboltype&    operator[](std::string key) const;
        inline Symboltype&          get(std::string key);
        inline const Symboltype&    get(std::string key) const;

        inline u64          size() const;
        inline u64          commit() const;
        inline u64          overlaps() const;
        inline void         resize(u64 size);
        inline void         remove(const std::string &str);
        inline bool         contains(const std::string &str) const;
        inline void         merge_from(const Symboltable<Symboltype, Hashfunction, LF>& other);

        inline void                             insert(const std::string &str, const Symboltype& val);
        inline void                             emplace(const std::string &str, Symboltype&& val);
        template <class... Args> inline void    emplace(const std::string &str, Args... args);

    protected:
        inline void         release_buffer();
        inline u64          hash(const std::string& str) const;
        inline void         insert_and_copy_into(SymboltableEntry<Symboltype> *buffer,
                                u64 capacity, SymboltableEntry<Symboltype> reference);

    protected:
        Hashfunction hash_function;

        SymboltableEntry<Symboltype> *symbols_buffer;
        u64 capacity;
        u64 load;
        u64 misses;

};

template <typename Symboltype, typename Hashfunction, float LF>
Symboltable<Symboltype, Hashfunction, LF>::
Symboltable() : hash_function(Hashfunction()), symbols_buffer(nullptr), 
    capacity(0), load(0), misses(0)
{
    
    this->resize(8);

}

template <typename Symboltype, typename Hashfunction, float LF>
Symboltable<Symboltype, Hashfunction, LF>::
Symboltable(u64 initial_size) : hash_function(Hashfunction()), symbols_buffer(nullptr), 
    capacity(0), load(0), misses(0)
{

    // Round to the nearest power of 2, we could bit twiddle tho.
    u64 k = 1;
    while (k < initial_size) k *= 2;
    this->resize(k);

}

template <typename Symboltype, typename Hashfunction, float LF>
Symboltable<Symboltype, Hashfunction, LF>::
Symboltable(const Symboltable<Symboltype, Hashfunction, LF>& rhs) 
    : hash_function(Hashfunction()), symbols_buffer(nullptr), 
    capacity(0), load(0), misses(0)
{
    
    this->resize(rhs.size());
    this->merge_from(rhs);

}

template <typename Symboltype, typename Hashfunction, float LF>
Symboltable<Symboltype, Hashfunction, LF>::
~Symboltable()
{

    this->release_buffer();

}

template <typename Symboltype, typename Hashfunction, float LF>
Symboltable<Symbol, Hashfunction, LF>& Symboltable<Symboltype, Hashfunction, LF>::
operator=(const Symboltable& rhs)
{

    this->release_buffer();   
    this->resize(rhs.capacity);
    this->merge_from(rhs);
    return *this;

}

template <typename Symboltype, typename Hashfunction, float LF>
void Symboltable<Symboltype, Hashfunction, LF>::
resize(u64 size)
{

    SF_ASSERT(size > this->capacity);

    // Create the new map, placement new initialize each entry.
    SymboltableEntry<Symboltype> *new_symbol_table = (SymboltableEntry<Symboltype>*)
        SF_MEMORY_ALLOC(sizeof(SymboltableEntry<Symboltype>) * size);
    for (u64 i = 0; i < size; ++i) new (new_symbol_table + i) SymboltableEntry();

    // If we haven't set the buffer, set it and fast-exit.
    if (this->symbols_buffer == nullptr)
    {
        this->symbols_buffer = new_symbol_table;
        this->capacity = size;
        return;
    }

    // Now the fun part, copying the entries over.
    this->misses = 0;
    for (u64 i = 0; i < this->capacity; ++i)
    {
        if (this->symbols_buffer[i].is_active())
        {
            this->insert_and_copy_into(new_symbol_table, size, 
                    this->symbols_buffer[i]);
        }
    }

    // Yeet the old contents.
    SF_MEMORY_FREE(this->symbols_buffer);
    this->symbols_buffer    = new_symbol_table;
    this->capacity          = size;

}

template <typename Symboltype, typename Hashfunction, float LF>
size_t Symboltable<Symboltype, Hashfunction, LF>::
hash(const std::string& str) const
{

    size_t hash_result = this->hash_function(str.c_str());
    return hash_result;

}

template <typename Symboltype, typename Hashfunction, float LF>
u64 Symboltable<Symboltype, Hashfunction, LF>::
size() const
{

    return this->capacity;

}

template <typename Symboltype, typename Hashfunction, float LF>
u64 Symboltable<Symboltype, Hashfunction, LF>::
overlaps() const
{

    return this->misses;

}

template <typename Symboltype, typename Hashfunction, float LF>
u64 Symboltable<Symboltype, Hashfunction, LF>::
commit() const
{

    return this->load;

}

template <typename Symboltype, typename Hashfunction, float LF>
void Symboltable<Symboltype, Hashfunction, LF>::
insert(const std::string &str, const Symboltype& val)
{

    // Calculate the current load factor and resize if needed.
    r32 current_load = (r32)this->load / (r32)this->capacity;
    if (current_load >= std::min(LF, 1.0f)) 
        this->resize(this->capacity * 2);

    // Insert the new entry.
    u64 hash = this->hash(str);
    u64 offset = hash % this->capacity;

    while (this->symbols_buffer[offset].is_active())
    {

        if (this->symbols_buffer[offset].get_key() == str) break;
        offset = (offset + 1) % this->capacity;
        misses += 1;

    }

    this->load++;
    this->symbols_buffer[offset].set(val, str, hash);

}

template <typename Symboltype, typename Hashfunction, float LF>
void Symboltable<Symboltype, Hashfunction, LF>::
emplace(const std::string &str, Symboltype&& val)
{

    // Calculate the current load factor and resize if needed.
    r32 current_load = (r32)this->load / (r32)this->capacity;
    if (current_load >= std::min(LF, 1.0f)) 
        this->resize(this->capacity * 2);

    // Insert the new entry.
    u64 hash = this->hash(str);
    u64 offset = hash % this->capacity;

    while (this->symbols_buffer[offset].is_active())
    {

        if (this->symbols_buffer[offset].get_key() == str) break;
        offset = (offset + 1) % this->capacity;
        misses += 1;

    }

    this->load++;
    this->symbols_buffer[offset].set(val, str, hash);

}

template <typename Symboltype, typename Hashfunction, float LF> template <class... Args>
void Symboltable<Symboltype, Hashfunction, LF>::
emplace(const std::string &str, Args... args)
{

    // Calculate the current load factor and resize if needed.
    r32 current_load = (r32)this->load / (r32)this->capacity;
    if (current_load >= std::min(LF, 1.0f)) 
        this->resize(this->capacity * 2);

    // Insert the new entry.
    u64 hash = this->hash(str);
    u64 offset = hash % this->capacity;

    while (this->symbols_buffer[offset].is_active())
    {

        if (this->symbols_buffer[offset].get_key() == str) break;
        offset = (offset + 1) % this->capacity;
        misses += 1;

    }

    this->load++;
    this->symbols_buffer[offset].set(Symboltype(args...), str, hash);

}

template <typename Symboltype, typename Hashfunction, float LF>
void Symboltable<Symboltype, Hashfunction, LF>::
remove(const std::string &str)
{

    // Insert the new entry.
    u64 hash = this->hash(str);
    u64 offset = hash % this->capacity;

    while (this->symbols_buffer[offset].is_active())
    {

        if (this->symbols_buffer[offset].get_key() == str) break;
        offset = (offset + 1) % this->capacity;

    }

    this->load--;
    this->symbols_buffer[offset].unset();

}

template <typename Symboltype, typename Hashfunction, float LF>
bool Symboltable<Symboltype, Hashfunction, LF>::
contains(const std::string& str) const
{

    u64 hash = this->hash(str);
    u64 offset = hash % this->capacity;

    while (this->symbols_buffer[offset].is_active())
    {

        if (this->symbols_buffer[offset].get_key() == str) return true;
        offset = (offset + 1) % this->capacity;

    }

    return false;

}

template <typename Symboltype, typename Hashfunction, float LF>
void Symboltable<Symboltype, Hashfunction, LF>::
insert_and_copy_into(SymboltableEntry<Symboltype> *buffer, 
        u64 capacity, SymboltableEntry<Symboltype> reference)
{

    u64 hash = this->hash(reference.get_key());
    u64 offset = hash % capacity;

    while (buffer[offset].is_active())
    {

        if (buffer[offset].get_key() == reference.get_key()) break;
        offset = (offset + 1) % capacity;
        this->misses++;

    }

    buffer[offset].set(reference.get_value(), reference.get_key(), hash);

}

template <typename Symboltype, typename Hashfunction, float LF>
void Symboltable<Symboltype, Hashfunction, LF>::
merge_from(const Symboltable<Symboltype, Hashfunction, LF>& other)
{

    u64 other_index = 0;
    while (other_index < other.capacity)
    {

        SymboltableEntry<Symboltype> *current_entry = &other.symbols_buffer[other_index];
        if (current_entry->is_active())
            this->insert(current_entry->get_key(), current_entry->get_value());
        other_index++;

    }

}

template <typename Symboltype, typename Hashfunction, float LF>
Symboltype& Symboltable<Symboltype, Hashfunction, LF>::
get(std::string key)
{

    u64 hash = this->hash(key);
    u64 offset = hash % this->capacity;

    while (this->symbols_buffer[offset].is_active())
    {

        if (this->symbols_buffer[offset].get_key() == key)
        {
            return this->symbols_buffer[offset].get_value();
        }

        offset = (offset + 1) % this->capacity;

    }

    SF_ASSERT(!"Attempting to fetch a non-existent symbol.");
    return this->symbols_buffer[offset].get_value();

}

template <typename Symboltype, typename Hashfunction, float LF>
const Symboltype& Symboltable<Symboltype, Hashfunction, LF>::
get(std::string key) const
{

    u64 hash = this->hash(key);
    u64 offset = hash % this->capacity;

    while (this->symbols_buffer[offset].is_active())
    {

        if (this->symbols_buffer[offset].get_key() == key)
        {
            return this->symbols_buffer[offset].get_value();
        }

        offset = (offset + 1) % this->capacity;

    }

    SF_ASSERT(!"Attempting to fetch a non-existent symbol.");
    return this->symbols_buffer[offset].get_value();

}

template <typename Symboltype, typename Hashfunction, float LF>
Symboltype& Symboltable<Symboltype, Hashfunction, LF>::
operator[](std::string key)
{

    return this->get(key);

}

template <typename Symboltype, typename Hashfunction, float LF>
const Symboltype& Symboltable<Symboltype, Hashfunction, LF>::
operator[](std::string key) const
{

    return this->get(key);

}

template <typename Symboltype, typename Hashfunction, float LF>
void Symboltable<Symboltype, Hashfunction, LF>::
release_buffer()
{

    if (this->symbols_buffer != nullptr)
    {

        for (u64 i = 0; i < this->capacity; ++i)
            (this->symbols_buffer + i)->~SymboltableEntry();

        SF_MEMORY_FREE(this->symbols_buffer);
        this->symbols_buffer    = nullptr;
        this->capacity          = 0;
        this->load              = 0;
    }

}

#endif
