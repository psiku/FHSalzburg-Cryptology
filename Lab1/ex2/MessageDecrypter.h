#pragma once

#include <unordered_map>
#include <string>
#include <vector>

class MessageDecrypter
{
public:
    MessageDecrypter() {}
    std::unordered_map<char, int> createDict();
    std::unordered_map<int, char> createReverseDict();
    std::string decryptMessage(std::vector<char> &letters, std::string &key, std::unordered_map<char, int> &lettersToNumber, std::unordered_map<int, char> &numbersToLetters);
};
