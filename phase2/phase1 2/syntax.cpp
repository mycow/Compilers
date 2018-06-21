# include <cstdio>
# include <cctype>
# include <string>
# include <cstdlib>
# include <iostream>
# include "lexer.h"
# include "syntax.h"

using namespace std;

int lookahead, nexttoken;
string lexbuf, nextbuf;

int peek(){
	if(!nexttoken)
		nexttoken = lexan(nextbuf);
	return nexttoken;
}

void match(int t) {
	if(lookahead != t){
		report("Does not match","");
		cerr << "Error: Expecting " << t << " Got: " << lookahead <<endl;
	}

	if(nexttoken) {
		lookahead = nexttoken;
		lexbuf = nextbuf;
		nexttoken = 0;
	} else
		cerr << lookahead << endl;
		lookahead = lexan(lexbuf);
}

void REMAINING_DEC() {
	while(lookahead != SEMICOLON) {
		match(COMMA);
		GLOBAL_DECLARATOR();
	}
	match(SEMICOLON);
}

void TRANSLATION_UNIT() {
	SPECIFIER();
	POINTERS();
	match(ID);

	if(lookahead == LEFTPAR) {
		match(LEFTPAR);

		if(isSpec(lookahead) || lookahead == VOID)
			FUNCTION_DEFINITION();
		else {
			match(RIGHTPAR);
			REMAINING_DEC();
		}
	} else if(lookahead == LEFTBRACKET) {
		match(LEFTBRACKET);
		match(NUM);
		match(RIGHTBRACKET);
		REMAINING_DEC();
	} else {
		REMAINING_DEC();
	}
}

void GLOBAL_DECLARATOR() {
	POINTERS();
	match(ID);
	if(lookahead == LEFTPAR) {
		match(LEFTPAR);
		match(RIGHTPAR);
	} else if(lookahead == LEFTBRACKET) {
		match(LEFTBRACKET);
		match(NUM);
		match(RIGHTBRACKET);
	}
}

void POINTERS(){
	while(lookahead == STAR) {
		match(STAR);
	}
}

void SPECIFIER() {
	if(lookahead == INT)
		match(INT);
	else if(lookahead == LONG)
		match(LONG);
	else if(lookahead == CHAR)
		match(CHAR);
	else
		report("Expecting SPECIFIER","");
}

void FUNCTION_DEFINITION() {
	match(RIGHTPAR);
	match(LEFTBRACE);
	DECLARATIONS();
	STATEMENTS();
	match(RIGHTBRACE);
}

void PARAMETERS() {
	if(lookahead == VOID)
		match(VOID);

	while(isSpec(lookahead))
		PARAMETER_LIST();
}

void PARAMETER_LIST() {
	PARAMETER();
	while(lookahead == COMMA) {
		match(COMMA);
		PARAMETER_LIST();
	}
}

void PARAMETER() {
	SPECIFIER();
	POINTERS();
	match(ID);
}

void DECLARATIONS() {
	DECLARATION();
	while(isSpec(lookahead)) {
		DECLARATIONS();
	}
}

void DECLARATION() {
	SPECIFIER();
	DECLARATOR_LIST();
	match(SEMICOLON);
}

void DECLARATOR_LIST() {
	DECLARATOR();
	while(lookahead == COMMA) {
		match(COMMA);
		DECLARATOR_LIST();
	}
}

void DECLARATOR() {
	POINTERS();
	match(ID);
	if(lookahead == LEFTBRACKET) {
		match(LEFTBRACKET);
		match(NUM);
		match(RIGHTBRACKET);
	}
}

void STATEMENTS() {
	STATEMENT();
	while(lookahead != RIGHTBRACE) {
		STATEMENTS();
	}
}

void STATEMENT() {
	if(lookahead == LEFTBRACE) {
		match(LEFTBRACE);
		DECLARATIONS();
		STATEMENTS();
		match(RIGHTBRACE);
	} else if(lookahead == RETURN) {
		match(RETURN);
		EXPRESSION();
		match(SEMICOLON);
	} else if(lookahead == WHILE) {
		match(WHILE);
		match(LEFTPAR);
		EXPRESSION();
		match(RIGHTPAR);
		STATEMENT();
	} else if(lookahead == 	IF) {
		match(IF);
		match(LEFTPAR);
		EXPRESSION();
		match(RIGHTPAR);
		STATEMENT();
		if(lookahead == ELSE) {
			match(ELSE);
			STATEMENT();
		}
	} else {
		EXPRESSION();
		if(lookahead == EQUAL) {
			match(EQUAL);
			EXPRESSION();
		} 

		match(SEMICOLON);
	}
}

void EXPRESSION_REMAIN() {
	if(lookahead == ID) {
		match(ID);
		if(lookahead == LEFTPAR) {
			match(LEFTPAR);
			if(lookahead != RIGHTPAR){
				EXPRESSION_LIST();
			} else {
				match(RIGHTPAR);
			}
		}
	} else if(lookahead == STRING) {
		match(STRING);
	} else if(lookahead == NUM) {
		match(NUM);
	} else if(lookahead == LEFTPAR) {
		PAREN_EXPR();
	}
}

