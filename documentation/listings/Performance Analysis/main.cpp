#include <cstdio>
#include "system.h"

void
report_time_statistics()
{

    size_t frequency = system_timestamp_frequency();
    size_t start = system_timestamp();
    size_t end = 0;
    size_t elapsed = 0;
    size_t rd_start = system_cpustamp(); 
    size_t rd_stop = 0;
    while (elapsed < frequency)
    {
        end = system_timestamp();
        rd_stop = system_cpustamp();
        elapsed = end - start;
    }

    printf("OS Frequency            : %lld\n", frequency);
    printf("OS Start                : %lld\n", start);
    printf("OS End                  : %lld\n", end);
    printf("OS Elapsed              : %lld\n", elapsed);

    double os_time = (double)elapsed / (double)frequency;
    printf("OS Time                 : %.9f second(s)\n", os_time);

    size_t rd_frequency = rd_stop - rd_start;
    double cpu_time = (double)(rd_stop - rd_start) / (double)(rd_frequency);
    printf("CPU Frequency (Guessed) : %lld\n", rd_frequency);
    printf("CPU Time (Guessed)      : %.9f second(s)\n", cpu_time);

}

int
main(int argc, char ** argv)
{

    report_time_statistics();

}
