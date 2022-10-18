#include "RegexSyntaxTree.hpp"
#include "Utils.hpp"

#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <set>

bool RegexSyntaxTree::isConcat(const std::string &s, int pos)
{
    char a, b;

    /////////////////////////////////
    if(isEscaped(s, pos))
        a = 'a';
    else
        a = s[pos];

    if(a == '\\')
        return false;
    /////////////////////////////////
    if(s[pos+1] == '\\') {
        if(pos+2 >= s.size())
            return false;
        b = 'a';
    } else {
        b = s[pos+1];
    }
    /////////////////////////////////

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
    m_root = m_treeBuilder(regex, 0, regex.size());

    m_ComputeNFL(m_root);
    m_ComputeFollowPos();
}

RegexSyntaxTree::~RegexSyntaxTree()
{
    for(auto node : m_nodes)
        delete node;
}

bool RegexSyntaxTree::isEscaped(const std::string &s, int pos) {
    int cnt = 0;
    pos--;
    while(pos >= 0 && s[pos] == '\\') {
        pos--;
        cnt++;
    }
    return cnt % 2;
}

RegexSyntaxTreeNode* RegexSyntaxTree::m_treeBuilder(const std::string &regex, int start, int end)
{
    LogInfo("Tree Builder Recursion: %s", regex.substr(start, end - start).c_str());

    bool removeBrackets = true;
    while(isLeftBracket(regex, start) && isRightBracket(regex, end - 1) && removeBrackets) {
        
        int bracketCnt = 0;

        for(int i = start; i < end; i++) {
            if(isLeftBracket(regex, i))
                bracketCnt++;
            if(isRightBracket(regex, i))
                bracketCnt--;
            if(i != start && i != end-1 && bracketCnt == 0) {
                removeBrackets = false;
                break;
            }
        }
        
        if(removeBrackets) {
            start++;
            end--;
        }
    }

    if(end - start == 1 || (end - start == 2 && regex[start] == '\\'))
    {
        bool special = false;
        char c = regex[end - 1];

        if(end - start == 1 && regex[start] == '$') {
            special = true;
        }

        if(end - start == 2 && regex[end - 1] == 'n') {
            c = '\n';
        }

        if(end - start == 2 && regex[end - 1] == 't') {
            c = '\t';
        }

        if(end - start == 2 && regex[end - 1] == '_') {
            c = ' ';
        }

        m_nodes.push_back(new RegexSyntaxTreeNode(c, special));

        if(regex[start] == REGEX_SEPARATOR) {
            int cnt = 0;
            for(int i = 0;i < start;i++)
                if(regex[i] == REGEX_SEPARATOR)
                    cnt++;

            m_regexCntMap[m_nodes[m_nodes.size() - 1]] = cnt;
        }

        return m_nodes[m_nodes.size() - 1];
    }

    
    // try to find a union operator
    int i;
    int bracketCnt = 0;

    for(i = start; i < end; i++)
    {
        if(isLeftBracket(regex, i))
            bracketCnt++;
        if(isRightBracket(regex, i))
            bracketCnt--;

        if(isUnion(regex, i) && bracketCnt == 0)
            break;
    }

    // found union
    if(i != end) {
        RegexSyntaxTreeNode* left = m_treeBuilder(regex, start, i);
        RegexSyntaxTreeNode* right = m_treeBuilder(regex, i + 1, end);

        m_nodes.push_back(new RegexSyntaxTreeNode('|', true, left, right));

        return m_nodes[m_nodes.size() - 1];
    }

    

    // try to find a concatenation operator
    bracketCnt = 0;

    for(i = start; i < end - 1; i++)
    {
        if(isLeftBracket(regex, i))
            bracketCnt++;
        if(isRightBracket(regex, i))
            bracketCnt--;
        if(isConcat(regex, i) && bracketCnt == 0)
            break;
    }

    // found concat
    if(i != end - 1) {
        RegexSyntaxTreeNode* left = m_treeBuilder(regex, start, i + 1);
        RegexSyntaxTreeNode* right = m_treeBuilder(regex, i + 1, end);

        m_nodes.push_back(new RegexSyntaxTreeNode('.', true, left, right));

        return m_nodes[m_nodes.size() - 1];
    }

    // found Kleene's operator
    if(isKleene(regex, end-1)) {
        RegexSyntaxTreeNode* left = m_treeBuilder(regex, start, end - 1);

        m_nodes.push_back(new RegexSyntaxTreeNode('*', true, left, nullptr));

        return m_nodes[m_nodes.size() - 1];
    }
    
    LogError("Substring parsing failed for: %s", regex.substr(start, end - start).c_str());
    return nullptr;
}

