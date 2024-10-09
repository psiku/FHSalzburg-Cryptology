#include "OutputFormater.h"
// function to format the output as was requested in exercise description
void OutputFormater::formatOutput(
    std::vector<std::vector<CharacterFrequency>> &sortedData)
{
  int length = sortedData[0].size();

  std::string output = "";

  for (int i = 0; i < length; i++)
  {
    for (std::vector<CharacterFrequency> charFreq : sortedData)
    {
      output += charFreq[i].letter;
      output += ":  ";
      output += std::to_string(charFreq[i].frequency);
      output += "%";

      // add semicolon only if it's not the last vector
      if (&charFreq != &sortedData.back())
      {
        output += "; ";
      }
    }
    output += "\n";
  }

  std::cout << output;
}
