#ifndef SIGMAFOX_COMPILER_SYMBOLS_SYMBOL_HPP
#define SIGMAFOX_COMPILER_SYMBOLS_SYMBOL_HPP
#include <definitions.hpp>
#include <compiler/parser/node.hpp>

// --- Symbol ------------------------------------------------------------------
//
// The symbol class is the method of which we track and manage the symbols in the
// symbol table. Symbols themselves are validated and checked for type and datatype
// as they're used in expressions.
//
// Typically, symbols are initialized and set as UNKNOWN and then promoted to their
// respective types as they're initialized. Promotions always return true when the
// promotion is able to cast up or down, and false when the promotion is not possible.
// These conditions only occur when a string is trying to be promoted to a non-string
// datatype, or a number is trying to promote to a string datatype.
//
// Promotion occurs in this order:      void -> integer -> real -> complex reals
// Strings can only be promoted from:   void -> string
//

enum class Symboltype
{
    SYMBOL_TYPE_UNKNOWN,
    SYMBOL_TYPE_DECLARED,
    SYMBOL_TYPE_VARIABLE,
    SYMBOL_TYPE_PROCEDURE,
    SYMBOL_TYPE_FUNCTION,
};

class Symbol
{
    public:
                    Symbol();
                    Symbol(string name, Symboltype type, shared_ptr<SyntaxNode> node, i32 arity = 0);
        virtual    ~Symbol();

        void        set_name(string name);
        void        set_type(Symboltype type);
        void        set_node(shared_ptr<SyntaxNode> node);
        void        set_arity(i32 arity);

        string                  get_name() const;
        Symboltype              get_type() const;
        shared_ptr<SyntaxNode>  get_node() const;
        i32                     get_arity() const;

        bool                    is_array() const;

    protected:
        i32                     arity;
        string                  name;
        Symboltype              type; 
        shared_ptr<SyntaxNode>  node;

};

#endif