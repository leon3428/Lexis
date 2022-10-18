#include "Minimizer.hpp"
#include "Dfa.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

Dfa *Minimizer::minimize(Dfa *dfa)
{
    std::vector<int> group, groupRepresentative, groupSplitInto;
    group.resize(dfa->getStateCount());
    groupRepresentative.resize(dfa->getStateCount());
    groupSplitInto.resize(dfa->getStateCount());
    std::fill(groupSplitInto.begin(), groupSplitInto.end(), -1);

    for(int i = 0; i < dfa->getStateCount(); ++i){
        group[i] = (dfa->getAcceptable(i) == -1);
        groupRepresentative[ group[i] ] = i;
    }
    
    int groupCnt = 2;
    bool change = true;
    while(change) {
        change = false;
        for(int i = 0; i < groupCnt; ++i)
            groupSplitInto[i] = -1; // didnt split

        for(int i = 0; i < dfa->getStateCount(); ++i){
            for(int j = 0; j < dfa->getAlphabetSize(); ++j){
                if( group[dfa->getTransitionInt(i, j)] != group[dfa->getTransitionInt( groupRepresentative[group[i]], j)] ){
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
    
    for(int i = 0; i < dfa->getStateCount(); ++i){
        if(dfa->getAcceptable(i) == -1)
            continue;
        if(dfa->getAcceptable(i) < dfa->getAcceptable( groupRepresentative[group[i]] ))
            groupRepresentative[group[i]] = i;
    }

    Dfa *minimizedDfa = new Dfa();    
    minimizedDfa -> setStartState(group[ dfa->getStartState() ]);
    for(int i = 0; i < groupCnt; ++i){
        for(int j = 0; j < dfa->getAlphabetSize(); ++j){
            minimizedDfa -> setTransitionInt(i, j, group[ dfa->getTransitionInt( groupRepresentative[i], j) ]);
        }
        minimizedDfa -> setAcceptable(i, dfa -> getAcceptable( groupRepresentative[i] ));
    }

    return minimizedDfa;
}