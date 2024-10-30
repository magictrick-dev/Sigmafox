#include <iostream>
#include <string>
#include <environment.h>
#include <platform/filesystem.h>
#include <platform/system.h>
#include <utilities/path.h>
#include <utilities/cli.h>

int
main(int argc, char ** argv)
{

    if (!Sigmafox::CLI::parse(argc, argv))
    {
        std::cout << "CLI parse failed." << std::endl;
    }
    else
    {
        std::cout << "CLI parse succeeded." << std::endl;
        for (u8 i = 0; i < 26; ++i)
        {
            if (Sigmafox::CLI::has_flag('a' + i))
                std::cout << "Flag: " << (char)('a' + i) << " is enabled." << std::endl;
        }
        for (u8 i = 0; i < 26; ++i)
        {
            if (Sigmafox::CLI::has_flag('A' + i))
                std::cout << "Flag: " << (char)('A' + i) << " is enabled." << std::endl;
        }
    }

    return 0;

}
