#ifndef ENCRYPTERDECRYPTER_H
#define ENCRYPTERDECRYPTER_H

#include <gmpxx.h>

class EncrypterDecrypter
{
public:
    mpz_class encryptDecryptMessage(const mpz_class &message, const mpz_class &exponent, const mpz_class &mod);
};

#endif // ENCRYPTERDECRYPTER_H