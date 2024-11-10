#include "EncrypterDecrypter.h"

mpz_class EncrypterDecrypter::encryptDecryptMessage(const mpz_class &message, const mpz_class &exponent, const mpz_class &mod)
{
    mpz_class result;
    mpz_powm(result.get_mpz_t(), message.get_mpz_t(), exponent.get_mpz_t(), mod.get_mpz_t());
    return result;
}