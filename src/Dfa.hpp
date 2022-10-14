#pragma once

#include <vector>

class Dfa{
    int m_stateCnt, m_alphabetSize;
    int m_startState;

    int *m_transitionTable;
    int *m_acceptable;

public:
    Dfa(int stateCnt);
    ~Dfa();

    int getTransition(int state, char symbol) const;
    void setTransition(int state, char symbol, int nextState);
};