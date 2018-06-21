/*
 * File:	lexer.h
 *
 * Description:	This file contains the public function and variable
 *		declarations for the lexical analyzer for Simple C.
 */

# ifndef LEXER_H
# define LEXER_H

extern int lineno, numerrors;

int lexan(std::string &lexbuf);
void report(const std::string &str, const std::string &arg = "");





enum {
	AUTO = 1,
	BREAK,
	CASE,
	CHAR,
	CONST,
	CONTINUE,
	DEFAULT,
	DO,
	DONE,
	DOUBLE,
	ELSE,
	ENUM,
	EXTERN,
	FLOAT,
	FOR,
	GOTO,
	ID,
	INT,
	IF,
	LONG,
	NUM,
	REGISTER,
	RETURN,
	SHORT,
	SIGNED,
	SIZEOF,
	STATIC,
	STRING,
	STRUCT,
	SWITCH,
	TYPEDEF,
	UNION,
	UNSIGNED,
	VOID,
	VOLATILE,
	WHILE,
	ASSIGN,
	ADD,
	OR,
	AND,
	EQUAL,
	NEQ,
	LESSTHAN,
	GREATERTHAN,
	GEQ,
	LEQ,
	NEGATE,
	STAR,
	DIV,
	PERCENT,
	ADDRESS,
	NOT,
	INCREMENT,
	DECREMENT,
	DOT,
	ARROW,
	LEFTPAR,
	RIGHTPAR,
	LEFTBRACKET,
	RIGHTBRACKET,
	SEMICOLON,
	COLON,
	COMMA,
	LEFTBRACE,
	RIGHTBRACE
};

# endif /* LEXER_H */
