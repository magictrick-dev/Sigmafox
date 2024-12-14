#ifndef SIGMAFOX_COMPILER_SYMBOLS_HPP
#define SIGMAFOX_COMPILER_SYMBOLS_HPP
#include <definitions.hpp>
#include <string>

enum class SymType
{
    SYMBOL_TYPE_UNDEFINED,
    SYMBOL_TYPE_VARIABLE,
    SYMBOL_TYPE_ARRAY,
    SYMBOL_TYPE_PROCEDURE,
    SYMBOL_TYPE_FUNCTION,
};

struct Symbol
{
    std::string     identifier;
    SymType         type;
    i32             arity;
};

// --- FNV1A32 Hash ------------------------------------------------------------
//
// A 32-bit hashing algorithm. It's not the fastest hashing algorithm in existence,
// but it is pretty fast for our purposes.
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

template <typename Symboltype = Symbol>
SymboltableEntry<Symboltype>::
SymboltableEntry()
{

    this->key       = "";
    this->hash      = (u64)-1;
    this->active    = false;

}

template <typename Symboltype = Symbol>
SymboltableEntry<Symboltype>::
~SymboltableEntry()
{

}

template <typename Symboltype = Symbol>
void SymboltableEntry<Symboltype>::
set(Symboltype symbol, std::string key, u64 hash)
{

    this->key = key;
    this->hash = hash;
    this->active = true;

}

template <typename Symboltype = Symbol>
void SymboltableEntry<Symboltype>::
unset()
{

    this->key = "";
    this->hash = (u64)-1;
    this->active = false;

}

template <typename Symboltype = Symbol>
bool SymboltableEntry<Symboltype>::
is_active() const
{

    return this->active;

}

template <typename Symboltype = Symbol>
u64 SymboltableEntry<Symboltype>::
get_hash() const
{

    return this->hash;

}

template <typename Symboltype = Symbol>
std::string SymboltableEntry<Symboltype>::
get_key() const
{

    return this->key;

}

template <typename Symboltype = Symbol>
const Symboltype& SymboltableEntry<Symboltype>::
get_value() const
{

    return this->symbol;

}

template <typename Symboltype = Symbol>
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
        inline virtual     ~Symboltable();

        inline void         resize(u64 size);
        inline void         insert(const std::string &str, const Symboltype& val);
        inline bool         contains(const std::string &str) const;

    protected:
        inline u64          hash(const std::string& str) const;
        inline void         insert_and_copy_into(SymboltableEntry<Symboltype> *buffer,
                                u64 capacity, SymboltableEntry<Symboltype> reference);

    protected:
        Hashfunction hash_function;

        SymboltableEntry<Symboltype> *symbols_buffer;
        u64 capacity;
        u64 load;

};

template <typename Symboltype, typename Hashfunction, float LF>
Symboltable<Symboltype, Hashfunction, LF>::
Symboltable() : hash_function(Hashfunction()), symbols_buffer(nullptr), capacity(0), load(0)
{
    
    this->resize(8);

}

template <typename Symboltype, typename Hashfunction, float LF>
Symboltable<Symboltype, Hashfunction, LF>::
~Symboltable()
{

    if (this->symbols_buffer != nullptr)
    {
        SF_MEMORY_FREE(this->symbols_buffer);
        this->symbols_buffer    = nullptr;
        this->capacity          = 0;
        this->load              = 0;
    }

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
void Symboltable<Symboltype, Hashfunction, LF>::
insert(const std::string &str, const Symboltype& val)
{

    u64 hash = this->hash(str);
    u64 offset = hash % this->capacity;

    while (this->symbols_buffer[offset].is_active())
    {

        if (this->symbols_buffer[offset].get_key() == str) break;
        offset = (offset + 1) % this->capacity;

    }

    this->symbols_buffer[offset].set(val, str, hash);

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

    }

    buffer[offset].set(reference.get_value(), reference.get_key(), hash);

}

#endif
