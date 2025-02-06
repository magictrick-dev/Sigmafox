#include <compiler/environment/environment.hpp>

Environment::
Environment()
{
    
    this->tables.emplace_back(Symboltable());
    
}

Environment::
~Environment()
{

}

