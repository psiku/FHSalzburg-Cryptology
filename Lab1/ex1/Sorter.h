#pragma once
#include <vector>
#include <algorithm>
#include "FrequencyAnalyzer.h"

class Sorter
{
public:
    static bool comparisonFunction(const CharacterFrequency &first, const CharacterFrequency &second);

    static void sortVector(std::vector<CharacterFrequency> &characters);
};
