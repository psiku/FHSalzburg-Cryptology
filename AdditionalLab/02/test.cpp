#include <iostream>
#include <string.h>
#include <iomanip>
#include <sodium.h>
#include <fstream>
#include <gmpxx.h>
#include <map>

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
void writeFile(const std::string &filePath, const std::string &content)
{
    std::ofstream file(filePath);
    if (!file)
        throw std::runtime_error("Could not open file: " + filePath);

    file << content;
}

// function to write the changes to the the file
std::string modifyContent(const std::string &content, const std::string &modification)
{
    return content + modification;
}

// collision loop
void findCollisionUsingMaps(const std::string &originalFileContent, const std::string &fakeFileContent,
                            std::string &modifiedFirstFile, std::string &modifiedSecondFile)
{
    std::map<std::string, std::string> firstFileHashMap, secondFileHashMap;
    int attempt = 0;

    while (true)
    {
        // modification which wll be added
        std::string firstFileModification = "<!-- original attempt: " + std::to_string(attempt) + " -->";
        std::string secondFileModification = "<!-- fake attempt: " + std::to_string(attempt) + " -->";

        std::string firstFileVersion = modifyContent(originalFileContent, firstFileModification);
        std::string secondFileVersion = modifyContent(fakeFileContent, secondFileModification);

        // computing hash prefix
        std::string firstFileHash = extractFirstFour(computeHash(firstFileVersion));
        std::string secondFileHash = extractFirstFour(computeHash(secondFileVersion));

        // add values to maps
        firstFileHashMap[firstFileHash] = firstFileModification;
        secondFileHashMap[secondFileHash] = secondFileModification;

        // this if statement was created with copilot -> it suggested me a way how to look if one hash is in the other map
        if (firstFileHashMap.find(secondFileHash) != firstFileHashMap.end())
        {

            std::string firstFileCorrepsondingModification = firstFileHashMap[secondFileHash];
            std::string secondFileCorrespondingModification = secondFileModification;

            // Modify the files again with these corresponding modifications
            modifiedFirstFile = modifyContent(originalFileContent, firstFileCorrepsondingModification);
            modifiedSecondFile = modifyContent(fakeFileContent, secondFileCorrespondingModification);
            return;
        }

        if (secondFileHashMap.find(firstFileHash) != secondFileHashMap.end())
        {
            // Collision found: Retrieve corresponding modifications
            std::string secondFileCorrespondingModification = secondFileHashMap[firstFileHash];
            std::string firstFileCorrepsondingModification = firstFileModification;

            // Modify the files again with these corresponding modifications
            modifiedFirstFile = modifyContent(originalFileContent, firstFileCorrepsondingModification);
            modifiedSecondFile = modifyContent(fakeFileContent, secondFileCorrespondingModification);
            return;
        }

        ++attempt;
    }
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
    std::string fakeFilePath = argv[2];

    // Read both files
    std::string originalFileContent = readFile(originalFilePath);
    std::string fakeFileContent = readFile(fakeFilePath);

    // Prepare for collision
    std::string modifiedFirstFile, modifiedSecondFile;
    findCollisionUsingMaps(originalFileContent, fakeFileContent, modifiedFirstFile, modifiedSecondFile);

    // Write the modified files back
    writeFile(originalFilePath, modifiedFirstFile);
    writeFile(fakeFilePath, modifiedSecondFile);

    // output that the collision is found
    std::cout << "Collision found!" << std::endl;
    std::cout << "Original file hash prefix: " << extractFirstFour(computeHash(modifiedFirstFile)) << std::endl;
    std::cout << "Fake file hash prefix: " << extractFirstFour(computeHash(modifiedSecondFile)) << std::endl;

    // print the modified content
    std::cout << "Modified content: \n"
              << std::endl;
    std::cout << modifiedFirstFile << std::endl;

    // print the modified content of changed file
    std::cout << "Modified content of changed file: \n"
              << std::endl;
    std::cout << modifiedSecondFile << std::endl;

    return 0;
}
