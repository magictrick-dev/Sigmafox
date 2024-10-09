#include <iostream>
#include "dynamics.h"

int
main(int argc, char ** argv)
{

    {

        // Dynamic casting to double.
        Sigmafox::Dynamic integer(12);
        std::cout << integer.value<double>() << std::endl;

        // Operator overloading.
        Sigmafox::Dynamic real_a(4);
        Sigmafox::Dynamic real_b(4);
        Sigmafox::Dynamic real_result = real_a + real_b;
        std::cout << "Result: " << real_result << std::endl;

        // Tracked memory allocator for testing.
        double* my_real = (double*)Sigmafox::memory_alloc(8);
        *my_real = 25.55;
        std::cout << *my_real << std::endl;
        Sigmafox::memory_free(my_real);

        std::cout << *Sigmafox::memory_get_stats() << std::endl;

    }
    

    return 0;
}
