/*
 * File:	type.h
 *
 * Description:	This file contains the public function and variable
 *		declarations for the lexical analyzer for Simple C.
 */

# ifndef TYPE_H
# define TYPE_H
# include <vector>
# include <ostream>

typedef std::vector<class Type> Parameters;

class Type {
	int _specifier;
	unsigned _indirection;
	enum{ARRAY, FUNCTION, SCALAR} _kind;
	unsigned _length;
	Parameters *_parameters;
public:
	Type(int specifier, unsigned indirection=0);
	Type(int specifier, unsigned indirection, unsigned length);
	Type(int specifier, unsigned indirection, Parameters *parameters);

	bool operator == (const Type &rhs) const;
	bool operator != (const Type &rhs) const;

	int specifier() const;
	unsigned indirection() const;
	unsigned length() const;
	Parameters *parameters() const;

	bool isArray() const;
	bool isFunction() const;
	bool isScalar() const;
};

std::ostream&operator<<(std::ostream&ostr, const Type &type);

# endif /* TYPE_H */
