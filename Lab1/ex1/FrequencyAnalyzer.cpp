#include "FrequencyAnalyzer.h"

std::vector<std::vector<char>>
FrequencyAnalyzer::splitData(int keyLength)
{
  std::vector<char> dataVector = this->fileLetters;

  // create varables
  std::vector<std::vector<char>> positionLetters(keyLength);

  int index = 0;

  int size = dataVector.size();

  // push variable to correct vector (1 variable goes to the first vector, 2nd to the second vector, etc.)
  for (int i = 0; i < size; i++)
  {
    positionLetters[index % keyLength].push_back(dataVector[index]);
    index++;
  }

  return positionLetters;
}

std::vector<CharacterFrequency>
FrequencyAnalyzer::caclulateLetterFrequency(std::vector<char> letterVector)
{
  // create frequency_map
  std::map<char, int> frequencyMap;

  // create a vector of CahracterFrequencies
  std::vector<CharacterFrequency> charFreq;

  // total size of vector
  float vectorLength = letterVector.size();

  for (auto &character : letterVector)
  {
    frequencyMap[character] += 1;
  }

  // convert to vector of structure
  for (auto &pair : frequencyMap)
  {
    char letter = pair.first;

    // CHATGPT HELP BY PROVIDING STATIC_CAST
    float freq = (static_cast<float>(pair.second) / vectorLength) * 100;

    int roundedValue = std::round(freq);

    CharacterFrequency character = CharacterFrequency(letter, roundedValue);
    charFreq.push_back(character);
  }

  return charFreq;
}
