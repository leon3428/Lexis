#include <iostream>

struct Token {
    std::string &program;
    int start, end;
    int regexId;
};

$DfaFunctions$

int main(int argc, char const *argv[])
{
    std::string program;
    std::cin >> program;

    Token (*stateDfaArray[])(std::string&, int) = { $DfaFunctionsArray$ }; // filled by the generator
    return 0;
}
