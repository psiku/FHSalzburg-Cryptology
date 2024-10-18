#include "FileHandler.h"

std::vector<char> FileHandler::readFromFile()
{
    // create variable for file
    std::ifstream file(this->inputFilePath);

    char ch;
    std::vector<char> letters;

    // return error if you can't open the file
    if (!file.is_open())
    {
        std::cerr << "Error : coulnd't open the file" << std::endl;
        return letters;
    }

    // get all letters
    while (file.get(ch))
    {

        letters.push_back(ch);
    }

    // close the file
    file.close();

    // return vector of only letters
    return letters;
}

void FileHandler::writeToFile(const std::string &output)
{
    std::ofstream file(this->outputFilePath);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open the output file for writing" << std::endl;
        return;
    }

    file << output; // Write the entire output at once

    file.close();
    std::cout << "Successfully written to the output file!" << std::endl;
}
