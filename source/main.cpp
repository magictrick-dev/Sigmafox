#include <core.h>
#include <cli.h>

#include <cstdio>

using namespace POSY;

int
main(int argc, char ** argv)
{

    // --- Command Line Argument Parsing -------------------------------
    //
    // Handles argument fetching, parsing, and grouping. Determines if
    // user-provided paths are valid. Groups adjacent flags. Slightly
    // over-engineered but designed to be extensible if desired.
    //

    // Construct the CLI arguments.
    cli_node *command_line_args = cli_construct(argc, argv);

    // Cycle through the arguments.
    cli_node *current = command_line_args;
    while (current != NULL)
    {
        printf("%s\n", current->args.value);
        current = current->next;
    }

    // --- Post Application Procedures ---------------------------------
    //
    // Here, anything that needs to get cleaned up will be processed here
    // before main terminates.
    //

    cli_destruct(command_line_args);
    smalloc_output_statistics();
    return 0;

}
