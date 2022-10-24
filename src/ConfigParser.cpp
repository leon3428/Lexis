#include "ConfigParser.hpp"
#include <fstream>
#include <unordered_map>
#include "Utils.hpp"
#include "RegexSyntaxTree.hpp"
#include "Dfa.hpp"

void ConfigParser::addRegexToState(const std::string &state, const std::string regex){
    if(m_stateRegex.find(state) != m_stateRegex.end())
        m_stateRegex[state] += "|";

    m_stateRegex[state] += "(" + regex + ")" + std::string(1, RegexSyntaxTree::REGEX_SEPARATOR); 
}

void ConfigParser::compileRegex() {
    for(auto el : m_stateRegex) {
        m_stateId[el.first] = m_stateDfa.size();

        RegexSyntaxTree t(el.second);

        m_stateDfa.emplace_back();
        Dfa tmp;
        t.exportDfa(tmp);
        Dfa::minimize(tmp, m_stateDfa[m_stateDfa.size() - 1]);
    }
}

void ConfigParser::m_writeDfa(std::ostream &stream) const {
    for(int i = 0;i < m_stateDfa.size();i++) {
        stream << "Token dfa" << i << "(const std::string &program, int start) {\n";
        stream << "\tint tokenId = -1;\n"; 
        stream << "\tint pos = start;\n";
        stream << "\tint lastAcceptablePos = -1;\n";
        stream << "\tgoto dfa" << i << "_state" << m_stateDfa[i].getStartState() << ";\n\n";

        for(int dfaState = 0; dfaState < m_stateDfa[i].getStateCount(); dfaState++) {
            stream << "\tdfa" << i << "_state" << dfaState << ":\n";
            stream << "\tif(pos >= program.size())\n";
            stream << "\t\treturn Token(start, lastAcceptablePos, tokenId);\n";
            
            if(dfaState == 0) {
                stream << "\treturn Token(start, lastAcceptablePos, tokenId);\n\n";
            } else {
                if(m_stateDfa[i].getAcceptable(dfaState) != -1) {
                    stream << "\ttokenId = " << m_stateDfa[i].getAcceptable(dfaState) << ";\n";
                    stream << "\tlastAcceptablePos = pos;\n";
                }  
                stream << "\tswitch(program[pos++]) {\n";

                for(int symbol = 0; symbol < m_stateDfa[i].getAlphabetSize(); symbol++) {
                    int nextState = m_stateDfa[i].getTransitionInt(dfaState, symbol);
                    if(nextState != 0) {
                        std::string ch(1, Dfa::getSymbolFromId(symbol));

                        if(ch == "\n")
                            ch = "\\n";
                        if(ch == "\t")
                            ch = "\\t";
                        if(ch == "\\")
                            ch = "\\\\";
                        if(ch == "'")
                            ch = "\\'";

                        stream << "\t\tcase '" << ch << "':\n";
                        stream << "\t\t\tgoto dfa" << i << "_state" << nextState << ";\n";
                        stream << "\t\t\tbreak;\n";
                    }
                }

                stream << "\t\tdefault:\n";
                stream << "\t\t\tgoto dfa" << i << "_state0;\n";
                stream << "\t}\n\n";
            } 
        }

        stream << "}\n\n";
    }
}

void ConfigParser::m_writeMain(std::ostream &stream) {
    stream << "\tgoto lexer_state" << m_stateId[m_startState] << ";\n"; 

    for(auto &row : m_lexerTransitionTable) {
        std::string state = row.first;

        stream << "\tlexer_state" << m_stateId[state] << ":\n\n";
        stream << "\t\tif(pos >= program.size())\n";
        stream << "\t\t\treturn 0;\n";
        stream << "\t\tresult = dfa" << m_stateId[state] << "(program, pos);\n";
        stream << "\t\tswitch(result.tokenId) {\n";
        stream << "\t\t\tcase -1:\n";
        stream << "\t\t\t\tpos++;\n";
        stream << "\t\t\t\tgoto lexer_state" << m_stateId[state] << ";\n";
        stream << "\t\t\t\tbreak;\n";

        for(int i = 0;i < row.second.size();i++) {
            Action &action = row.second[i];
            stream << "\t\t\tcase " << i << ":\n";
            if(action.newline)
                stream << "\t\t\t\tlineCnt++;\n";
            if(action.keep != -1)
                stream << "\t\t\t\tpos += " << action.keep << ";\n";
            else
                stream << "\t\t\t\tpos = result.end;\n";
            
            if(action.tokenName != "-")
                stream << "\t\t\t\tstd::cout << \"" << action.tokenName << " \" << lineCnt << \" \" << program.substr(result.start, pos - result.start) << std::endl;\n";
            stream << "\t\t\t\tgoto lexer_state" << m_stateId[action.nextState] << ";\n"; 
            stream << "\t\t\t\tbreak;\n";
        }
        stream << "\t\t}\n";
    }
    
}

