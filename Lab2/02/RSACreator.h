#ifndef RSA_CREATOR_H
#define RSA_CREATOR_H

#include <iostream>
#include <gmpxx.h>
#include <ctime>
#include <vector>
#include <stdexcept>

class RSACreator
{
private:
    mp_size_t bitSize;
    size_t keySize;

public:
    RSACreator(mp_size_t bitSize, size_t keySize);
    mpz_class generatePrime(gmp_randstate_t randomState);
    mpz_class computeModulus(mpz_class firstPrime, mpz_class secondPrime);
    std::vector<mpz_class> generateTwoDifferentPrimes(gmp_randstate_t randomState);
    mpz_class generateTotientFunction(mpz_class firstPrime, mpz_class secondPrime);
    mpz_class selectPublicExponent(mpz_class totient, gmp_randstate_t randomState);
    mpz_class computePrivateExponent(mpz_class e, mpz_class totient);
    void printKeys(gmp_randstate_t randomState);
};

#endif // RSA_CREATOR_H