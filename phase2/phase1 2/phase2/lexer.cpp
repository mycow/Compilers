/*
 * File:	lexer.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the lexical analyzer for Simple C.
 */

# include <cstdio>
# include <cctype>
# include <string>
# include <cstdlib>
# include <iostream>
# include "lexer.h"

using namespace std;
int numerrors, lineno = 1;


/* Later, we will associate token values with each keyword */

static string keywords[] = {
    "auto",
    "break",
    "case",
    "char",
    "const",
    "continue",
    "default",
    "do",
    "double",
    "else",
    "enum",
    "extern",
    "float",
    "for",
    "goto",
    "if",
    "int",
    "long",
    "register",
    "return",
    "short",
    "signed",
    "sizeof",
    "static",
    "struct",
    "switch",
    "typedef",
    "union",
    "unsigned",
    "void",
    "volatile",
    "while",
};


/* Associate token values with each keyword */

# define numKeywords (sizeof(keywords) / sizeof(keywords[0]))


/*
 * Function:	report
 *
 * Description:	Report an error to the standard error prefixed with the
 *		line number.  We'll be using this a lot later with an
 *		optional string argument, but C++'s stupid streams don't do
 *		positional arguments, so we actually resort to snprintf.
 *		You just can't beat C for doing things down and dirty.
 */

void report(const string &str, const string &arg)
{
    char buf[1000];

    snprintf(buf, sizeof(buf), str.c_str(), arg.c_str());
    cerr << "line " << lineno << ": " << buf << endl;
    numerrors ++;
}

/*
 * Function:	keywordMatch
 *
 * Description:	Match keyword to proper enumeration
 */

int keywordMatch(string lexbuf)
{
	if (lexbuf == "auto")return AUTO;
	else if (lexbuf == "break")return BREAK;
	else if (lexbuf == "case")return CASE;
	else if (lexbuf == "char")return CHAR;
	else if (lexbuf == "const")return CONST;
	else if (lexbuf == "continue")return CONTINUE;
	else if (lexbuf == "default")return DEFAULT;
	else if (lexbuf == "do")return DO;
	else if (lexbuf == "double")return DOUBLE;
	else if (lexbuf == "else")return ELSE;
	else if (lexbuf == "enum")return ENUM;
	else if (lexbuf == "extern")return EXTERN;
	else if (lexbuf == "float")return FLOAT;
	else if (lexbuf == "for")return FOR;
	else if (lexbuf == "goto")return GOTO;
	else if (lexbuf == "if")return IF;
	else if (lexbuf == "int")return INT;
	else if (lexbuf == "long")return LONG;
	else if (lexbuf == "register")return REGISTER;
	else if (lexbuf == "return")return RETURN;
	else if (lexbuf == "short")return SHORT;
	else if (lexbuf == "signed")return SIGNED;
	else if (lexbuf == "sizeof")return SIZEOF;
	else if (lexbuf == "static")return STATIC;
	else if (lexbuf == "struct")return STRUCT;
	else if (lexbuf == "switch")return SWITCH;
	else if (lexbuf == "typedef")return TYPEDEF;
	else if (lexbuf == "union")return UNION;
	else if (lexbuf == "unsigned")return UNSIGNED;
	else if (lexbuf == "void")return VOID;
	else if (lexbuf == "volatile")return VOLATILE;
	else if (lexbuf == "while")return WHILE;
	return -1;
}

/*
 * Function:	lexan
 *
 * Description:	Read and tokenize the standard input stream.  The lexeme is
 *		stored in a buffer.
 */

