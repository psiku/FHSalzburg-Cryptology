#pragma once
#include <vector>
#include <cmath>
#include <map>

// structure which will be used to sort vector by frequency
struct CharacterFrequency
{
	char letter;
	int frequency;

	CharacterFrequency(char letter, float frequency)
		: letter(letter), frequency(frequency) {}
};

class FrequencyAnalyzer
{
private:
	std::vector<char> fileLetters;

public:
	FrequencyAnalyzer(std::vector<char> &fileLetters) : fileLetters(fileLetters) {}
	std::vector<std::vector<char>> splitData(int keyLength);
	std::vector<CharacterFrequency> caclulateLetterFrequency(std::vector<char> letterVector);
};
