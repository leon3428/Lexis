#include "ConfigParser.hpp"
#include <fstream>
#include "Utils.hpp"
#include "RegexSyntaxTree.hpp"

void ConfigParser::addRegexToState(const std::string &state, const std::string regex){
    if(m_stateRegex.find(state) != m_stateRegex.end())
        m_stateRegex[state] += "|";
    m_stateRegex[state] += "(" + regex + ")" + std::string(1, RegexSyntaxTree::REGEX_SEPARATOR); 
}

void ConfigParser::compileRegex() {
    for(auto el : m_stateRegex) {
        m_sateId[el.first] = m_stateDfa.size();

        RegexSyntaxTree t(el.second);

        m_stateDfa.emplace_back();
        t.exportDfa(m_stateDfa[m_stateDfa.size() - 1]);
    }
}

void ConfigParser::generateLexer(std::string &inPath, std::string &outPath) {
    std::ifstream inputFile(inPath);
    std::ofstream outputFile(outPath);
    std::string line;

    while (getline(inputFile, line)) {
        // Output the text from the file
        line = utils::trim(line);

        if(line == "$DfaFunctions$") {
            
        } else {
            outputFile << line << '\n';
        }
    }

    inputFile.close();
    outputFile.close();
}