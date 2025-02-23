#include <iostream>
#include <fstream>
#include <iomanip>
#include <sodium.h>

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

// this was also created with chatgpt
int main(const int argc, const char *const argv[])
{
    if (sodium_init() == -1)
        return 1;

    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
        return 2;
    }

    const char *file_path = argv[1];
    std::string fileAsString = readFile(file_path);

    // hash the string using sha256
    unsigned char hash[crypto_hash_sha256_BYTES];
    crypto_hash_sha256(hash, reinterpret_cast<const unsigned char *>(fileAsString.c_str()), fileAsString.size());

    // print the hash
    std::cout << "Hash: ";
    for (const auto &byte : hash)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    std::cout << std::endl;

    return 0;
}