#ifndef SIGMAFOX_COMPILER_ENVIRONMENT_HPP
#define SIGMAFOX_COMPILER_ENVIRONMENT_HPP
#include <definitions.hpp>
#include <compiler/tokenizer.hpp>
#include <compiler/symbols/table.hpp>

// --- Environment -------------------------------------------------------------
//
// The environment is shared between all the components of the compiler. During
// the parsing phase, the environment is used to store the symbol tables, tokenizers,
// and validation routines that are used to parse the source code.
//
// Symbol tables function as a stack, with the bottom-most table being the global
// symbol table. As scopes are pushed, tables are pushed. The environment does not
// directly allow for the global to be popped, so there is a possibility that the
// pop_table() function will return false if the global table is attempted to be
// popped. An assertion is thrown if this occurs.
//
// When you query for symbols, the symbol_exists() function will check all tables.
// The symbol_exists_locally() will only check the current table on the stack, likewise
// the symbol_exists_globally() will check only the global table. If you want to
// know if a symbol exists, but not in the global table, you can use the symbol_exists()
// function and then check if it doesn't exist locally. The get/set routines only
// allow for symbols to move into the local or global scopes, respectively. It would
// not be possible to insert symbols into higher scopes (nor would it make sense).
//
// In order to validate if a certain include has been included, the environment
// provides the necessary functionality to check if a file has already been included
// and checks if the file is circularly included. This is done by maintaining a list
// of included files and checking if the file is already in the list.
//

class Environment
{

    public:
                        Environment();
        virtual        ~Environment();

        Symboltable&    get_local_table();
        Symboltable&    get_global_table();

        void            push_table();
        bool            pop_table();

        bool            symbol_exists(string identifier);
        bool            symbol_exists_locally(string identifier);
        bool            symbol_exists_globally(string identifier);

        Symbol*         get_symbol(string identifier);
        Symbol*         get_symbol_locally(string identifier);
        Symbol*         get_symbol_globally(string identifier);

        void            set_symbol_locally(string identifier, Symbol symbol);
        void            set_symbol_globally(string identifier, Symbol symbol);

    protected:
        vector<Symboltable> tables;
        unordered_map<string, shared_ptr<Tokenizer>> tokenizers;

};

#endif