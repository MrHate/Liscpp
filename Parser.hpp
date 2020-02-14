#ifndef _LISP_INCLUDE_PARSER
#define _LISP_INCLUDE_PARSER

#include <iosfwd>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <vector>
#include <stack>

#include "types.hpp"

namespace LISP {

class Parser {

	ListExp* root;

	void tokenize(std::string& s){

		//trim
		s.erase(0, s.find_first_not_of(" "));
		s.erase(s.find_last_not_of(" ") + 1);
		
		// insert spaces before and after each parenthesis
		std::regex re("\\(|\\)");
		s = std::regex_replace(s, re, " $& ");

	}

	void parenthesize(std::vector<std::string>& tokens){
		std::stack<List> listStack;

		List curList;

		for(std::string& token: tokens) 
			if(token == "(") {
				listStack.push(curList);

				curList.clear();
			}
			else if (token == ")") {
				assert(listStack.size() > 0);
				Exp* pe = new ListExp(curList);
				curList = listStack.top(); listStack.pop();
				curList.push_back(new ExpAtom(pe));
			}
			else {
				Atom* a = nullptr;
				if(token[0] == '"')
					a = new StringAtom(token.substr(1, token.size() - 2));
				else if(isdigit(token[0]))
					a = new NumAtom(std::stod(token));
				else 
					a = new SymbolAtom(token);

				curList.push_back(a);
			}
		
		root = new ListExp(curList);

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

	void print (std::ostream& cout) { root->print(cout); }
	const ListExp* get() const { return root; }

};

} // namespace LISP

#endif
