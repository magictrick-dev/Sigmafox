#include <compiler/symbols/table.hpp>

Symboltable::
Symboltable()
{
}

Symboltable::
~Symboltable()
{
}

bool Symboltable::
exists(string key) const
{
    return this->symbols.find(key) != this->symbols.end();
}

bool Symboltable::
insert(Symbol symbol)
{
    if (this->exists(symbol.get_name()))
    {
        return false;
    }

    this->symbols[symbol.get_name()] = symbol;
    return true;
}

Symbol* Symboltable::
find(string key)
{
    if (!this->exists(key))
    {
        return nullptr;
    }

    return &this->symbols[key];
}