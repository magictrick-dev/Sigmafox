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

    std::cout << "Hello, World." << std::endl;
    for (int i = 0; i < argc; ++i)
    {
        std::cout << argv[i] << std::endl;
    }

    char cwd[260];
    u32 cwd_str_size = file_current_working_directory(260, cwd);
    std::cout << "Current working directory: " << cwd << std::endl;

    char output_path[260];
    std::string test_path = cwd;
    test_path += "./../Ninetails/source/././platform";
    file_canonicalize_path(260, output_path, test_path.c_str());
    std::cout << "Test canonicalized path: " << output_path << std::endl;

    Sigmafox::Filepath wd = Sigmafox::Filepath::cwd();
    std::cout << "Current Working Directory: " << wd << std::endl;
    std::cout << "      Length: " << wd.size() << std::endl;
    std::cout << "    Capacity: " << wd.capacity() << std::endl;
    std::cout << "Path is file?: " << wd.is_valid_file() << std::endl;
    std::cout << "Path is directory?: " << wd.is_valid_directory() << std::endl;
    std::cout << "Path is valid?: " << wd.is_valid() << std::endl;

    std::string tpath = wd.c_str();
    tpath += "/././../../Development/Sigmafox/source/./../README.md";
    Sigmafox::Filepath canonp(tpath.c_str());
    std::cout << "Raw path: " << canonp << std::endl;
    canonp.canonicalize();
    std::cout << "Canon path: " << canonp << std::endl;
    std::cout << "Path is file?: " << canonp.is_valid_file() << std::endl;
    std::cout << "Path is directory?: " << canonp.is_valid_directory() << std::endl;
    std::cout << "Path is valid?: " << canonp.is_valid() << std::endl;

    Sigmafox::Filepath bad_path("C:\\Development\\Sigamfox\\NoExists");
    bad_path.canonicalize();
    std::cout << "Canon path: " << bad_path << std::endl;
    std::cout << "Path is file?: " << bad_path.is_valid_file() << std::endl;
    std::cout << "Path is directory?: " << bad_path.is_valid_directory() << std::endl;
    std::cout << "Path is valid?: " << bad_path.is_valid() << std::endl;

    return 0;
}
