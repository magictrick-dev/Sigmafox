#include <iostream>
#include "dynamics.h"

int
main(int argc, char ** argv)
{

    {
        Dynamic real_a(4);
        Dynamic real_b(4);
        Dynamic result = real_a.value<double>() + real_b.value<double>();
        std::cout << result.value<double>() << std::endl;
    }
    

    return 0;
}
