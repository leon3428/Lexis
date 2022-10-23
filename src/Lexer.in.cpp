#include <iostream>

struct Token {
    int start, end;
    int tokenId;

    Token(int start, int end, int tokenId) 
        : start(start), end(end), tokenId(tokenId) {}
    Token() {}
};

$DfaFunctions$

int main(int argc, char const *argv[])
{
    std::string program;

    for (std::string line; std::getline(std::cin, line);) {
        program += line + "\n";
    }
    std::cout << program << std::endl;

    int pos = 0;
    int lineCnt = 1;
    Token result;

    $main$

    return 0;
}
