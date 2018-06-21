/*
 * File:	parser.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the recursive-descent parser for
 *		Simple C.
 */

# include <cstdlib>
# include <iostream>
# include "checker.h"
# include "tokens.h"
# include "lexer.h"

using namespace std;

static int lookahead, nexttoken;
static string lexbuf, nextbuf;

static Type expression(bool& lvalue), castExpression(bool& lvalue);
static void statement(Type& type);


/*
 * Function:	error
 *
 * Description:	Report a syntax error to standard error.
 */

static void error()
{
    if (lookahead == DONE)
	report("syntax error at end of file");
    else
	report("syntax error at '%s'", lexbuf);

    exit(EXIT_FAILURE);
}


/*
 * Function:	match
 *
 * Description:	Match the next token against the specified token.  A
 *		failure indicates a syntax error and will terminate the
 *		program since our parser does not do error recovery.
 */

static void match(int t)
{
    if (lookahead != t)
	error();

    if (nexttoken) {
	lookahead = nexttoken;
	lexbuf = nextbuf;
	nexttoken = 0;
    } else
	lookahead = lexan(lexbuf);
}


/*
 * Function:	peek
 *
 * Description:	Return the next token in the input stream and save it so
 *		that match() will later return it.
 */

static int peek()
{
    if (!nexttoken)
	nexttoken = lexan(nextbuf);

    return nexttoken;
}


/*
 * Function:	number
 *
 * Description:	Match the next token as a number and return its value.
 */

static unsigned long number()
{
    string buf;


    buf = lexbuf;
    match(NUM);
    return strtoul(buf.c_str(), NULL, 0);
}


/*
 * Function:	identifier
 *
 * Description:	Match the next token as an identifier and return its name.
 */

static string identifier()
{
    string buf;


    buf = lexbuf;
    match(ID);
    return buf;
}


/*
 * Function:	isSpecifier
 *
 * Description:	Return whether the given token is a type specifier.
 */

static bool isSpecifier(int token)
{
    return token == CHAR || token == INT || token == LONG;
}


/*
 * Function:	specifier
 *
 * Description:	Parse a type specifier.  Simple C has char, int, and long.
 *
 *		specifier:
 *		  char
 *		  int
 *		  long
 */

static int specifier()
{
    int typespec = ERROR;


    if (isSpecifier(lookahead)) {
	typespec = lookahead;
	match(lookahead);
    } else
	error();

    return typespec;
}


/*
 * Function:	pointers
 *
 * Description:	Parse pointer declarators (i.e., zero or more asterisks).
 *
 *		pointers:
 *		  empty
 *		  * pointers
 */

static unsigned pointers()
{
    unsigned count = 0;


    while (lookahead == '*') {
	match('*');
	count ++;
    }

    return count;
}


/*
 * Function:	declarator
 *
 * Description:	Parse a declarator, which in Simple C is either a scalar
 *		variable or an array, with optional pointer declarators.
 *
 *		declarator:
 *		  pointers identifier
 *		  pointers identifier [ num ]
 */

static void declarator(int typespec)
{
    unsigned indirection;
    string name;


    indirection = pointers();
    name = identifier();

    if (lookahead == '[') {
	match('[');
	declareVariable(name, Type(typespec, indirection, number()));
	match(']');

    } else
	declareVariable(name, Type(typespec, indirection));
}


/*
 * Function:	declaration
 *
 * Description:	Parse a local variable declaration.  Global declarations
 *		are handled separately since we need to detect a function
 *		as a special case.
 *
 *		declaration:
 *		  specifier declarator-list ;
 *
 *		declarator-list:
 *		  declarator
 *		  declarator , declarator-list
 */

static void declaration()
{
    int typespec;


    typespec = specifier();
    declarator(typespec);

    while (lookahead == ',') {
	match(',');
	declarator(typespec);
    }

    match(';');
}


/*
 * Function:	declarations
 *
 * Description:	Parse a possibly empty sequence of declarations.
 *
 *		declarations:
 *		  empty
 *		  declaration declarations
 */

static void declarations()
{
    while (isSpecifier(lookahead))
	declaration();
}


/*
 * Function:	primaryExpression
 *
 * Description:	Parse a primary expression.
 *
 *		primary-expression:
 *		  ( expression )
 *		  identifier ( expression-list )
 *		  identifier ( )
 *		  identifier
 *		  string
 *		  num
 *
 *		expression-list:
 *		  expression
 *		  expression , expression-list
 */

