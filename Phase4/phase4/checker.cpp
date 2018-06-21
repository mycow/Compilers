/*
 * File:	checker.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the semantic checker for Simple C.
 *
 *		Extra functionality:
 *		- inserting an undeclared symbol with the error type
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
static const Type error, integer(INT);

static string redefined = "redefinition of '%s'";
static string redeclared = "redeclaration of '%s'";
static string conflicting = "conflicting types for '%s'";
static string undeclared = "'%s' undeclared";
static string invalid_return = "invalid return type";
static string test_expression = "invalid type for test expression";
static string lvalue_require = "lvalue required in expression";
static string invalid_binary = "invalid operands to binary %s";
static string invalid_unary = "invalid operands to unary %s";
static string invalid_cast = "invalid operand in cast %s";
static string invalid_sizeof = "invalid operands in sizeof %s";
static string not_func = "called object is not a function";
static string invalid_arguments = "invalid arguments to called function";


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
 * Function:    checkLogicalOR
 *
 * Description: 
 *      
 *      
 */

Type checkLogicalOR(const Type& left, const Type& right)
{
    if(left == error || right == error){
        return error;
    }
    if(left.promote().isLogical() && right.promote().isLogical()){
        return integer;
    } else {
        report(invalid_binary, "||");
        return error;
    }
}


/*
 * Function:    checkLogicalAND
 *
 * Description: 
 *      
 *      
 */

Type checkLogicalAND(const Type& left, const Type& right)
{
    if(left == error || right == error){
        return error;
    }
    if(left.promote().isLogical() && right.promote().isLogical()){
        return integer;
    } else {
        report(invalid_binary, "&&");
        return error;
    }
}


/*
 * Function:    checkEQExpression
 *
 * Description: 
 *      
 *      
 */

Type checkEQUAL(const Type& left, const Type& right)
{
    Type compare;
    Type lPromote = left.promote();
    Type rPromote = right.promote();

    if(left == error || right == error){
        return error;
    }

    if(compare.isCompatibleWith(lPromote,rPromote)){
        return integer;
    } else {
        report(invalid_binary, "==");
        return error;
    }
}


/*
 * Function:    checkNEQExpression
 *
 * Description: 
 *      
 *      
 */

Type checkNOTEQUAL(const Type& left, const Type& right)
{
    Type compare;
    Type lPromote = left.promote();
    Type rPromote = right.promote();

    if(left == error || right == error){
        return error;
    }

    if(compare.isCompatibleWith(lPromote,rPromote)){
        return integer;
    } else {
        report(invalid_binary, "!=");
        return error;
    }
}


/*
 * Function:    checkLTExpression
 *
 * Description: 
 *      
 *      
 */

Type checkLESSTHAN(const Type& left, const Type& right)
{
    Type compare;
    Type lPromote = left.promote();
    Type rPromote = right.promote();

    if(left == error || right == error){
        return error;
    }

    if(lPromote == rPromote){
        lPromote = Type(INT);
        return lPromote;
    } else {
        report(invalid_binary, "<");
        return Type();
    }
    
    return lPromote;
}


/*
 * Function:    checkGTExpression
 *
 * Description: 
 *      
 *      
 */

Type checkGREATERTHAN(const Type& left, const Type& right)
{
    Type compare;
    Type lPromote = left.promote();
    Type rPromote = right.promote();

    if(left == error || right == error){
        return error;
    }

    if(lPromote == rPromote){
        lPromote = Type(INT);
        return lPromote;
    } else {
        report(invalid_binary, ">");
        return Type();
    }
    
    return lPromote;
}


/*
 * Function:    checkLEQExpression
 *
 * Description: 
 *      
 *      
 */

Type checkLEQ(const Type& left, const Type& right)
{
    Type compare;
    Type lPromote = left.promote();
    Type rPromote = right.promote();

    if(left == error || right == error){
        return error;
    }

    if(lPromote == rPromote){
        lPromote = Type(INT);
        return lPromote;
    } else {
        report(invalid_binary, "<=");
        return Type();
    }
    
    return lPromote;
}


