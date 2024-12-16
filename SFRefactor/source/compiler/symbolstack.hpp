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

        inline Symboltype&      insert_symbol_locally(const std::string& str);
        inline Symboltype&      insert_symbol_globally(const std::string& str);
        
    protected:
        Symboltable<Symboltype> table_root;
        std::vector<Symboltable<Symboltype>> table_stack;

};

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

}

template <typename Symboltype>
void SymboltableStack<Symboltype>::
pop_table()
{

}

template <typename Symboltype>
bool SymboltableStack<Symboltype>::
identifier_exists(const std::string& str) const
{

    return false;
}

template <typename Symboltype>
bool SymboltableStack<Symboltype>::
identifier_exists_locally(const std::string& str) const
{

    return false;
}

template <typename Symboltype>
bool SymboltableStack<Symboltype>::
identifier_exists_globally(const std::string& str) const
{

    return false;
}

template <typename Symboltype>
Symboltype& SymboltableStack<Symboltype>::
insert_symbol_locally(const std::string& str)
{
    
    static Symboltype symbol;
    return symbol;

}

template <typename Symboltype>
Symboltype& SymboltableStack<Symboltype>::
insert_symbol_globally(const std::string& str)
{
    
    static Symboltype symbol;
    return symbol;

}

#endif
