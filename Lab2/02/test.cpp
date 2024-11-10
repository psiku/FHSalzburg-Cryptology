// exercise PKC_02
#include "RSACreator.h"
#include <gmpxx.h>

int main()
{
    // initializating randomState for all random operations
    gmp_randstate_t randState;

    gmp_randinit_default(randState);
    gmp_randseed_ui(randState, time(NULL));

    // size for prime numbers
    mp_size_t bitSize = 1024;

    // keyLength of N
    size_t keySize = 2048;

    RSACreator rsaCreator(bitSize, keySize);

    rsaCreator.printKeys(randState);

    // clear the randState
    gmp_randclear(randState);

    return 0;
}
