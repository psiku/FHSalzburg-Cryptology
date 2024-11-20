#include <iostream>
#include <gmpxx.h>
#include <sodium.h>
#include "dbghelp.h"
#include <string>
#include <iomanip>
#include <vector>

// user modes to choose functionality of program
#define SERVER_GENERATE_PARTIAL_KEY "ServerGeneratePartialKey"
#define CLIENT_GENERATE_PARTIAL_KEY "ClientGeneratePartialKey"
#define SERVER_GENERATE_SESSION_KEY "ServerGenerateSessionKey"
#define CLIENT_GENERATE_SESSION_KEY "ClientGenerateSessionKey"
#define SERVER_ENCRYPT "ServerEncrypt"
#define CLIENT_DECRYPT "ClientDecrypt"

// this function to print hexadecimal strings was created with using CHATGPT for laboratory 3
void print_hex(const unsigned char *data, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)data[i];
    }
}

// Provided function for converting hex strings to arrays
bool HexStringToArray(const std::string &hex_string, unsigned char array[], const size_t array_size)
{
    if (hex_string.length() != 2 * array_size)
        return false;
    for (size_t i = 0; i < array_size; i++)
    {
        const std::string str_part(hex_string.c_str() + 2 * i, 2);
        try
        {
            const auto byte = std::stoul(str_part, nullptr, 16);
            array[i] = byte;
        }
        catch (...)
        {
            return false;
        }
    }
    return true;
}

// partial key generation
void generatePartialKey(const std::string &mode, const mpz_class &p, const mpz_class &g)
{
    mpz_class partialKey;
    mpz_class number;

    // set random number generator
    gmp_randstate_t prng_state;
    gmp_randinit_default(prng_state);
    gmp_randseed_ui(prng_state, time(nullptr));

    // generate number
    mpz_urandomm(number.get_mpz_t(), prng_state, p.get_mpz_t());

    // calculate partial
    mpz_powm(partialKey.get_mpz_t(), g.get_mpz_t(), number.get_mpz_t(), p.get_mpz_t());

    // print partial key
    if (mode == SERVER_GENERATE_PARTIAL_KEY)
    {
        std::cout << "x: " << number << std::endl;
        std::cout << "S: " << partialKey << std::endl;
    }
    else if (mode == CLIENT_GENERATE_PARTIAL_KEY)
    {
        std::cout << "y: " << number << std::endl;
        std::cout << "C: " << partialKey << std::endl;
    }

    gmp_randclear(prng_state);
}

// sesion key generator
void generateSessionKey(const std::string &mode, const mpz_class &p, const mpz_class &partialKey, const mpz_class &number)
{
    mpz_class sessionKey;

    // calculate session key
    mpz_powm(sessionKey.get_mpz_t(), partialKey.get_mpz_t(), number.get_mpz_t(), p.get_mpz_t());

    // convert result to string
    std::string sessionKeyString = sessionKey.get_str();
    const size_t sessionKeySize = sessionKeyString.size();

    // Compute the SHA-256 hash of the session value
    unsigned char hash[crypto_hash_sha256_BYTES];
    crypto_hash_sha256(hash, reinterpret_cast<const unsigned char *>(sessionKeyString.c_str()), sessionKeySize);

    // print session key
    print_hex(hash, sizeof(hash));
    std::cout << std::endl;
}

void serverEncrypt(const std::string &sessionKeyHex, const std::string &message)
{
    // convert sessionkey from hex to byte array
    unsigned char sessionKey[crypto_aead_aes256gcm_KEYBYTES];
    if (!HexStringToArray(sessionKeyHex, sessionKey, crypto_aead_aes256gcm_KEYBYTES))
    {
        std::cerr << "Invalid session key." << std::endl;
        return;
    }

    // constant nonce (all zeroes)
    unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES] = {0};

    // size of message
    size_t messageSize = message.size();

    // prepare buffer for the ciphertext
    unsigned char ciphertext[messageSize + crypto_aead_aes256gcm_ABYTES];
    unsigned long long ciphertext_len;

    // encrypt the message
    if (crypto_aead_aes256gcm_encrypt(ciphertext, &ciphertext_len,
                                      reinterpret_cast<const unsigned char *>(message.c_str()), messageSize,
                                      nullptr, 0, nullptr, nonce, sessionKey) != 0)
    {
        std::cerr << "Encryption failed." << std::endl;
        return;
    }

    // print the ciphertext
    print_hex(ciphertext, ciphertext_len);
    std::cout << std::endl;
}

