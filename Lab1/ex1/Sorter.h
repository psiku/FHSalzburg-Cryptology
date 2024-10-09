#pragma once
#include <vector>
#include <algorithm>
#include "FrequencyAnalyzer.h"

class Sorter
{
public:
    // Make the comparison function static
    static bool comparisonFunction(const CharacterFrequency& first, const CharacterFrequency& second);

    // Sort function for sorting character frequencies
    static void sortVector(std::vector<CharacterFrequency>& characters);
};
