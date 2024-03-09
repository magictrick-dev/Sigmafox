#ifndef POSY_CLI_H
#define POSY_CLI_H
#include <core.h>

namespace POSY
{  

    // --- CLI Node ----------------------------------------------------
    //
    // A transparent structure which contains information regarding a
    // given CLI argument. It contains pointers to the next nodes in the
    // list (given they're non-NULL pointers) to make traversal and parsing
    // of CLI arguments easier.
    //

    struct cli_argument;
    typedef struct
    {

        struct
        {
            const char *value;
            u32         index;
        } args;
        
        cli_argument *next;
        cli_argument *prev;

    } cli_node;

    // --- CLI Argument API --------------------------------------------
    //
    // Since this program requires CLI input, we need a system for handling
    // flags in the event that we want to extend the functionality beyond
    // passing file paths into the CLI interface. For now, it just spits
    // back a doubly-linked list of arguments.
    //
    // cli_construct()
    //      Creates the CLI argument list from argc/argv from main and
    //      returns a pointer to the node list. While the return value is
    //      a pointer with potential to fail to allocate, it guarantees that
    //      as long as it doesn't fail, it will return a pointer.
    //
    // cli_destruct()
    //      Frees the CLI argument list from memory. You don't actually
    //      need to call this, but as good practice, here it is.
    //
    //

    cli_node*   cli_construct(int argc, char ** argv);
    void        cli_destruct(cli_node *root);

};

#endif
