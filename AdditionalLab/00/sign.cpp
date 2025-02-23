#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <gmpxx.h>
#include <sodium.h>

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

// Function to encrypt/decrypt the message
mpz_class encryptDecryptMessage(const mpz_class &message, const mpz_class &exponent, const mpz_class &mod)
{
    mpz_class result;
    mpz_powm(result.get_mpz_t(), message.get_mpz_t(), exponent.get_mpz_t(), mod.get_mpz_t());
    return result;
}

// Function to compute the hash of a string
mpz_class computeHash(const std::string &message)
{
    // Buffer to store the hash
    unsigned char hash[crypto_hash_sha512_BYTES];

    // Compute the hash
    crypto_hash_sha512(hash, reinterpret_cast<const unsigned char *>(message.data()), message.size());

    // Convert the hash to a GMP integer
    mpz_class hash_GMP;
    libsodium_to_GMP(hash, hash_GMP);

    return hash_GMP;
}

// Function to read a file into a string -> i got this with the help of chatgpt
std::string readFile(const std::string &filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Error: Could not open file " + filePath);
    }

    // Read the entire file into a string
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

// Function to sign the message
mpz_class signMessage(const std::string &message, const mpz_class &d, const mpz_class &N)
{
    // Compute the hash of the message
    mpz_class hash = computeHash(message);

    // Sign the hash
    return encryptDecryptMessage(hash, d, N);
}

// Function to verify the signature
bool verifySignature(const std::string &message, const mpz_class &signature, const mpz_class &e, const mpz_class &N)
{
    // Compute the hash of the message
    mpz_class hashOfMessage = computeHash(message);

    // Decrypt the signature using the public key
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
        std::cerr << "Usage for signing: " << argv[0] << " Sign <file_path> <d> <N>" << std::endl;
        std::cerr << "Usage for verification: " << argv[0] << " Verify <file_path> <signature> <e> <N>" << std::endl;
        return 2;
    }

    std::string mode = argv[1];
    std::string filePath = argv[2];

    try
    {
        // Read the file content
        std::string fileContent = readFile(filePath);

        if (mode == SIGN)
        {
            if (argc != 5)
            {
                std::cerr << "Usage for signing: " << argv[0] << " Sign <file_path> <d> <N>" << std::endl;
                return 2;
            }

            mpz_class d(argv[3]);
            mpz_class N(argv[4]);

            // Sign the file content
            mpz_class signature = signMessage(fileContent, d, N);
            std::cout << "Signature: " << signature << std::endl;
        }
        else if (mode == VERIFY)
        {
            if (argc != 6)
            {
                std::cerr << "Usage for verification: " << argv[0] << " Verify <file_path> <signature> <e> <N>" << std::endl;
                return 2;
            }

            mpz_class signature(argv[3]);
            mpz_class e(argv[4]);
            mpz_class N(argv[5]);

            // Verify the file content
            bool isValid = verifySignature(fileContent, signature, e, N);
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
            std::cerr << "Unknown mode. Use Sign or Verify." << std::endl;
            return 3;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 4;
    }

    return 0;
}
