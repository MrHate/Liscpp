#include <iostream>
#include <string>

#include "Parser.hpp"

using namespace std;

int main(int argc, char *argv[]) {

	if(argc < 2) {
		cout << "Usage: main <file>" << endl;
		exit(1);
	}

	Parser parser(argv[1]);
	while(1) {
		string s = parser.nextAtom();
		if(parser.eof()) break;
		cout << s;
	}

	return 0;

}
