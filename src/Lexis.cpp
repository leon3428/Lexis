#include <iostream>
#include <LexisConfig.hpp>
#include "Utils.hpp"
#include "ConfigParser.hpp"
#include "Dfa.hpp"

int main(int argc, char const *argv[])
{
    LogInfo("%d", Dfa::getSymbolId('\n'));
    std::cout << argv[0] << " Version: " << LEXIS_VERSION_MAJOR << '.' << LEXIS_VERSION_MINOR << std::endl;

    ConfigParser configParser; 

    configParser.addRegexToState("state0", "(a|b)*abb");
    configParser.addRegexToState("state0", "aba");

    configParser.addRegexToState("state1", "\\\\a|\\t\\n");
    configParser.addRegexToState("state1", "\\\\");

    configParser.compileRegex();

    std::string inPath = "src/Lexer.in.cpp";
    std::string outPath = "Lexer.cpp";
    configParser.generateLexer(inPath, outPath);
    return 0;
}
