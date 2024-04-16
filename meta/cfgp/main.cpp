// --- CGFP --------------------------------------------------------------------
//
// This metaprogram generates an AST layout based on a provided grammar file.
// The output is a C++ file which contains the grammar definitions and visitor
// pattern for horizontal function expressions. The main idea is that this makes
// it easier to extend functionality without having to manually write each class
// definition.
//

#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>

struct property_definition
{
    std::string type;
    std::string name;
};

struct grammar_definition
{
    std::string name;
    std::vector<property_definition> props;
};

// --- Helpers -----------------------------------------------------------------

inline static std::string
lower_string(std::string str)
{
    std::string out;
    for (auto c : str)
        out += tolower(c);
    return out;
}

inline static std::string
trim_string(std::string str)
{

    size_t left = 0;
    for (size_t idx = 0; idx < str.length(); ++idx)
    {
        if (isspace(str[idx])) left++;
        else break;
    }

    str.erase(0, left);

    size_t right = str.length();
    while (isspace(str[right-1])) right--;
    str = str.substr(0, right);
    return str;

}

inline static std::vector<std::string>
split_string(std::string str, std::string delim)
{
    
    std::vector<std::string> output;

    size_t position = 0;
    std::string token;

    // Straight off the stackoverflow press. Why isn't this a default
    // standard library method? Such a common thing to do...
    while ((position = str.find(delim)) != std::string::npos)
    {
        token = str.substr(0, position);
        output.push_back(token);
        str.erase(0, position + delim.length());
    }

    output.push_back(str);

    return output;

}


// --- Generators --------------------------------------------------------------

static void generate_break(std::ofstream& of, std::string name)
{
    size_t break_size = 72 - name.length();
    of << "// --- " << name << " ";
    for (size_t d = 0; d < break_size; ++d)
        of << "-";
    of << std::endl;
}

static void
generate_header(std::ofstream& of)
{
    of << "// This file was partially generated using a metaprogram, cgfp.\n"
       << "// See ./meta/cgfp for details.\n" << std::endl;
    of << "#ifndef SIGMAFOX_EXPRESSION_H" << std::endl;
    of << "#define SIGMAFOX_EXPRESSION_H\n" << std::endl;
}

static void
generate_footer(std::ofstream& of)
{
    of << "#endif";
}

static void
generate_base(std::ofstream& of, std::string base_class_name)
{
    generate_break(of, base_class_name);
    of << "\nclass " << base_class_name << std::endl;
    of << "{" << std::endl;
    of << "    public:\n        virtual void accept(" <<
        base_class_name + "Visitor" << ") = 0;\n";
    of << "};\n" << std::endl;
}

static void
generate_visitor(std::ofstream& of, std::vector<grammar_definition>& gdef, std::string base)
{
    generate_break(of, base + "Visitor");
    of << "\nclass " << base + "Visitor" << std::endl;
    of << "{" << std::endl;
    of << "    public:" << std::endl;
    for (auto& def : gdef)
    {
        of << "        inline void visit_" << lower_string(def.name) << "(" <<
            base + "Visitor visitor);" << std::endl;
    }
    of << "};\n" << std::endl;
}

static void
generate_derived(std::ofstream& of, grammar_definition &definition, std::string base)
{
    generate_break(of, definition.name);
    of << "\nclass " << definition.name << " : public " << base << std::endl;
    of << "{" << std::endl;
    of << "    public:\n";
    of << "        inline virtual void accept(" << base + "Visitor visitor) override\n"
       << "        { visitor.visit_" + lower_string(definition.name) << "(this); }\n\n";
    of << "    protected:\n";

    for (size_t idx = 0; idx < definition.props.size(); ++idx)
    {
        of  << "        " << definition.props[idx].type << " "
            << definition.props[idx].name << ";" << std::endl;
    }

    of << "\n};\n" << std::endl;
}

// --- Runtime -----------------------------------------------------------------

int
main(int argc, char ** argv)
{

    if (argc != 3)
    {
        std::cout << "CFGP expects a filepath to a grammar file." << std::endl;
        std::cout << "./cfgp [grammar file] [output name]" << std::endl;
        return 1;
    }

    std::cout   << "Attempting to generate definitions files for " << argv[1]
                << " to " << argv[2] << std::endl;
    
    // Launch the filestreams.
    std::ifstream input_fs(argv[1]);
    std::ofstream output_fs(argv[2]);

    // Validate the filestreams.
    if (!input_fs.is_open() || !output_fs.is_open())
    {
        input_fs.close();
        output_fs.close();
        std::cout << "Unable to open the required files." << std::endl;
        return 2;
    }

    // Generate file.
    generate_header(output_fs);
    generate_base(output_fs, "Expression");

    // Parse the input file. Not the prettiest code I've written, but it does get
    // the job done, I guess.
    std::vector<grammar_definition> definitions;
    std::string current_line;
    while (std::getline(input_fs, current_line))
    {
        
        // Something something Python does this better...
        if (std::all_of(current_line.begin(), current_line.end(), isspace)) continue;

        // Get the name.
        std::vector<std::string> pair = split_string(current_line, ":");

        grammar_definition gdef = {};
        gdef.name = trim_string(pair[0]);

        // Now split again to get the type definitions.
        std::vector<std::string> types = split_string(trim_string(pair[1]), ",");
        for (std::string& str : types)
        {
            property_definition pdef = {};
            str = trim_string(str);
            std::vector<std::string> kv = split_string(str, " ");
            pdef.type = kv[0];
            pdef.name = kv[1];
            gdef.props.push_back(pdef);
        }
 
        definitions.push_back(gdef);

    }

    generate_visitor(output_fs, definitions, "Expression");

    // Print definitions
    for (auto& def : definitions)
        generate_derived(output_fs, def, "Expression");

    generate_footer(output_fs);

}
