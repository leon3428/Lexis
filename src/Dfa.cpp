#include "Dfa.hpp"
#include "Utils.hpp"
#include <algorithm>

void Dfa::print() const  {
    std::cout << m_startState << std::endl;
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
        return '\n';
    
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

void Dfa::minimize(const Dfa &src, Dfa &dst) {
    std::vector<int> group, groupRepresentative, groupSplitInto;
    std::vector<int> order;

    for(int i = 0; i < src.getStateCount(); ++i)
        order.push_back(src.getAcceptable(i));

    sort(order.begin(), order.end());
    order.erase(unique(order.begin(), order.end()), order.end());

    group.resize(src.getStateCount());
    groupRepresentative.resize(src.getStateCount());
    groupSplitInto.resize(src.getStateCount());
    std::fill(groupSplitInto.begin(), groupSplitInto.end(), -1);

    int groupCnt = order.size() + 1;
    group[0] = 0;
    groupRepresentative[0] = 0;
    for(int i = 1; i < src.getStateCount(); ++i){
        int regexId = src.getAcceptable(i);
        group[i] = (std::lower_bound(order.begin(), order.end(), regexId) - order.begin()) + 1; 
        groupRepresentative[ group[i] ] = i;
    }
    
    bool change = true;
    while(change) {
        change = false;
        for(int i = 0; i < groupCnt; ++i)
            groupSplitInto[i] = -1; // didnt split

        for(int i = 1; i < src.getStateCount(); ++i){
            for(int j = 0; j < src.getAlphabetSize(); ++j){
                if(group[src.getTransitionInt(i, j)] != group[src.getTransitionInt( groupRepresentative[group[i]], j)] ){
                    change = true;
                    
                    if(groupSplitInto[ group[i] ] == -1){
                        groupSplitInto[ group[i] ] = groupCnt;
                        group[i] = groupCnt;
                        groupRepresentative[ groupCnt ] = i;
                        groupCnt ++;
                    }
                    else{
                        group[i] = groupSplitInto[ group[i] ];
                    }
                    continue;
                }                
            }
        }
    }

    
    dst.setStartState(group[ src.getStartState() ]);
    
    for(int i = 0; i < groupCnt; ++i){
        for(int j = 0; j < src.getAlphabetSize(); ++j){
            dst.setTransitionInt(i, j, group[ src.getTransitionInt( groupRepresentative[i], j) ]);
        }
        dst.setAcceptable(i, src.getAcceptable( groupRepresentative[i] ));
    }
}