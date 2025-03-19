#include <compiler/environment.hpp>

// --- Environment -------------------------------------------------------------

Environment::
Environment()
{
    
    this->tables.emplace_back(Symboltable());
    this->begin_defined = false;
    
}

Environment::
~Environment()
{

}

Symboltable& Environment::
get_local_table()
{
    
    return this->tables.back();
    
}

Symboltable& Environment::
get_global_table()
{
    
    return this->tables.front();
    
}

void Environment::
push_table()
{
    
    this->tables.emplace_back(Symboltable());
    
}

bool Environment::
pop_table()
{
    
    if (this->tables.size() == 1)
    {
        SF_ASSERT(!"You can not pop the global table. You did something terribly wrong.");
        return false;
    }
    
    this->tables.pop_back();
    return true;
    
}

bool Environment::
symbol_exists(string identifier)
{
    
    for (auto& table : this->tables)
    {
        if (table.find(identifier))
            return true;
    }
    
    return false;
    
}

bool Environment::
symbol_exists_locally(string identifier)
{
    
    return this->tables.back().find(identifier);
    
}

bool Environment::
symbol_exists_globally(string identifier)
{
    
    return this->tables.front().find(identifier);
    
}

bool Environment::
symbol_exists_but_not_locally(string identifier)
{
    
    // We don't check the last table because that's the local table.
    for (i64 i = 0; i < this->tables.size() - 1; i++)
    {
        if (this->tables[i].find(identifier))
            return true;
    }
    
    return false;
    
}

Symbol* Environment::
get_symbol(string identifier)
{
    
    for (auto& table : this->tables)
    {
        Symbol* symbol = table.find(identifier);
        if (symbol)
            return symbol;
    }
    
    return nullptr;
    
}

Symbol* Environment::
get_symbol_locally(string identifier)
{
    
    return this->tables.back().find(identifier);
    
}

Symbol* Environment::
get_symbol_globally(string identifier)
{
    
    return this->tables.front().find(identifier);
    
}

void Environment::
set_symbol_locally(string identifier, Symbol symbol)
{
    
    this->tables.back().insert(symbol);
    
}

void Environment::
set_symbol_globally(string identifier, Symbol symbol)
{
    
    this->tables.front().insert(symbol);
    
}

bool Environment::
is_begin_defined() const
{
    
    return this->begin_defined;
    
}

void Environment::
define_begin()
{
    
    this->begin_defined = true;
    
}
