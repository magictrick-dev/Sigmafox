#include <compiler/symbols/symbol.hpp>

Symbol::
Symbol()
{
    this->name = "";
    this->type = Symboltype::SYMBOL_TYPE_UNKNOWN;
    this->node = nullptr;
    this->arity = 0;
}

Symbol::
Symbol(string name, Symboltype type, shared_ptr<SyntaxNode> node, i32 arity)
{
    this->name = name;
    this->type = type;
    this->node = node;
    this->arity = arity;
}

Symbol::
~Symbol()
{
}

void Symbol::
set_name(string name)
{
    this->name = name;
}

void Symbol::
set_type(Symboltype type)
{
    this->type = type;
}

void Symbol::
set_node(shared_ptr<SyntaxNode> node)
{
    this->node = node;
}

void Symbol::
set_arity(i32 arity)
{
    this->arity = arity;
}

string Symbol::
get_name() const
{
    return this->name;
}

Symboltype Symbol::
get_type() const
{
    return this->type;
}

shared_ptr<SyntaxNode> Symbol::
get_node() const
{
    return this->node;
}

i32 Symbol::
get_arity() const
{
    return this->arity;
}

bool Symbol::
is_array() const
{
    return (this->type == Symboltype::SYMBOL_TYPE_VARIABLE && this->arity > 0);
}
