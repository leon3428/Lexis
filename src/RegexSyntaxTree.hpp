#pragma once

#include <string>
#include <vector>

#include "Dfa.hpp"
#include "unordered_map"

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

/**
 * @brief Constructs a syntax tree for a given regex, and computes followPos
 * 
 */
class RegexSyntaxTree{
private:
    RegexSyntaxTreeNode* m_treeBuilder(const std::string &regex, int start, int end);

    std::vector<RegexSyntaxTreeNode* > m_nodes;
    RegexSyntaxTreeNode *m_root;
    std::unordered_map<RegexSyntaxTreeNode*, int> m_regexCntMap; 

    void m_ComputeNFL(RegexSyntaxTreeNode* node);
    void m_ComputeFollowPos();

public:
    static const char REGEX_SEPARATOR = 17; // DC1

    /**
     * @brief Construct a new Regex Syntax Tree object
     * 
     * @param regex ab - concat, | - union, * - Kleene's operator, \ - escape ch
     */
    RegexSyntaxTree(const std::string &regex);

    ~RegexSyntaxTree();

    /**
     * @brief exports dfa from the constructed tree
     * 
     * @param dst destination dfa object
     */
    void exportDfa(Dfa &dst);

    /**
     * @brief prints the structure of the tree
     * 
     */
    void print();

    static bool isEscaped(const std::string &s, int pos);

    static bool isConcat(const std::string &s, int pos);

    static inline bool isLeftBracket(const std::string &s, int pos) { return s[pos] == '(' && !isEscaped(s, pos); }

    static inline bool isRightBracket(const std::string &s, int pos) { return s[pos] == ')' && !isEscaped(s, pos); }

    static inline bool isEpsilon(const std::string &s, int pos) { return s[pos] == '$' && !isEscaped(s, pos); }

    static inline bool isUnion(const std::string &s, int pos) { return s[pos] == '|' && !isEscaped(s, pos); }

    static inline bool isKleene(const std::string &s, int pos) { return s[pos] == '*' && !isEscaped(s, pos); }
};