static Type primaryExpression(bool& lvalue)
{
	Type type;
	Parameters *temp = nullptr;
    if (lookahead == '(') {
		match('(');
		type = expression(lvalue);
		match(')');

    } else if (lookahead == STRING) {
    	string st = lexbuf;
		match(STRING);
		type = Type(CHAR, 0, st.length()-2);
		lvalue = false;

    } else if (lookahead == NUM) {
		match(NUM);
		if(lexbuf == "long")
			type = Type(LONG, 0);
		lvalue = false;

    } else if (lookahead == ID) {
    	string name = identifier();
		type = checkIdentifier(identifier())->type();

		if (lookahead == '(') {
		    match('(');

		    if (lookahead != ')') {
				temp->push_back(expression(lvalue));

				while (lookahead == ',') {
				    match(',');
				    temp->push_back(expression(lvalue));
				}
				type = Type(type.specifier(), type.indirection(), temp);
		    }

		    match(')');
			lvalue = false;
			Symbol* func = checkFunction(name);
			Type t = func->type();
			type = checkFunctionCall(t, *temp);
		}

    } else
		error();

	return type;
}


/*
 * Function:	postfixExpression
 *
 * Description:	Parse a postfix expression.
 *
 *		postfix-expression:
 *		  primary-expression
 *		  postfix-expression [ expression ]
 */

static Type postfixExpression(bool& lvalue)
{
    Type left = primaryExpression(lvalue);

    while (lookahead == '[') {
		match('[');
		Type right = expression(lvalue);
		match(']');
		left = checkPOSTFIX(left, right, lvalue);
		lvalue = true;
    }

    return left;
}


/*
 * Function:	unaryExpression
 *
 * Description:	Parse a unary expression.
 *
 *		unary-expression:
 *		  postfix-expression
 *		  ! cast-expression
 *		  - cast-expression
 *		  * cast-expression
 *		  & cast-expression
 *		  sizeof unary-expression
 *		  sizeof ( specifier pointers )
 */

static Type unaryExpression(bool& lvalue)
{
	Type type;

    if (lookahead == '!') {
		match('!');
		type = castExpression(lvalue);
		type = checkNOT(type);
		lvalue = false;

    } else if (lookahead == '-') {
		match('-');
		type = castExpression(lvalue);
		type = checkNEG(type);
		lvalue = false;

    } else if (lookahead == '*') {
		match('*');
		type = castExpression(lvalue);
		type = checkDEREF(type);
		lvalue = true;

    } else if (lookahead == '&') {
		match('&');
		type = castExpression(lvalue);
		type = checkADDR(type, lvalue);
		lvalue = false;

    } else if (lookahead == SIZEOF) {
		match(SIZEOF);

		if (lookahead == '(' && isSpecifier(peek())) {
		    match('(');
		    specifier();
		    pointers();
		    match(')');
		} else{
		    type = unaryExpression(lvalue);
		    type = checkSIZEOF(type);
		    lvalue = false;
		}

		lvalue = false;

    } else
		type = postfixExpression(lvalue);


	return type;
}


/*
 * Function:	castExpression
 *
 * Description:	Parse a cast expression.  If the token after the opening
 *		parenthesis is not a type specifier, we could have a
 *		parenthesized expression instead.
 *
 *		cast-expression:
 *		  unary-expression
 *		  ( specifier pointers ) cast-expression
 */

static Type castExpression(bool& lvalue)
{
	Type right;
    if (lookahead == '(' && isSpecifier(peek())) {
		match('(');
		int spec = specifier();
		int indi = pointers();
		Type left = Type(spec, indi);
		match(')');
		right = castExpression(lvalue);
		left = checkCAST(left, right);

    } else
		right = unaryExpression(lvalue);
	
	lvalue = false;

	return right;
}


/*
 * Function:	multiplicativeExpression
 *
 * Description:	Parse a multiplicative expression.
 *
 *		multiplicative-expression:
 *		  cast-expression
 *		  multiplicative-expression * cast-expression
 *		  multiplicative-expression / cast-expression
 *		  multiplicative-expression % cast-expression
 */

static Type multiplicativeExpression(bool& lvalue)
{
    Type left = castExpression(lvalue);

    while (1) {
		if (lookahead == '*') {
		    match('*');
		    Type right = castExpression(lvalue);
		    left = checkMUL(left, right);
		    lvalue = false;

		} else if (lookahead == '/') {
		    match('/');
		    Type right = castExpression(lvalue);
		    left = checkDIV(left, right);
		    lvalue = false;

		} else if (lookahead == '%') {
		    match('%');
		    Type right = castExpression(lvalue);
		    left = checkPERCENT(left, right);
		    lvalue = false;

		} else
		    break;
    }
    return left;
}


/*
 * Function:	additiveExpression
 *
 * Description:	Parse an additive expression.
 *
 *		additive-expression:
 *		  multiplicative-expression
 *		  additive-expression + multiplicative-expression
 *		  additive-expression - multiplicative-expression
 */

