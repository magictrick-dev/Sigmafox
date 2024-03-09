#include <core.h>
#include <cli.h>

#include <cstdio>

int
main(int argc, char ** argv)
{

    for (size_t i = 0; i < argc; ++i)
        printf("%s\n", argv[i]);

    return 0;

}
