#include <iostream>
#include <gmpxx.h>
#include <sodium.h>
#include "dbghelp.h"
#include "certhelp.h"

// modes that we can use
#define SERVER_READ_PUBLIC_KEY "ServerReadPublicKey"
#define CLIENT_READ_PRIVATE_KEY "ClientReadPrivateKey"
#define SERVER_CREATE_CHALLENGE "ServerCreateChallenge"
#define CLIENT_CREATE_RESPONSE "ClientCreateResponse"
#define SERVER_VERIFY_RESPONSE "ServerVerifyResponse"

// code from laboratory 3
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

void serverReadPublicKey(const char *pathToCertFile)
{
    mpz_class e, N;

    if (!ReadPublicKeyFromFile(pathToCertFile, e, N))
    {
        std::cerr << "Failed to read public key from certificate!" << std::endl;
        return;
    }

    std::cout << "Public key: (" << e.get_str() << ", " << N.get_str() << ")" << std::endl;
}

void clientReadPrivateKey(const char *pathToKeyFile)
{
    mpz_class d, N;

    if (!ReadPrivateKeyFromFile(pathToKeyFile, d, N))
    {
        std::cerr << "Failed to read private key from PEM file!" << std::endl;
        return;
    }

    std::cout << "Private key: (" << d.get_str() << ", " << N.get_str() << ")" << std::endl;
}

void serverCreateChallange(const char *e, const char *N)
{
    // change e and N to mpz_class
    mpz_class e_mpz(e);
    mpz_class N_mpz(N);

    // initialize random number and challenge
    mpz_class number;
    mpz_class challenge;

    // generate random number -> code from ex01 of the same laboratory (generatePartialKey)
    gmp_randstate_t prng_state;
    gmp_randinit_default(prng_state);
    gmp_randseed_ui(prng_state, time(nullptr));

    // generate number
    mpz_urandomm(number.get_mpz_t(), prng_state, N_mpz.get_mpz_t());

    // compute challenge
    mpz_powm(challenge.get_mpz_t(), number.get_mpz_t(), e_mpz.get_mpz_t(), N_mpz.get_mpz_t());

    // print challenge and the random number
    std::cout << "Challenge: " << challenge.get_str() << std::endl;
    std::cout << "Random number: " << number.get_str() << std::endl;
}

void clientCreateResponse(const char *challenge, const char *d, const char *N)
{
    // create mpz_class from the challenge, d and N
    mpz_class challenge_mpz(challenge);
    mpz_class d_mpz(d);
    mpz_class N_mpz(N);

    // initialize response
    mpz_class response;

    // decrypt the challenge
    mpz_powm(response.get_mpz_t(), challenge_mpz.get_mpz_t(), d_mpz.get_mpz_t(), N_mpz.get_mpz_t());

    // Convert decrypted to string for hashing
    std::string resposneStr = response.get_str();

    // Compute the hash of the decrypted string
    mpz_class responseHash = computeHash(resposneStr.c_str());

    // Output the response
    std::cout << "Response: " << responseHash.get_str() << std::endl;
}

void serverVerifyResponse(const char *h_str, const char *r_str)
{
    // convert h and r to mpz_class
    mpz_class h(h_str), r(r_str), h_prime;

    // convert r to string for hashing
    std::string r_str_gmp = r.get_str();

    // hash r
    unsigned char hash[crypto_hash_sha512_BYTES];
    crypto_hash_sha512(hash, (const unsigned char *)r_str_gmp.c_str(), r_str_gmp.size());

    // convert hash to mpz_class
    libsodium_to_GMP(hash, h_prime);

    // compare h and h'
    if (h == h_prime)
    {
        std::cout << "Authentication successful" << std::endl;
    }
    else
    {
        std::cout << "Authentication failed" << std::endl;
    }
}

int main(const int argc, const char *const argv[])
{
    if (sodium_init() == -1)
        return 1;

    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <mode> <pathToFile>" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    if (mode == SERVER_READ_PUBLIC_KEY)
    {
        const char *pathToFile = argv[2];
        serverReadPublicKey(pathToFile);
    }
    else if (mode == CLIENT_READ_PRIVATE_KEY)
    {
        const char *pathToFile = argv[2];
        clientReadPrivateKey(pathToFile);
    }
    else if (mode == SERVER_CREATE_CHALLENGE)
    {
        if (argc != 4)
        {
            std::cerr << "Usage: " << argv[0] << " " << SERVER_CREATE_CHALLENGE << " <e> <N>" << std::endl;
            return 2;
        }
        const char *e = argv[2];
        const char *N = argv[3];
        serverCreateChallange(e, N);
    }
    else if (mode == CLIENT_CREATE_RESPONSE)
    {
        if (argc != 5)
        {
            std::cerr << "Usage: " << argv[0] << " " << CLIENT_CREATE_RESPONSE << " <challenge> <d> <N>" << std::endl;
            return 2;
        }
        const char *challenge = argv[2];
        const char *d = argv[3];
        const char *N = argv[4];
        clientCreateResponse(challenge, d, N);
    }
    else if (mode == SERVER_VERIFY_RESPONSE)
    {
        if (argc != 4)
        {
            std::cerr << "Usage: " << argv[0] << " " << SERVER_VERIFY_RESPONSE << " <h> <r>" << std::endl;
            return 2;
        }
        const char *h = argv[2];
        const char *r = argv[3];
        serverVerifyResponse(h, r);
    }
    else
        std::cerr << "Unknown mode" << std::endl;

    return 0;
}
