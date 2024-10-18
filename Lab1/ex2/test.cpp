#include <iostream>
#include <vector>
#include <unordered_map>
#include "FileHandler.h"
#include "MessageDecrypter.h"

// After testing the key for the input_en.txt file is : ITS
int main(const int argc, const char *const argv[])
{
    if (argc < 3)
    {
        std::cerr << "Description: Encrypts the contents of a file" << std::endl;
        std::cerr << "Usage: " << argv[0] << " <in> <out> <key>" << std::endl;
        std::cerr << "  in: Input file to encrypt" << std::endl;
        std::cerr << "  out: Encrypted output file" << std::endl;
        std::cerr << " key : Key to decrypt the message " << std::endl;
        return 1;
    }

    // values from command line
    std::string inputFilePath = argv[1];
    std::string outputFilePath = argv[2];
    std::string key = argv[3];

    // check the key
    if (key.empty())
    {
        std::cerr << "Error: Empty key provided." << std::endl;
        return 1;
    }

    for (char &keyChar : key)
    {
        if (!isalpha(keyChar))
        {
            std::cerr << "Error: Key must consist only of alphabetical values." << std::endl;
            return 1;
        }
    }

    // read from file
    FileHandler fileHandler = FileHandler(inputFilePath, outputFilePath);
    std::vector<char> letters = fileHandler.readFromFile();

    if (letters.empty())
    {
        std::cerr << "Error: Failed to read from input file." << std::endl;
        return 1;
    }

    // decrypt the message
    MessageDecrypter decrypter = MessageDecrypter();

    std::unordered_map<char, int> dict = decrypter.createDict();
    std::unordered_map<int, char> reverseDict = decrypter.createReverseDict();

    std::string decryptedMessage = decrypter.decryptMessage(letters, key, dict, reverseDict);

    // write to file
    fileHandler.writeToFile(decryptedMessage);

    return 0;
}
