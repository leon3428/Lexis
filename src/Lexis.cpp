#include <iostream>
#include <LexisConfig.h>
#include "Utils.h"

int main(int argc, char const *argv[])
{
    std::cout << argv[0] << "Version: " << LEXIS_VERSION_MAJOR << '.' << LEXIS_VERSION_MINOR << std::endl;
    return 0;
}
