#include <sodium.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <cstring>

#define SIGN "Sign"
#define VERIFY "Verify"

// this function to print hexadecimal strings was created with using CHATGPT
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

// Also ChatGOT helped me with choosing between pointers and references. Cause i ahd some errors, and after changing pointers to references or other way around, it worked.
void sign(const char *message, int message_len)
{
    // generate key pair
    unsigned char public_key[crypto_sign_PUBLICKEYBYTES];
    unsigned char secret_key[crypto_sign_SECRETKEYBYTES];
    crypto_sign_keypair(public_key, secret_key);

    // sign the message
    unsigned char signed_message[crypto_sign_BYTES + message_len];
    unsigned long long signed_message_len;
    crypto_sign(signed_message, &signed_message_len,
                reinterpret_cast<const unsigned char *>(message), message_len,
                secret_key);

    // output signed message and public key
    std::cout << "Signed message: ";
    print_hex(signed_message, signed_message_len);
    std::cout << std::endl;

    std::cout << "Public key: ";
    print_hex(public_key, crypto_sign_PUBLICKEYBYTES);
    std::cout << std::endl;
}

void verify(const std::string &signed_message_hex, const std::string &public_key_hex)
{
    // length of signed message -> each byte is represented by 2 hex characters
    size_t signed_message_len = signed_message_hex.length() / 2;

    unsigned char signed_message[signed_message_len];
    unsigned char public_key[crypto_sign_PUBLICKEYBYTES];

    // convert hex strings to binary arrays
    if (!HexStringToArray(signed_message_hex, signed_message, signed_message_len) ||
        !HexStringToArray(public_key_hex, public_key, crypto_sign_PUBLICKEYBYTES))
    {
        std::cerr << "Invalid input format." << std::endl;
        return;
    }

    // array to store the verified message
    unsigned char verified_message[signed_message_len];
    unsigned long long verified_message_len;

    // verify the signed message
    if (crypto_sign_open(verified_message, &verified_message_len,
                         signed_message, signed_message_len,
                         public_key) != 0)
    {
        std::cout << "Signature invalid." << std::endl;
    }
    else
    {
        std::cout << "Signature valid." << std::endl;
    }
}

int main(int argc, char *argv[])
{
    if (sodium_init() == -1)
    {
        std::cerr << "Libsodium initialization failed." << std::endl;
        return 1;
    }

    if (argc < 3)
    {
        std::cerr << "Usage for signing: " << argv[0] << " Sign <message>" << std::endl;
        std::cerr << "Usage for verification: " << argv[0] << " Verify <signed_message> <public_key>" << std::endl;
        return 2;
    }

    std::string mode = argv[1];

    if (mode == SIGN)
    {
        if (argc != 3)
        {
            std::cerr << "Usage: " << argv[0] << " Sign <message>" << std::endl;
            return 2;
        }

        // get message from command line
        const char *message = argv[2];
        size_t message_len = strlen(message);

        // sign the message
        sign(message, message_len);
    }
    else if (mode == VERIFY)
    {
        if (argc != 4)
        {
            std::cerr << "Usage: " << argv[0] << " Verify <signed_message> <public_key>" << std::endl;
            return 2;
        }

        std::string signed_message_hex = argv[2];
        std::string public_key_hex = argv[3];

        // verify the signed message
        verify(signed_message_hex, public_key_hex);
    }
    else
    {
        std::cerr << "Unknown mode. Use Sign or Verify." << std::endl;
        return 3;
    }

    return 0;
}
