/*
 * File:	checker.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the semantic checker for Simple C.
 *
 *		Extra functionality:
 *		- inserting an undeclared symbol with the error type
 *		- scaling the operands and results of pointer arithmetic
 *		- explicit type conversions and promotions
 */

# include <iostream>
# include "lexer.h"
# include "checker.h"
# include "tokens.h"
# include "Symbol.h"
# include "Scope.h"
# include "Type.h"


using namespace std;

static Scope *outermost, *toplevel;
static const Type error, character(CHAR), integer(INT), longInt(LONG);

static string redefined = "redefinition of '%s'";
static string redeclared = "redeclaration of '%s'";
static string conflicting = "conflicting types for '%s'";
static string undeclared = "'%s' undeclared";

static string invalid_return = "invalid return type";
static string invalid_test = "invalid type for test expression";
static string invalid_lvalue = "lvalue required in expression";
static string invalid_operands = "invalid operands to binary %s";
static string invalid_operand = "invalid operand to unary %s";
static string invalid_cast = "invalid operand in cast expression";
static string invalid_sizeof = "invalid operand in sizeof expression";
static string invalid_function = "called object is not a function";
static string invalid_arguments = "invalid arguments to called function";


/*
 * Function:	debug
 */

static void debug(const string &str, const Type &t1, const Type &t2)
{
    cout << "line " << lineno << ": " << str << " " << t1 << " to " << t2 << endl;
}


/*
 * Function:	promote
 *
 * Description:	Perform type promotion on the given expression.  An array
 *		is promoted to a pointer by explicitly inserting an address
 *		operator.  A character is promoted to an integer by
 *		explicitly inserting a type cast.
 */

static Type promote(Expression *&expr)
{
    if (expr->type().isArray()) {
	debug("promoting", expr->type(), expr->type().promote());
	expr = new Address(expr, expr->type().promote());

    } else if (expr->type() == character) {
	debug("promoting", character, integer);
	expr = new Cast(integer, expr);
    }

    return expr->type();
}


/*
 * Function:	convert
 *
 * Description:	Attempt to convert the given expression to the given type
 *		as if by assignment.
 */

static Type convert(Expression *&expr, Type type)
{
    if (expr->type().isArray() && type.isPointer())
	promote(expr);

    if (expr->type() != type && expr->type().isNumeric() && type.isNumeric()) {
	debug("assigning", expr->type(), type);
	expr = new Cast(type, expr);
    }

    return expr->type();
}


/*
 * Function:	extend
 *
 * Description:	Attempt to extend the type of the given expression to the
 *		given type.  The type of the given expression is only
 *		extended, not truncated.
 */

static Type extend(Expression *&expr, Type type)
{
    if (expr->type() != type && expr->type().isNumeric() && type.isNumeric())
	if (expr->type() == character || type == longInt) {
	    debug("extending", expr->type(), type);
	    expr = new Cast(type, expr);
	}

    return promote(expr);
}


/*
 * Function:	openScope
 *
 * Description:	Create a scope and make it the new top-level scope.
 */

Scope *openScope()
{
    toplevel = new Scope(toplevel);

    if (outermost == nullptr)
	outermost = toplevel;

    return toplevel;
}


/*
 * Function:	closeScope
 *
 * Description:	Remove the top-level scope, and make its enclosing scope
 *		the new top-level scope.
 */

Scope *closeScope()
{
    Scope *old = toplevel;

    toplevel = toplevel->enclosing();
    return old;
}


/*
 * Function:	defineFunction
 *
 * Description:	Define a function with the specified NAME and TYPE.  A
 *		function is always defined in the outermost scope.  This
 *		definition always replaces any previous definition or
 *		declaration.
 */

Symbol *defineFunction(const string &name, const Type &type)
{
    Symbol *symbol = outermost->find(name);

    if (symbol != nullptr) {
	if (symbol->type().isFunction() && symbol->type().parameters()) {
	    report(redefined, name);
	    delete symbol->type().parameters();

	} else if (type != symbol->type())
	    report(conflicting, name);

	outermost->remove(name);
	delete symbol;
    }

    symbol = new Symbol(name, type);
    outermost->insert(symbol);

    return symbol;
}


/*
 * Function:	declareFunction
 *
 * Description:	Declare a function with the specified NAME and TYPE.  A
 *		function is always declared in the outermost scope.  Any
 *		redeclaration is discarded.
 */

Symbol *declareFunction(const string &name, const Type &type)
{
    Symbol *symbol = outermost->find(name);

    if (symbol == nullptr) {
	symbol = new Symbol(name, type);
	outermost->insert(symbol);

    } else if (type != symbol->type()) {
	report(conflicting, name);
	delete type.parameters();
    }

    return symbol;
}


