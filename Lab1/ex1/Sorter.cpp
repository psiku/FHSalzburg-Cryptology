#include "Sorter.h"

// Define the static comparison function
bool Sorter::comparisonFunction(const CharacterFrequency &first,
                                const CharacterFrequency &second) {
  return first.frequency >
         second.frequency; // Sort in descending order of frequency
}

// Sort vector using the comparison function
void Sorter::sortVector(std::vector<CharacterFrequency> &characters) {
  // Use the static comparison function
  std::sort(characters.begin(), characters.end(), Sorter::comparisonFunction);
}
