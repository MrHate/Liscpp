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
		builtins["expr"] =  [](const List& list) { return new NumAtom(std::pow(dynamic_cast<const NumAtom*>(eval_atom(list[1]))->val, dynamic_cast<const NumAtom*>(eval_atom(list[2]))->val)); };
		builtins["eq?"] =  [](const List& list) { return new NumAtom(dynamic_cast<const NumAtom*>(eval_atom(list[1]))->val == dynamic_cast<const NumAtom*>(eval_atom(list[2]))->val); };
		builtins["equal?"] =  [](const List& list) { return new NumAtom(dynamic_cast<const NumAtom*>(eval_atom(list[1]))->val == dynamic_cast<const NumAtom*>(eval_atom(list[2]))->val); };
		builtins["not"] =  [](const List& list) { return new NumAtom(!(dynamic_cast<const NumAtom*>(eval_atom(list[1]))->val)); };
		builtins["rount"] =  [](const List& list) { return new NumAtom(std::round(dynamic_cast<const NumAtom*>(eval_atom(list[1]))->val)); };
		// TODO: append
		// TODO: apply
		// TODO: begin
		builtins["car"] = [](const List& list) { return dynamic_cast<const ListExp*>(dynamic_cast<const ExpAtom*>(list[1])->exp)->list[0]; };
		// TODO: cdr
		// TODO: cons
		builtins["null?"] = [](const List& list) { return new NumAtom(dynamic_cast<const ListExp*>(dynamic_cast<const ExpAtom*>(list[1])->exp)->list.empty()); };
		builtins["length"] = [](const List& list) { return new NumAtom(dynamic_cast<const ListExp*>(dynamic_cast<const ExpAtom*>(list[1])->exp)->list.size()); };
		// TODO: list
		builtins["list?"] =  [](const List& list) { return new NumAtom(eval_atom(list[1])->kind == Atom::EXP); };
		// TODO: map
		builtins["print"] =  [](const List& list) { eval_atom(list[1])->print(std::cout); return new NumAtom(0); };
		builtins["println"] =  [](const List& list) { eval_atom(list[1])->print(std::cout); std::cout << std::endl; return new NumAtom(0); };
		builtins["number?"] =  [](const List& list) { return new NumAtom(eval_atom(list[1])->kind == Atom::NUM); };
		// TODO: procedure?
		builtins["symbol?"] =  [](const List& list) { return new NumAtom(eval_atom(list[1])->kind == Atom::SYM); };
		builtins["quote"] = [](const List& list) { return list[1]; };

		name2exp.push_back(builtins);
		name2exp.push_back(Dict());
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
	const Atom* val = eval_atom(list[2]);
	Env::getEnv().enter(symbol, [val](const List& list) { return val; });
	return val;
}


const Atom* eval_proc(const std::string oper, const List& list) {
	return Env::getEnv().lookup(oper)(list);
}

const Atom* eval_atom_front(const Atom* a) {
	//std::cout<<"front"<<std::endl;
	if(a->kind != Atom::SYM) return eval_atom(a);

	const std::string symbol = dynamic_cast<const SymbolAtom*>(a)->name;
	//std::cout<<symbol<<std::endl;
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
		case Atom::EXP:
			return eval(dynamic_cast<const ExpAtom*>(a)->exp);
		case Atom::STR:
		case Atom::NUM:
			return a;
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

				// I treat empty list as zero
				if(list.empty()) return new NumAtom(0);
				// To evaluate lists of single element like "(k)"
				if(list.size() == 1) return eval_atom(list[0]);

				const Atom* head = eval_atom_front(list[0]);
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
