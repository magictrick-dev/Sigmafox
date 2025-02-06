#ifndef SIGMAFOX_COMPILER_SYMBOLS_TABLE_HPP
#define SIGMAFOX_COMPILER_SYMBOLS_TABLE_HPP
#include <unordered_map>
#include <definitions.hpp>
#include <compiler/symbols/symbol.hpp>

class Symboltable
{

    public:
                        Symboltable();
        virtual        ~Symboltable();

        bool            exists(string key) const;
        bool            insert(Symbol symbol);
        Symbol*         find(string key);

    protected:
        std::unordered_map<std::string, Symbol> symbols;

};

#endif