#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <vector>
#include <stack>

#include <iostream>
#include "types.hpp"

namespace LISP {

class Parser {

	List exps;

	void tokenize(std::string& s){

		//trim
		s.erase(0, s.find_first_not_of(" "));
		s.erase(s.find_last_not_of(" ") + 1);
		
		// insert spaces before and after each parenthesis
		std::regex re("\\(|\\)");
		s = std::regex_replace(s, re, " $& ");

	}

	void parenthesize(std::vector<std::string>& tokens){
		std::stack<Atom*> operStack;
		std::stack<List> listStack;

		Atom* curOper = nullptr;
		List curList;

		for(std::string& token: tokens) 
			if(token == "(") {
				operStack.push(curOper);
				listStack.push(curList);

				curOper = nullptr;
				curList.clear();
			}
			else if (token == ")") {
				assert(operStack.size() > 0);
				Exp* pe = new Exp(curOper, curList);
				curOper = operStack.top(); operStack.pop();
				curList = listStack.top(); listStack.pop();
				curList.push_back(pe);
			}
			else {
				Atom* a = nullptr;
				if(token[0] == '"')
					a = new StringAtom(token.substr(1, token.size() - 2));
				else if(isdigit(token[0]))
					a = new NumAtom(std::stod(token));
				else 
					a = new SymbolAtom(token);

				if(!curOper) curOper = a;
				else curList.push_back(new Exp(a));
			}
		
		exps = curList;

	}

public:

	Parser(std::string filename) {

		// read file and parse it into lists
		std::string sourceFile;
		{
			std::ifstream fin(filename);
			assert(fin);

			while(1){
				char c = fin.get();
				if(c == EOF) break;
				sourceFile.push_back(c);
			}
		}
		tokenize(sourceFile);

		std::vector<std::string> tokens;
		{
			std::stringstream sin(sourceFile);
			while(sin){
				std::string token;
				sin >> token;
				tokens.push_back(token);
			}
		}
		parenthesize(tokens);

	}

	void print () {
		for(auto& e: exps) e->print(std::cout);
	}

};

}
