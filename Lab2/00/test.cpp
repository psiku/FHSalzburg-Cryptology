// exercise PKC_00
#include <iostream>
#include <gmpxx.h>

int main(const int argc, const char *const argv[])
{
    if (argc < 3)
    {
        std::cerr << "Description: Adds two numbers" << std::endl;
        std::cerr << "Usage: " << argv[0] << " <a> <b>" << std::endl;
        std::cerr << "  a: First number to add" << std::endl;
        std::cerr << "  b: Second number to add" << std::endl;
        return 1;
    }
    const mpz_class a(argv[1]), b(argv[2]);
    const mpz_class c = a * b;

    std::cout << a << " * " << b << " = " << c << std::endl;
    return 0;
}
