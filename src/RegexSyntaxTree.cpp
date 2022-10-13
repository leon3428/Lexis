#include "RegexSyntaxTree.hpp"
#include "Utils.hpp"

#include <iostream>

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

void joinPosVectors(std::vector<RegexSyntaxTreeNode*> &dst, const std::vector<RegexSyntaxTreeNode*> &src1, const std::vector<RegexSyntaxTreeNode*> &src2)
{
    dst.reserve(src1.size() + src2.size());
    
    for(auto &el : src1)
        dst.push_back(el);
    
    for(auto &el : src2)
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
            joinPosVectors(node -> firstPos, node -> leftChild -> firstPos, node -> rightChild -> firstPos);
            joinPosVectors(node -> lastPos, node -> leftChild -> lastPos, node -> rightChild -> lastPos);
        }
        else if(node -> val == '.')
        {
            node -> nullable = node -> leftChild -> nullable && node -> rightChild -> nullable;
            if(node -> leftChild -> nullable)
                joinPosVectors(node -> firstPos, node -> leftChild -> firstPos, node -> rightChild -> firstPos);
            else
                node -> firstPos = node -> leftChild -> firstPos;
            if(node -> rightChild -> nullable)
                joinPosVectors(node -> lastPos, node -> leftChild -> lastPos, node -> rightChild -> lastPos);
            else
                node -> lastPos = node -> rightChild -> lastPos;
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