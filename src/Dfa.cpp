#include "Dfa.hpp"
#include "Utils.hpp"

void Dfa::print() const  {
    for(char c = ' ';c <= '~';c++) {
        std::cout << c << ' ';
    }
    std::cout << "\\t \\n\n";
    for(int i = 0;i < m_transitionTable.size(); i++){
        for(int j = 0;j < m_alphabetSize; j++) {
            std::cout << m_transitionTable[i][j] << ' ';
        }
        std::cout << '\n';
    }
    for(int i = 0;i < m_transitionTable.size(); i++){
        std::cout << m_acceptable[i] << ' ';
    }
    std::cout << '\n';
}

int Dfa::getSymbolId(char symbol)
{
    int ret = symbol - ' ';

    if(symbol == '\t')
        ret = '~' - ' ' + 1;
    if(symbol == '\n')
        ret = '~' - ' ' + 2;
    
    DebugAssert(ret < 0 || ret >= m_alphabetSize, "Symbol out of range");
    return ret;
}

char Dfa::getSymbolFromId(int symbolId) {
    if(symbolId == '~' - ' ' + 1)
        return '\t';
    if(symbolId == '~' - ' ' + 2)
        return '\t';
    
    DebugAssert(symbolId < 0 || ' ' + symbolId > '~', "Symbol out of range");
    return ' ' + symbolId;
}

void Dfa::setTransition(const int state, const char symbol, const int nextState) { 
    // grow dfa if necessary
    while(m_acceptable.size() <= state) {
        m_transitionTable.emplace_back();
        m_acceptable.push_back(-1);
    }

    m_transitionTable[state][getSymbolId(symbol)] = nextState; 
}

void Dfa::setTransitionInt(const int state, const int symbol, const int nextState) { 
    // grow dfa if necessary
    while(m_acceptable.size() <= state) {
        m_transitionTable.emplace_back();
        m_acceptable.push_back(-1);
    }

    m_transitionTable[state][symbol] = nextState; 
}

void Dfa::setAcceptable(int state, int val) {
    // grow dfa if necessary
    while(m_acceptable.size() <= state) {
        m_transitionTable.emplace_back();
        m_acceptable.push_back(-1);
    }

    m_acceptable[state] = val;
}