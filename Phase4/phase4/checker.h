/*
 * File:	checker.h
 *
 * Description:	This file contains the public function declarations for the
 *		semantic checker for Simple C.
 */

# ifndef CHECKER_H
# define CHECKER_H
# include "Scope.h"

Scope *openScope();
Scope *closeScope();

Symbol *defineFunction(const std::string &name, const Type &type);
Symbol *declareFunction(const std::string &name, const Type &type);
Symbol *declareVariable(const std::string &name, const Type &type);
Symbol *checkIdentifier(const std::string &name);
Type checkLogicalOR(const Type &left, const Type &right);
Type checkLogicalAND(const Type &left, const Type &right);
Type checkEQUAL(const Type &left, const Type &right);
Type checkNOTEQUAL(const Type &left, const Type &right);
Type checkLESSTHAN(const Type &left, const Type &right);
Type checkGREATERTHAN(const Type &left, const Type &right);
Type checkLEQ(const Type &left, const Type &right);
Type checkGEQ(const Type &left, const Type &right);
Type checkADD(const Type &left, const Type &right);
Type checkSUB(const Type &left, const Type &right);
Type checkMUL(const Type &left, const Type &right);
Type checkDIV(const Type &left, const Type &right);
Type checkPERCENT(const Type &left, const Type &right);
Type checkCAST(const Type &left, const Type &right);
Type checkNOT(const Type &type);
Type checkNEG(const Type &type);
Type checkDEREF(const Type &type);
Type checkADDR(const Type &type, bool &lvalue);
Type checkSIZEOF(const Type &type);
Type checkPOSTFIX(const Type &left, const Type &right, bool &lvalue);
Symbol *checkFunction(const std::string funcName);
Type checkFunctionCall(const Type &type, const Parameters &params);
void checkRETURN(const Type &type, const Type &ret);
void checkWHILE(const Type &type);
void checkIF(const Type &type);
void checkASSIGN(const Type &left, const Type &right, bool &lvalue);


# endif /* CHECKER_H */
