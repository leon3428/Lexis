#include "RegexSyntaxTree.hpp"
#include "Utils.hpp"

#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <set>

bool RegexSyntaxTree::m_isConcat(char a, char b)
{
    // TODO: special characters
    if(a == '|')
        return false;
    if(a == '(')
        return false;
    if(b == '|')
        return false;
    if(b == ')')
        return false;
    if(b == '*')
        return false;

    return true;
}

RegexSyntaxTree::RegexSyntaxTree(const std::string &regex)
{
    // pre calculate number of nodes to avoid unnecessary reallocations
    // TODO: special characters
    int nodeCnt = regex.size();

    for(int i = 0;i < regex.size();i++)
    {
        if(regex[i] == '(' || regex[i] == ')')
            nodeCnt--;
        if(i < regex.size() - 1 && m_isConcat(regex[i], regex[i + 1]))
            nodeCnt++;
    }

    m_nodes.reserve(nodeCnt);

    m_root = m_treeBuilder(regex, 0, regex.size());

    LogInfo("Calculated: %d, Real: %d", nodeCnt, m_nodes.size());

    m_ComputeNFL(m_root);
    m_ComputeFollowPos();
}



RegexSyntaxTreeNode* RegexSyntaxTree::m_treeBuilder(const std::string &regex, int start, int end)
{
    // TODO: special characters

    LogInfo("Tree Builder Recursion: %s", regex.substr(start, end - start).c_str());

    while(regex[start] == '(' && regex[end - 1] == ')') {
        start++;
        end--;
    }

    if(end - start == 1)
    {
        m_nodes.emplace_back(regex[start], false);

        return &m_nodes[m_nodes.size() - 1];
    }

    // try to find a union operator
    int i;
    int bracketCnt = 0;

    for(i = start; i < end; i++)
    {
        if(regex[i] == '(')
            bracketCnt++;
        if(regex[i] == ')')
            bracketCnt--;

        if(regex[i] == '|' && bracketCnt == 0)
            break;
    }

    // found union
    if(i != end) {
        RegexSyntaxTreeNode* left = m_treeBuilder(regex, start, i);
        RegexSyntaxTreeNode* right = m_treeBuilder(regex, i + 1, end);

        m_nodes.emplace_back('|', true, left, right);

        return &m_nodes[m_nodes.size() - 1];
    }

    // try to find a concatenation operator
    bracketCnt = 0;

    for(i = start; i < end - 1; i++)
    {
        if(regex[i] == '(')
            bracketCnt++;
        if(regex[i] == ')')
            bracketCnt--;
        if(m_isConcat(regex[i], regex[i+1]) && bracketCnt == 0)
            break;
    }

    // found concat
    if(i != end - 1) {
        RegexSyntaxTreeNode* left = m_treeBuilder(regex, start, i + 1);
        RegexSyntaxTreeNode* right = m_treeBuilder(regex, i + 1, end);

        m_nodes.emplace_back('.', true, left, right);

        return &m_nodes[m_nodes.size() - 1];
    }

    // found Kleene's operator
    if(regex[end - 1] == '*') {
        RegexSyntaxTreeNode* left = m_treeBuilder(regex, start, end - 1);

        m_nodes.emplace_back('*', true, left, nullptr);

        return &m_nodes[m_nodes.size() - 1];
    }
    
    LogError("Substring parsing failed for: %s", regex.substr(start, end - start).c_str());
    return nullptr;
}

void pushPosVectors(std::vector<RegexSyntaxTreeNode*> &dst, const std::vector<RegexSyntaxTreeNode*> &src) {
    dst.reserve(src.size());
    
    for(auto &el : src)
        dst.push_back(el);
}

void RegexSyntaxTree::m_ComputeNFL(RegexSyntaxTreeNode* node) {
    if(node -> leftChild != nullptr)
        m_ComputeNFL(node -> leftChild);
    if(node -> rightChild != nullptr)
        m_ComputeNFL(node -> rightChild);

    // nullable
    if(node -> isOperator)
    {
        if(node -> val == '*') {
            node -> nullable = true;
            node -> firstPos = node -> leftChild -> firstPos;
            node -> lastPos = node -> leftChild -> lastPos;
        }
        else if(node -> val == '|')
        {
            node -> nullable = node -> leftChild -> nullable || node -> rightChild -> nullable;
            pushPosVectors(node -> firstPos, node -> leftChild -> firstPos);
            pushPosVectors(node -> firstPos, node -> rightChild -> firstPos);
            pushPosVectors(node -> lastPos, node -> leftChild -> lastPos);
            pushPosVectors(node -> lastPos, node -> rightChild -> lastPos);
        }
        else if(node -> val == '.')
        {
            node -> nullable = node -> leftChild -> nullable && node -> rightChild -> nullable;
            
            if(node -> leftChild -> nullable) {
                pushPosVectors(node -> firstPos, node -> leftChild -> firstPos);
                pushPosVectors(node -> firstPos, node -> rightChild -> firstPos);
            } else {
                node -> firstPos = node -> leftChild -> firstPos;
            }

            if(node -> rightChild -> nullable) {
                pushPosVectors(node -> lastPos, node -> leftChild -> lastPos);
                pushPosVectors(node -> lastPos, node -> rightChild -> lastPos);
            } else {
                node -> lastPos = node -> rightChild -> lastPos;
            }
        }  
    } else {
        if(node -> val == '$')
        {
            node -> nullable = true;
        } else {
            node -> nullable = false;
            (node -> firstPos).push_back(node);
            (node -> lastPos).push_back(node);
        }
        
    }
}

