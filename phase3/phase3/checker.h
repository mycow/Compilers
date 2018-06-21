/*
 * File:	checker.h
 *
 * Description:	This file contains the public function and variable
 *		declarations for the lexical analyzer for Simple C.
 */

# ifndef CHECKER_H
# define CHECKER_H
# include <string>
# include "type.h"

void openSCOPE();
void closeSCOPE();
//void decARRAY(std::string, int, int, std::string);
//void decSCALAR(std::string, int, int);
//void decFUNCTION(std::string, Type);
void decVARIABLE(std::string, Type);
void defFUNCTION(std::string, Type);
void useIDENTIFIER(std::string);

# endif /* CHECKER_H */
