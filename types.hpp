#include <iosfwd>
#include <vector>
#include <string>

namespace LISP {

// Atoms

class Atom {
	int kind;
public:
	enum { NONE = 0, NUM, STR, SYM };

	explicit Atom(int kind = 0): kind(kind) {}
	int Kind() const { return kind; }
	virtual void print(std::ostream&) const = 0;
};

class NumAtom: public Atom {
	double val;
public:
	explicit NumAtom(double val): Atom(Atom::NUM), val(val) {};
	double Value() const { return val; }
	virtual void print(std::ostream& cout) const { cout << val; }
};

class StringAtom: public Atom {
	std::string val;
public:
	explicit StringAtom(std::string s): Atom(Atom::STR), val(s) {};
	std::string Value() const { return val; }
	virtual void print(std::ostream& cout) const { cout << '"' << val << '"'; }
};

class SymbolAtom: public Atom {
	std::string name;
public:
	explicit SymbolAtom(std::string name): Atom(Atom::SYM), name(name) {}
	std::string Value() const { return name; }
	virtual void print(std::ostream& cout) const { cout << name; }
};

// Exp

class Exp;
typedef std::vector<Exp*> List;

class Exp {
public:
	virtual void print(std::ostream& cout) const = 0;
};

class AtomExp: public Exp {
	Atom* a;
public:
	explicit AtomExp(Atom* a): a(a) {}
	virtual void print(std::ostream& cout) const { a->print(cout); }
};

class ListExp: public Exp {
	List list;
public:
	explicit ListExp(const List& list): list(list) {}
	virtual void print(std::ostream& cout) const {
		cout << "( ";
		for(auto& e: list) {
			e->print(cout);
			cout << ' ';
		}
		cout << ")";
	}
};

} // namespace LISP