/*
 * Function:	declareVariable
 *
 * Description:	Declare a variable with the specified NAME and TYPE.  Any
 *		redeclaration is discarded.
 */

Symbol *declareVariable(const string &name, const Type &type)
{
    Symbol *symbol = toplevel->find(name);

    if (symbol == nullptr) {
	symbol = new Symbol(name, type);
	toplevel->insert(symbol);

    } else if (outermost != toplevel)
	report(redeclared, name);

    else if (type != symbol->type())
	report(conflicting, name);

    return symbol;
}


/*
 * Function:	checkIdentifier
 *
 * Description:	Check if NAME is declared.  If it is undeclared, then
 *		declare it as having the error type in order to eliminate
 *		future error messages.
 */

Symbol *checkIdentifier(const string &name)
{
    Symbol *symbol = toplevel->lookup(name);

    if (symbol == nullptr) {
	report(undeclared, name);
	symbol = new Symbol(name, error);
	toplevel->insert(symbol);
    }

    return symbol;
}


/*
 * Function:	checkCall
 *
 * Description:	Check a function call expression: the type of the object
 *		being called must be a function type, and the number and
 *		types of arguments must agree.
 */

Expression *checkCall(Symbol *id, Expressions &args)
{
    const Type &t = id->type();
    Type result = error;


    if (t != error) {
	if (!t.isFunction())
	    report(invalid_function);

    	else {
	    Parameters *params = t.parameters();
	    result = Type(t.specifier(), t.indirection());


	    if (params != nullptr) {
		if (params->size() != args.size())
		    report(invalid_arguments);

		else {
		    for (unsigned i = 0; i < args.size(); i ++)
			if (convert(args[i], (*params)[i]) != (*params)[i]) {
			    report(invalid_arguments);
			    result = error;
			    break;
			}
		}

	    } else
		for (unsigned i = 0; i < args.size(); i ++)
		    promote(args[i]);
	}
    }

    return new Call(id, args, result);
}


/*
 * Function:	checkArray
 *
 * Description:	Check an array index expression: the left operand must have
 *		type "pointer to T" and the right operand must have a
 *		numeric type, and the result has type T.
 */

Expression *checkArray(Expression *left, Expression *right)
{
    const Type &t1 = promote(left);
    const Type &t2 = extend(right, longInt);
    Type result = error;

    right = new Multiply(right, new Number(t1.deref().size()), longInt);
    Expression *expr = new Add(left, right, t1);

    if (t1 != error && t2 != error) {
	if (t1.isPointer() && t2 == longInt)
	    result = t1.deref();
	else
	    report(invalid_operands, "[]");
    }

    return new Dereference(expr, result);
}


/*
 * Function:	checkNot
 *
 * Description:	Check a logical negation expression: the operand must have a
 *		logical type, and the result has type int.
 */

Expression *checkNot(Expression *expr)
{
    const Type &t = promote(expr);
    Type result = error;


    if (t != error) {
	if (t.isLogical())
	    result = integer;
	else
	    report(invalid_operand, "!");
    }

    return new Not(expr, result);
}


/*
 * Function:	checkNegate
 *
 * Description:	Check an arithmetic negation expression: the operand must
 *		have a numeric type, and the result has that type.
 */

Expression *checkNegate(Expression *expr)
{
    const Type &t = promote(expr);
    Type result = error;


    if (t != error) {
	if (t.isNumeric())
	    result = t;
	else
	    report(invalid_operand, "-");
    }

    return new Negate(expr, result);
}


/*
 * Function:	checkDereference
 *
 * Description:	Check a dereference expression: the operand must have type
 *		"pointer to T," and the result has type T.
 */

Expression *checkDereference(Expression *expr)
{
    const Type &t = promote(expr);
    Type result = error;


    if (t != error) {
	if (t.isPointer())
	    result = t.deref();
	else
	    report(invalid_operand, "*");
    }

    return new Dereference(expr, result);
}


/*
 * Function:	checkAddress
 *
 * Description:	Check an address expression: the operand must be an lvalue,
 *		and if the operand has type T, then the result has type
 *		"pointer to (T)."
 */

Expression *checkAddress(Expression *expr)
{
    const Type &t = expr->type();
    Type result = error;


    if (t != error) {
	if (expr->lvalue())
	    result = Type(t.specifier(), t.indirection() + 1);
	else
	    report(invalid_lvalue);
    }

    return new Address(expr, result);
}


/*
 * Function:	checkSizeof
 *
 * Description:	Check a sizeof expression: the type of the operand cannot
 *		be a function type.
 */

Expression *checkSizeof(Expression *expr)
{
    const Type &t = expr->type();


    if (t != error)
	if (t.isFunction())
	    report(invalid_sizeof);

    return new Number(expr->type().size());
}


