#include <iostream>
#include <string.h>
#include <iomanip>
#include <gmpxx.h>
#include <sodium.h>
#include "dbghelp.h"

// FLAGS FOR MEANING OF PROGRAM
#define VERIFY "Verify"
#define SIGN "Sign"

template <size_t N> // Number of bytes (array size)

void libsodium_to_GMP(const unsigned char (&libsodium_value)[N], mpz_class &GMP_value)
{
    GMP_value = 0;
    for (const auto &libsodium_byte : libsodium_value)
    {
        GMP_value *= 256;
        GMP_value += libsodium_byte;
    }
}

// function to decrypt/encrypt the message
mpz_class encryptDecryptMessage(const mpz_class &message, const mpz_class &exponent, const mpz_class &mod)
{
    mpz_class result;

    mpz_powm(result.get_mpz_t(), message.get_mpz_t(), exponent.get_mpz_t(), mod.get_mpz_t());

    return result;
}

mpz_class computeHash(const char *const &message)
{

    // buffer to store the hash
    unsigned char hash[crypto_hash_sha512_BYTES];

    // compute the length of the message
    const size_t message_length = strlen(message);

    // compute the hash
    crypto_hash_sha512(hash, reinterpret_cast<const unsigned char *>(message), message_length);

    // convert the hash to a GMP integer
    mpz_class hash_GMP;
    libsodium_to_GMP(hash, hash_GMP);

    return hash_GMP;
}

mpz_class signMessage(const char *const &message, const mpz_class &d, const mpz_class &N)
{
    // first we have to compute the hash of the message
    mpz_class hash = computeHash(message);

    // we are make a signature out of hash
    mpz_class signature;

    signature = encryptDecryptMessage(hash, d, N);

    return signature;
}

bool verifySignature(const char *const &message, const mpz_class &signature, const mpz_class &e, const mpz_class &N)
{
    // first we compute the hash of the message
    mpz_class hashOfMessage = computeHash(message);

    // now we are decrypting the signature with public key
    mpz_class decryptedSignature = encryptDecryptMessage(signature, e, N);

    return decryptedSignature == hashOfMessage;
}

int main(int argc, const char *const argv[])
{
    if (sodium_init() == -1)
    {
        std::cerr << "Libsodium initialization failed." << std::endl;
        return 1;
    }

    if (argc < 5)
    {
        std::cerr << "Usage for signing: " << argv[0] << " Sign <message> <d> <N>" << std::endl;
        std::cerr << "Usage for verification: " << argv[0] << " Verify <message> <signature> <e> <N>" << std::endl;
        std::cerr << "<message> : " << " Message you want to sign " << std::endl;
        std::cerr << "<d> : " << " Private key " << std::endl;
        std::cerr << "<N> : " << " Modulus " << std::endl;
        std::cerr << "<e> : " << " Public exponent " << std::endl;
        std::cerr << "<signature> : " << " Signature you want to verify " << std::endl;
        return 2;
    }

    std::string mode = argv[1];
    const char *const message = argv[2];

    if (mode == SIGN)
    {
        if (argc != 5)
        {
            std::cerr << "Usage for signing: " << argv[0] << " Sign <message> <d> <N>" << std::endl;
            std::cerr << "<message> : " << " Message you want to sign " << std::endl;
            std::cerr << "<d> : " << " Private key " << std::endl;
            std::cerr << "<N> : " << " Modulus " << std::endl;
            return 2;
        }

        mpz_class d(argv[3]);
        mpz_class N(argv[4]);

        mpz_class signature = signMessage(message, d, N);
        std::cout << signature << std::endl;
    }
    else if (mode == VERIFY)
    {
        if (argc != 6)
        {
            std::cerr << "Usage for verification: " << argv[0] << " Verify <message> <signature> <e> <N>" << std::endl;
            std::cerr << "<message> : " << " Message you want to sign " << std::endl;
            std::cerr << "<signature> : " << " Signature you want to verify " << std::endl;
            std::cerr << "<e> : " << " Public exponent " << std::endl;
            std::cerr << "<N> : " << " Modulus " << std::endl;
            return 2;
        }

        mpz_class signature(argv[3]);
        mpz_class e(argv[4]);
        mpz_class N(argv[5]);

        bool isValid = verifySignature(message, signature, e, N);
        if (isValid)
        {
            std::cout << "Signature valid." << std::endl;
        }
        else
        {
            std::cout << "Signature invalid." << std::endl;
        }
    }
    else
    {
        std::cerr << "Unknown mode. Use Sign OR Verify" << std::endl;
        return 3;
    }

    return 0;
}