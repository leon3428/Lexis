#include <iostream>
#include "../../src/RegexSyntaxTree.hpp"
#include "../../src/Dfa.hpp"

int main(int argc, char const *argv[])
{
    std::string regex, in;
    
    std::cin >> regex >> in;
    
    regex = "(" + regex + ")";
    regex += RegexSyntaxTree::REGEX_SEPARATOR;

    RegexSyntaxTree t(regex);
    Dfa originalDfa, dfa;

    t.exportDfa(originalDfa);
    Dfa::minimize(originalDfa, dfa);

    int state = dfa.getStartState();
    for(char c : in) {
        state = dfa.getTransition(state, c);
    }

    if(dfa.getAcceptable(state) == -1)
        std::cout << 0 << std::endl;
    else
        std::cout << 1 << std::endl;

    return 0;
}
