// --- Sigmafox CLI Parser Utility ---------------------------------------------
//
// Written by Chris DeJong Oct. 2024 / Northern Illinois University
//
//      The command line utility parses and validates command line arguments
//      and appropriately sets the ApplicationParameters to necessary constants.
//      The end-point function, parse, returns true/false depending if the CLI
//      specification matches the arguments that were passed in.
//      
// CLI Context-Free Grammar
//
//      cli          : (argument)* source_file (argument)*
//      argument     : parameter | switch
//      switch       : "-"(FLAG*)
//      parameter    : "--output-name" PATH | "--output-directory" PATH
//
// The command-line parser converts all CLI arguments (excluding the first, by
// convention) to higher order class objects. These are then parsed and validated,
// modifying ApplicationParameters as needed.
//
// -----------------------------------------------------------------------------
#ifndef SIGAMFOX_UTILITIES_CLI_H
#define SIGAMFOX_UTILITIES_CLI_H
#include <string>
#include <vector>
#include <environment.h>
#include <definitions.h>

namespace Sigmafox
{

    // --- CLIArgument ---------------------------------------------------------
    //
    // Serves as the virtual interface for all CLI argument types.
    //

    class CLIArgument
    {
        public:
            inline          CLIArgument(i32 index, ccptr argument);
            inline virtual ~CLIArgument() = 0;

            inline i32      get_index() const;
            inline ccptr    get_argument() const;

        protected:
            i32     raw_index;
            ccptr   raw_argument;
    };

    inline CLIArgument::
    CLIArgument(i32 index, ccptr argument)
    {
        this->raw_index = index;
        this->raw_argument = argument;
    }

    inline i32 CLIArgument::
    get_index() const
    {
        return this->raw_index;
    }

    inline ccptr CLIArgument::
    get_argument() const
    {
        return this->raw_argument;
    }

    // --- CLISwitch -----------------------------------------------------------
    //
    // Corresponds to the CLI Switch specification, "-[A-Za-z]". Its existence
    // implies true, and ommittance implies false.
    //

    class CLISwitch : public CLIArgument
    {
        public:
            inline          CLISwitch(i32 index, ccptr argument);
            inline virtual ~CLISwitch();
    };

    // --- CLIParameter --------------------------------------------------------
    //
    // Corresponds to the CLI Parameters specification, "--[name]". Usage is
    // dependent on the parser's definition for that specific term. Some parameters
    // act as full-length aliases to switches, others require n-arguments afterwards.
    //
    
    class CLIParameter : public CLIArgument
    {
        public:
            inline          CLIParameter(i32 index, ccptr argument);
            inline virtual ~CLIParameter();
    };

    // --- CLIValue ------------------------------------------------------------
    //
    // If the argument term does not validate to parameter or switch, it is a
    // value. This particular subtype will classify itself based on what it is
    // or be an error.
    //

    enum class CLIValueType
    {
        Error,
        Numerical,
        String,
        Filepath,
    };

    class CLIValue : public CLIArgument
    {

        public:
            inline          CLIValue(i32 index, ccptr argument);
            inline virtual ~CLIValue();

        protected:
            CLIValueType    type;

    };

    inline CLIValue::
    CLIValue(i32 index, ccptr argument) : CLIArgument(index, argument)
    {
        this->type = CLIValueType::Error;
    }

    inline CLIValue::
    ~CLIValue()
    {

    }

    // --- CLI -----------------------------------------------------------------
    //
    // The global container for CLI and the parser implementation.
    //

    class CLI
    {

        public:
            static inline bool  parse(i32 argc, cptr* argv);

        protected:
            inline              CLI();
            inline virtual     ~CLI();
            static inline CLI&  self();

            static inline CLIArgument* classify();
            static inline CLIArgument* classify_switch();
            static inline CLIArgument* classify_parameter();
            static inline CLIArgument* classify_value();

        protected:
            std::vector<CLIArgument*> arguments;
            i32     argi;
            i32     argc;
            cptr   *argv;
            
    };

    inline CLIArgument* CLI::
    classify_switch()
    {

    }

    inline CLIArgument* CLI::
    classify_parameter()
    {

    }

    inline CLIArgument* CLI::
    classify_value()
    {

    }

    inline CLIArgument* CLI::
    classify()
    {

        CLI& self = CLI::self();
        ccptr argument = self.argv[self.argi];
        u64 length = strlen(argument);

        // Arguments must be of 2 length for any classification to be valid.
        if (length < 2)
        {
            CLIValue *error_value = new CLIValue(self.argi, self.argv[self.argi]);
            return error_value;
        }

        // We need only the first and second characters to find which type it is.
        char first = argument[0];
        char second = argument[1];
        if (first == '-' && second == '-') return self.classify_parameter();
        else if (first == '-' && second != '-') return self.classify_switch();
        else return self.classify_value();


    }

    inline bool CLI::
    parse(i32 argc, cptr* argv)
    {

        // Get self and set initial values.
        CLI& self = CLI::self();
        self.argc = argc;
        self.argv = argv;
        self.argi = 0;

        // Classify all the arguments.
        for (i32 i = 1; i < argc; ++i)
        {
            self.argi = i;
            self.arguments.push_back(self.classify());
        }

    }

    inline CLI& CLI::
    self()
    {

        static CLI *instance = nullptr;
        if (instance == nullptr)
        {
            instance = new CLI();
        }

        return *instance;

    }

    inline CLI::
    CLI()
    {

    }

    inline CLI::
    ~CLI()
    {

    }

};

#endif
