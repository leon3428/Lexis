#pragma once

#include <string>
#include "Dfa.hpp"
#include <vector>
#include <unordered_map>
#include <fstream>

class ConfigParser {
private:
    std::vector<Dfa> m_stateDfa;
    std::unordered_map<std::string, std::string> m_stateRegex;
    std::unordered_map<std::string, int> m_sateId;

    void m_writeDfa(std::ostream &stream) const;

public:
    void addRegexToState(const std::string &state, const std::string regex);
    void compileRegex();

    void generateLexer(std::string &inPath, std::string &outPath) const;
};