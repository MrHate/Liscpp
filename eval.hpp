#ifndef _LISP_INCLUDE_EVAL
#define _LISP_INCLUDE_EVAL

#include <string>
#include <vector>
#include <map>

#include "types.hpp"

namespace {

using namespace LISP;

class Env {
	typedef std::map<std::string, Exp*> Dict;
	std::vector<Dict> name2exp;
public:
	void enter(std::string name, Exp* e) { name2exp.back()[name] = e; }
	void beginScope() { name2exp.push_back(Dict()); }
	void endScope() { assert(name2exp.size() > 1); name2exp.resize(name2exp.size() - 1); }
	Exp* lookup(std::string name) const {
		for(auto i = name2exp.rbegin(); i != name2exp.rend(); ++i)
			if(i->count(name)) return i->at(name);
		return nullptr;
	}
};

Atom* eval_if(const List& list) {
	return nullptr;
}

void eval_define(const List& list) {

}

Atom* eval_proc(std::string oper, const List& list) {
	return nullptr;
}

} // anonymous namespace

namespace LISP {

const Atom* eval(Exp* e) {
	if(e->kind == Exp::ATOM) return dynamic_cast<const AtomExp*>(e)->a;

	const List& list = dynamic_cast<ListExp*>(e)->list;
	const Atom* head = eval(list[0]);
	assert(head->kind == Atom::SYM);
	const std::string oper = dynamic_cast<const SymbolAtom*>(head)->name;

	return nullptr;
}

} // namespace LISP

#endif
