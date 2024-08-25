// Generated C++ using Sigmafox Ver. 0.3.0A
// Northern Illinois University
#include <iostream>
#include <string>


int
main(int argc, char **argv)
{

    int a = 4;
    int b = 6;
    int result = a + b;

    {

        int a = 7;
        int b = 3;
        int result = a + b;

    };

    int c = 0;
    while (a < 4)
    {

        c = c + a;
        a = a + 1;

    };

    int d = 0;
    for (int idx = 0; idx < a; idx += 2)
    {

        // Cache iterator to restore initial value at end-of-loop.
        int __idx_iter = idx;

        d = d + ( idx * b );


        idx = __idx_iter;

    };

    if (a != 0)
    {

        int hello = 20;

    }

    else if (1)
    {

        int world = 40;

    }


}
