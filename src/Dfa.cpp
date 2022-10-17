#include "Dfa.hpp"
#include "Utils.hpp"

int Dfa::m_getSymbolId(char symbol)
{
    int ret = symbol - ' ';

    if(symbol == '\t')
        ret = '~' - ' ' + 1;
    if(symbol == '\n')
        ret = '~' - ' ' + 2;
    
    DebugAssert(ret < 0 || ret >= m_alphabetSize, "Symbol out of range");
    return ret;
}

void Dfa::setTransition(const int state, const char symbol, const int nextState) { 
    // grow dfa if necessary
    while(m_acceptable.size() <= state) {
        m_transitionTable.emplace_back();
        m_acceptable.push_back(-1);
    }

    m_transitionTable[state][m_getSymbolId(symbol)] = nextState; 
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