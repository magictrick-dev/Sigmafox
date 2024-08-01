#include <runtime.h>
#include <platform/system.h>
#include <core/utilities/allocator.h>

int
main(int argc, char ** argv)
{

    // Invoke initialization procedure. If init fails, immediately call shutdown
    // with -1 to indicate a fail.
    int init_code = environment_initialize(argc, argv);
    if (init_code == STATUS_CODE_HELP)
    {
        return 0;
    }
    else if (init_code != STATUS_CODE_SUCCESS)
    {
        printf("-- Initialization failed with status code: %i\n", init_code);
        environment_shutdown(init_code);
        return init_code;
    }

    // Process the runtime. The returned runtime code is then fed back to shutdown.
    int runtime_code = environment_runtime();
    environment_shutdown(runtime_code);


}
