#ifndef SIGMAFOX_COMPILER_SYMBOLSTACK_HPP
#define SIGMAFOX_COMPILER_SYMBOLSTACK_HPP
#include <vector>
#include <definitions.hpp>
#include <compiler/symbols.hpp>

template <typename Symboltype>
class SymboltableStack
{

    public:
        inline          SymboltableStack();
        inline virtual ~SymboltableStack();

        inline void     push_table();
        inline void     pop_table();

        inline bool     identifier_exists(const std::string& str) const;
        inline bool     identifier_exists_locally(const std::string &str) const;
        inline bool     identifier_exists_globally(const std::string &str) const;
        inline bool     identifier_exists_above(const std::string &str) const;

        template <class... Args> inline void insert_symbol_locally(const std::string& str, Args... args);
        template <class... Args> inline void insert_symbol_globally(const std::string& str, Args... args);
        inline void insert_symbol_locally(const std::string& str, Symboltype&& symbol);
        inline void insert_symbol_globally(const std::string& str, Symboltype&& symbol);

        Symboltype*             get_symbol(const std::string& str);
        Symboltype*             get_symbol_locally(const std::string &str);
        Symboltype*             get_symbol_globally(const std::string &str);
        
        Symboltable<Symboltype>&    get_table();
        Symboltable<Symboltype>&    get_root_table();
        bool                        is_root_table() const;
        
    protected:
        inline static Symboltable<Symboltype> global_table = Symboltable<Symboltype>();
        std::vector<Symboltable<Symboltype>> table_stack;

};

template <typename Symboltype>
Symboltable<Symboltype>& SymboltableStack<Symboltype>::
get_table()
{ 
    return this->table_stack[this->table_stack.size() - 1];
}

template <typename Symboltype>
Symboltable<Symboltype>& SymboltableStack<Symboltype>::
get_root_table() 
{ 

    return this->global_table;

}

template <typename Symboltype>
bool SymboltableStack<Symboltype>::
is_root_table() const 
{ 
    return this->table_stack.size() == 0;
}

template <typename Symboltype>
SymboltableStack<Symboltype>::
SymboltableStack()
{

}

template <typename Symboltype>
SymboltableStack<Symboltype>::
~SymboltableStack()
{

}

template <typename Symboltype>
void SymboltableStack<Symboltype>::
push_table()
{

    table_stack.emplace_back();

}

template <typename Symboltype>
void SymboltableStack<Symboltype>::
pop_table()
{

    SF_ASSERT(this->table_stack.size() >= 1); // Bottom-most table is root.
    this->table_stack.pop_back();

}

template <typename Symboltype>
bool SymboltableStack<Symboltype>::
identifier_exists(const std::string& str) const
{

    // Check local table.
    for (i32 i = this->table_stack.size() - 1; i >= 0; --i)
    {

        const Symboltable<Symboltype>& current_table = this->table_stack[i];
        if (current_table.contains(str))
            return true;

    }

    // Check global table.
    if (this->table_stack.size() == 1 && this->global_table.contains(str))
        return true;

    return false;
}

template <typename Symboltype>
bool SymboltableStack<Symboltype>::
identifier_exists_locally(const std::string& str) const
{

    if (this->table_stack.size() == 0)
    {
        // Check global table.
        if (this->global_table.contains(str))
            return true;
    }

    else 
    {

        if (this->table_stack[this->table_stack.size() - 1].contains(str))
            return true;

    }

    return false;

}

template <typename Symboltype>
bool SymboltableStack<Symboltype>::
identifier_exists_globally(const std::string& str) const
{

    // Check global table.
    if (this->global_table.contains(str))
        return true;

    return false;

}

template <typename Symboltype>
bool SymboltableStack<Symboltype>::
identifier_exists_above(const std::string& str) const
{

    for (i32 i = this->table_stack.size() - 2; i >= 0; --i)
    {

        const Symboltable<Symboltype>& current_table = this->table_stack[i];
        if (current_table.contains(str))
            return true;

    }

    // Check the dependency table.
    if (this->table_stack.size() > 0 && this->global_table.contains(str))
        return true;

    return false;

}

template <typename Symboltype> template <class... Args>
void SymboltableStack<Symboltype>::
insert_symbol_locally(const std::string& str, Args... args)
{
    
    SF_ASSERT(!this->identifier_exists_locally(str));

    if (this->table_stack.size() == 0)
    {
        this->global_table.emplace(str, args...);
        return;
    }

    Symboltable<Symboltype> &table = this->table_stack[this->table_stack.size() - 1];
    table.emplace(str, args...);

}

template <typename Symboltype> template <class... Args>
void SymboltableStack<Symboltype>::
insert_symbol_globally(const std::string& str, Args... args)
{

    SF_ASSERT(!this->identifier_exists_globally(str));
    this->global_table.emplace(str, args...);

}

template <typename Symboltype>
void SymboltableStack<Symboltype>::
insert_symbol_locally(const std::string& str, Symboltype&& symbol)
{

    SF_ASSERT(!this->identifier_exists_locally(str));

    if (this->table_stack.size() == 0)
    {
        this->global_table.emplace(str, symbol);
        return;
    }

    Symboltable<Symboltype> &table = this->table_stack[this->table_stack.size() - 1];
    table.emplace(str, symbol);

}

template <typename Symboltype>
void SymboltableStack<Symboltype>::
insert_symbol_globally(const std::string& str, Symboltype&& symbol)
{

    SF_ASSERT(!this->identifier_exists_globally(str));
    this->global_table.emplace(str, symbol);
    return;

}

template <typename Symboltype>
Symboltype* SymboltableStack<Symboltype>::
get_symbol(const std::string& str)
{

    // Essentially, we're fetcing from the top-most table to the bottom-most table.
    // This allows us to ensure we're grabbing the most local symbol available.
    for (i32 i = this->table_stack.size() - 1; i >= 0; --i)
    {

        Symboltable<Symboltype>& current_table = this->table_stack[i];
        if (current_table.contains(str)) 
            return &current_table[str];

    }

    // Check the global table.
    if (this->global_table.contains(str))
        return &this->global_table[str];

    return nullptr;

}

template <typename Symboltype>
Symboltype* SymboltableStack<Symboltype>::
get_symbol_locally(const std::string& str)
{

    // Check the global table if we're at the root.
    if (this->table_stack.size() == 0)
    {
        if (this->global_table.contains(str))
            return &this->global_table[str];
    }

    // Otherwise, we just check the top-most table.
    else
    {
        Symboltable<Symboltype>& current_table = this->table_stack[this->table_stack.size() - 1];
        if (current_table.contains(str))
            return &current_table[str];
    }
    
    return nullptr;

}

template <typename Symboltype>
Symboltype* SymboltableStack<Symboltype>::
get_symbol_globally(const std::string& str)
{

    if (this->global_table.contains(str))
        return &this->global_table[str];
    return nullptr;

}

#endif
