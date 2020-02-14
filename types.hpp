#ifndef _LISP_INCLUDE_TYPES
#define _LISP_INCLUDE_TYPES

#include <iosfwd>
#include <vector>
#include <string>

namespace LISP {
	class Atom;
	class Exp;
} // namespace LISP

namespace {
	void _print(const LISP::Atom*, std::ostream&);
	void _print(const LISP::Exp* , std::ostream&);
} // anonymous namespace

namespace LISP {

// Atoms

class Atom {
public:
	enum { NONE = 0, NUM, STR, SYM, EXP, LAMBDA };
	const int kind;

	explicit Atom(int kind = 0): kind(kind) {}
	virtual void print(std::ostream&) const = 0;
};

class NumAtom: public Atom {
public:
	const double val;
	explicit NumAtom(double val): Atom(Atom::NUM), val(val) {};
	virtual void print(std::ostream& cout) const { cout << val; }
};

class StringAtom: public Atom {
public:
	const std::string val;
	explicit StringAtom(std::string s): Atom(Atom::STR), val(s) {};
	virtual void print(std::ostream& cout) const { cout << val; }
};

class SymbolAtom: public Atom {
public:
	const std::string name;
	explicit SymbolAtom(std::string name): Atom(Atom::SYM), name(name) {}
	virtual void print(std::ostream& cout) const { cout << name; }
};

class ExpAtom: public Atom {
public:
	const Exp* exp;
	explicit ExpAtom(Exp* exp): Atom(Atom::EXP), exp(exp) {}
	virtual void print(std::ostream& cout) const { _print(exp, cout); }
};

// Exp
typedef std::vector<Atom*> List;

class Exp {
public:
	enum { NONE = 0, ATOM, LIST };
	const int kind;
	explicit Exp(int kind = 0): kind(kind) {}
	virtual void print(std::ostream& cout) const = 0;
};

class AtomExp: public Exp {
public:
	const Atom* a;
	explicit AtomExp(Atom* a): Exp(Exp::ATOM), a(a) {}
	virtual void print(std::ostream& cout) const { _print(a, cout); }
};

class ListExp: public Exp {
public:
	const List list;
	explicit ListExp(const List& list): Exp(Exp::LIST), list(list) {}
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

namespace {
	void _print(const LISP::Atom* a, std::ostream& cout) { a->print(cout); }
	void _print(const LISP::Exp* e, std::ostream& cout) { e->print(cout); }
} // anonymous namespace

#endif
