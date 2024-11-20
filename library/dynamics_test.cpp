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
        Sigmafox::Dynamic real_result = real_a + real_b + 55.0;
        std::cout << "Result: " << real_result << std::endl;
        real_result += complex<double>(1, -2);
        real_result -= 2.0;
        real_result *= 0.5;
        std::cout << "Result: " << real_result << std::endl;

        Sigmafox::Dynamic complex_a(complex<double>(4, 3));
        Sigmafox::Dynamic complex_b(complex<double>(1, -11));
        Sigmafox::Dynamic complex_result = complex_a + complex_b;
        std::cout << "Result: " << complex_result << std::endl;
        complex_result += 4.0;
        std::cout << "Result: " << complex_result << std::endl;

        // Tracked memory allocator for testing.
        double* my_real = (double*)Sigmafox::memory_alloc(8);
        *my_real = 25.55;
        std::cout << *my_real << std::endl;
        Sigmafox::memory_free(my_real);


    }
    
    std::cout << *Sigmafox::memory_get_stats() << std::endl;

    return 0;
}
