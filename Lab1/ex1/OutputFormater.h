#pragma once
#include "FrequencyAnalyzer.h"
#include <iostream>
#include <string>
#include <vector>

class OutputFormater {

public:
  void formatOutput(std::vector<std::vector<CharacterFrequency>> &sortedData);
};
