/*
 * File:	symbol.cpp
 *
 * Description:	Constructor and accessors for Symbol class.
 *		
 *		
 */

# include <cstdlib>
# include <iostream>
# include "scope.h"

Symbol::Symbol(std::string name, Type t)
	:_name(name),_type(t)
{

}

std::string Symbol::getName() {
	return _name;
}

Type Symbol::getType() {
	return _type;
}
