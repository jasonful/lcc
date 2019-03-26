/* Sieve of Eratosthenes */
#include <ulp_c.h>

#define SQROOT 10
#define MAX (SQROOT * SQROOT)
unsigned primes[MAX];

void entry()
{
    unsigned i, factor, product;

    for (i = 0; i < MAX; i++)
        primes[i] = 1;

    for (factor = 2; factor <= SQROOT; factor++) 
        for (product = factor << 1; product < MAX; product += factor)
            primes[product] = 0;

    wake_when_ready();
}
