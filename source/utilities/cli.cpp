#include <iostream>
#include <utilities/cli.hpp>

// --- CLIArgument Implementation ----------------------------------------------

CLIArgument::
CLIArgument(i32 index, ccptr argument)
{
    this->raw_index = index;
    this->raw_argument = argument;
}

CLIArgument::
~CLIArgument()
{

}

i32 CLIArgument::
get_index() const
{
    return this->raw_index;
}

ccptr CLIArgument::
get_argument() const
{
    return this->raw_argument;
}

CLIArgumentType CLIArgument::
get_type() const
{
    return this->type;
}

// --- CLIValue Implementation -------------------------------------------------

CLIValue::
CLIValue(i32 index, ccptr argument) : CLIArgument(index, argument)
{
    
}

CLIValue::
~CLIValue()
{

}

CLIArgument* CLIValue::
parse(i32 index, ccptr argument)
{

    i32 length = strlen(argument);
    SF_ASSERT(length >= 1);

    // Number or number-adjacent.
    i32 character_index = 0;
    if (isdigit(argument[character_index]))
    {

        character_index++;
        u64 multiplier = 1;
        bool is_memory_type = false;
        bool is_real_type = false;

        while (character_index < length)
        {
            
            char c = argument[character_index];

            // Check if its a real value.
            if (c == '.')
            {
                if (is_real_type == false)
                {
                    is_real_type = true;
                }
                else
                {
                    return CLIValue::error(index, argument);
                }
            }

            // Parsing memory values.
            else if (tolower(c) == 'k')
            {
                if (is_memory_type == true ||
                    character_index == length - 1 ||
                    is_real_type == true)
                {
                    return CLIValue::error(index, argument);
                }
                c = argument[++character_index];
                if (!(tolower(c) == 'b'))
                {
                    return CLIValue::error(index, argument);
                }

                is_memory_type = true;
                multiplier = 1024;
                
            }

            else if (tolower(c) == 'm')
            {
                if (is_memory_type == true ||
                    character_index == length - 1 ||
                    is_real_type == true)
                {
                    return CLIValue::error(index, argument);
                }
                c = argument[++character_index];
                if (!(tolower(c) == 'b'))
                {
                    return CLIValue::error(index, argument);
                }

                is_memory_type = true;
                multiplier = 1024*1024;

            }

            else if (tolower(c) == 'g')
            {
                if (is_memory_type == true ||
                    character_index == length - 1 ||
                    is_real_type == true)
                {
                    return CLIValue::error(index, argument);
                }
                c = argument[++character_index];
                if (!(tolower(c) == 'b'))
                {
                    return CLIValue::error(index, argument);
                }

                is_memory_type = true;
                multiplier = 1024*1024*1024;
            }

            else if (!isdigit(c))
            {
                return CLIValue::error(index, argument);
            }

            character_index++;

        }

        CLIValue *value = new CLIValue(index, argument);
        if (is_real_type)
        {
            value->set_type(CLIArgumentType::Real);
            value->parsed_value.real = atof(argument);
        }
        else
        {
            value->set_type(CLIArgumentType::Numeric);
            if (is_memory_type)
            {
                value->parsed_value.uint = atoll(argument) * multiplier;
            }
            else
            {
                value->parsed_value.sint = atoi(argument);
            }
        }

        return value;

    }

    // A string literal.
    else
    {
        CLIValue *value = new CLIValue(index, argument);
        value->set_type(CLIArgumentType::String);
        value->parsed_value.string = argument;
        return value;
    }

}

CLIArgument* CLIValue::
error(i32 index, ccptr argument)
{
    CLIArgument *error = new CLIValue(index, argument);
    error->set_type(CLIArgumentType::Error);
    return error;
}

// --- CLISwitch Implementation ------------------------------------------------

CLISwitch::
CLISwitch(i32 index, ccptr argument) : CLIArgument(index, argument)
{

    // Preset all to false.
    for (i32 i = 0; i < 52; ++i) this->flags[i] = false;

    // Actually parse to flags.
    i32 length = strlen(argument);
    for (i32 i = 1; i < length; ++i)
    {

        i32 offset = this->offset_at(argument[i]);
        this->flags[offset] = true;

    }

    this->type = CLIArgumentType::Switch;

}

bool& CLISwitch::
operator[](char c)
{
    SF_ASSERT(isalpha(c)); // Explode if it isn't correct.
    i32 offset = this->offset_at(c);
    return this->flags[offset];
}

bool& CLISwitch::
operator[](i32 idx)
{
    SF_ASSERT(idx < 52); // Ensure we're in bounds.
    return this->flags[idx];
}

i32 CLISwitch::
offset_at(char c)
{
    SF_ASSERT(isalpha(c)); // Ensures that we properly parsed.
    i32 offset = 0;
    if (isupper(c)) offset = 'A' - c;
    else offset = ('a' - c) + 26;
    return offset;
}

CLISwitch::
~CLISwitch()
{

}

