#include <compiler/symbols/symbol.hpp>

Symbol::
Symbol()
{
    this->name = "";
    this->type = Symboltype::SYMBOL_TYPE_UNKNOWN;
    this->datatype = Datatype::DATA_TYPE_UNKNOWN;
}

Symbol::
Symbol(string name, Symboltype type, Datatype datatype)
{
    this->name = name;
    this->type = type;
    this->datatype = datatype;
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
set_datatype(Datatype datatype)
{
    this->datatype = datatype;
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

Datatype Symbol::
get_datatype() const
{
    return this->datatype;
}

bool Symbol::
promote(Datatype type)
{
    if (type == Datatype::DATA_TYPE_STRING && (this->datatype == Datatype::DATA_TYPE_UNKNOWN ||
        this->datatype == Datatype::DATA_TYPE_VOID))
    {
        this->datatype = type;
        return true;
    }
    else if (this->datatype == Datatype::DATA_TYPE_STRING && type != Datatype::DATA_TYPE_STRING)
    {
        return false;
    }
    else if (this->datatype != Datatype::DATA_TYPE_STRING && type == Datatype::DATA_TYPE_STRING)
    {
        return false;
    }

    // Even if the promotion is less, we can let it pass.
    if (this->datatype < type)
        this->datatype = type;
    return true;

}