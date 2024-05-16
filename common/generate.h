#ifndef SIGMAFOX_COMMON_GENERATE_H
#define SIGMAFOX_COMMON_GENERATE_H
#include <core.h>

#include <string>
#include <vector>
#include <sstream>

#define TABS_TO_SPACES  4
#define TAB_CONSTANT    4

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

    bool            pointer     = false;
    EAccesstype     access      = EAccesstype::Direct;
    EConstness      constness   = EConstness::Mutable;

    inline bool     is_valid() const { return (this->type != "" && this->name != ""); };
};

struct Methodname
{
    Typename                signature;
    std::vector<Typename>   parameters;

    EVirtual                virtualness = EVirtual::Concrete;

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

        void add_property(Typename property, EScope scope);
        void add_method(Methodname method, EScope scope);

        Methodname&     set_constructor(Methodname method);
        Methodname&     set_destructor(Methodname method);

        inline Typename signature() { return this->class_signature; };

        virtual std::string     generate(size_t tab_depth) override;
 

    private:
        Typename                class_signature;
        ClassDefinition        *inherits;

        std::vector<Typename>   properties_public;
        std::vector<Typename>   properties_private;
        std::vector<Typename>   properties_protected;

        std::vector<Methodname> methods_public;
        std::vector<Methodname> methods_private; 
        std::vector<Methodname> methods_protected;
};























// --- Generetable Refactor --------------------------------------------
#include <core/stringutils.h>
#include <string>
#include <sstream>

class GeneratableSource
{
    public:
        virtual std::string     to_string(size_t tab_depth) = 0;
};

class SourceType : GeneratableSource
{
    public:
        inline SourceType()
            : source_type(""), source_name("") {};

        inline SourceType(std::string type, std::string name)
            : source_type(type), source_name(name) {};

        inline SourceType(std::string type, std::string name, bool is_inline, bool is_static)
            : source_type(type), source_name(name), is_inline(is_inline), is_static(is_static) { }

        virtual inline std::string to_string(size_t tab_depth) override
        {
            std::stringstream output;
            if (is_static) output << "static ";
            if (is_inline) output << "inline ";
            output << this->source_type << " " << this->source_name;
            return output.str();
        }

        bool is_inline = false;
        bool is_static = false;
        std::string source_type;
        std::string source_name;

};

class SourceFunctionDecleration : public GeneratableSource
{
    public:
        inline SourceFunctionDecleration(SourceType callname)
        {
            this->function_type = callname;
        }

        inline void append_parameter(SourceType type)
        {
            this->params.push_back(type);
        }

        virtual inline std::string to_string(size_t tab_depth) override
        {

            std::stringstream output;
            output << this->function_type.to_string(0);

            // Output the parameters.
            output << "(";
            for (size_t i = 0; i < this->params.size(); ++i)
            {
                output << this->params[i].to_string(0);
                if (i < this->params.size() - 1)
                    output << ", ";
            }
            output << ");";
            
            return string_tabulate_lines(tab_depth, output.str());

        }

    protected:
        SourceType function_type;
        std::vector<SourceType> params;
};

class SourceClassDecleration : public GeneratableSource
{
    public:
        inline SourceClassDecleration(std::string class_name)
        {
            this->class_name = class_name;
        };

        inline void append_public_property(SourceType type)
        {
            this->public_properties.push_back(type);
        }

        inline void append_protected_property(SourceType type)
        {
            this->protected_properties.push_back(type);
        }

        inline void append_private_property(SourceType type)
        {
            this->private_properties.push_back(type);
        }

        inline void append_public_method(SourceFunctionDecleration method)
        {
            this->public_methods.push_back(method);
        }

        inline void append_protected_method(SourceFunctionDecleration method)
        {
            this->protected_methods.push_back(method);
        }

        inline void append_private_method(SourceFunctionDecleration method)
        {
            this->private_methods.push_back(method);
        }

        virtual inline std::string to_string(size_t tab_depth) override
        {
            
            std::stringstream output;
            output << "class " << this->class_name << std::endl;
            output << "{" << std::endl;

            if (this->public_properties.size() > 0 || this->public_methods.size() > 0)
            {

                output << string_tabulate_lines(TAB_CONSTANT, "public:") << std::endl;
                
                // All of methods first.
                for (size_t i = 0; i < this->public_methods.size(); ++i)
                {
                    
                }

            }
        
            output << "}" << std::endl;

        }

    protected:
        std::string class_name;

        std::vector<SourceType> public_properties;
        std::vector<SourceType> protected_properties;
        std::vector<SourceType> private_properties;

        std::vector<SourceFunctionDecleration> public_methods;
        std::vector<SourceFunctionDecleration> protected_methods;
        std::vector<SourceFunctionDecleration> private_methods;

};

// SourceHeaderDocument
//      Provides a method of generating header source files.
class SourceHeaderDocument : public GeneratableSource
{
    public:
        inline SourceHeaderDocument() { };

        virtual inline std::string to_string(size_t tab_depth) override
        {

            std::stringstream output;
            output << "#pragma once"<< std::endl << std::endl;

            // Now we can process all our generatables.
            for (size_t i = 0; i < this->source_function_declerations.size(); ++i)
            {
                std::string function_decleration = this->source_function_declerations[i].to_string(0);
                output << string_tabulate_lines(0, function_decleration) << std::endl;

                if (i == this->source_function_declerations.size() - 1)
                    output << std::endl;
            }


            // Finally, we take our string and then bump it by the tab-depth.
            return string_tabulate_lines(tab_depth, output.str());

        }
        
        inline void append_function(SourceFunctionDecleration def)
        { 
            this->source_function_declerations.push_back(def);
        }

    protected:
        std::string     include_name;
        std::string     include_path;

        std::vector<SourceFunctionDecleration> source_function_declerations;
};

#endif
