#include <iostream>
#include <runtime.h>
#include <core/utilities/allocator.h>

int
main(int argc, char ** argv)
{

    // Invoke initialization procedure. If init fails, immediately call shutdown
    // with -1 to indicate a fail.
    int init_code = environment_initialize(argc, argv);
    if (init_code != STATUS_CODE_SUCCESS)
    {
        printf("Initialization failed with status code: %i\n", init_code);
        environment_shutdown(init_code);
        return init_code;
    }

    // Process the runtime. The returned runtime code is then fed back to shutdown.
    int runtime_code = environment_runtime();
    environment_shutdown(runtime_code);

    // Memory statistics and post-run debug information. This is automatically
    // compiled out when the debug build is turned off.
#if defined(SIGMAFOX_DEBUG_BUILD)

    printf("\n\n");
    printf("------------------------------------------------------------\n");
    printf("              Runtime Debug Diagnostics\n");
    printf("------------------------------------------------------------\n");

    memory_alloc_stats memory_stats = {};
    if (!memory_statistics(&memory_stats))
        std::cout << "      Warning, potential memory mismatch." << std::endl;

    std::cout << "      Current Allocation Useage : " << memory_stats.current_useage << " bytes" << std::endl;
    std::cout << "      Peak Allocation Useage    : " << memory_stats.peak_useage << " bytes" << std::endl;
    std::cout << "      Allocation Size Total     : " << memory_stats.memory_allocated << " bytes" << std::endl;
    std::cout << "      Free Size Total           : " << memory_stats.memory_freed << " bytes" << std::endl;
    std::cout << "      Allocation Calls          : " << memory_stats.alloc_calls << std::endl;
    std::cout << "      Free Calls                : " << memory_stats.free_calls << std::endl;

    printf("------------------------------------------------------------\n");

#endif

}
