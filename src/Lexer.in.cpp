#include <iostream>

struct Token {
    const std::string &program;
    int start, end;
    int tokenId;

    Token(const std::string &program, int start, int end, int tokenId) 
        : program(program), start(start), end(end), tokenId(tokenId) {}
};

$DfaFunctions$

int main(int argc, char const *argv[])
{
    std::string program;
    std::cin >> program;

    Token (*stateDfaArray[])(const std::string&, int) = {
        $DfaFunctionsArray$
    };
    return 0;
}
