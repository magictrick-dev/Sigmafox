#include <cli.h>
#include <core.h>

namespace POSY
{

    cli_node*
    cli_construct(int argc, char **argv)
    {

        // Contiguous linked-list.
        u64 cli_list_size = sizeof(cli_node) * argc;
        cli_node *root = smalloc_array(cli_node, argc);

        u64 index = 0;
        while (index < argc)
        {

            if (index == 0)
                root[index].prev = NULL;
            else if (index < argc - 1)
                root[index].next = &root[index + 1];
            else
                root[index].next = NULL;

            root[index].args.value = argv[index];
            root[index].args.index = index;

            index++;

        }

        return root;

    }

    void
    cli_destruct(cli_node *root)
    {

        POSY_ASSERT(root != NULL);
        if (root == NULL) return;

        smalloc_free(root);

    }

}
