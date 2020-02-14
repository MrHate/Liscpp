#include <iostream>
#include <string>

#include "Parser.hpp"
#include "eval.hpp"

using namespace std;

int main(int argc, char *argv[]) {

	if(argc < 2) {
		cout << "Usage: main <file>" << endl;
		exit(1);
	}

	LISP::Parser parser(argv[1]);
	cout << "Parser.print: " << endl;
	parser.print(std::cout);
	cout << endl << "-------------------------------" << endl;

	const LISP::Atom* ret = LISP::eval(parser.get());
	cout << "-------------------------------" << endl;
	cout << "Result: " << endl;
	ret->print(cout);
	cout << endl;

	return 0;

}