static Type additiveExpression(bool& lvalue)
{
    Type left = multiplicativeExpression(lvalue);

    while (1) {
		if (lookahead == '+') {
		    match('+');
		    Type right = multiplicativeExpression(lvalue);
		    left = checkADD(left, right);
		    lvalue = false;

		} else if (lookahead == '-') {
		    match('-');
		    Type right = multiplicativeExpression(lvalue);
		    left = checkSUB(left, right);
		    lvalue = false;

		} else
		    break;
    }
    return left;
}


/*
 * Function:	relationalExpression
 *
 * Description:	Parse a relational expression.  Note that Simple C does not
 *		have shift operators, so we go immediately to additive
 *		expressions.
 *
 *		relational-expression:
 *		  additive-expression
 *		  relational-expression < additive-expression
 *		  relational-expression > additive-expression
 *		  relational-expression <= additive-expression
 *		  relational-expression >= additive-expression
 */

static Type relationalExpression(bool& lvalue)
{
    Type left = additiveExpression(lvalue);

    while (1) {
		if (lookahead == '<') {
		    match('<');
		    Type right = additiveExpression(lvalue);
		    left = checkLESSTHAN(left, right);
		    lvalue = false;

		} else if (lookahead == '>') {
		    match('>');
		    Type right = additiveExpression(lvalue);
		    left = checkGREATERTHAN(left, right);
		    lvalue = false;

		} else if (lookahead == LEQ) {
		    match(LEQ);
		    Type right = additiveExpression(lvalue);
		    left = checkLEQ(left, right);
		    lvalue = false;

		} else if (lookahead == GEQ) {
		    match(GEQ);
		    Type right = additiveExpression(lvalue);
		    left = checkGEQ(left, right);
		    lvalue = false;

		} else
		    break;
    }
    return left;
}


/*
 * Function:	equalityExpression
 *
 * Description:	Parse an equality expression.
 *
 *		equality-expression:
 *		  relational-expression
 *		  equality-expression == relational-expression
 *		  equality-expression != relational-expression
 */

static Type equalityExpression(bool& lvalue)
{
    Type left = relationalExpression(lvalue);

    while (1) {
		if (lookahead == EQL) {
		    match(EQL);
		    Type right = relationalExpression(lvalue);
		    left = checkEQUAL(left, right);
			lvalue = false;

		} else if (lookahead == NEQ) {
		    match(NEQ);
		    Type right = relationalExpression(lvalue);
		    left = checkNOTEQUAL(left, right);
			lvalue = false;

		} else
		    break;
    }
    return left;
}


/*
 * Function:	logicalAndExpression
 *
 * Description:	Parse a logical-and expression.  Note that Simple C does
 *		not have bitwise-and expressions.
 *
 *		logical-and-expression:
 *		  equality-expression
 *		  logical-and-expression && equality-expression
 */

static Type logicalAndExpression(bool& lvalue)
{
    Type left = equalityExpression(lvalue);

    while (lookahead == AND) {
		match(AND);
		Type right = equalityExpression(lvalue);
		left = checkLogicalAND(left, right);
		lvalue = false;
    }
    return left;
}


/*
 * Function:	expression
 *
 * Description:	Parse an expression, or more specifically, a logical-or
 *		expression, since Simple C does not allow comma or
 *		assignment as an expression operator.
 *
 *		expression:
 *		  logical-and-expression
 *		  expression || logical-and-expression
 */

static Type expression(bool& lvalue)
{
    Type left = logicalAndExpression(lvalue);

    while (lookahead == OR) {
		match(OR);
		Type right = logicalAndExpression(lvalue);
		left = checkLogicalOR(left, right);
		lvalue = false;
    }
    return left;
}


/*
 * Function:	statements
 *
 * Description:	Parse a possibly empty sequence of statements.  Rather than
 *		checking if the next token starts a statement, we check if
 *		the next token ends the sequence, since a sequence of
 *		statements is always terminated by a closing brace.
 *
 *		statements:
 *		  empty
 *		  statement statements
 */

static void statements(Type& type)
{
    while (lookahead != '}')
	statement(type);
}


/*
 * Function:	statement
 *
 * Description:	Parse a statement.  Note that Simple C has so few
 *		statements that we handle them all in this one function.
 *
 *		statement:
 *		  { declarations statements }
 *		  return expression ;
 *		  while ( expression ) statement
 *		  if ( expression ) statement
 *		  if ( expression ) statement else statement
 *		  expression = expression ;
 *		  expression ;
 */

