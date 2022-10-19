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

void Dfa::minimize(const Dfa &src, Dfa &dst) {
    std::vector<int> group, groupRepresentative, groupSplitInto;
    group.resize(src.getStateCount());
    groupRepresentative.resize(src.getStateCount());
    groupSplitInto.resize(src.getStateCount());
    std::fill(groupSplitInto.begin(), groupSplitInto.end(), -1);

    int groupCnt = 0;
    for(int i = 0; i < src.getStateCount(); ++i){
        int regexId = src.getAcceptable(i);
        if(regexId == -1)
            group[i] = 0;
        else{
            group[i] = regexId + 1; // svaki regex je posebna grupa
            groupCnt = std::max(groupCnt, regexId + 2);
        }
        groupRepresentative[ group[i] ] = i;
    }
    
    bool change = true;
    while(change) {
        change = false;
        for(int i = 0; i < groupCnt; ++i)
            groupSplitInto[i] = -1; // didnt split

        for(int i = 0; i < src.getStateCount(); ++i){
            for(int j = 0; j < src.getAlphabetSize(); ++j){
                if( group[src.getTransitionInt(i, j)] != group[src.getTransitionInt( groupRepresentative[group[i]], j)] ){
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