CLIArgument* CLISwitch::
parse(i32 index, ccptr argument)
{

    // These conditions should be checked by CLI::parse() and therefore must
    // always be true.
    u64 string_length = strlen(argument);
    SF_ASSERT(string_length >= 1);
    SF_ASSERT(argument[0] == '-');

    b32 valid_format = true;
    for (u64 i = 1; i < string_length; ++i)
    {

        if (!isalpha(argument[i]))
        {
            valid_format = false;
            break;
        }

    }

    if (valid_format)
    {
        CLISwitch *result = new CLISwitch(index, argument);
        return result;
    }

    else
    {
        CLIArgument *result = CLIValue::error(index, argument);
        return result;
    }

}

// --- CLIParameter Implementation ---------------------------------------------

CLIParameter::
CLIParameter(i32 argc, ccptr argument) : CLIArgument(argc, argument)
{

    // No additional parsing is required.
    this->type = CLIArgumentType::Parameter;

}

CLIParameter::
~CLIParameter()
{

}

std::string CLIParameter::
get_name() const
{
    return this->name;
}

CLIArgument* CLIParameter::
parse(i32 index, ccptr argument)
{

    u64 length = strlen(argument);
    SF_ASSERT(length >= 2);
    SF_ASSERT(argument[0] == '-');
    SF_ASSERT(argument[1] == '-');

    std::string name; // We can parse the name now.
    b32 valid_format = true;
    for (u64 i = 2; i < length; ++i)
    {
        if (isalpha(argument[i]) || argument[i] == '-')
            name += argument[i];
        else
        {
            valid_format = false;
            break;
        }
    }

    if (valid_format)
    {
        CLIParameter *result = new CLIParameter(index, argument);
        result->name = name;
        return result;
    }

    else
    {
        CLIArgument *result = CLIValue::error(index, argument);
        return result;
    }

}

// --- CLI Implementation ------------------------------------------------------
//
// I'm not going to lie, this entire implementation is one giant anti-pattern.
// The largest crime here is that I'm using an internal singleton fetcher just
// to manage internal state and hide it from the user.
//

CLIArgument* CLI::
classify()
{

    CLI& self = CLI::self();
    ccptr argument = self.argv[self.argi];
    u64 length = strlen(argument);

    // We need only the first and second characters to find which type it is.
    char first = argument[0];
    char second = argument[1];
    if (first == '-' && second == '-') return CLIParameter::parse(self.argi, argument);
    else if (first == '-' && second != '-') return CLISwitch::parse(self.argi, argument); 
    else return CLIValue::parse(self.argi, argument);

}

bool CLI::
parse(i32 argc, cptr* argv)
{

    // Get self and set initial values.
    CLI& self = CLI::self();
    self.argc = argc;
    self.argv = argv;
    self.argi = 0;

    // Classify all the arguments.
    for (i32 i = 0; i < argc; ++i)
    {
        self.argi = i;
        CLIArgument *result = self.classify();
        if (result == nullptr) return false;
        self.arguments.push_back(result);
    }

    // Check all arguments if there was an error type.
    for (i32 i = 0; i < self.arguments.size(); ++i)
    {
        if (self.arguments[i]->get_type() == CLIArgumentType::Error)
        {
            return false; // Error was found.
        }
    }

    return true;

}

bool CLI::
has_flag(char c)
{
    if (!isalpha(c)) return false;
    CLI& self = CLI::self();
    if (!self.arguments.size()) return false;

    for (i32 i = 0; i < self.arguments.size(); ++i)
    {
        if (self.arguments[i]->get_type() == CLIArgumentType::Switch)
        {
            CLISwitch &s = *((CLISwitch*)self.arguments[i]);
            if (s[c] == true)
                return true;
        }
    }

    return false;

}

bool CLI::
has_parameter(ccptr parameter)
{
    CLI& self = CLI::self();
    if (!self.arguments.size()) return false;

    for (i32 i = 0; i < self.arguments.size(); ++i)
    {
        if (self.arguments[i]->get_type() == CLIArgumentType::Parameter)
        {
            CLIParameter &p = *((CLIParameter*)self.arguments[i]);
            if (p.get_name() == parameter) return true;
        }
    }

    return false;
}

void CLI::
header()
{
    std::cout << "|---------------------------------------|--------------------|" << std::endl;
    std::cout << "|     Sigamfox Language Transpiler      | Version 1.0.0      |" << std::endl;
    std::cout << "|     Northern Illinois University      | Feb. 2025          |" << std::endl;
    std::cout << "|---------------------------------------|--------------------|" << std::endl;
}

void CLI::
short_help()
{
    CLI::header();
    std::cout << "Useage: sigmafox [file-name]" << std::endl;
    std::cout << "      The provided file name is the entry-point script for a" << std::endl;
    std::cout << "      given project. This will automatically convert any dependencies" << std::endl;
    std::cout << "      or include files for you." << std::endl;
}

void CLI::
long_help()
{
    CLI::short_help();
}

CLI& CLI::
self()
{

    static CLI *instance = nullptr;
    if (instance == nullptr)
    {
        instance = new CLI();
    }

    return *instance;

}

CLI::
CLI()
{

}

CLI::
~CLI()
{

}