void ConfigParser::generateLexer(std::string &inPath, std::string &outPath) {
    std::ifstream inputFile(inPath);
    std::ofstream outputFile(outPath);
    std::string line;

    while (getline(inputFile, line)) {
        // Output the text from the file
        std::string trimmedLine = utils::trim(line);

        if(trimmedLine == "$DfaFunctions$")
        {
            m_writeDfa(outputFile);

        } else if(trimmedLine == "$main$")
        {
            m_writeMain(outputFile);
        } else
        {
            outputFile << line << '\n';
        }
    }

    inputFile.close();
    outputFile.close();
}

void ConfigParser::modifyRegex(std::string &regex, std::unordered_map<std::string, std::string> &nameToRegex) const {
    // removes {}
    std::vector<int> endpoints;
    for(int i = 0; i < (int)regex.size(); ++i){
        if( (regex[i] == '{' || regex[i] == '}') && !RegexSyntaxTree::isEscaped(regex, i) )
            endpoints.push_back(i);
    }

    std::string parsedRegex = "";
    int idEndpoint = 0, idx = 0;

    while(idx < (int)regex.size()) {
        if(idEndpoint < (int)endpoints.size() && idx == endpoints[idEndpoint]) {
            int beg = endpoints[idEndpoint];
            int len = endpoints[idEndpoint + 1] - endpoints[idEndpoint] + 1;
            std::string regexName = regex.substr(beg, len);
            parsedRegex += "(" + nameToRegex[regexName] + ")";

            idx = endpoints[idEndpoint + 1];
            idEndpoint += 2;
        } else {
            parsedRegex += regex[idx]; 
        }
        idx ++;
    }

    regex = parsedRegex;
}

void ConfigParser::parseInput(std::istream& stream){
    /* 
    states = stanja
    names = imena leksickih jedinki
    nameToRegex = key je {vrijednost} i vraca regex sa zamijenjenim {}
    */

    std::string inp;
    std::unordered_map<std::string, std::string> nameToRegex;
    bool start = true;
    std::vector<std::string> states, names;
    bool foundStartState = false;

    while(stream >> inp){
        if(inp == "%X"){
            start = false;
            stream >> inp;
            while(inp != "%L"){
                states.push_back(inp);
                stream >> inp;
            }
        }

        if(inp == "%L") {
            stream >> inp;
            while(inp[0] != '<') {
                names.push_back(inp);
                stream >> inp;
            }
        }

        if(start) {
            std::string regex;
            stream >> regex;
            modifyRegex(regex, nameToRegex);
            nameToRegex[inp] = regex;
            LogInfo("input: %s regex: %s", inp.c_str(), regex.c_str());
        } else { // stanje regex akcija
            int endpos = inp.find(">");
            std::string state = inp.substr(1, endpos - 1);
            std::string regex = inp.substr(endpos + 1);

            if(!foundStartState) {
                foundStartState = true;
                m_startState = state;
            }

            modifyRegex(regex, nameToRegex);
            LogInfo("state: %s regex: %s", state.c_str(), regex.c_str());
            addRegexToState(state, regex);

            m_lexerTransitionTable[state].emplace_back(state, "-", -1, false);
            int regexId = m_lexerTransitionTable[state].size() - 1;

            std::string lexicIndividual;
            stream >> lexicIndividual; // ili stanje ili -

            while(inp != "}") {
                stream >> inp;
                if(inp == "NOVI_REDAK"){
                    m_lexerTransitionTable[state][regexId].newline = true;
                } else if(inp == "UDJI_U_STANJE") {
                    std::string nextState;
                    stream >> nextState;
                    m_lexerTransitionTable[state][regexId].nextState = nextState;
                } else if(inp == "VRATI_SE") {
                    int keep;
                    stream >> keep;
                    m_lexerTransitionTable[state][regexId].keep = keep;
                } else if(inp != "}") {
                    m_lexerTransitionTable[state][regexId].tokenName = inp;
                }
            }
        }
    }
}