#include "Dfa.hpp"
#include "Utils.hpp"

Dfa::Dfa(int stateCnt) 
    : m_stateCnt(stateCnt), m_alphabetSize(100)
{
    m_transitionTable = new int[stateCnt * m_alphabetSize];
    m_acceptable = new int[stateCnt];
}

Dfa::~Dfa() {
    delete[] m_transitionTable;
    delete[] m_acceptable;
}

int Dfa::getTransition(int state, char symbol) const {
    DebugAssert(symbol < ' ' || symbol > '~', "Invalid symbol");
    DebugAssert(state < 0 || state >= m_stateCnt, "Invalid state");

    return m_transitionTable[state * m_alphabetSize + symbol - ' '];
}

void Dfa::setTransition(int state, char symbol, int nextState) {
    DebugAssert(symbol < ' ' || symbol > '~', "Invalid symbol");
    DebugAssert(state < 0 || state >= m_stateCnt, "Invalid state");

    m_transitionTable[state * m_alphabetSize + symbol - ' '] = nextState;
}