void pushPosVector(std::vector<RegexSyntaxTreeNode*> &dst, const std::vector<RegexSyntaxTreeNode*> &src) {
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
            pushPosVector(node -> firstPos, node -> leftChild -> firstPos);
            pushPosVector(node -> firstPos, node -> rightChild -> firstPos);
            pushPosVector(node -> lastPos, node -> leftChild -> lastPos);
            pushPosVector(node -> lastPos, node -> rightChild -> lastPos);
        }
        else if(node -> val == '.')
        {
            node -> nullable = node -> leftChild -> nullable && node -> rightChild -> nullable;
            
            if(node -> leftChild -> nullable) {
                pushPosVector(node -> firstPos, node -> leftChild -> firstPos);
                pushPosVector(node -> firstPos, node -> rightChild -> firstPos);
            } else {
                node -> firstPos = node -> leftChild -> firstPos;
            }

            if(node -> rightChild -> nullable) {
                pushPosVector(node -> lastPos, node -> leftChild -> lastPos);
                pushPosVector(node -> lastPos, node -> rightChild -> lastPos);
            } else {
                node -> lastPos = node -> rightChild -> lastPos;
            }
        } else if(node -> val == '$') {
            node -> nullable = true;
        }
    } else {
        node -> nullable = false;
        (node -> firstPos).push_back(node);
        (node -> lastPos).push_back(node);
    }
}

void RegexSyntaxTree::m_ComputeFollowPos() {
    for(auto node : m_nodes)
    {
        if(node -> isOperator) {
            if(node -> val == '.') {
                for(auto lastPos : node -> leftChild -> lastPos) {
                    for(auto firstPos : node -> rightChild -> firstPos) {
                        (lastPos -> followPos).push_back(firstPos);
                    }
                }
            }
            if(node -> val == '*') {
                for(auto lastPos : node -> leftChild -> lastPos) {
                    for(auto firstPos : node -> leftChild -> firstPos) {
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
        std::cout << "Val: " << m_nodes[i] -> val << " Nullable: " << m_nodes[i] -> nullable << " Addr: " << m_nodes[i] << " Left: " << m_nodes[i] -> leftChild << " Right: " << m_nodes[i] -> rightChild << std::endl;
        std::cout << "FirstPos: ";
        for(int j = 0; j < m_nodes[i] -> firstPos.size(); j++) {
            std::cout << m_nodes[i] -> firstPos[j] << " ";
        }
        std::cout << std::endl;
        std::cout << "LastPos: ";
        for(int j = 0; j < m_nodes[i] -> lastPos.size(); j++) {
            std::cout << m_nodes[i] -> lastPos[j] << " ";
        }
        std::cout << std::endl;
        std::cout << "FollowPos: ";
        for(int j = 0; j < m_nodes[i] -> followPos.size(); j++) {
            std::cout << m_nodes[i] -> followPos[j] << " ";
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

    dst.setStartState(1);

    for(int i = 0;i < states.size();i++) {
        auto newState = new std::set<RegexSyntaxTreeNode*, setCmp>();
        RegexSyntaxTreeNode* prevNode = nullptr;
        int acceptable = -1;

        for(RegexSyntaxTreeNode* curNode : *states[i]) {
            if(curNode -> val == REGEX_SEPARATOR) {
                if(acceptable == -1)
                    acceptable = m_regexCntMap[curNode];
                else
                    acceptable = std::min(acceptable, m_regexCntMap[curNode]);
            }        

            if(prevNode != nullptr && prevNode -> val != curNode -> val) // different input char
            {
                if(statesMap.find(*newState) == statesMap.end()) {
                    // newState does not exists
                    statesMap[*newState] = states.size();
                    states.push_back(newState);
                }
            
                if(prevNode -> val != REGEX_SEPARATOR)
                    dst.setTransition(i + 1, prevNode -> val, statesMap[*newState] + 1);
                newState = new std::set<RegexSyntaxTreeNode*, setCmp>();
            } 
             

            insertPosVector(*newState, curNode -> followPos);
            prevNode = curNode;
        }

        if(!(states[i] -> empty()))
        {
            dst.setAcceptable(i+1, acceptable);

            if(statesMap.find(*newState) == statesMap.end()) {
                // newState does not exists
                statesMap[*newState] = states.size();
                states.push_back(newState);
            }

            if(prevNode -> val != REGEX_SEPARATOR)
                dst.setTransition(i + 1, prevNode -> val, statesMap[*newState] + 1);
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
