#ifndef SIGMAFOX_COMPILER_GENERATION_SOURCETREE_HPP
#define SIGMAFOX_COMPILER_GENERATION_SOURCETREE_HPP
#include <definitions.hpp>
#include <compiler/generation/sourcefile.hpp>

class Sourcetree
{

    public:
                        Sourcetree(string output_directory);
        virtual        ~Sourcetree();

        bool            insert_source(GeneratableSourcefile *source);
        bool            source_exists(string name) const;
        bool            commit() const;

    protected:
        string output_directory;
        std::unordered_map<string, GeneratableSourcefile*> map;

};

#endif