/*
 * Function:    checkGEQxpression
 *
 * Description: 
 *      
 *      
 */

Type checkGEQ(const Type& left, const Type& right)
{
    Type compare;
    Type lPromote = left.promote();
    Type rPromote = right.promote();

    if(left == error || right == error){
        return error;
    }

    if(lPromote == rPromote){
        lPromote = Type(INT);
        return lPromote;
    } else {
        report(invalid_binary, ">=");
        return Type();
    }
    
    return lPromote;
}


/*
 * Function:    checkADD
 *
 * Description: 
 *      
 *      
 */

Type checkADD(const Type& left, const Type& right)
{
    Type lPromote = left.promote();
    Type rPromote = right.promote();

    if(left == error || right == error){
        return error;
    }

    if(lPromote.isNumeric() && rPromote.isNumeric()){
        if(lPromote == Type(LONG) || rPromote == Type(LONG))
            return Type(LONG);
        return integer;
    } else if(lPromote.isPointer() && rPromote.isNumeric()) {
        return lPromote;
    } else if(rPromote.isPointer() && lPromote.isNumeric()) {
        return rPromote;
    } else {
        report(invalid_binary, "+");
        return error;
    }
}


/*
 * Function:    checkSUB
 *
 * Description: 
 *      
 *      
 */

Type checkSUB(const Type& left, const Type& right)
{
    Type lPromote = left.promote();
    Type rPromote = right.promote();

    if(left == error || right == error){
        return error;
    }

    if(lPromote.isNumeric() && rPromote.isNumeric()){
        return integer;
    } else if(lPromote.isPointer() && rPromote.isNumeric()) {
        return lPromote;
    } else if(lPromote.isPointer() && rPromote.isPointer()) {
        return Type(LONG);
    } else {
        report(invalid_binary, "-");
        return error;
    }
}


/*
 * Function:    checkSTAR
 *
 * Description: 
 *      
 *      
 */

Type checkMUL(const Type& left, const Type& right)
{
    if(left == error || right == error){
        return error;
    }
    if(left.promote().isNumeric() && right.promote().isNumeric()){
        return integer;
    } else {
        report(invalid_binary, "*");
        return error;
    }
}


/*
 * Function:    checkDIV
 *
 * Description: 
 *      
 *      
 */

Type checkDIV(const Type& left, const Type& right)
{
    if(left == error || right == error){
        return error;
    }
    if(left.promote().isNumeric() && right.promote().isNumeric()){
        return integer;
    } else {
        report(invalid_binary, "/");
        return error;
    }
}


/*
 * Function:    checkPERCENT
 *
 * Description: 
 *      
 *      
 */

Type checkPERCENT(const Type& left, const Type& right)
{
    if(left == error || right == error){
        return error;
    }
    if(left.promote().isNumeric() && right.promote().isNumeric()){
        return integer;
    } else {
        report(invalid_binary, "%");
        return error;
    }
}


/*
 * Function:    checkCastExpression
 *
 * Description: 
 *      
 *      
 */

Type checkCAST(const Type& left, const Type& right)
{
    Type lPromote = left.promote();
    Type rPromote = right.promote();

    if(left == error || right == error){
        return error;
    }
    if(lPromote.isPointer() && rPromote.isPointer())
        return integer;
    else if(lPromote.isNumeric() && rPromote.isNumeric())
        return integer;
    else if((lPromote.isPointer() && rPromote == Type(LONG)) || (lPromote.isPointer() && rPromote == Type(LONG))){
        return integer;
    } else {
        report(invalid_cast);
        return error;
    }
}


/*
 * Function:    checkNOT
 *
 * Description: 
 *      
 *      
 */

Type checkNOT(const Type& type)
{
    if(type == error){
        return error;
    }

    if(type.isLogical())
        return integer;
     else {
        report(invalid_unary, "!");
        return error;
    }
}


/*
 * Function:    checkNEG
 *
 * Description: 
 *      
 *      
 */

Type checkNEG(const Type& type)
{
    if(type == error){
        return error;
    }
    if(type.isNumeric()){
        return integer;
    } else {
        report(invalid_unary, "-");
        return error;
    }
}


