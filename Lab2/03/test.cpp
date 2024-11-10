// exercise PKC_03
#include <iostream>
#include <gmpxx.h>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "EncrypterDecrypter.h"
#include "FileHandler.h"

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        std::cerr << "Description: RSA Encryption/Decryption" << std::endl;
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file> <e / d> <N>" << std::endl;
        std::cerr << "  input_file: File with messegae to encrypt / decrypt" << std::endl;
        std::cerr << "  output_file: File to write the result" << std::endl;
        std::cerr << "  e/d: Exponent used for encryption or decryption" << std::endl;
        std::cerr << "  N: modulus" << std::endl;
        return 1;
    }

    // variables from command line
    std::string input_file(argv[1]);
    std::string output_file(argv[2]);
    mpz_class exponent(argv[3]);
    mpz_class N(argv[4]);

    EncrypterDecrypter encrypterDecrypter;
    FileHandler fileHandler(input_file, output_file);

    // Reading the message from the input file
    mpz_class message = fileHandler.readFromFile();

    // Encrypting or decrypting the message using the RSA formula
    mpz_class result = encrypterDecrypter.encryptDecryptMessage(message, exponent, N);

    // Writing the result to the output file
    fileHandler.writeToFile(result);

    return 0;
}
