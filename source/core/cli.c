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

    int current_index = parameters->arg_current;
    if (current_index >= parameters->arg_count)
    {
        token->type = CLI_TOKEN_EOA;
        token->index = current_index;
        return;
    }

    cc64 argument = parameters->arguments[current_index];
    char current = argument[0];

    if (current == '-')
    {

        char follower = argument[1];
        if (follower == '-')
        {
            
        }

    }

}

b32 
command_line_parse(runtime_parameters *parameters, memory_arena *arena,
        int argument_count, char ** arguments)
{

    

    return true;
}

