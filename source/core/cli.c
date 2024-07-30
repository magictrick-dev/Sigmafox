#include <core/cli.h>

b32 
cli_parser_match_string_caseless(cc64 string, cc64 match_to)
{

    u32 s = 0;
    b32 f = false;
    while(tolower(string[s]) == tolower(match_to[s]))
    {
        if (string[s] == '\0' && match_to[s] != '\0')
            break;
        if (string[s] != '\0' && match_to[s] == '\0')
            break;
        if (string[s] == '\0' && match_to[s] == '\0')
        {
            f = true;
            break;
        }
        s++;
    }
    
    return f;

}

void 
cli_parser_get_next_token(runtime_parameters *parameters, cli_token *token)
{

    assert(parameters != NULL);
    assert(token != NULL);

    int current_index = parameters->arg_current++;
    if (current_index >= parameters->arg_count)
    {
        token->type = CLI_TOKEN_EOA;
        token->index = current_index;
        return;
    }

    cc64 argument = parameters->arguments[current_index];
    char current = argument[0];

    if (isdigit(current))
    {
        
        cli_token_type type = CLI_TOKEN_NUMBER;
        i32 eos = 0;
        while (argument[eos] != '\0') eos++;

        u64 modifier = 1;
        if (tolower(argument[eos] == 'b'))
        {

            switch (argument[eos - 1])
            {
                case 'k':
                {

                    modifier = 1024;

                } break;

                case 'm':
                {

                    modifier = 1024 * 1024;

                } break;

                case 'g':
                {

                    modifier = 1024 * 1024 * 1024;

                } break;
                
                default:
                {
                    
                    token->type = CLI_TOKEN_UNDEFINED;
                    token->index = current_index;
                    return;

                } break;
            };

        }

        u64 value = atoll(argument) * modifier;
        token->type = CLI_TOKEN_NUMBER;
        token->value = value;
        return;

    }

    else if (current == '-')
    {

        char follower = argument[1];
        if (follower == '-')
        {
            cc64 string_token = argument + 2;

            if (cli_parser_match_string_caseless(string_token, "output-name"))
            {
                token->type = CLI_TOKEN_ARGUMENT_OUTPUT_NAME;
                token->index = current_index;
                return;
            }

            if (cli_parser_match_string_caseless(string_token, "output-directory"))
            {
                token->type = CLI_TOKEN_ARGUMENT_OUTPUT_DIR;
                token->index = current_index;
                return;
            }

            if (cli_parser_match_string_caseless(string_token, "compile"))
            {
                token->type = CLI_TOKEN_ARGUMENT_COMPILE;
                token->index = current_index;
                return;
            }

            if (cli_parser_match_string_caseless(string_token, "help"))
            {
                token->type = CLI_TOKEN_ARGUMENT_HELP;
                token->index = current_index;
                return;
            }

            if (cli_parser_match_string_caseless(string_token, "trim-comments"))
            {
                token->type = CLI_TOKEN_ARGUMENT_TRIM_COMMENTS;
                token->index = current_index;
                return;
            }

            if (cli_parser_match_string_caseless(string_token, "memory-limit-size"))
            {
                token->type = CLI_TOKEN_ARGUMENT_MEM_LIMIT;
                token->index = current_index;
                return;
            }

            if (cli_parser_match_string_caseless(string_token, "string-pool-limit"))
            {
                token->type = CLI_TOKEN_ARGUMENT_POOL_LIMIT;
                token->index = current_index;
                return;
            }

            token->type = CLI_TOKEN_UNDEFINED_ARGUMENT;
            token->index = current_index;
            return;

        }

        else
        {
            token->type = CLI_TOKEN_SWITCH;
            token->index = current_index;
            return;
        }

    }

}

b32 
command_line_parse(runtime_parameters *parameters)
{

    // Initialize all options/flags. Unused flags are marked -1.
    for (u32 i = 0; i < 26; ++i) parameters->flags[i] = -1;
    parameters->options.compile = 0;
    parameters->options.help = 0;
    parameters->options.trim_comments = 0;

    return true;
}

