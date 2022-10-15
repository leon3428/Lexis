#pragma once

#include <string>
#include <vector>

#include "Dfa.hpp"

struct RegexSyntaxTreeNode {
    char val;
    bool nullable;
    bool isOperator;
    std::vector<RegexSyntaxTreeNode*> firstPos, lastPos, followPos;

    RegexSyntaxTreeNode *leftChild, *rightChild;

    RegexSyntaxTreeNode(char val, bool isOperator)
        : val(val), isOperator(isOperator) ,nullable(false), leftChild(nullptr), rightChild(nullptr) {}

    RegexSyntaxTreeNode(char val, bool isOperator, RegexSyntaxTreeNode* leftChild, RegexSyntaxTreeNode* rightChild)
        : val(val), isOperator(isOperator), nullable(false), leftChild(leftChild), rightChild(rightChild) {}
};


class RegexSyntaxTree{
private:
    RegexSyntaxTreeNode* m_treeBuilder(const std::string &regex, int start, int end);

    std::vector<RegexSyntaxTreeNode> m_nodes;
    RegexSyntaxTreeNode *m_root;

    bool m_isConcat(char a, char b);
    void m_ComputeNFL(RegexSyntaxTreeNode* node);
    void m_ComputeFollowPos();

public:
    RegexSyntaxTree(const std::string &regex);

    void exportDfa(Dfa &dst);

    void print();
};

