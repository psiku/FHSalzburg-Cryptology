#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <gmpxx.h>

class FileHandler
{
private:
    std::string inputFileName;
    std::string outputFileName;

public:
    FileHandler(std::string inputFileName, std::string outputFileName) : inputFileName(inputFileName), outputFileName(outputFileName) {}
    mpz_class readFromFile();
    void writeToFile(const mpz_class &number);
};

#endif // FILE_HANDLER_H