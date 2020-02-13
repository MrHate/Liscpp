#include <fstream>
#include <string>

class Parser {

	std::ifstream fin;

public:

	Parser(std::string filename): fin(filename) {}
	
	std::string nextAtom() {
		if(fin.eof()) return "";

		std::string atom;
		fin >> atom;
		return atom;
	}
	bool eof() const { return fin.eof(); }

};
