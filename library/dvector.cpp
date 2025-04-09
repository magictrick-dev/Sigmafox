#if defined(SF_DVECTOR_UNIT_TEST) && SF_DVECTOR_UNIT_TEST == 1
#include <chrono>
#include <string>
#include <iostream>
#include <functional>
#include "dvector.hpp"

class HighResolutionTimer
{

    public:
        void start()
        {
            start_time = std::chrono::high_resolution_clock::now();
        }

        double stop()
        {
            auto end_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = end_time - start_time;
            return duration.count(); // Returns the time in seconds
        }

        double measure(std::function<void()> func)
        {
            start();
            func();
            auto result = stop();
            return result;
        }

    private:
        std::chrono::high_resolution_clock::time_point start_time;
};

void sample_runtime()
{

    dvector<double, 8> a_vector = { 1.0, 2.0, 3.0, 4.0, 1.0, 2.0, 3.0, 4.0 };
    dvector<double, 8> b_vector = { 4.0, 3.0, 2.0, 1.0, 4.0, 3.0, 2.0, 1.0 };

    for (size_t i = 0; i < 1024; ++i)
    {


        b_vector *= 2;
        a_vector += b_vector;
        a_vector -= (b_vector / 2.0);
        a_vector += b_vector;

    }

}

int 
main(int argc, char ** argv)
{

    if (argc < 2) return -1;
    std::cout << "Performing dvector benchmark..." << std::endl;

    double minimum_time = 10000000000000.0;
    double maximum_time = 0.0;
    double average_time = 0.0;
    for (int i = 0; i < std::stoi(argv[1]); ++i)
    {

        HighResolutionTimer timer;
        double result = timer.measure(sample_runtime);
        minimum_time = std::min(result, minimum_time);
        maximum_time = std::max(result, maximum_time);
        average_time += result;

    }

    average_time /= std::stoi(argv[1]);

    std::cout << "Test: " << TEST_NAME << std::endl;
    std::cout << "Average runtime: " << average_time << "s" << std::endl;
    std::cout << "Minimum runtime: " << minimum_time << "s" << std::endl;
    std::cout << "Maximum runtime: " << maximum_time << "s" << std::endl;

    return 0;

}

#endif

