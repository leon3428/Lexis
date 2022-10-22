#include "ConfigParser.hpp"
#include <fstream>
#include <string>
#include <unordered_map>
#include "Utils.hpp"
#include "RegexSyntaxTree.hpp"

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
        t.exportDfa(m_stateDfa[m_stateDfa.size() - 1]);
    }
}

void ConfigParser::m_writeDfa(std::ostream &stream) const {
    for(int i = 0;i < m_stateDfa.size();i++) {
        stream << "Token dfa" << i << "(const std::string &program, int start) {\n";
        stream << "\tint tokenId = -1;\n"; 
        stream << "\tint pos = start;\n";
        stream << "\tgoto dfa" << i << "_state" << m_stateDfa[i].getStartState() << ";\n\n";

        for(int dfaState = 0; dfaState < m_stateDfa[i].getStateCount(); dfaState++) {
            stream << "\tdfa" << i << "_state" << dfaState << ":\n";
            
            if(m_stateDfa[i].getAcceptable(dfaState) == -1) {
                stream << "\treturn Token(program, start, pos, tokenId);\n\n";
            } else {
                stream << "\ttokenId = " << m_stateDfa[i].getAcceptable(dfaState) << ";\n";
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

void ConfigParser::generateLexer(std::string &inPath, std::string &outPath) const {
    std::ifstream inputFile(inPath);
    std::ofstream outputFile(outPath);
    std::string line;

    while (getline(inputFile, line)) {
        // Output the text from the file
        std::string trimmedLine = utils::trim(line);

        if(trimmedLine == "$DfaFunctions$") {
            m_writeDfa(outputFile);
        } else if(trimmedLine == "$DfaFunctionsArray$") {
            for(int i = 0;i < m_stateDfa.size(); i++) {
                outputFile << "\t\t&dfa" << i;
                if(i != m_stateDfa.size() - 1)
                    outputFile << ",";
                outputFile << '\n';
            }
        } else {
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
        if( (regex[i] == '{' || regex[i] == '}') && (i == 0 || regex[i - 1] != '\\') )
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

void ConfigParser::parseInput(std::string &filename){
    /* 
    states = stanja
    names = imena leksickih jedinki
    nameToRegex = key je {vrijednost} i vraca regex sa zamijenjenim {}
    */

    std::string inp;
    std::ifstream stream(filename);
    std::unordered_map<std::string, std::string> nameToRegex;
    bool start = true;
    std::vector<std::string> states, names;

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
            modifyRegex(regex, nameToRegex);
            LogInfo("state: %s regex: %s", state.c_str(), regex.c_str());
            addRegexToState(state, regex);

            std::string lexicIndividual;
            stream >> lexicIndividual; // ili stanje ili -

            while(inp != "}") {
                stream >> inp;
                if(inp == "NOVI_REDAK"){
                    continue;
                } else if(inp == "UDJI_U_STANJE") {
                    std::string nextState;
                    stream >> nextState;
                } else if(inp == "VRATI_SE") {
                    int groupFirstN;
                    stream >> groupFirstN;
                }
            }
        }
    }
}