void RegexSyntaxTree::m_ComputeFollowPos() {
    for(auto &node : m_nodes)
    {
        if(node.isOperator) {
            if(node.val == '.') {
                for(auto lastPos : node.leftChild -> lastPos) {
                    for(auto firstPos : node.rightChild -> firstPos) {
                        (lastPos -> followPos).push_back(firstPos);
                    }
                }
            }
            if(node.val == '*') {
                for(auto lastPos : node.leftChild -> lastPos) {
                    for(auto firstPos : node.leftChild -> firstPos) {
                        (lastPos -> followPos).push_back(firstPos);
                    }
                }
            }
        }
    }
}

void RegexSyntaxTree::print() {
    for(int i = 0;i < m_nodes.size();i++)
    {
        std::cout << "Val: " << m_nodes[i].val << " Nullable: " << m_nodes[i].nullable << " Addr: " << &m_nodes[i] << " Left: " << m_nodes[i].leftChild << " Right: " << m_nodes[i].rightChild << std::endl;
        std::cout << "FirstPos: ";
        for(int j = 0; j < m_nodes[i].firstPos.size(); j++) {
            std::cout << m_nodes[i].firstPos[j] << " ";
        }
        std::cout << std::endl;
        std::cout << "LastPos: ";
        for(int j = 0; j < m_nodes[i].lastPos.size(); j++) {
            std::cout << m_nodes[i].lastPos[j] << " ";
        }
        std::cout << std::endl;
        std::cout << "FollowPos: ";
        for(int j = 0; j < m_nodes[i].followPos.size(); j++) {
            std::cout << m_nodes[i].followPos[j] << " ";
        }
        std::cout << std::endl << std::endl;
    }
}

struct setCmp {
    bool operator() (RegexSyntaxTreeNode* a, RegexSyntaxTreeNode* b) const {
        if(a -> val != b -> val)
            return a -> val < b -> val;
        else   
            return  a < b;
    }
};

struct SetHasher {
    std::size_t operator()(std::set<RegexSyntaxTreeNode*, setCmp> const& s) const {
        std::size_t seed = s.size();
        for(auto z : s) {
            uint32_t x = reinterpret_cast<uint64_t>(z);
            x = ((x >> 16) ^ x) * 0x119de1f3;
            x = ((x >> 16) ^ x) * 0x119de1f3;
            x = (x >> 16) ^ x;
            seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

void insertPosVector(std::set<RegexSyntaxTreeNode*, setCmp> &dst, const std::vector<RegexSyntaxTreeNode*> &src) {
    for(auto &el : src)
        dst.insert(el);
}


void RegexSyntaxTree::exportDfa(Dfa &dst) {
    std::vector<std::set<RegexSyntaxTreeNode*, setCmp>* > states;
    std::unordered_map<std::set<RegexSyntaxTreeNode*, setCmp> , int, SetHasher > statesMap;

    states.push_back(new std::set<RegexSyntaxTreeNode*, setCmp>());
    insertPosVector(*states[0], m_root -> firstPos);
    statesMap[*states[0]] = 0;

    for(int i = 0;i < states.size();i++) {
        //std::cout << i << ":\n";  

        auto newState = new std::set<RegexSyntaxTreeNode*, setCmp>();
        RegexSyntaxTreeNode* prevNode = nullptr;

        for(RegexSyntaxTreeNode* curNode : *states[i]) {

            if(prevNode != nullptr && prevNode -> val != curNode -> val) // different input char
            {
                if(statesMap.find(*newState) == statesMap.end()) {
                    // newState does not exists
                    statesMap[*newState] = states.size();
                    states.push_back(newState);
                }
            
                dst.setTransition(i, prevNode -> val, statesMap[*newState]);
                newState = new std::set<RegexSyntaxTreeNode*, setCmp>();
            } 
             

            insertPosVector(*newState, curNode -> followPos);
            prevNode = curNode;

            /*for(auto a : *newState)
                std::cout << a << std::endl;
            std::cout << std::endl;*/
        }

        if(!(states[i] -> empty()))
        {
            if(statesMap.find(*newState) == statesMap.end()) {
                // newState does not exists
                statesMap[*newState] = states.size();
                states.push_back(newState);
            }

            dst.setTransition(i, prevNode -> val, statesMap[*newState]);
        }
    }

    for(int i = 0;i < states.size(); i++) {
        LogInfo("State %d:", i);
        for(RegexSyntaxTreeNode *n : *states[i])
        {
            LogInfo("%#08x", reinterpret_cast<uint64_t>(n));
        }
        LogInfo("");

        delete states[i];
    }
}












/*
void RegexSyntaxTree::exportDfa(Dfa &dst) {
    std::unordered_map<std::vector<RegexSyntaxTreeNode*>, int, VectorHasher> statesMap;
    std::vector<StateVector> states;

    states.emplace_back(m_root -> firstPos);
    statesMap[states[0].getState()] = 0;

    for(int i = 0;i < states.size();i++)
    {
        StateVector newState;

                

    }
}

StateVector::StateVector() {}

StateVector::StateVector(std::vector<RegexSyntaxTreeNode*> &src)
{
    push(src);
}

void StateVector::push(std::vector<RegexSyntaxTreeNode*> &src) {
    for(RegexSyntaxTreeNode* p : src) {
        m_state.push_back(p);
    }
}

std::vector<RegexSyntaxTreeNode*>& StateVector::getState() { 
    std::sort(m_state.begin(), m_state.end(), [] (const RegexSyntaxTreeNode* a, const RegexSyntaxTreeNode* b) {
        return a -> val < b -> val;
    });
    return m_state;
}*/