/*
 * Function:	checkCast
 *
 * Description:	Check a cast expression: the result and operand must either
 *		both have numeric types, both have pointer types, or one
 *		has type long and the other has a pointer type.
 */

Expression *checkCast(const Type &type, Expression *expr)
{
    Type t = expr->type();
    Type result;


    if (t != error) {
	if (type.isNumeric() && t.isNumeric()) {
	    result = convert(expr, type);

	} else if (type.isPointer() && (t.isPointer() || t == longInt)) {
	    result = promote(expr);

	} else if (t.isPointer() && type == longInt) {
	    result = promote(expr);

	} else {
	    expr = new Cast(error, expr);
	    report(invalid_cast);
	}

	/*
	cout << "source type = " << t << endl;
	cout << "result type = " << result << endl;
	cout << "target type = " << type << endl;
	*/

	if (result != error && result != type)
	    expr = new Cast(type, expr);
    }

    return expr;
}


/*
 * Function:	checkMult
 *
 * Description:	Check a multiplicative expression: both operands must have
 *		a numeric type, and the result has type long if either
 *		operand has type long and has type int otherwise.
 */

static Type checkMult(Expression *&left, Expression *&right, const string &op)
{
    const Type &t1 = extend(left, right->type());
    const Type &t2 = extend(right, left->type());
    Type result = error;


    if (t1 != error && t2 != error) {
	if (t1.isNumeric() && t2.isNumeric())
	    result = t1;
	else
	    report(invalid_operands, op);
    }

    return result;
}


/*
 * Function:	checkMultiply
 *
 * Description:	Check a multiplication expression.
 */

Expression *checkMultiply(Expression *left, Expression *right)
{
    Type t = checkMult(left, right, "*");
    return new Multiply(left, right, t);
}


/*
 * Function:	checkDivide
 *
 * Description:	Check a division expression.
 */

Expression *checkDivide(Expression *left, Expression *right)
{
    Type t = checkMult(left, right, "/");
    return new Divide(left, right, t);
}

/*
 * Function:	checkRemainder
 *
 * Description:	Check a remainder expression.
 */

Expression *checkRemainder(Expression *left, Expression *right)
{
    Type t = checkMult(left, right, "%");
    return new Remainder(left, right, t);
}


/*
 * Function:	checkAdd
 *
 * Description:	Check an addition expression: if both operands have a
 *		numeric type, then the result has type long if either has
 *		type long and has type int otherwise; if one operand has a
 *		pointer type and the other has a numeric type, then the
 *		result has that pointer type.
 */

Expression *checkAdd(Expression *left, Expression *right)
{
    Type t1 = left->type();
    Type t2 = right->type();
    Type result = error;


    if (t1 != error && t2 != error) {
	if (t1.isNumeric() && t2.isNumeric()) {
	    t1 = extend(left, t2);
	    t2 = extend(right, t1);
	    result = t1;

	} else if (t1.isPointer() && t2.isNumeric()) {
	    t1 = promote(left);
	    t2 = extend(right, longInt);
	    right = new Multiply(right, new Number(t1.deref().size()), longInt);
	    result = t1;

	} else if (t1.isNumeric() && t2.isPointer()) {
	    t1 = extend(left, longInt);
	    t2 = promote(right);
	    left = new Multiply(left, new Number(t2.deref().size()), longInt);
	    result = t2;

	} else
	    report(invalid_operands, "+");
    }

    return new Add(left, right, result);
}


/*
 * Function:	checkSubtract
 *
 * Description:	Check a subtraction expression: if both operands have a
 *		numeric type, then the result has type long if either has
 *		type long and has type int otherwise; if the left operand
 *		has a pointer type and the right operand has a numeric
 *		type, then the result has that pointer type; if both
 *		operands are identical pointer types, then the result has
 *		type long.
 */

Expression *checkSubtract(Expression *left, Expression *right)
{
    Expression *tree;
    Type t1 = left->type();
    Type t2 = right->type();
    Type result = error;


    if (t1 != error && t2 != error) {
	if (t1.isNumeric() && t2.isNumeric()) {
	    t1 = extend(left, t2);
	    t2 = extend(right, t1);
	    result = t1;

	} else if (t1.isPointer() && t1.promote() == t2.promote()) {
	    t1 = promote(left);
	    t2 = promote(right);
	    result = longInt;

	} else if (t1.isPointer() && t2.isNumeric()) {
	    t1 = promote(left);
	    t2 = extend(right, longInt);
	    right = new Multiply(right, new Number(t1.deref().size()), longInt);
	    result = t1;

	} else
	    report(invalid_operands, "-");
    }

    tree = new Subtract(left, right, result);

    if (t1.isPointer() && t1 == t2)
	tree = new Divide(tree, new Number(t1.deref().size()), longInt);

    return tree;
}


