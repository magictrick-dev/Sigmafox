#include <iostream>
#include "dynamics.h"

int
main(int argc, char ** argv)
{

    {

        Sigmafox::Dynamic uninitialized;
        Sigmafox::Dynamic integer(12);
        std::cout << integer << std::endl;

        Sigmafox::Dynamic real_a(4);
        Sigmafox::Dynamic real_b(4);

    }
    

    return 0;
}
