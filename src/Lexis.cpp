#include <iostream>
#include <LexisConfig.hpp>
#include "Utils.hpp"
#include "RegexSyntaxTree.hpp"
#include "Dfa.hpp"

int main(int argc, char const *argv[])
{
    std::cout << argv[0] << " Version: " << LEXIS_VERSION_MAJOR << '.' << LEXIS_VERSION_MINOR << std::endl;

    //std::string regex = "\\|\\$|\\|";
    //std::string regex = "\\\\\\\\";
    //std::string regex = "(\\\\|\\|)*(\\*|\\t)";
    //std::string regex = "\\$\\$$";
    //std::string regex = "\\\\\\\t";
    std::string regex = "\\_\\_*";

    regex = "(" + regex + ")";
    regex += char(17);

    RegexSyntaxTree t(regex);

    t.print();

    return 0;
}
