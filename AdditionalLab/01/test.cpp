#include <iostream>
#include <string.h>
#include <iomanip>
#include <sodium.h>
#include <fstream>
#include <gmpxx.h>

// function to read a file into a string -> i created this with hekp of chatgpt
std::string readFile(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file)
        throw std::runtime_error("Could not open file: " + filePath);

    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

// function to compute the hash of a file
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

// the same function i used in previous laboratory but i change *char to string for making it easier to use
mpz_class computeHash(const std::string &message)
{
    // buffer to store the hash
    unsigned char hash[crypto_hash_sha256_BYTES];

    // compute the length of the message
    const size_t message_length = message.size();

    // compute the hash
    crypto_hash_sha256(hash, reinterpret_cast<const unsigned char *>(message.c_str()), message_length);

    // convert the hash to a GMP integer
    mpz_class hash_GMP;
    libsodium_to_GMP(hash, hash_GMP);

    return hash_GMP;
}

// function to convert a hash to hexadecimal string
std::string hashToHex(const mpz_class &hash)
{
    std::stringstream ss;
    ss << std::hex << hash.get_mpz_t();
    return ss.str();
}

// function to extract the first 4 hex characters of a hash
std::string extractFirstFour(const mpz_class &hash)
{
    std::string hash_hex = hashToHex(hash);
    return hash_hex.substr(0, 4);
}
// function to write the changes to the second file
std::string modifyContent(const std::string &content, const std::string &modification)
{
    return content + modification;
}

// brute force loop
std::string bruteForceMatch(const std::string &fileContent, const std::string &targetHashPrefix)
{
    std::string modifiedContent = fileContent;
    int attempt = 0;

    while (true)
    {
        mpz_class currentHash = computeHash(modifiedContent);
        std::string currentHashPrefix = extractFirstFour(currentHash);
        if (currentHashPrefix == targetHashPrefix)
        {
            return modifiedContent;
        }
        std::string modification = "<!-- attempt: " + std::to_string(attempt) + " -->";
        modifiedContent = modifyContent(fileContent, modification);

        attempt++;
    }
}

// function to write the changes to the second file
void writeFile(const std::string &filePath, const std::string &content)
{
    std::ofstream file(filePath);
    if (!file)
        throw std::runtime_error("Could not open file: " + filePath);

    file << content;
}

int main(const int argc, const char *const argv[])
{
    if (sodium_init() == -1)
        return 1;

    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << "<original_file_path> <changed_file_path>" << std::endl;
        return 2;
    }

    std::string originalFilePath = argv[1];
    std::string changedFilePath = argv[2];

    std::string originalContent = readFile(originalFilePath);
    std::string targetHashPrefix = extractFirstFour(computeHash(originalContent));

    std::string fileToModify = readFile(changedFilePath);

    std::string modifiedContent = bruteForceMatch(fileToModify, targetHashPrefix);

    writeFile(changedFilePath, modifiedContent);

    // print the first four characters of the hash from the original file
    std::cout << "Target hash prefix: " << targetHashPrefix << std::endl;
    // print the first four characters of the hash from the modified file
    std::cout << "Modified hash prefix: " << extractFirstFour(computeHash(modifiedContent)) << std::endl;
    std::cout << std::endl;

    // print the modified content
    std::cout << "Modified content: \n"
              << std::endl
              << modifiedContent << std::endl;

    return 0;
}