/*
 * Function:	checkCompare
 *
 * Description:	Check an equality or relational expression: the types of
 *		both operands must be compatible, and the result has type
 *		int.
 */

static
Type checkCompare(Expression *&left, Expression *&right, const string &op)
{
    const Type &t1 = extend(left, right->type());
    const Type &t2 = extend(right, left->type());
    Type result = error;


    if (t1 != error && t2 != error) {
	if (t1.isCompatibleWith(t2))
	    result = integer;
	else
	    report(invalid_operands, op);
    }

    return result;
}


/*
 * Function:	checkEqual
 *
 * Description:	Check an equality expression: left == right.
 */

Expression *checkEqual(Expression *left, Expression *right)
{
    Type t = checkCompare(left, right, "==");
    return new Equal(left, right, t);
}


/*
 * Function:	checkNotEqual
 *
 * Description:	Check an inequality expression: left != right.
 */

Expression *checkNotEqual(Expression *left, Expression *right)
{
    Type t = checkCompare(left, right, "!=");
    return new NotEqual(left, right, t);
}


/*
 * Function:	checkLessThan
 *
 * Description:	Check a less-than expression: left < right.
 */

Expression *checkLessThan(Expression *left, Expression *right)
{
    Type t = checkCompare(left, right, "<");
    return new LessThan(left, right, t);
}


/*
 * Function:	checkGreaterThan
 *
 * Description:	Check a greater-than expression: left > right.
 */

Expression *checkGreaterThan(Expression *left, Expression *right)
{
    Type t = checkCompare(left, right, ">");
    return new GreaterThan(left, right, t);
}


/*
 * Function:	checkLessOrEqual
 *
 * Description:	Check a less-than-or-equal expression: left <= right.
 */

Expression *checkLessOrEqual(Expression *left, Expression *right)
{
    Type t = checkCompare(left, right, "<=");
    return new LessOrEqual(left, right, t);
}


/*
 * Function:	checkGreaterOrEqual
 *
 * Description:	Check a greater-than-or-equal expression: left >= right.
 */

Expression *checkGreaterOrEqual(Expression *left, Expression *right)
{
    Type t = checkCompare(left, right, ">=");
    return new GreaterOrEqual(left, right, t);
}


/*
 * Function:	checkLogical
 *
 * Description:	Check a logical-or or logical-and expression: the types of
 *		both operands must be logical types and the result has type
 *		int.
 */

static
Type checkLogical(Expression *&left, Expression *&right, const string &op)
{
    const Type &t1 = promote(left);
    const Type &t2 = promote(right);
    Type result = error;


    if (t1 != error && t2 != error) {
	if (t1.isLogical() && t2.isLogical())
	    result = integer;
	else
	    report(invalid_operands, op);
    }

    return result;
}


/*
 * Function:	checkLogicalAnd
 *
 * Description:	Check a logical-and expression: left && right.
 */

Expression *checkLogicalAnd(Expression *left, Expression *right)
{
    Type t = checkLogical(left, right, "&&");
    return new LogicalAnd(left, right, t);
}


/*
 * Function:	checkLogicalOr
 *
 * Description:	Check a logical-or expression: left || right.
 */

Expression *checkLogicalOr(Expression *left, Expression *right)
{
    Type t = checkLogical(left, right, "||");
    return new LogicalOr(left, right, t);
}


/*
 * Function:	checkAssignment
 *
 * Description:	Check an assignment statement: the left operand must be an
 *		lvalue and the type of the operands must be compatible.
 */

Statement *checkAssignment(Expression *left, Expression *right)
{
    const Type &t1 = left->type();
    const Type &t2 = convert(right, left->type());


    if (t1 != error && t2 != error) {
	if (!left->lvalue())
	    report(invalid_lvalue);

	else if (!t1.isCompatibleWith(t2))
	    report(invalid_operands, "=");
    }

    return new Assignment(left, right);
}


/*
 * Function:	checkReturn
 *
 * Description:	Check a return statement: the type of the expression must
 *		be compatible with the given type, which should be the
 *		return type of the enclosing function.
 */

void checkReturn(Expression *&expr, const Type &type)
{
    const Type &t = convert(expr, type);

    if (t != error && !t.isCompatibleWith(type))
	report(invalid_return);
}


/*
 * Function:	checkTest
 *
 * Description:	Check if the type of the expression is a legal type in a
 *		test expression in a while, if-then, or if-then-else
 *		statement: the type must be a logical type.
 */

void checkTest(Expression *&expr)
{
    const Type &t = promote(expr);

    if (t != error && !t.isLogical())
	report(invalid_test);
}
