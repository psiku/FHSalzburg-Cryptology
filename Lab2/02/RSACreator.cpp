#include "RSACreator.h"

RSACreator::RSACreator(mp_size_t bitSize, size_t keySize)
{
    this->bitSize = bitSize;
    this->keySize = keySize;
}

mpz_class RSACreator::generatePrime(gmp_randstate_t randomState)
{
    mpz_class randomNumber;
    mpz_class primeNumber;

    // generate a random number
    mpz_urandomb(randomNumber.get_mpz_t(), randomState, this->bitSize);

    // get the next prime after the random number
    mpz_nextprime(primeNumber.get_mpz_t(), randomNumber.get_mpz_t());

    return primeNumber;
}

mpz_class RSACreator::computeModulus(mpz_class firstPrime, mpz_class secondPrime)
{
    return firstPrime * secondPrime;
}

std::vector<mpz_class> RSACreator::generateTwoDifferentPrimes(gmp_randstate_t randomState)
{
    std::vector<mpz_class> primes;
    mpz_class firstPrime = this->generatePrime(randomState);
    mpz_class secondPrime;

    // do while prime numbers are different
    do
    {
        secondPrime = this->generatePrime(randomState);
    } while (firstPrime == secondPrime);

    primes.push_back(firstPrime);
    primes.push_back(secondPrime);

    return primes;
}

mpz_class RSACreator::generateTotientFunction(mpz_class firstPrime, mpz_class secondPrime)
{
    return (firstPrime - 1) * (secondPrime - 1);
}

mpz_class RSACreator::selectPublicExponent(mpz_class totient, gmp_randstate_t randomState)
{
    mpz_class e;
    mpz_class gcd;

    // generate a random number between 2 and φ(N) while their gcd is equal to 1
    do
    {
        mpz_urandomm(e.get_mpz_t(), randomState, totient.get_mpz_t());
        e += 2; // add two because we do not want to have 0 or 1

        // calculate gcd
        mpz_gcd(gcd.get_mpz_t(), e.get_mpz_t(), totient.get_mpz_t());

    } while (gcd != 1 && e < totient - 1);

    return e;
}

mpz_class RSACreator::computePrivateExponent(mpz_class e, mpz_class totient)
{
    mpz_class d;
    if (mpz_invert(d.get_mpz_t(), e.get_mpz_t(), totient.get_mpz_t()) == 0)
    {
        throw std::runtime_error("Inverse does not exist.");
    }
    return d;
}

void RSACreator::printKeys(gmp_randstate_t randomState)
{
    std::vector<mpz_class> primes = this->generateTwoDifferentPrimes(randomState);
    mpz_class firstPrime = primes[0];
    mpz_class secondPrime = primes[1];

    mpz_class modulus = this->computeModulus(firstPrime, secondPrime);
    mpz_class totient = this->generateTotientFunction(firstPrime, secondPrime);
    mpz_class e = this->selectPublicExponent(totient, randomState);
    mpz_class d = this->computePrivateExponent(e, totient);

    std::cout << "Public key: (" << e << ", " << modulus << ")" << std::endl;
    std::cout << "Private key: (" << d << ", " << modulus << ")" << std::endl;
}
