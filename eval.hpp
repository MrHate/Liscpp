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
const Atom* eval_atom_front(const Atom*);

static int def_depth = 0;

class Env {
	using Func = std::function<const Atom*(const List&)>;
	using Dict = std::map<std::string, Func>;

	std::vector<Dict> name2exp;

	explicit Env() {
		Dict builtins;
		builtins["+"] =  [](const List& list) { 
			double res = dynamic_cast<const NumAtom*>(eval_atom(list[1]))->val;
			for(int i=2; i<list.size(); ++i) res += dynamic_cast<const NumAtom*>(eval_atom(list[i]))->val;
			return new NumAtom(res);
	 	};
		builtins["-"] =  [](const List& list) { 
			double res = dynamic_cast<const NumAtom*>(eval_atom(list[1]))->val;
			for(int i=2; i<list.size(); ++i) res -= dynamic_cast<const NumAtom*>(eval_atom(list[i]))->val;
			return new NumAtom(res);
	 	};
		builtins["*"] =  [](const List& list) { 
			double res = dynamic_cast<const NumAtom*>(eval_atom(list[1]))->val;
			for(int i=2; i<list.size(); ++i) res *= dynamic_cast<const NumAtom*>(eval_atom(list[i]))->val;
			return new NumAtom(res);
	 	};
		builtins["/"] =  [](const List& list) { 
			double res = dynamic_cast<const NumAtom*>(eval_atom(list[1]))->val;
			for(int i=2; i<list.size(); ++i) res /= dynamic_cast<const NumAtom*>(eval_atom(list[i]))->val;
			return new NumAtom(res);
	 	};
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
		builtins["append"] = [](const List& list) { 
			auto& origin = dynamic_cast<const ListExp*>(dynamic_cast<const ExpAtom*>(list[2])->exp)->list;
			auto newlist = List(dynamic_cast<const ListExp*>(dynamic_cast<const ExpAtom*>(list[1])->exp)->list);
			newlist.insert(newlist.end(), origin.begin(), origin.end());
			return new ExpAtom(new ListExp(newlist));
		};
		builtins["apply"] = [](const List& list) {
			auto formals = List(dynamic_cast<const ListExp*>(dynamic_cast<const ExpAtom*>(list[1])->exp)->list);
			formals.insert(formals.begin(), list[2]);
			return eval(new ListExp(formals));
		};
		builtins["begin"] = [](const List& list) {
			for(int i=1;i<list.size()-1;++i) eval_atom(list[i]);
			return eval_atom(list.back());
		};
		builtins["car"] = [](const List& list) { return dynamic_cast<const ListExp*>(dynamic_cast<const ExpAtom*>(list[1])->exp)->list[0]; };
		builtins["cons"] = [](const List& list) { 
			auto& origin = dynamic_cast<const ListExp*>(dynamic_cast<const ExpAtom*>(list[2])->exp)->list;
			auto newlist = List(origin);
			newlist.insert(newlist.begin(), list[1]);
			return new ExpAtom(new ListExp(newlist));
		};
		builtins["cdr"] = [](const List& list) { 
			auto& origin = dynamic_cast<const ListExp*>(dynamic_cast<const ExpAtom*>(list[1])->exp)->list;
			return new ExpAtom(new ListExp(List(origin.begin()+1, origin.end())));
		};
		builtins["null?"] = [](const List& list) { return new NumAtom(dynamic_cast<const ListExp*>(dynamic_cast<const ExpAtom*>(list[1])->exp)->list.empty()); };
		builtins["length"] = [](const List& list) { return new NumAtom(dynamic_cast<const ListExp*>(dynamic_cast<const ExpAtom*>(list[1])->exp)->list.size()); };
		builtins["list"] = [](const List& list) { return new ExpAtom(new ListExp(List(list.begin()+1, list.end()))); };
		builtins["list?"] =  [](const List& list) { return new NumAtom(eval_atom(list[1])->kind == Atom::EXP); };
		// TODO: map
		builtins["print"] =  [](const List& list) { eval_atom(list[1])->print(std::cout); return new NumAtom(0); };
		builtins["println"] =  [](const List& list) { eval_atom(list[1])->print(std::cout); std::cout << std::endl; return new NumAtom(0); };
		builtins["number?"] =  [](const List& list) { return new NumAtom(eval_atom(list[1])->kind == Atom::NUM); };
		builtins["procedure?"] = [](const List& list) {
			if(list[1]->kind == Atom::LAMBDA) return new NumAtom(1);
			auto a = eval_atom_front(list[1]);
			if(a->kind == Atom::LAMBDA) return new NumAtom(1);
			if(a->kind != Atom::SYM) return new NumAtom(0);
			if(Env::getEnv().isBuiltin(dynamic_cast<const SymbolAtom*>(a)->name)) return new NumAtom(1);
			return new NumAtom(eval_atom(a)->kind == Atom::LAMBDA);
		};
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

	void enter(std::string name, const Func f, bool isProc = false) { name2exp.back()[name] = f; }
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
	const bool isBuiltin(std::string name) const {
		for(auto& i: name2exp[0])
			if(i.first == name) return true;
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
	if(val->kind == Atom::LAMBDA){
		Env::getEnv().enter(symbol, [val](const List& list) { 
				if(list.empty()) return val;

				const LambdaAtom* la = dynamic_cast<const LambdaAtom*>(val);
				Env::getEnv().beginScope();
				for(int i=0;i<la->formals.size();++i){
					auto v = eval_atom(list[i+1]);
					Env::getEnv().enter(la->formals[i], [v](const List& list) { return v; });
				}
				auto res = eval_atom(la->body);
				Env::getEnv().endScope();
				return res;
			});
	}
	else 
		Env::getEnv().enter(symbol, [val](const List& list) { return val; });
	return val;
}

const Atom* eval_set(const List& list) {
	assert(list.size() == 3);
	const std::string symbol = dynamic_cast<const SymbolAtom*>(list[1])->name;
	assert(Env::getEnv().exist(symbol));
	return eval_define(list);
}

const Atom* eval_proc(const std::string oper, const List& list) {
	return Env::getEnv().lookup(oper)(list);
}

const Atom* eval_atom_front(const Atom* a) {
	//std::cout<<"front"<<std::endl;
	if(a->kind != Atom::SYM) return eval_atom(a);

	const std::string symbol = dynamic_cast<const SymbolAtom*>(a)->name;
	//std::cout<<symbol<<std::endl;
	if(symbol == "define" || symbol == "if" || symbol == "set!" || symbol == "lambda" || Env::getEnv().exist(symbol)) return a;
	
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

const Atom* eval_lambda(const List& list) {
	// 1. store the whole list into dictory
	// 2. add formals into innermost dictory
	// 3. eval as usual
	// 4. remove the innermost dictory

	assert(list.size() == 3);
	assert(dynamic_cast<const SymbolAtom*>(list[0])->name == "lambda");
	auto formals = LambdaAtom::LambdaFormals();
	for(const Atom* a: dynamic_cast<const ListExp*>(dynamic_cast<const ExpAtom*>(list[1])->exp)->list)
		formals.push_back(dynamic_cast<const SymbolAtom*>(a)->name);
	return new LambdaAtom(formals, list[2]);

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
					else if(oper == "set!") return eval_set(list);
					else if(oper == "lambda") return eval_lambda(list);
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
