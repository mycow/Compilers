/*
 * File:	type.cpp
 *
 * Description:	Constructors for overloaded types and additional helpers.
 *		
 *		
 */
# include "tokens.h"
# include "type.h"
# include <cassert>

using std::ostream;

Type::Type(int specifier, unsigned indirection)
	:_specifier(specifier), _indirection(indirection)
{
	_kind = SCALAR;
}

Type::Type(int specifier, unsigned indirection, unsigned length)
	:_specifier(specifier), _indirection(indirection), _length(length)
{
	_kind = ARRAY;
}

Type::Type(int specifier, unsigned indirection, Parameters *parameters)
	:_specifier(specifier), _indirection(indirection),  _parameters(parameters)
{
	_kind = FUNCTION;
}

bool Type::operator != (const Type &rhs) const {
	return ! operator == (rhs);
}

bool Type::operator == (const Type &rhs) const {
	if(_kind != rhs._kind)
		return false;
	if(_specifier != rhs._specifier)
		return false;
	if(_indirection != rhs._indirection)
		return false;
	if(_kind == SCALAR)
		return true;
	if(_kind == ARRAY)
		return _length == rhs._length;

	assert(_kind == FUNCTION);
	if(!_parameters || !rhs._parameters)
		return true;

	return *_parameters == *rhs._parameters;
}

ostream &operator <<(ostream &ostr, const Type &type) {
	if(type.specifier() == CHAR)
		ostr << "char";
	else if(type.specifier() == INT)
		ostr << "int";
	else if(type.specifier() == LONG)
		ostr << "long";
	else
		ostr << "error";

	if(type.indirection() > 0) {
		ostr << "";
		for(unsigned i = 0; i < type.indirection(); i++)
			ostr << "*";
	}
	if(type.isArray())
		ostr << "[" <<type.length() << "]";
	else if(type.isFunction())
		ostr << "()";
	return ostr;
}

int Type::specifier() const {
	return _specifier;
}

unsigned Type::indirection() const {
	return _indirection;
}

unsigned Type::length() const {
	return _length;
}

Parameters *Type::parameters() const {
	return _parameters;
}

bool Type::isArray() const {
	if(_kind == ARRAY)
		return true;

	return false;
}

bool Type::isFunction() const {
	if(_kind == FUNCTION)
		return true;

	return false;
}

bool Type::isScalar() const {
	if(_kind == SCALAR)
		return true;

	return false;
}