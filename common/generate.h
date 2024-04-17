#ifndef SIGMAFOX_COMMON_GENERATE_H
#define SIGMAFOX_COMMON_GENERATE_H
#include <string>
#include <vector>
#include <sstream>

#define TABS_TO_SPACES  4

// --- Base Generatable Class --------------------------------------------------
//
// The generatable class is the base class which all generatable structures
// have. It provides a set of utilities which either need to be implemented
// or come pre-implemented such that any inheriting class can use.
//
// The generate method is the most important implementation too for anything
// that is deemed "generatable". A generatable object must be able to convert
// itself to an outputtable string.
//
// The generate method takes "tab_depth" as a parameter which basically determines
// how many tabs (converted to spaces) will everything that is generated will be.
//

class Generatable
{
    public:
        void                    docbox_append(std::string& str);
        std::string&            docbox();

        virtual std::string     generate(size_t tab_depth) = 0;

    private:
        std::string documentation_box;
};

// --- Class Generation Utilities ----------------------------------------------
//
// Generating class files is a pretty straight forward process. Many of the
// esoteric features of C++ can be avoided since they can be tweaked post-generation
// when required. The main features we care about are single-inheritance since it
// is something that the CGFP meta-utility needs. For the actual compiler, we
// mainly are concerned about the modularization within the language specification.
// We can therefore safely ignore any horizontal inheritance.
//
// Extensibility is the name of the game here, so we generally want to allow for
// on-the-fly tweaking when we generate methods and properties for a given class.
// As such, we don't want to hide any members for our Typenames and Methodnames
// and let the user fill them out as they create them.
//
// To avoid flags on things that are typically mutually exlusive, we use enum
// structures to handle this. For example, a function is either concrete, virtual
// or pure virtual.
//

enum class EVirtual
{
    Concrete    = 0,
    Virtual     = 1,
    Pure        = 2,
};

enum class EConstness
{
    Mutable     = 0,
    Immutable   = 1,
};

enum class EAccesstype
{
    Direct      = 0,
    Reference   = 1,
};

struct Typename
{
    std::string     type;
    std::string     name;
    bool            pointer;

    EAccessType     access;
    EConstness      constness;
};

struct Methodname
{
    Typename                signature;
    std::vector<Typename>   parameters;

    bool                    implemented;
    std::string             implementation;

    bool                    inlined;
};

enum class EScope
{
    Public      = 0,
    Protected   = 1,
    Private     = 2,
};

class ClassDefinition : public Generatable
{
    public:
        ClassDefinition(Typename signature, SF_OPTIONAL ClassDefinition *parent);

        Typename&       add(Typename property, EScope scope);
        Methodname&     add(Methodname method, EScope scope);
        Typename&       add_typename();
        Methodname&     add_methodname();

        virtual std::string     generate() override;
        
    private:
        Typename                class_signature;
        ClassGeneration        *inherits;

        std::vector<Typename>   properties_public;
        std::vector<Typename>   properties_private;
        std::vector<Typename>   properties_protected;

        std::vector<Methodname> methods_public;
        std::vector<Methodname> methods_private; 
        std::vector<Methodname> methods_protected;
};

#endif