int lexan(string &lexbuf)
{
    int p;
    unsigned i;
    static int c = cin.get();


    /* The invariant here is that the next character has already been read
       and is ready to be classified.  In this way, we eliminate having to
       push back characters onto the stream, merely to read them again. */

  while (!cin.eof()) {
	lexbuf.clear();


	/* Ignore white space */

	while (isspace(c)) {
	    if (c == '\n')
		lineno ++;

	    c = cin.get();
	}


	/* Check for an identifier or a keyword */

	if (isalpha(c) || c == '_') {
	    do {
		lexbuf += c;
		c = cin.get();
	    } while (isalnum(c) || c == '_');

	    for (i = 0; i < numKeywords; i ++)
		if (keywords[i] == lexbuf)
		    break;

	    if (i < numKeywords)
			return keywordMatch(lexbuf);
	    else
			return ID;


	/* Check for a number */

	} else if (isdigit(c)) {
	    do {
		lexbuf += c;
		c = cin.get();
	    } while (isdigit(c));
	    errno = 0;
	    strtol(lexbuf.c_str(), NULL, 0);

	    if (errno != 0)
		report("integer constant too large");

	    if (c == 'l' || c == 'L') {
		lexbuf += c;

		c = cin.get();
  }
  return NUM;
	/* There must be an easier way to do this.  It might seem stupid at
	   this point to recognize each token separately, but eventually
	   we'll be returning separate token values to the parser, so we
	   might as well do it now. */

	} else {
	    lexbuf += c;

	    switch(c) {


	    /* Check for '||' */

	    case '|':
		c = cin.get();

		if (c == '|') {
		    lexbuf += c;
		    c = cin.get();
		    return OR;
		}

		//cout << "illegal:" << lexbuf << endl;
		//return true;


	    /* Check for '=' and '==' */

	    case '=':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
			return EQUAL;
		}

		return ASSIGN;


	    /* Check for '&' and '&&' */

	    case '&':
		c = cin.get();

		if (c == '&') {
		    lexbuf += c;
		    c = cin.get();
			return AND;
		}

		return ADDRESS;


	    /* Check for '!' and '!=' */

	    case '!':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
			return NEQ;
		}

		return NOT;


	    /* Check for '<' and '<=' */

	    case '<':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
			return LEQ;
		}

		return LESSTHAN;


	    /* Check for '>' and '>=' */

	    case '>':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
			return GEQ;
		}

		return GREATERTHAN;

	    /* Check for simple, single character tokens */

	    case '-':
		c = cin.get();
		return NEGATE;

	    case '+':
		c = cin.get();
		return ADD;

	    case '*':
		c = cin.get();
		return STAR;

		case '%':
		c = cin.get();
		return PERCENT;

		case ':':
		c = cin.get();
		return COLON;

		case ';':
		c = cin.get();
		return SEMICOLON;

	    case '(':
		c = cin.get();
		return LEFTPAR;

		case ')':
		c = cin.get();
		return RIGHTPAR;

		case '[':
		c = cin.get();
		return LEFTBRACKET;

		case ']':
		c = cin.get();
		return RIGHTBRACKET;

	    case '{':
		c = cin.get();
		return LEFTBRACE;

		case '}':
		c = cin.get();
		return RIGHTBRACE;

		case '.':
		c = cin.get();
		return DOT;

		case ',':
		c = cin.get();
		return COMMA;


	    /* Check for '/' or a comment */

	    case '/':
		c = cin.get();

		if (c == '*') {
		    do {
			while (c != '*' && !cin.eof()) {
			    if (c == '\n')
				lineno ++;

			    c = cin.get();
			}

			c = cin.get();
		    } while (c != '/' && !cin.eof());

		    c = cin.get();
		    break;

		} else {
		    return DIV;
		}


	    /* Check for a string literal */

	    case '"':
		do {
		    p = c;
		    c = cin.get();
		    lexbuf += c;
		} while ((c != '"' || p == '\\') && c != '\n' && !cin.eof());

		if (c == '\n' || cin.eof())
		    report("malformed string literal");

		c = cin.get();
		return STRING;


	    /* Handle EOF here as well */

	    case EOF:
		return DONE;


	    /* Everything else is illegal */

	    default:
		c = cin.get();
		break;
	    }//switch end
	}//else end

}//while
return DONE;
}
