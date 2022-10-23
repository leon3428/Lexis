#pragma once

#include <string>
#include "Dfa.hpp"
#include <vector>
#include <unordered_map>
#include <fstream>

struct Action{
    std::string nextState;
    std::string tokenName;
    int keep;
    bool newline;

    Action(const std::string& nextState, const std::string &tokenName, int keep, bool newline) 
        : nextState(nextState), tokenName(tokenName), keep(keep), newline(newline) {}
    
    Action()
        : nextState(""), tokenName("-"), keep(-1), newline(false) {}
};

class ConfigParser {
private:
    std::vector<Dfa> m_stateDfa;
    std::unordered_map<std::string, std::string> m_stateRegex;
    std::unordered_map<std::string, int> m_stateId;
    std::string m_startState;
    std::unordered_map<std::string, std::vector<Action> > m_lexerTransitionTable;

    void m_writeDfa(std::ostream &stream) const;
    void m_writeMain(std::ostream &stream);

public:
    void addRegexToState(const std::string &state, const std::string regex);
    void compileRegex();
    void parseInput(std::string &filename);

    void generateLexer(std::string &inPath, std::string &outPath);
    void modifyRegex(std::string &regex, std::unordered_map<std::string, std::string> &nameToRegex) const;
};