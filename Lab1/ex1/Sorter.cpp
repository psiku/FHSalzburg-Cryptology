#include "Sorter.h"

// comparasion function for sort() method
bool Sorter::comparisonFunction(const CharacterFrequency &first,
                                const CharacterFrequency &second)
{
  return first.frequency >
         second.frequency;
}

// function to sort the vector based on frequency of the characters
void Sorter::sortVector(std::vector<CharacterFrequency> &characters)
{

  std::sort(characters.begin(), characters.end(), Sorter::comparisonFunction);
}
