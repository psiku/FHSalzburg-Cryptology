#include "MessageDecrypter.h"

std::unordered_map<char, int> MessageDecrypter::createDict()
{
    std::unordered_map<char, int> dict;
    for (char ch = 'A'; ch <= 'Z'; ++ch)
    {
        dict[ch] = ch - 'A';
    }
    return dict;
}

std::unordered_map<int, char> MessageDecrypter::createReverseDict()
{
    std::unordered_map<int, char> reverseDict;
    for (char ch = 'A'; ch <= 'Z'; ++ch)
    {
        reverseDict[ch - 'A'] = ch;
    }
    return reverseDict;
}

std::string MessageDecrypter::decryptMessage(std::vector<char> &letters, std::string &key, std::unordered_map<char, int> &lettersToNumber, std::unordered_map<int, char> &numbersToLetters)
{
    // value initialization:
    int keyIndex = 0;
    int keyLength = key.size();
    std::string finalMessage;

    // decrypt every letter
    for (char character : letters)
    {
        if (isalpha(character))
        {
            // take correct position from key
            char keyChar = key[keyIndex % keyLength];

            // change letters to numbers
            int keyValue = lettersToNumber.at(toupper(keyChar));
            int characterValue = lettersToNumber.at(toupper(character));

            // add integers and change it back to the char
            int decryptedCharacterValue = (characterValue - keyValue + 26) % 26;
            char decryptedCharacter = numbersToLetters.at(decryptedCharacterValue);

            finalMessage += decryptedCharacter;
            keyIndex++;
        }
        else
        {
            finalMessage += character;
        }
    }

    return finalMessage;
}