/*
 * Function:    checkDEREF
 *
 * Description: 
 *      
 *      
 */

Type checkDEREF(const Type& type)
{
    if(type == error){
        return error;
    }
    if(type.promote().isPointer()){
        return Type(type.promote().specifier(), type.promote().indirection()-1);
    } else {
        report(invalid_unary, "*");
        return error;
    }
}


/*
 * Function:    checkADDR
 *
 * Description: 
 *      
 *      
 */

Type checkADDR(const Type& type, bool& lvalue)
{
    if(type == error){
        return error;
    }
    if(lvalue)
        return Type(type.specifier(), type.indirection());
    else {
        report(lvalue_require);
        return error;
    }
}


/*
 * Function:    checkSIZEOF
 *
 * Description: 
 *      
 *      
 */

Type checkSIZEOF(const Type& type)
{
    if(type == error)
        return error;

    if(type.isLogical())
        return integer;
    else {
        report(invalid_sizeof);
        return error;
    }
}


/*
 * Function:    checkPOSTFIX
 *
 * Description: 
 *      
 *      
 */

Type checkPOSTFIX(const Type& left, const Type& right, bool& lvalue)
{
    Type lPromote = left.promote();
    Type rPromote = right.promote();

    if(left == error || right == error){
        return error;
    }

    if(lPromote.isPointer() && rPromote.isNumeric())
        return Type(lPromote.specifier(), lPromote.indirection()-1);
    else {
        report(invalid_binary, "[]");
        return error;
    }
}


/*
 * Function:    checkFUNCTION
 *
 * Description: 
 *      
 *      
 */

Symbol* checkFunction(const string funcName) {
    Symbol *temp = toplevel->lookup(funcName);
    if(temp == nullptr)
        temp = declareFunction(funcName, Type(INT,0));
    return temp;
}


/*
 * Function:    checkFUNCTIONCALL
 *
 * Description: 
 *      
 *      
 */

Type checkFunctionCall(const Type& type, const Parameters& params)
{
    Type compare = Type();
    if(type == error)
        return error;

    if(type.isFunction())
        compare = Type(type.specifier());
    else {
        report(not_func);
        return error;
    }

    Parameters *temp = type.parameters();
    if(temp != NULL && temp->size() == params.size()){
        for(int i = 0; i < params.size(); i++){
            if(params[i].isLogical()){
                if(compare.isCompatibleWith((*temp)[i], params[i])){
                    compare = Type(type.specifier(), type.indirection());
                    return compare;
                }else {
                    report(invalid_arguments);
                    return error;
                }
            } else {
                report(invalid_arguments);
                return error;
            }
        }
        return error;
    } else {
        report(invalid_arguments);
        return error;
    }
}


/*
 * Function:    checkRETURN
 *
 * Description: Check that return type is valid with function type
 *      
 *      
 */

void checkRETURN(const Type &type, const Type& ret)
{
    Type compare;
    if(type == error || ret == error)
        return;

    if(type.isFunction())
        compare = checkFunctionCall(type, *(type.parameters()));

    if(!compare.isCompatibleWith(type, ret)){
        report(invalid_return);
        return;
    }
}


/*
 * Function:    checkWHILE
 *
 * Description: 
 *      
 *      
 */

void checkWHILE(const Type &type)
{
    if(type == error)
        return;
    if(!type.isLogical())
        report(test_expression);
}


/*
 * Function:    checkIF
 *
 * Description: 
 *      
 *      
 */

void checkIF(const Type &type)
{
    if(type == error)
        return;
    if(!type.isLogical())
        report(test_expression);
}


/*
 * Function:    checkASSIGN
 *
 * Description: 
 *      
 *      
 */

void checkASSIGN(const Type &left, const Type &right, bool &lvalue)
{
    Type compare;
    if(left == error || right == error)
        return;

    if(!lvalue) {
        report(lvalue_require);
        return;
    }
    if(!compare.isCompatibleWith(left, right))
        report(invalid_binary, "=");
}