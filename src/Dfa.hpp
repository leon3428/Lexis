#pragma once

#include <vector>
#include <array>

/**
 * @brief class stores generated dfa
 * 
 */
class Dfa{
    static const int m_alphabetSize = 100;
    int m_startState;

    std::vector<std::array<int, m_alphabetSize> > m_transitionTable;
    std::vector<int> m_acceptable;

    /**
     * @brief function converts an symbol of the alphabet into it's corrsponding id value used to index the transition table
     * 
     * @param symbol all printable characters + TAB + new line
     * @return int 
     */
    static int m_getSymbolId(char symbol);

public:
    /**
     * @brief Get the Start State object
     * 
     * @return int 
     */
    inline int getStartState() const { return m_startState; }

    /**
     * @brief Set the Start State object
     * 
     * @param startState 
     */
    inline void setStartState(int startState) { m_startState = startState; }

    /**
     * @brief Get the Transition object
     * 
     * @param state 
     * @param symbol 
     * @return int 
     */
    inline int getTransition(int state, char symbol) const { return m_transitionTable[state][m_getSymbolId(symbol)]; }

    /**
     * @brief Get the Transition object by id
     * 
     * @param state 
     * @param symbol 
     * @return int 
     */
    inline int getTransitionInt(int state, int symbol) const { return m_transitionTable[state][symbol]; }

    /**
     * @brief Set the Transition object; dfa will expand if state larger than dfa
     * 
     * @param state 
     * @param symbol 
     * @param nextState 
     */
    void setTransition(int state, char symbol, int nextState);

    /**
     * @brief Set the Transition object; dfa will expand if state larger than dfa
     * 
     * @param state 
     * @param symbol 
     * @param nextState 
     */
    void setTransitionInt(int state, int symbol, int nextState);

    /**
     * @brief Get the Acceptable object;
     * 
     * @param state 
     * @return int; -1 if not acceptable; index of a first acceptable regex otherwise 
     */
    inline int getAcceptable(int state) const { return m_acceptable[state]; }

    /**
     * @brief Set the Acceptable object
     * 
     * @param state 
     * @param val -1 if not acceptable; index of a first acceptable regex otherwise 
     */
    void setAcceptable(int state, int val);

    /**
     * @brief Get the State Count of dfa
     * 
     * @return int 
     */
    inline int getStateCount() const { return m_acceptable.size(); }

    /**
     * @brief Get the Alphabet Size
     * 
     * @return int 
     */
    inline int getAlphabetSize() const { return m_alphabetSize; }
};