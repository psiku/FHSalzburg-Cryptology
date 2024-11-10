// exercise PKC_01
#include <iostream>
#include <gmpxx.h>

// function to decrypt/encrypt the message
mpz_class encryptDecryptMessage(const mpz_class &message, const mpz_class &exponent, const mpz_class &mod)
{
    mpz_class result;

    mpz_powm(result.get_mpz_t(), message.get_mpz_t(), exponent.get_mpz_t(), mod.get_mpz_t());

    return result;
}

int main(const int argc, const char *const argv[])
{
    if (argc < 4)
    {
        std::cerr << "Description: RSA Encryption/Decryption" << std::endl;
        std::cerr << "Usage: " << argv[0] << " <message> <exponent> <modulus>" << std::endl;
        std::cerr << "  m: Messege to be encrypted or decrypted (single number)" << std::endl;
        std::cerr << "  e/d: Exponent used for encryption or decryption" << std::endl;
        std::cerr << "  N: modulus" << std::endl;
        return 1;
    }

    // storing variables : message, exponent, modulus N and the result
    mpz_class message(argv[1]);

    mpz_class exponent(argv[2]);

    mpz_class N(argv[3]);

    // calculate the result

    mpz_class result = encryptDecryptMessage(message, exponent, N);

    std::cout << result << std::endl;

    return 0;
}