// i had problems with this function to work so i used chatgpt to help me find mymistakes
// and with the help of the chatgpt i was able to fix the function. He provided me with converting the hex string to byte array and preapring the buffer for the decrypted message
void clientDecrypt(const std::string &ciphertextHex, const std::string &sessionKeyHex)
{
    // convert sessionkey from hex to byte array
    unsigned char sessionKey[crypto_aead_aes256gcm_KEYBYTES];
    if (!HexStringToArray(sessionKeyHex, sessionKey, crypto_aead_aes256gcm_KEYBYTES))
    {
        std::cerr << "Invalid session key." << std::endl;
        return;
    }

    // constant nonce (all zeroes)
    unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES] = {0};

    // convert ciphertext from hex to byte array (CHATGPT help with that)
    size_t ciphertextSize = ciphertextHex.size() / 2;
    std::vector<unsigned char> ciphertext(ciphertextSize);
    if (!HexStringToArray(ciphertextHex, ciphertext.data(), ciphertextSize))
    {
        std::cerr << "Invalid ciphertext." << std::endl;
        return;
    }

    // prepare buffer for decrypted message (CHATGPT help with that)
    std::vector<unsigned char> decrypted(ciphertextSize - crypto_aead_aes256gcm_ABYTES);
    unsigned long long decryptedLen;

    // decrypt the message
    if (crypto_aead_aes256gcm_decrypt(decrypted.data(), &decryptedLen, nullptr,
                                      ciphertext.data(), ciphertextSize,
                                      nullptr, 0, nonce, sessionKey) != 0)
    {
        std::cerr << "Decryption failed: message forged or incorrect key." << std::endl;
        return;
    }

    // print the decrypted message
    std::cout << std::string(decrypted.begin(), decrypted.begin() + decryptedLen) << std::endl;
}

int main(const int argc, const char *const argv[])
{
    if (sodium_init() == -1)
    {
        std::cerr << "Failed to initialize libsodium." << std::endl;
        return 1;
    }

    if (argc < 2 || argc > 4)
    {
        std::cerr << "Description: Handles Diffie-Hellman key exchange (partial and session keys).\n";
        std::cerr << "Usage: " << argv[0] << " <mode> [partialKey privateNumber]\n";
        std::cerr << "Modes:\n";
        std::cerr << "  1. For partial key: " << argv[0] << SERVER_GENERATE_PARTIAL_KEY << " or " << argv[0] << CLIENT_GENERATE_PARTIAL_KEY << "\n";
        std::cerr << "  2. For session key: " << argv[0] << SERVER_GENERATE_SESSION_KEY << " <partialKey> <privateNumber> or " << argv[0] << CLIENT_GENERATE_SESSION_KEY << " <partialKey> <privateNumber>\n";
        std::cerr << "  3. For encryption: " << argv[0] << " " << SERVER_ENCRYPT << " <message> <sessionKey>\n";
        std::cerr << "  4. For decryption: " << argv[0] << " " << CLIENT_DECRYPT << " <ciphertext> <sessionKey>\n";
        return 2;
    }

    const std::string mode = argv[1];

    const char *const g_as_text = "\
    f7e1a085d69b3ddecbbcab5c36b857b97994afbbfa3aea82f\
    9574c0b3d0782675159578ebad4594fe67107108180b44916\
    7123e84c281613b7cf09328cc8a6e13c167a8b547c8d28e0a\
    3ae1e2bb3a675916ea37f0bfa213562f1fb627a01243bcca4\
    f1bea8519089a883dfe15ae59f06928b665e807b552564014\
    c3bfecf492a";

    const mpz_class g(g_as_text, 16);

    const char *const p_as_text = "\
    fd7f53811d75122952df4a9c2eece4e7f611b7523cef4400c\
    31e3f80b6512669455d402251fb593d8d58fabfc5f5ba30f6\
    cb9b556cd7813b801d346ff26660b76b9950a5a49f9fe8047\
    b1022c24fbba9d7feb7c61bf83b57e7c6a8a6150f04fb83f6\
    d3c51ec3023554135a169132f675f3ae2b61d72aeff222031\
    99dd14801c7";

    const mpz_class p(p_as_text, 16);

    if ((mode == SERVER_GENERATE_PARTIAL_KEY || mode == CLIENT_GENERATE_PARTIAL_KEY) && argc == 2)
    {
        // Generate partial key
        generatePartialKey(mode, p, g);
    }
    else if ((mode == SERVER_GENERATE_SESSION_KEY || mode == CLIENT_GENERATE_SESSION_KEY) && argc == 4)
    {
        // Parse partialKey and privateNumber from command-line arguments
        const mpz_class partialKey(argv[2]);
        const mpz_class privateNumber(argv[3]);

        // Generate session key
        generateSessionKey(mode, p, partialKey, privateNumber);
    }
    else if (mode == SERVER_ENCRYPT)
    {
        const std::string message = argv[2];
        const std::string sessionKey = argv[3];
        serverEncrypt(sessionKey, message);
    }
    else if (mode == CLIENT_DECRYPT)
    {
        const std::string ciphertextHex = argv[2];
        const std::string sessionKeyHex = argv[3];
        clientDecrypt(ciphertextHex, sessionKeyHex);
    }
    else
    {
        std::cerr << "Invalid input. Use the following:\n";
        std::cerr << "  1. For partial key: " << argv[0] << SERVER_GENERATE_PARTIAL_KEY << " or " << argv[0] << CLIENT_GENERATE_PARTIAL_KEY << "\n";
        std::cerr << "  2. For session key: " << argv[0] << SERVER_GENERATE_SESSION_KEY << " <partialKey> <privateNumber> or " << argv[0] << CLIENT_GENERATE_SESSION_KEY << " <partialKey> <privateNumber>\n";
        std::cerr << "  3. For encryption: " << argv[0] << " " << SERVER_ENCRYPT << " <message> <sessionKey>\n";
        std::cerr << "  4. For decryption: " << argv[0] << " " << CLIENT_DECRYPT << " <ciphertext> <sessionKey>\n";
        return 3;
    }

    return 0;
}
