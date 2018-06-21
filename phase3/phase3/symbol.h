/*
 * File:	symbol.h
 *
 * Description:	This file contains the public function and variable
 *		declarations for the lexical analyzer for Simple C.
 */

# ifndef SYMBOL_H
# define SYMBOL_H
# include "type.h"

class Symbol {
	std::string _name;
	Type _type;
public:
	Symbol(std::string, Type);
	std::string getName();
	Type getType();
};
# endif /* SYMBOL_H */