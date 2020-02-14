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
	parser.print(std::cout);

	return 0;

}