static void statement(Type& type)
{
	bool lvalue = false;
	Type temp, left, right;

    if (lookahead == '{') {
		match('{');
		openScope();
		declarations();
		statements(type);
		closeScope();
		match('}');

    } else if (lookahead == RETURN) {
		match(RETURN);
		temp = expression(lvalue);
		checkRETURN(type, temp);
		match(';');

    } else if (lookahead == WHILE) {
		match(WHILE);
		match('(');
		temp = expression(lvalue);
		checkWHILE(temp);
		match(')');
		statement(temp);

    } else if (lookahead == IF) {
		match(IF);
		match('(');
		temp = expression(lvalue);
		checkIF(temp);
		match(')');
		statement(temp);

		if (lookahead == ELSE) {
		    match(ELSE);
		    statement(temp);
		}

    } else {
		left = expression(lvalue);

		if (lookahead == '=') {
		    match('=');
		    right = expression(lvalue);
		    checkASSIGN(left, right, lvalue);
		}

		match(';');
    }
}


/*
 * Function:	parameter
 *
 * Description:	Parse a parameter, which in Simple C is always a scalar
 *		variable with optional pointer declarators.
 *
 *		parameter:
 *		  specifier pointers ID
 */

static Type parameter()
{
    unsigned indirection;
    int typespec;
    string name;


    typespec = specifier();
    indirection = pointers();
    name = identifier();

    Type type = Type(typespec, indirection);
    declareVariable(name, type);
    return type;
}


/*
 * Function:	parameters
 *
 * Description:	Parse the parameters of a function, but not the opening or
 *		closing parentheses.
 *
 *		parameters:
 *		  void
 *		  parameter-list
 *
 *		parameter-list:
 *		  parameter
 *		  parameter , parameter-list
 */

static Parameters *parameters()
{
    Parameters *params = new Parameters();


    if (lookahead == VOID)
		match(VOID);

    else {
		params->push_back(parameter());

		while (lookahead == ',') {
		    match(',');
		    params->push_back(parameter());
		}
    }

    return params;
}


/*
 * Function:	globalDeclarator
 *
 * Description:	Parse a declarator, which in Simple C is either a scalar
 *		variable, an array, or a function, with optional pointer
 *		declarators.
 *
 *		global-declarator:
 *		  pointers identifier
 *		  pointers identifier ( )
 *		  pointers identifier [ num ]
 */

static void globalDeclarator(int typespec)
{
    unsigned indirection;
    string name;


    indirection = pointers();
    name = identifier();

    if (lookahead == '(') {
		match('(');
		declareFunction(name, Type(typespec, indirection, nullptr));
		match(')');

    } else if (lookahead == '[') {
		match('[');
		declareVariable(name, Type(typespec, indirection, number()));
		match(']');

    } else
		declareVariable(name, Type(typespec, indirection));
}


/*
 * Function:	remainingDeclarators
 *
 * Description:	Parse any remaining global declarators after the first.
 *
 * 		remaining-declarators
 * 		  ;
 * 		  , global-declarator remaining-declarators
 */

static void remainingDeclarators(int typespec)
{
    while (lookahead == ',') {
		match(',');
		globalDeclarator(typespec);
    }

    match(';');
}


/*
 * Function:	globalOrFunction
 *
 * Description:	Parse a global declaration or function definition.
 *
 * 		global-or-function:
 * 		  specifier pointers identifier remaining-decls
 * 		  specifier pointers identifier [ num ] remaining-decls
 * 		  specifier pointers identifier ( ) remaining-decls 
 * 		  specifier pointers identifier ( parameters ) { ... }
 */

static void globalOrFunction()
{
    unsigned indirection;
    int typespec;
    string name;


    typespec = specifier();
    indirection = pointers();
    name = identifier();

    if (lookahead == '[') {
		match('[');
		declareVariable(name, Type(typespec, indirection, number()));
		match(']');
		remainingDeclarators(typespec);

    } else if (lookahead == '(') {
		match('(');

	if (lookahead == ')') {
	    declareFunction(name, Type(typespec, indirection, nullptr));
	    match(')');
	    remainingDeclarators(typespec);

	} else {
	    openScope();
	    Symbol *func = defineFunction(name, Type(typespec, indirection, parameters()));
	    Type type = func->type();
	    match(')');
	    match('{');
	    declarations();
	    statements(type);
	    closeScope();
	    match('}');
	}

    } else {
		declareVariable(name, Type(typespec, indirection));
		remainingDeclarators(typespec);
    }
}


/*
 * Function:	main
 *
 * Description:	Analyze the standard input stream.
 */

int main()
{
    openScope();
    lookahead = lexan(lexbuf);

    while (lookahead != DONE)
		globalOrFunction();

    closeScope();
    exit(EXIT_SUCCESS);
}
