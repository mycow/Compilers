/*
 * File:	scope.cpp
 *
 * Description:	
 *		
 *		
 */

# include <cstdlib>
# include <iostream>
# include <vector>
# include "scope.h"
# include "symbol.h"

using namespace std;

Scope::Scope(Scope *enclosing)
	:_enclosing(enclosing) {}

void Scope::insert(Symbol *symbol) {
	_symbols.push_back(symbol);
}

void Scope::remove(const string &name) {
	for(int i = 0; i < _symbols.size(); i++) {
		if(_symbols[i]->getName() == name) {
			_symbols.erase(_symbols.begin() + i);
		}
	}
}

Symbol *Scope::find(const string &name) const {
	for(int i = 0; i < _symbols.size(); i++) {
		if(_symbols[i]->getName() == name) {
			return _symbols[i];
		}
	}
	return nullptr;
}

Symbol *Scope::lookup(const string &name) const {
	Symbol *exist = find(name);
	if(exist){
		return exist;
	} else if(enclosing()) {
		enclosing()->lookup(name);
	}
	return nullptr;
}

Scope *Scope::enclosing() const {
	return _enclosing;
}

const Symbols &Scope::symbols() const {
	return _symbols;
}