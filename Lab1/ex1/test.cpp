// test.cpp
//
#include "FileReader.h"
#include "FrequencyAnalyzer.h"
#include "OutputFormater.h"
#include "Sorter.h"
#include <vector>

int main(const int argc, const char *const argv[])
{
  if (argc < 2)
  {
    std::cerr << "Description: Encrypts the contents of a file" << std::endl;
    std::cerr << "Usage: " << argv[0] << " <in> <out>" << std::endl;
    std::cerr << "  in: Input file to encrypt" << std::endl;
    return 1;
  }

  std::string filePath = argv[1];

  FileReader fileReader = FileReader(filePath);

  std::vector<char> letters = fileReader.readFromFile();

  FrequencyAnalyzer frequencyAnalyzer = FrequencyAnalyzer(letters);

  std::vector<std::vector<char>> posLet =
      frequencyAnalyzer.splitData(3);

  std::vector<std::vector<CharacterFrequency>> sortedData;

  Sorter sorter;
  OutputFormater fomater;

  // for each vector of letters calculate the frequency of the letters and push it to the vector of vectors
  for (int i = 0; i < 3; i++)
  {
    std::vector<CharacterFrequency> keyPositionVector =
        frequencyAnalyzer.caclulateLetterFrequency(posLet[i]);
    sorter.sortVector(keyPositionVector);

    sortedData.push_back(keyPositionVector);
  }

  // print the result
  fomater.formatOutput(sortedData);

  return 0;
}
