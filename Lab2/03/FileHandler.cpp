#include "FileHandler.h"

mpz_class FileHandler::readFromFile()
{
    std::string filename = this->inputFileName;

    // open as binary file (it doesn't wokr with text files)
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(1);
    }

    mpz_class number = 0;
    char c;
    while (file.get(c))
    {
        // this line was created with help of CHATGPT, cause I didn't know how to convert do char * 256^(position) without having a position
        number = number * 256 + static_cast<unsigned char>(c);
    }

    file.close();
    return number;
}

void FileHandler::writeToFile(const mpz_class &number)
{
    std::string filename = this->outputFileName;

    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(1);
    }

    mpz_class n = number;
    std::vector<char> chars;

    // xtract bytes from the number
    while (n > 0)
    {
        // converting one byte to char
        char c = static_cast<char>(n.get_ui() % 256);
        chars.push_back(c);
        // shifting the number so we can get rid of extracted character
        n /= 256;
    }

    // character are added in reverse order, so we need to reverse the vector
    std::reverse(chars.begin(), chars.end());

    // writing to file char by char
    for (const char &c : chars)
    {
        file.put(c);
    }

    file.close();
}