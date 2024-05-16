#include <vector>
#include <sstream>
#include <core/stringutils.h>

std::string 
string_to_uppercase(const std::string& str)
{
    std::string output_string;
    for (char c : str)
        output_string += toupper(c);
    return output_string;
}

std::string 
string_to_lowercase(const std::string& str)
{
    std::string output_string;
    for (char c : str)
        output_string += tolower(c);
    return output_string;
}

std::string 
string_tabulate_lines(size_t tab_size, const std::string &str)
{
    
    std::stringstream input_string {str};
    std::stringstream output_string;

    std::vector<std::string> lines;
    std::string current_line;
    while (std::getline(input_string, current_line))
        lines.push_back(current_line);

    for (size_t i = 0; i < lines.size(); ++i)
    {
        for (size_t i = 0; i < tab_size; ++i)
            output_string << " ";
        output_string << lines[i];

        if (i < lines.size() - 1)
            output_string << std::endl;
    }

    return output_string.str();

}

std::string 
string_replace_all(const std::string& str, char match, char replace)
{
    
    std::string output;
    for (char c : str)
    {
        if (c == match)
            output += replace;
        else
            output += c;
    }

    return output;

}

std::string
string_remove_all(const std::string& str, char match)
{

    std::string output;
    for (char c : str)
    {
        if (c != match)
            output += c;
    }

    return output;

}
