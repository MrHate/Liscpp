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
	cout << endl;

	LISP::eval(parser.get())->print(cout);

	return 0;

}
