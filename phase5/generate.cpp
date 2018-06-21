/*
 * File:	generate.cpp
 *
 * Description:	Generate assembly for given Simple C input.
 *		For this assignment: Call, Function, Assignment, ID, Num, Block
 *							 Everything is an integer scalar or literal.
 *
 *
 *
 *
 */

# include <iostream>
# include <sstream>
# include "lexer.h"
# include "checker.h"
# include "tokens.h"
# include "Symbol.h"
# include "Scope.h"
# include "Type.h"
# include "Tree.h"
# include "Register.h"


using namespace std;

Register *rax = new Register("%rax", "%eax", "%al");
Register *rdi = new Register("%rax", "%edi", "%dil");
Register *rsi = new Register("%rsi", "%esi", "%sil");
Register *rdx = new Register("%rdx", "%edx", "%dl");
Register *rcx = new Register("%rcx", "%ecx", "%cl");
Register *r8 = new Register("%R8", "%R8d", "%R8b");
Register *r9 = new Register("%R9", "%R9d", "%R9b");
std::vector <Register *>paramRegs = {rdi, rsi, rdx, rcx, r8, r9};

void Call::generate() {
	if(_args.size()%2 != 0) {
		cout << "\tsubq\t$8, %rsp" << endl;
	}

	for(int i = _args.size() - 1; i >=0; i--) {
		_args[i]->generate();
		if(i > 5) {
			cout << "\tpushq\t" << _args[i]->_operand << endl;
		} else {
			cout << "\tmovl\t" << _args[i]->_operand << ", " << paramRegs[i]->name(32) << endl;
		}
	}
	cout << "\tcall\t" << _id->name() << endl;
}

void Function::generate() {
	//Symbol list of scope
	const Symbols &symbol_list = _body->declarations()->symbols();
	//save number of parameters
	unsigned parameter_num = _id->type().parameters()->size();
	int offset = 0;
	//Symbols symbol_list = the_scope->symbols();

	for(unsigned i = 0; i < symbol_list.size(); i++) {
		if(i >= 6 && i < parameter_num)
			symbol_list[i]->_offset = 16 + 8 * (i - 6);
		else {
			offset -= symbol_list[i]->type().size();
			symbol_list[i]->_offset = offset;
		}
	}

	unsigned framesize = -offset;

	while(framesize%16 != 0)
		framesize++;

	//prologue
	cout << "\ttpushq\t%rbp" << endl;
	cout << "\tmovq\t%rsp, %rbp" << endl;
	cout << "\tsubq\t$" << framesize << ", %rsp" << endl << endl;

	for(unsigned i = 0; i < min(6U, parameter_num); i++){
		cout << "\tmovl\t" << paramRegs[i] << ", "
		<< symbol_list[i]->_offset << "(%rbp)" << endl;
	}

	_body->generate();

	//epilogue
	cout << "\tmovq\t%rbp, %rsp" << endl;
	cout << "\tpopq\t%rbp" << endl;
	cout << "\tret" << endl << endl;
}

void Block::generate() {
	//generate code for all statements in function body
	for(int i = 0; i < _stmts.size(); i++) {
		_stmts[i]->generate();
	}
}

void Identifier::generate() {
	int id_offset = _symbol->_offset;
	stringstream offstring;

	if(id_offset) {
		offstring << id_offset;
		_operand = offstring.str() + "(%ebp)";
	} else {
		_operand = _symbol->name();
	}
}

void Number::generate() {
	//numbers are stored with $
	_operand += "$" + value();
}

void Assignment::generate() {
	//movl	right, left
	cout << "movl\t";
	_right->generate();
	_left->generate();
	cout << _right->_operand << ", " << _left->_operand << endl;
}