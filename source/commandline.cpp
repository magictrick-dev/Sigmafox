#include <commandline.h>

void
sigmafox_cli_print_header()
{
    
    printf("\n");
    printf("          ____ ___ ____ __  __    _    _____ _____  __\n");
    printf("         / ___|_ _/ ___|  \\/  |  / \\  |  ___/ _ \\ \\/ /\n");
    printf("         \\___ \\| | |  _| |\\/| | / _ \\ | |_ | | | \\  / \n");
    printf("          ___) | | |_| | |  | |/ ___ \\|  _|| |_| /  \\ \n");
    printf("         |____/___\\____|_|  |_/_/   \\_\\_|   \\___/_/\\_\\\n\n");
    printf("--------------------------------------------------------------------\n\n");
    printf("  Sigmafox           / Northern Illinois University\n");
    printf("  Primary Maintainer / Chris DeJong\n");
    printf("                     / Z1836870@students.niu.edu\n");
    printf("                     / magiktrikdev@gmail.com\n\n");
    printf("--------------------------------------------------------------------\n\n");
}

void
sigmafox_cli_print_description()
{
    printf("  A parser for the COSY scripting language to C++.\n");
    printf("  To properly use Sigmafox, provide a list of source files to\n");
    printf("  transpile and any additional parameters (if supported).\n\n");
    printf("--------------------------------------------------------------------\n\n");
}

void
sigmafox_cli_print_useage()
{
    printf("  Useage: Sigmafox [source files...]\n");
    printf("          ./Sigmafox test.foxy includes/ex.foxy includes/eg.foxy\n\n");
}


