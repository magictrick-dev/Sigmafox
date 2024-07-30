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

            switch (tolower(argument[eos - 1]))
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
cli_parser_match_argument(runtime_parameters *parameters)
{

    cli_token argument_token = {0};
    cli_parser_get_next_token(parameters, &argument_token);

    switch (argument_token.type)
    {

        case CLI_TOKEN_ARGUMENT_OUTPUT_NAME:
        {

        } break;

        case CLI_TOKEN_ARGUMENT_OUTPUT_DIR:
        {

        } break;

        case CLI_TOKEN_ARGUMENT_HELP:
        {

        } break;

        case CLI_TOKEN_ARGUMENT_COMPILE:
        {

        } break;

        case CLI_TOKEN_ARGUMENT_TRIM_COMMENTS:
        {

        } break;

        case CLI_TOKEN_ARGUMENT_MEM_LIMIT:
        {

        } break;

        case CLI_TOKEN_ARGUMENT_POOL_LIMIT:
        {

        } break;

        case CLI_TOKEN_SWITCH:
        {

        } break;

        default:
        {
            return CLI_PARSER_BREAK;
        } break;
    }

    return CLI_PARSER_CONTINUE;

}

b32
cli_parser_match_default(runtime_parameters *parameters)
{

    // Handle all arguments / flags. 
    cli_parser_code current_code;
    while ((current_code = cli_parser_match_argument(parameters)) == CLI_PARSER_CONTINUE);
    if (current_code == CLI_PARSER_ERROR) return CLI_PARSER_ERROR;

    // Is the current argument a file?
    cli_token source_token = {0};
    cli_parser_get_next_token(parameters, &source_token);

    if (source_token.type != CLI_TOKEN_FILE)
    {

        // If we prematurely reach EOA, then we just exit.
        if (source_token.type == CLI_TOKEN_EOA)
        {
            printf("Unexpected end of command line arguments.\n");
            return false;
        }
        
        // However, if we don't have a file argument, we give the appropriate
        // message. We expect a file at this point, so if it isn't we consider
        // it an error.
        printf("Unexpected command line argument at %d: '%s'\n",
                source_token.index,
                parameters->arguments[source_token.index]);
        return CLI_PARSER_ERROR;

    }

    parameters->source_file_path = parameters->arguments[source_token.index];

    // Handle remaining arguments / flags.
    while ((current_code = cli_parser_match_argument(parameters)) == CLI_PARSER_CONTINUE);
    if (current_code == CLI_PARSER_ERROR) return false;
    if (parameters->arg_current < parameters->arg_count)
    {
        for (i32 idx = parameters->arg_current; idx < parameters->arg_count; ++idx)
            printf("Unexpected command line argument at %d: '%s'\n",
                    idx,
                    parameters->arguments[idx]);
        return CLI_PARSER_ERROR;
    }

    return CLI_PARSER_BREAK;

}

b32 
command_line_parse(runtime_parameters *parameters)
{

    // Initialize all options/flags. Unused flags are marked -1.
    for (u32 i = 0; i < 26; ++i) parameters->flags[i] = -1;
    parameters->options.compile = 0;
    parameters->options.help = 0;
    parameters->options.trim_comments = 0;

    // Parse until all arguments are handled or we encounter an error.
    if (cli_parser_match_default(parameters) == CLI_PARSER_ERROR) return false;
    return true;
}