void EXPRESSION_LIST() {
	EXPRESSION();
	if(lookahead == COMMA){
		match(COMMA);
		EXPRESSION_LIST();
	}
}

void PAREN_EXPR() {
	match(LEFTPAR);
	EXPRESSION();
	match(RIGHTPAR);
}


bool isSpec(int t) {
	if(t == INT || t == LONG || t == CHAR)
		return true;
	else
		return false;
}

void INDEX_EXPRESSION() {
	EXPRESSION_REMAIN();
	while(lookahead == LEFTBRACKET){
		match(LEFTBRACKET);
		EXPRESSION();
		match(RIGHTBRACKET);
		cout << "index" << endl;
	}
}

void UNARY_EXPRESSION() {
	INDEX_EXPRESSION();
	if(lookahead == ADDRESS){
		match(ADDRESS);
		UNARY_EXPRESSION();
		cout << "addr" << endl;
	} else if(lookahead == STAR){
		match(STAR);
		UNARY_EXPRESSION();
		cout << "deref" << endl;
	} else if(lookahead == NOT) {
		match(NOT);
		UNARY_EXPRESSION();
		cout << "not" << endl;
	} else if(lookahead == NEGATE) {
		match(NEGATE);
		UNARY_EXPRESSION();
		cout << "neg" << endl;
	} else if(lookahead == SIZEOF) {
		match(SIZEOF);
		if(lookahead == LEFTPAR && isSpec(peek())){
			match(LEFTPAR);
			SPECIFIER();
			POINTERS();
			match(RIGHTPAR);
		} else {
			UNARY_EXPRESSION();
		}
		cout << "sizeof" << endl;
	}
}

void CAST_EXPRESSION() {
	if(lookahead == LEFTPAR && isSpec(peek())){
		match(LEFTPAR);
		SPECIFIER();
		POINTERS();
		match(RIGHTPAR);
		cout << "cast" << endl;
	} else {
		UNARY_EXPRESSION();
	}
}

void MULTIPLICATIVE_EXPRESSION() {
	CAST_EXPRESSION();
	while(lookahead == STAR || lookahead == DIV || lookahead == PERCENT) {
		if(lookahead == STAR) {
			match(STAR);
			cout << "mul" << endl;
		} else if(lookahead == DIV) {
			match(DIV);
			cout << "div" << endl;
		} else if(lookahead == PERCENT) {
			match(PERCENT);
			cout << "rem" << endl;
		}
	}
}

void ADDITIVE_EXPRESSION() {
	MULTIPLICATIVE_EXPRESSION();
	while(lookahead == ADD || lookahead == NEGATE) {
		if(lookahead == ADD) {
			match(ADD);
			MULTIPLICATIVE_EXPRESSION();
			cout << "add" << endl;
		} else if(lookahead == NEGATE) {
			match(NEGATE);
			MULTIPLICATIVE_EXPRESSION();
			cout << "sub" << endl;
		}
	}
}

void RELATIONAL_EXPRESSION() {
	ADDITIVE_EXPRESSION();
	while(lookahead == LESSTHAN || lookahead == LEQ || lookahead == GREATERTHAN || lookahead == GEQ) {
		if(lookahead == LESSTHAN) {
			match(LESSTHAN);
			cout << "ltn" << endl;
			ADDITIVE_EXPRESSION();
		} else if(lookahead == LEQ) {
			match(LEQ);
			cout << "leq" << endl;
			ADDITIVE_EXPRESSION();
		} else if(lookahead == GREATERTHAN) {
			match(GREATERTHAN);
			cout << "gtn" << endl;
			ADDITIVE_EXPRESSION();
		} else if(lookahead == GEQ) {
			match(GEQ);
			cout << "geq" << endl;
			ADDITIVE_EXPRESSION();
		}
	}
}

void EQUALITY_EXPRESSION() {
	RELATIONAL_EXPRESSION();
	while(lookahead == EQUAL || lookahead == NEQ) {
		if(lookahead == EQUAL) {
			match(EQUAL);
			cout << "eql" << endl;
			RELATIONAL_EXPRESSION();
		} else if(lookahead == NEQ) {
			match(NEQ);
			cout << "neq" << endl;
			RELATIONAL_EXPRESSION();
		}
	}
}

void LOGICAL_AND_EXPRESSION() {
	EQUALITY_EXPRESSION();
	while(lookahead == AND) {
		match(AND);
		EQUALITY_EXPRESSION();
		cout << "and" << endl;
	}
}

void LOGICAL_OR_EXPRESSION() {
	LOGICAL_AND_EXPRESSION();
	while(lookahead == OR) {
 		match(OR);
		LOGICAL_AND_EXPRESSION();
		cout << "or" << endl;
	}
}

void EXPRESSION() {
	LOGICAL_OR_EXPRESSION();
}

int main() {
	nexttoken = 0;
	lookahead = lexan(lexbuf);

	while(lookahead != DONE){
		TRANSLATION_UNIT();
	}
}
