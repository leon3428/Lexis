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

    int groupCnt = 0;
    for(int i = 0; i < dfa->getStateCount(); ++i){
        int regexId = dfa->getAcceptable(i);
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