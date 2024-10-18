#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

class FileHandler
{
private:
    std::string inputFilePath;
    std::string outputFilePath;

public:
    FileHandler(std::string inputFilePath, std::string outputFilePath) : inputFilePath(inputFilePath), outputFilePath(outputFilePath) {}
    std::vector<char> readFromFile();
    void writeToFile(const std::string &output);
};
