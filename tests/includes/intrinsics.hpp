#ifndef TESTS_INCLUDES_INTRINSICS_FOX
#define TESTS_INCLUDES_INTRINSICS_FOX
#include <iostream>
#include <complex>
#include <vector>
#include <string>
#include <cstdint>

typedef std::complex<double> complexd;

inline std::complex<double> 
add_pair(double a, std::complex<double> b)
{

    std::complex<double> result = a + b;
    add_pair = result;

}

inline std::complex<double> 
mul_pair(double a, std::complex<double> b)
{

    std::complex<double> result = a * b;
    mul_pair = result;

}

#endif
