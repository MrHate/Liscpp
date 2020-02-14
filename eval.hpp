#ifndef _LISP_INCLUDE_EVAL
#define _LISP_INCLUDE_EVAL

#include <string>
#include <vector>
#include <map>
#include <any>
#include <cmath>

#include <iostream>
#include "types.hpp"

namespace LISP {
	const Atom* eval(const Exp*);
} //namespace LISP

namespace {

using namespace LISP;

const Atom* eval_atom(const Atom*);

static int def_depth = 0;


class Env {
	using Func = std::function<const Atom*(const List&)>;
	using Dict = std::map<std::string, Func>;

	std::vector<Dict> name2exp;

	explicit Env() {
		Dict builtins;
		builtins["+"] =  [](const List& list) { return new NumAtom(dynamic_cast<const NumAtom*>(eval_atom(list[1]))->val +  dynamic_cast<const NumAtom*>(eval_atom(list[2]))->val); };
		builtins["-"] =  [](const List& list) { return new NumAtom(dynamic_cast<const NumAtom*>(eval_atom(list[1]))->val -  dynamic_cast<const NumAtom*>(eval_atom(list[2]))->val); };
		builtins["*"] =  [](const List& list) { return new NumAtom(dynamic_cast<const NumAtom*>(eval_atom(list[1]))->val *  dynamic_cast<const NumAtom*>(eval_atom(list[2]))->val); };
		builtins["/"] =  [](const List& list) { return new NumAtom(dynamic_cast<const NumAtom*>(eval_atom(list[1]))->val /  dynamic_cast<const NumAtom*>(eval_atom(list[2]))->val); };
		builtins[">"] =  [](const List& list) { return new NumAtom(dynamic_cast<const NumAtom*>(eval_atom(list[1]))->val >  dynamic_cast<const NumAtom*>(eval_atom(list[2]))->val); };
		builtins["<"] =  [](const List& list) { return new NumAtom(dynamic_cast<const NumAtom*>(eval_atom(list[1]))->val <  dynamic_cast<const NumAtom*>(eval_atom(list[2]))->val); };
		builtins[">="] = [](const List& list) { return new NumAtom(dynamic_cast<const NumAtom*>(eval_atom(list[1]))->val >= dynamic_cast<const NumAtom*>(eval_atom(list[2]))->val); };
		builtins["<="] = [](const List& list) { return new NumAtom(dynamic_cast<const NumAtom*>(eval_atom(list[1]))->val <= dynamic_cast<const NumAtom*>(eval_atom(list[2]))->val); };
		builtins["="] =  [](const List& list) { return new NumAtom(dynamic_cast<const NumAtom*>(eval_atom(list[1]))->val == dynamic_cast<const NumAtom*>(eval_atom(list[2]))->val); };
		builtins["abs"] =  [](const List& list) { return new NumAtom(std::abs(dynamic_cast<const NumAtom*>(eval_atom(list[1]))->val)); };
		builtins["max"] =  [](const List& list) { return new NumAtom(std::max(dynamic_cast<const NumAtom*>(eval_atom(list[1]))->val, dynamic_cast<const NumAtom*>(eval_atom(list[2]))->val)); };
		builtins["min"] =  [](const List& list) { return new NumAtom(std::min(dynamic_cast<const NumAtom*>(eval_atom(list[1]))->val, dynamic_cast<const NumAtom*>(eval_atom(list[2]))->val)); };
		// TODO: append

		name2exp.push_back(builtins);
	}

public:
	static Env& getEnv() {
		static Env _env;
		return _env;
	}

	void enter(std::string name, const Func f) { name2exp.back()[name] = f; }
	void beginScope() { name2exp.push_back(Dict()); }
	void endScope() { assert(name2exp.size() > 1); name2exp.resize(name2exp.size() - 1); }
	const Func& lookup(std::string name) const {
		for(auto i = name2exp.rbegin(); i != name2exp.rend(); ++i)
			if(i->count(name)) return i->at(name);
		assert(0);
	}
	const bool exist(std::string name) const {
		for(auto i = name2exp.rbegin(); i != name2exp.rend(); ++i)
			if(i->count(name)) return true;
		return false;
	}
};

const Atom* eval_if(const List& list) {
	assert(list.size() == 4);
	const NumAtom* comp = dynamic_cast<const NumAtom*>(eval_atom(list[1]));
	return comp->val? eval_atom(list[2]): eval_atom(list[3]);
}

const Atom* eval_define(const List& list) {
	assert(list.size() == 3);
	const std::string symbol = dynamic_cast<const SymbolAtom*>(list[1])->name;
	const NumAtom* val = dynamic_cast<const NumAtom*>(eval_atom(list[2]));
	Env::getEnv().enter(symbol, [val](const List& list) { return val; });
	return val;
}


const Atom* eval_proc(const std::string oper, const List& list) {
	return Env::getEnv().lookup(oper)(list);
}

const Atom* eval_atom_front(const Atom* a) {
	if(a->kind != Atom::SYM) return eval_atom(a);

	const std::string symbol = dynamic_cast<const SymbolAtom*>(a)->name;
	if(symbol == "define" || symbol == "if" || Env::getEnv().exist(symbol)) return a;
	
	return eval_atom(a);
}

const Atom* eval_atom(const Atom* a) {
	switch(a->kind) {
		case Atom::SYM:
			{
				const std::string symbol = dynamic_cast<const SymbolAtom*>(a)->name;
				return Env::getEnv().lookup(symbol)(List());
			}
		case Atom::NUM:
			return a;
		case Atom::EXP:
			return eval(dynamic_cast<const ExpAtom*>(a)->exp);
		default:
			assert(0);
	}
}


} // anonymous namespace

namespace LISP {

const Atom* eval(const Exp* e) {
	assert(e);
	switch(e->kind) {
		case Exp::ATOM:
			return eval_atom(dynamic_cast<const AtomExp*>(e)->a);
		case Exp::LIST:
			{
				const List& list = dynamic_cast<const ListExp*>(e)->list;
				const Atom* head = eval_atom_front(list[0]);
				if(list.size() == 1) return head;

				if(head->kind == Atom::SYM) {
					const std::string oper = dynamic_cast<const SymbolAtom*>(head)->name;
					if(oper == "if") return eval_if(list);
					else if(oper == "define") return eval_define(list);
					else return eval_proc(oper, list);
				}
				else {
					// TODO: may be return a list as an array value, here I just return the last evaluation
					const Atom* ret = head;
					for(int i=1; i<list.size(); ++i) ret = eval_atom(list[i]);
					return ret;
				}
			}
		default:
			assert(0);
	}

}

} // namespace LISP

#endif
