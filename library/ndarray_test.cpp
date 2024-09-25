#include <iostream>
#include "ndarray.h"

class Test
{
    public:
        Test() { std::cout << "Test construct." << std::endl; };
        ~Test() { std::cout << "Test destruct." << std::endl; };
};

int
main(int argc, char ** argv)
{

#define DIMENSION_Z 4
#define DIMENSION_Y 4
#define DIMENSION_X 2

    {
        NDArray<float> floats_array({32});
        for (size_t idx = 0; idx < floats_array.size(); ++idx)
            floats_array[{idx}] = idx + 1;
    }

    {
        NDArray<float> floats_array({DIMENSION_Z, DIMENSION_Y, DIMENSION_X});
        float number = 0.0f;
        for (size_t z = 0; z < DIMENSION_Z; ++z)
            for (size_t y = 0; y < DIMENSION_Y; ++y)
                for (size_t x = 0; x < DIMENSION_X; ++x)
                    floats_array[{z, y, x}] = number++;

        for (size_t z = 0; z < DIMENSION_Z; ++z)
            for (size_t y = 0; y < DIMENSION_Y; ++y)
                for (size_t x = 0; x < DIMENSION_X; ++x)
                    printf("%f\n", floats_array[{z, y, x}]);
    }


    return 0;

}

