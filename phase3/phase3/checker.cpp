/*
 * File:	checker.cpp
 *
 * Description:	Checks legality of expressions.
 *		
 *		
 */
# include <iostream>
# include <string>
# include "checker.h"
# include "scope.h"
# include "lexer.h"

using namespace std;

static string redef = "redefinition of '%s'"; //type matches, name matches, no params
static string confl = "conflicting types for '%s'"; //name matches, type doesn't
static string redec = "redeclaration of '%s'"; //type and name match
static string undec = "'%s' undeclared"; //no name

Scope *globalScope;
Scope *currentScope;

void openSCOPE() {
	currentScope = new Scope(currentScope);
	if(!globalScope) {
		globalScope = currentScope;
		currentScope = globalScope;
	}
	cout << "Open Scope" << endl;
}

void closeSCOPE() {
	if(currentScope) {
		currentScope = currentScope->enclosing();
	}
	cout << "Close Scope" << endl;
}

void decVARIABLE(string name, Type type){
	//Symbol *sym = new Symbol(name, type);
	Symbol *symbol = currentScope->find(name);
	if(!symbol){
		symbol = new Symbol(name, type);
		currentScope->insert(symbol);
		cout << "Declared " << symbol->getType() << " " << symbol->getName() << endl;
	} else if((symbol->getType() != type && name.compare(symbol->getName()) == 0) && (currentScope == globalScope)) {
		report(confl, name);
	} else if((symbol->getType() == type && name.compare(symbol->getName()) == 0) && (currentScope!=globalScope)) {
		report(redec, name);
	}
}

void defFUNCTION(string name, Type type) {
	Symbol *symbol = globalScope->find(name);

	if(!symbol) {
		//symbol = globalScope->lookup(name);
		symbol = new Symbol(name, type);
		globalScope->insert(symbol);
		cout << "Define " << symbol->getType() << " " << symbol->getName() << endl;
	} else if(symbol->getType() != type && name.compare(symbol->getName()) == 0) {
		report(confl, name);
	} else if(symbol->getType() == type && name.compare(symbol->getName()) == 0 && (symbol->getType()).parameters() != nullptr){
		report(redef, name);
	}
}

void useIDENTIFIER(string name) {
	if(!currentScope->lookup(name)) {
		if(!globalScope->find(name)){
			report(undec, name);
		}
	}
}

/*void decARRAY(string varName, int specifierType, int indirection, string arrSize) {
	cout << "Declaring " << varName << ": (" << specifierType << ", "
	<< indirection << ", ARRAY)" << " of size " << arrSize << endl;
}

void decSCALAR(string varName, int specifierType, int indirection) {
	cout << "Declaring " << varName << ": (" << specifierType << ", "
	<< indirection << ", SCALAR)" << endl;
}*/

/*void decFUNCTION(string name, Type type) {
	Symbol *symbol = globalScope->find(name);

	if(!symbol) {
		symbol = new Symbol(name, type);
		globalScope->insert(symbol);
	} else if(symbol->getType().specifier() != type.specifier() && symbol->getType().indirection() != type.indirection()) {
		cout << "symbol exists 3" << endl;
		report(confl, name);
	}
}*/