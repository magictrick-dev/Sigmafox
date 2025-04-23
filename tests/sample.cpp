#include <iostream>
#include <complex>
#include <vector>
#include <string>
#include <cstdint>

typedef std::complex<double> complexd;

#include "includes\math.hpp"

static inline void 
print_statement(std::complex<double> stm)
{

    std::cout << stm;

}

inline std::complex<double> 
add_numbers(double a, std::complex<double> b)
{

    std::complex<double> result = a + b;
    add_numbers = result;

}


int
main(int argc, char **argv)
{

    std::complex<double> foo = 8 + 16 - 26 + std::pow(10, 2);
    double bar = 16.0 / ( 2 + 2 ) * ( std::pow(2, ( 1 + 1 )) );
    std::complex<double> baz = 32 - 16 + 4.0 * std::complex<double>(0.0, 26.0);
    std::cout << foo;
    std::cout << bar;
    std::cout << baz;
    std::cout << "\n";
    std::string user_input;
    std::cin >> user_input
    std::cout << user_input;

    {

        int64_t foo = 8 + 16 - 26 + std::pow(10, 2);
        double bar = 16.0 / ( 2 + 2 ) * ( std::pow(2, ( 1 + 1 )) );
        std::complex<double> baz = 32 - 16 + 4.0 * std::complex<double>(0.0, 26.0);
        std::cout << foo;
        std::cout << bar;
        std::cout << baz;
        std::cout << "\n";
        std::string user_input;
        std::cin >> user_input
        std::cout << user_input;
        int64_t idx = 0;
        while (idx <= 1024)
        {

            idx = idx + 1;
            std::cout << idx;

        }

        // This is a ploop statement, but ploop is not supported.
        for (int64_t i = 0; i < 10; i += 1)
        {

            std::complex<double> test = i * add_pair(i + 2.0, 13.0 * std::complex<double>(0.0, 4.0));
            std::cout << test;

        }

        for (int64_t i = 0; i < 10; i += 1)
        {

            std::complex<double> test = i * add_pair(i + 2.0, 13.0 * std::complex<double>(0.0, 4.0));
            std::cout << test;

        }

        if (idx < 100)
        {

            std::complex<double> test = idx * mul_pair(idx + 2.0, 13.0 * std::complex<double>(0.0, 4.0));
            std::cout << "Basic if.";

        }
        else if (idx > 100)
        {

            std::cout << "Chained elseif.";
            example(idx + std::complex<double>(0.0, 4.0));

        }
        else if (1 == 1)
        {

            std::cout << "Fall back case.";

        }


    }

    foo = add_numbers(bar, baz);
    print_statement(foo);

    return 0;

}
