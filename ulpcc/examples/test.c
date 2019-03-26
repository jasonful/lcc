#include <ulp_c.h>

struct {
    unsigned x;
    unsigned y;
    unsigned z;
} point, point2;

unsigned rgu[5];
unsigned rgu2[5];
unsigned *addr_rgu;
unsigned iequalj_test, iltj_test, igtj_test, ilej_test, igej_test, ptr_test, array_test;

unsigned i = 0;
unsigned j = 0;
unsigned *p;

unsigned xor_test, compl_test, neg_test;
signed s;

void entry()
{
    point.x = 8;
    point.y = 9;
    point2 = point;

    addr_rgu = &(rgu[0]);

    rgu[i] = 100;
    rgu[i + 1] = 101;
    rgu[i + 2] = 102;
    rgu[i + 3] = 103;
    rgu[i + 4] = 104;
    for (i = 0; i < 5; i++)
    {
        rgu[i] = i + 700;
    }
    array_test = (rgu[1] == 701);

    i = 200;
    for (p = &rgu2[0]; p < rgu2 + 5; p++)
    {
        *p = i++;
    }
    ptr_test = (rgu2[1] == 201);

    i = j;

    if (i == j)
    {
        iequalj_test = 1;
    }
    else
    {
        iequalj_test = 0;
    }

    ilej_test = (i <= j);
    igej_test = (i >= j);

    i = 0; j = 1;
    iltj_test = (i < j); 
    i = 5;

    j = 4;
    igtj_test = (i > j);

    i = 3;
    xor_test = ((i ^ 5) == 6);

    i = 3;
    compl_test = (~i == 0xFFFC);

    s = 7;
    neg_test = ((unsigned)-s == 0xFFF9);
}
