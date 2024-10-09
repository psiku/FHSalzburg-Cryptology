#include "FileReader.h"

std::vector<char> FileReader::readFromFile()
{
  // create variable for file
  std::ifstream file(this->filePath);

  char ch;
  std::vector<char> encryptedLetters;

  // return error if you can't open the file
  if (!file.is_open())
  {
    std::cerr << "Error : coulnd't open the file" << std::endl;
    return encryptedLetters;
  }

  // get only alphabetical character
  while (file.get(ch))
  {
    // check if the character is alphabetcal
    if (std::isalpha(ch))
    {
      encryptedLetters.push_back(ch);
    }
  }

  // close the file
  file.close();

  // return vector of only letters
  return encryptedLetters;
}
