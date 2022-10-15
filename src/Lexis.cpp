#include <iostream>
#include <LexisConfig.hpp>
#include "Utils.hpp"
#include "RegexSyntaxTree.hpp"
#include "Dfa.hpp"

int main(int argc, char const *argv[])
{
    std::cout << argv[0] << " Version: " << LEXIS_VERSION_MAJOR << '.' << LEXIS_VERSION_MINOR << std::endl;

    //RegexSyntaxTree t("ab|cd|ef(gh|ij)*ab|de*d");
    //RegexSyntaxTree t("a|bcdef*g*h*i*(((j)))|k");
    RegexSyntaxTree t("(a|b)*abb#"); //+ char(17)
    //RegexSyntaxTree t("$|$a|a$");
    //RegexSyntaxTree t("(a|b)((c|d)*e)*");

    //t.print();

    Dfa dfa;
    t.exportDfa(dfa);
    return 0;
}
