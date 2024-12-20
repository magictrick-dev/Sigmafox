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

        template <class... Args> inline void insert_symbol_locally(const std::string& str, Args... args);
        template <class... Args> inline void insert_symbol_globally(const std::string& str, Args... args);
        inline void insert_symbol_locally(const std::string& str, Symboltype&& symbol);
        inline void insert_symbol_globally(const std::string& str, Symboltype&& symbol);

        Symboltype*             get_symbol(const std::string& str);
        Symboltype*             get_symbol_locally(const std::string &str);
        Symboltype*             get_symbol_globally(const std::string &str);
        
    protected:
        std::vector<Symboltable<Symboltype>> table_stack;

};

template <typename Symboltype>
SymboltableStack<Symboltype>::
SymboltableStack()
{

    // Root table.
    this->table_stack.emplace_back();

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

    SF_ASSERT(this->table_stack.size() > 1); // Bottom-most table is root.
    this->table_stack.pop_back();

}

template <typename Symboltype>
bool SymboltableStack<Symboltype>::
identifier_exists(const std::string& str) const
{

    for (i32 i = this->table_stack.size() - 1; i >= 0; --i)
    {

        const Symboltable<Symboltype>& current_table = this->table_stack[i];
        if (current_table.contains(str))
            return true;

    }

    return false;
}

template <typename Symboltype>
bool SymboltableStack<Symboltype>::
identifier_exists_locally(const std::string& str) const
{

    if (this->table_stack[this->table_stack.size() - 1].contains(str))
        return true;
    return false;

}

template <typename Symboltype>
bool SymboltableStack<Symboltype>::
identifier_exists_globally(const std::string& str) const
{

    if (this->table_stack[0].contains(str))
        return true;
    return false;

}

template <typename Symboltype> template <class... Args>
void SymboltableStack<Symboltype>::
insert_symbol_locally(const std::string& str, Args... args)
{
    
    SF_ASSERT(!this->identifier_exists_locally(str));
    Symboltable<Symboltype> &table = this->table_stack[this->table_stack.size() - 1];
    table.emplace(str, args...);

}

template <typename Symboltype> template <class... Args>
void SymboltableStack<Symboltype>::
insert_symbol_globally(const std::string& str, Args... args)
{

    SF_ASSERT(!this->identifier_exists_globally(str));
    Symboltable<Symboltype> &table = this->table_stack[0];
    table.emplace(str, args...);

}

template <typename Symboltype>
void SymboltableStack<Symboltype>::
insert_symbol_locally(const std::string& str, Symboltype&& symbol)
{

    SF_ASSERT(!this->identifier_exists_locally(str));
    Symboltable<Symboltype> &table = this->table_stack[this->table_stack.size() - 1];
    table.emplace(str, symbol);

}

template <typename Symboltype>
void SymboltableStack<Symboltype>::
insert_symbol_globally(const std::string& str, Symboltype&& symbol)
{

    SF_ASSERT(!this->identifier_exists_globally(str));
    Symboltable<Symboltype> &table = this->table_stack[0];
    table.emplace(str, symbol);

}

template <typename Symboltype>
Symboltype* SymboltableStack<Symboltype>::
get_symbol(const std::string& str)
{

    for (i32 i = this->table_stack.size() - 1; i >= 0; --i)
    {

        Symboltable<Symboltype>& current_table = this->table_stack[i];
        if (current_table.contains(str)) 
            return &current_table[str];

    }

    return nullptr;

}

template <typename Symboltype>
Symboltype* SymboltableStack<Symboltype>::
get_symbol_locally(const std::string& str)
{

    Symboltable<Symboltype>& current_table = this->table_stack[this->table_stack.size() - 1];
    if (current_table.contains(str))
        return &current_table[str];
    return nullptr;

}

template <typename Symboltype>
Symboltype* SymboltableStack<Symboltype>::
get_symbol_globally(const std::string& str)
{

    Symboltable<Symboltype>& current_table = this->table_stack[0];
    if (current_table.contains(str))
        return &current_table[str];
    return nullptr;

}

#endif
