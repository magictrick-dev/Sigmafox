#ifndef TESTS_INCLUDES_MATH_FOX
#define TESTS_INCLUDES_MATH_FOX
#include <iostream>
#include <complex>
#include <vector>
#include <string>
#include <cstdint>

typedef std::complex<double> complexd;

#include "intrinsics.hpp"

static inline void 
example(std::complex<double> a)
{

    std::complex<double> idx = a;
    while (idx <= 10)
    {

        idx = idx + 1;

    }


}

#endif
