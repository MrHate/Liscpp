#ifndef _LISP_INCLUDE_EVAL
#define _LISP_INCLUDE_EVAL

#include <string>
#include <vector>
#include <map>

#include "types.hpp"

namespace LISP {
	const Atom* eval(const Exp*);
} //namespace LISP

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

const Atom* eval_if(const List& list) {
	assert(list.size() == 4);
	const NumAtom* comp = dynamic_cast<const NumAtom*>(eval(list[1]));
	return comp->val? eval(list[2]): eval(list[3]);
}

void eval_define(const List& list) {

}

const Atom* eval_proc(const std::string oper, const List& list) {
	if(oper == "+") {
		assert(list.size() == 3);
		const NumAtom *lhs = dynamic_cast<const NumAtom*>(eval(list[1])), *rhs = dynamic_cast<const NumAtom*>(eval(list[2]));
		assert(lhs && rhs);
		return new NumAtom(lhs->val + rhs->val);
	}
	else if(oper == "-") {
		assert(list.size() == 3);
		const NumAtom *lhs = dynamic_cast<const NumAtom*>(eval(list[1])), *rhs = dynamic_cast<const NumAtom*>(eval(list[2]));
		return new NumAtom(lhs->val - rhs->val);
	}
	else if(oper == "*") {
		assert(list.size() == 3);
		const NumAtom *lhs = dynamic_cast<const NumAtom*>(eval(list[1])), *rhs = dynamic_cast<const NumAtom*>(eval(list[2]));
		return new NumAtom(lhs->val * rhs->val);
	}
	else if(oper == "/") {
		assert(list.size() == 3);
		const NumAtom *lhs = dynamic_cast<const NumAtom*>(eval(list[1])), *rhs = dynamic_cast<const NumAtom*>(eval(list[2]));
		return new NumAtom(lhs->val / rhs->val);
	}
	return nullptr;
}

} // anonymous namespace

namespace LISP {

const Atom* eval(const Exp* e) {
	if(!e) return nullptr;
	if(e->kind == Exp::ATOM) return dynamic_cast<const AtomExp*>(e)->a;

	const List& list = dynamic_cast<const ListExp*>(e)->list;
	const Atom* head = eval(list[0]);
	if(list.size() == 1) return head;

	assert(head->kind == Atom::SYM);
	const std::string oper = dynamic_cast<const SymbolAtom*>(head)->name;

	if(oper == "if") return eval_if(list);
	else if(oper == "define") eval_define(list);
	else return eval_proc(oper, list);

	return nullptr;
}

} // namespace LISP

#endif
