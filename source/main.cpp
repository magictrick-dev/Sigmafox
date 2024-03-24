// -----------------------------------------------------------------------------
//  ____ ___ ____ __  __    _    _____ _____  __
// / ___|_ _/ ___|  \/  |  / \  |  ___/ _ \ \/ /
// \___ \| | |  _| |\/| | / _ \ | |_ | | | \  / 
//  ___) | | |_| | |  | |/ ___ \|  _|| |_| /  \
// |____/___\____|_|  |_/_/   \_\_|   \___/_/\_\
//                                             
// -----------------------------------------------------------------------------
// Sigmafox             / Northern Illinois University
// Primary Maintainer   / Chris DeJong
//                      / Z1836870@students.niu.edu
//                      / magiktrikdev@gmail.com
// -----------------------------------------------------------------------------

#include <core.h>
#include <commandline.h>
#include <source.h>
#include <lexer.h>

bool
cli_validate_arguments(int argc, char ** argv)
{

    // Provides the user the necessary information to use the program.
    if (argc <= 1)
    {
        sigmafox_cli_print_header();
        sigmafox_cli_print_description();
        sigmafox_cli_print_useage();
        return false;
    }

    // Cycles through all the arguments and determines if they're valid
    // file paths. If any of the file paths are invalid, fast-exit and
    // return false and indicate to the user that the path is invalid.
    for (size_t idx = 1; idx < argc; ++idx)
    {
        if (!sigmafox_file_exists(argv[idx]))
        {
            sigmafox_cli_print_header();
            sigmafox_cli_print_description();
            sigmafox_cli_print_useage();
            printf("  Error: The file %s was not found.\n\n", argv[idx]);
            return false;
        }
    }

    return true;

}

// --- Entry Point -------------------------------------------------------------
//
// Initiates the parser routine.
//

class Shape
{
    public:
        virtual int             area() = 0;
        virtual const char *    name() = 0;
};

class Square : public Shape
{
    public:
        inline Square(int s) : s(s) {};
        inline int area() { return s * s; };
        inline const char * name() { return n; }

    protected:
        int s;
        const char * n = "Square";
};

class Rectangle : public Shape
{
    public:
        inline Rectangle(int w, int h) : w(w), h(h) {};
        inline int area() { return w * h; };
        inline const char * name() { return n; }

    protected:
        int w;
        int h;
        const char * n = "Rectangle";
};

class Triangle : public Shape
{
    public:
        inline Triangle(int h, int b) : h(h), b(b) {}
        inline int area() { return (int)((0.5f) * (f32)(b * h)); };
        inline const char * name() { return n; }

    protected:
        int h;
        int b;
        const char * n = "Triangle";
};

int
main(int argc, char ** argv)
{

    // --- CLI Validation ------------------------------------------------------
    //
    // Validates the command line arguments.
    //
    
    if (!cli_validate_arguments(argc, argv)) return RETURN_STATUS_INIT_FAIL;
    sigmafox_cli_print_header();

    // NOTE(Chris): This is a temporary check, we are only supporting one
    //              input file at a time.
    if (argc >= 3)
    {
        sigmafox_cli_print_useage();
        printf("  Error: This is a temporary error; only one source file may\n");
        printf("         be submitted at time.\n\n");
        return RETURN_STATUS_INIT_FAIL;
    }

    // --- Source File Loading -------------------------------------------------
    // 
    // Loads all the source files the user provided into memory and then does a
    // pre-parse on them to break them up into line-by-line segments.
    //

    memory_arena test_arena = {};
    sigmafox_memarena_create(&test_arena, SF_MEGABYTES(64));

    Shape* shape_list[3] = { NULL, NULL, NULL };

    Square *s = new (&test_arena) Square(10);
    Rectangle *r = new (&test_arena) Rectangle(3, 4);
    Triangle *t = new (&test_arena) Triangle(10, 2);

    shape_list[0] = s;
    shape_list[1] = r;
    shape_list[2] = t;

    for (size_t i = 0; i < 3; ++i)
    {
        int area = shape_list[i]->area();
        const char * name = shape_list[i]->name();
        printf("Shape is %s : Area is %i\n", name, area);
    }

    delete s;
    delete r;
    delete t;

    // --- Cleanup -------------------------------------------------------------
    //
    // Any necessary cleanup is here before exit.
    //

    return RETURN_STATUS_SUCCESS;

}
