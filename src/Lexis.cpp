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
    std::string regex = "(a|b)*abb";
    regex += char(17);
    //regex += "|aba";
    //regex += char(17);
    //RegexSyntaxTree t("$|$a|a$");
    //RegexSyntaxTree t("(a|b)((c|d)*e)*");

    RegexSyntaxTree t(regex);

    //t.print();

    Dfa dfa;
    t.exportDfa(dfa);

    for(char c = ' '; c <= '~'; c++) {
        std::cout << c << ' ';
    }
    std::cout << std::endl;
    for(int i = 0;i < dfa.getStateCount();i++) {
        for(int j = 0;j < dfa.getAlphabetSize();j++) {
            std::cout << dfa.getTransition(i, j) << " ";
        }
        std::cout << std::endl;
    }

    for(int i = 0; i< dfa.getStateCount();i++) {
        std::cout << dfa.getAcceptable(i) << " ";
    }
    std::cout << std::endl;
    return 0;
}
