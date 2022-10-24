#include <iostream>
#include <LexisConfig.hpp>
#include "Utils.hpp"
#include "ConfigParser.hpp"
#include "Dfa.hpp"

int main(int argc, char const *argv[])
{
    std::cout << argv[0] << " Version: " << LEXIS_VERSION_MAJOR << '.' << LEXIS_VERSION_MINOR << std::endl;

    ConfigParser configParser; 

    configParser.parseInput(std::cin);
    configParser.compileRegex();

    std::string inPath = "src/Lexer.in.cpp";
    std::string outPath = "test/test2/Lexer.cpp";
    configParser.generateLexer(inPath, outPath);
    return 0;
}
