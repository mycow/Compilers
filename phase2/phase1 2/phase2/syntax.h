/*
 * File:	syntax.h
 *
 * Description:	This file contains the public function and variable
 *		declarations for the lexical analyzer for Simple C.
 */

# ifndef SYNTAX_H
# define SYNTAX_H

int peek();

void match(int);

void FUNCTION_DEFINITION_HELPER();

void REMAINING_DEC();

void TRANSLATION_UNIT();

void GLOBAL_DECLARATION();

void GLOBAL_DECLARATOR_LIST();

void GLOBAL_DECLARATOR();

void POINTERS();

void EXPRESSION_REMAIN();

void SPECIFIER();

void FUNCTION_DEFINITION();

void PARAMETERS();

void PARAMETER_LIST();

void PARAMETER();

void DECLARATIONS();

void DECLARATION();

void DECLARATOR_LIST();

void DECLARATOR();

void STATEMENTS();

void STATEMENT();

void EXPRESSION_LIST();

void PAREN_EXPR();

bool isSpec(int);

void INDEX_EXPRESSION();

void UNARY_EXPRESSION();

void CAST_EXPRESSION();

void MULTIPLICATIVE_EXPRESSION();

void ADDITIVE_EXPRESSION();

void RELATIONAL_EXPRESSION();

void EQUALITY_EXPRESSION();

void LOGICAL_AND_EXPRESSION();

void LOGICAL_OR_EXPRESSION();

void EXPRESSION();

int main();

# endif /* SYNTAX_H */
