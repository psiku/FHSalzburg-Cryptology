#pragma once
#include<string>
#include<fstream>
#include<vector>
#include<iostream>

class FileReader
{
private:
	std::string filePath;
public:
	FileReader(std::string filePath) : filePath(filePath) {}
	std::vector<char> readFromFile(std::string pathFile);
};

