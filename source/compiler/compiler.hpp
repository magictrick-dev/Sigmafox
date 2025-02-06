#ifndef SIGMAFOX_COMPILER_COMPILER_HPP
#define SIGMAFOX_COMPILER_COMPILER_HPP
#include <definitions.hpp>
#include <compiler/parser/parser.hpp>
#include <compiler/symbols/table.hpp>
#include <compiler/environment/environment.hpp>

class Compiler
{

    public:
                    Compiler();
        virtual    ~Compiler();

    protected:
        vector<string> source_files;

};

#endif