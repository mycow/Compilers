/*
 * File:	generator.cpp
 *
 * Description:	This file contains the public and member function
 *		definitions for the code generator for Simple C.
 *
 *		Extra functionality:
 *		- putting all the global declarations at the end
 */

# include <sstream>
# include <iostream>
# include "generator.h"
# include "Register.h"
# include "machine.h"
# include "Tree.h"

using namespace std;

/* This needs to be set to zero if temporaries are placed on the stack. */

# define SIMPLE_PROLOGUE 0


/* Okay, I admit it ... these are lame, but they work. */

# define isNumber(expr)		(expr->_operand[0] == '$')
# define isRegister(expr)	(expr->_register != nullptr)
# define isMemory(expr)		(!isNumber(expr) && !isRegister(expr))


/* The registers that we are using in the assignment. */

static Register *rax = new Register("%rax", "%eax", "%al");
static Register *rdi = new Register("%rdi", "%edi", "%dil");
static Register *rsi = new Register("%rsi", "%esi", "%sil");
static Register *rdx = new Register("%rdx", "%edx", "%dl");
static Register *rcx = new Register("%rcx", "%ecx", "%cl");
static Register *r8 = new Register("%r8", "%r8d", "%r8b");
static Register *r9 = new Register("%r9", "%r9d", "%r9b");
static Register *parameters[] = {rdi, rsi, rdx, rcx, r8, r9};
vector<Register *> registers = { rax, rdi, rsi, rdx, rcx, r8, r9 };

/* global variables */
int temp_offset;
typedef std::vector<string> Strings;
Strings sts;
Label *retLbl;

/*
 * Function:	suffix (private)
 *
 * Description:	Return the suffix for an opcode based on the given size.
 */

static string suffix(unsigned size)
{
    return size == 1 ? "b\t" : (size == 4 ? "l\t" : "q\t");
}


/*
 * Function:	align (private)
 *
 * Description:	Return the number of bytes necessary to align the given
 *		offset on the stack.
 */

static int align(int offset)
{
    if (offset % STACK_ALIGNMENT == 0)
	    return 0;

    return STACK_ALIGNMENT - (abs(offset) % STACK_ALIGNMENT);
}


/*
 * Function:	operator << (private)
 *
 * Description:	Write an expression to the specified stream.  This function
 *		first checks to see if the expression is in a register, and
 *		if not then uses its operand.
 */

static ostream &operator <<(ostream &ostr, Expression *expr)
{
    if (expr->_register != nullptr)
	return ostr << expr->_register;

    return ostr << expr->_operand;
}


/*
 * Function:	assign
 *
 * Description:	Assigns expression to register.
 *
 */

void assign(Expression *expr, Register *reg)
{
  if (expr != nullptr) {
    if (expr->_register != nullptr)
      expr->_register->_node = nullptr;
      expr->_register = reg;
    }
    if (reg != nullptr) {
      if (reg->_node != nullptr)
        reg->_node->_register = nullptr;

      reg->_node = expr;
  }
}


/*
 * Function:	assigntemp
 *
 * Description:	Create temporaries for stack spills.
 *
 */

void assigntemp(Expression *expr) {
  stringstream ss;

  temp_offset -= expr->type().size();
  ss << temp_offset << "(%rbp)";
  expr->_operand = ss.str();
}

/*
 * Function:	load
 *
 * Description:	Helper function to load expression into given register.
 *
 */

void load(Expression *expr, Register *reg) {
  if (reg->_node != expr) {
    if (reg->_node != nullptr) {
      unsigned size = reg->_node->type().size();
      assigntemp(reg->_node);
      cout << "\tmov\t" << reg->name(size);
      cout << ", " << reg->_node->_operand;
      cout << "\t# spill" << endl;
    }

    if (expr != nullptr) {
      unsigned size = expr->type().size();
      cout << "\tmov\t" << expr << ", ";
      cout << reg->name(size) << endl;
    }

    assign(expr, reg);
  }
}


/*
 * Function:	release
 *
 * Description:	Releases all registers by setting to nullptr.
 *
 */

void release()
{
  for (unsigned i = 0; i < registers.size(); i ++)
    assign(nullptr, registers[i]);
}


/*
 * Function:	getreg
 *
 * Description:	Return next available register.
 *
 */

Register *getreg() {
  for (unsigned i = 0; i < registers.size(); i ++)
    if (registers[i]->_node == nullptr)
      return registers[i];

  load(nullptr, registers[0]);
  return registers[0];
}


/*
 * Function:	Expression::test
 *
 * Description:	Intermediate step for expressions that require
 *              a comparison.
 */

void Expression::test(const Label &label, bool ifTrue) {
  generate();

  if(_register == nullptr)
    load(this,getreg());

  cout << "\t cmp\t$0, " << this << endl;
  cout << (ifTrue ? "\tjne\t" : "\tje\t") << label << endl;

  assign(this, nullptr);
}


/*
 * Function:	GreaterThan::test
 *
 * Description:	Intermediate step for expressions that require
 *              a comparison.
 */

void GreaterThan::test(const Label &label, bool onTrue)
{
  _left->generate();
  _right->generate();

  if (_left->_register == nullptr)
    load(_left, getreg());

  cout << "\tcmp\t" << _right << ", " << _left << endl;
  cout << (onTrue ? "\tjg\t" : "\tjle\t") << label << endl;

  assign(_left, nullptr);
  assign(_right, nullptr);
}


/*
 * Function:	LessThan::test
 *
 * Description:	Intermediate step for expressions that require
 *              a comparison.
 */

void LessThan::test(const Label &label, bool onTrue)
{
  _left->generate();
  _right->generate();

  if (_left->_register == nullptr)
    load(_left, getreg());

  cout << "\tcmp\t" << _right << ", " << _left << endl;
  cout << (onTrue ? "\tjl\t" : "\tjge\t") << label << endl;

  assign(_left, nullptr);
  assign(_right, nullptr);
}


/*
 * Function:	Number::generate
 *
 * Description:	Generate code for a number.  Since there is really no code
 *		to generate, we simply update our operand.
 */

void Number::generate()
{
    stringstream ss;


    ss << "$" << _value;
    _operand = ss.str();
}


/*
 * Function:	Identifier::generate
 *
 * Description:	Generate code for an identifier.  Since there is really no
 *		code to generate, we simply update our operand.
 */

void Identifier::generate()
{
    stringstream ss;


    if (_symbol->_offset == 0)
	   ss << global_prefix << _symbol->name() << global_suffix;
    else
	   ss << _symbol->_offset << "(%rbp)";

    _operand = ss.str();
}


/*
 * Function:	String::generate
 *
 * Description:	Generate code for an identifier.  Since there is really no
 *		code to generate, we simply update our operand.
 */

void String::generate()
{
    stringstream ss;

    Label *st = new Label();
    ss << *st;
    _operand = ss.str();
    ss << ":\t.asciz " << _value << endl;
    sts.push_back(ss.str());
}

/*
 * Function:	Call::generate
 *
 * Description:	Generate code for a function call.  Arguments are first
 *		evaluated in case any them are in fact other function
 *		calls.  The first six arguments are placed in registers and
 *		any remaining arguments are pushed on the stack from right
 *		to left.  Each argument on the stack always requires eight
 *		bytes, so the stack will always be aligned on a multiple of
 *		eight bytes.  To ensure 16-byte alignment, we adjust the
 *		stack pointer if necessary.
 *
 *		NOT FINISHED: Ignores any return value.
 */

void Call::generate()
{
    unsigned size, bytesPushed = 0;


    /* Generate code for all the arguments first. */

    for (unsigned i = 0; i < _args.size(); i ++)
	     _args[i]->generate();

    for (unsigned i = 0; i < registers.size(); i++)
      load(nullptr, registers[i]);


    /* Adjust the stack if necessary. */

    if (_args.size() > NUM_ARGS_IN_REGS) {
    	bytesPushed = align((_args.size() - NUM_ARGS_IN_REGS) * SIZEOF_ARG);

    	if (bytesPushed > 0)
    	    cout << "\tsubq\t$" << bytesPushed << ", %rsp" << endl;
    }


    /* Move the arguments into the correct registers or memory locations. */

    for (int i = _args.size() - 1; i >= 0; i --) {
	    size = _args[i]->type().size();

    	if (i < NUM_ARGS_IN_REGS) {
			if(_args[i]->type().isFunction()) {
			    cout << "\tmov" << suffix(size) << "%eax" << ", ";
			    cout << parameters[i]->name(size) << endl;
			}
    	    cout << "\tmov" << suffix(size) << _args[i] << ", ";
    	    cout << parameters[i]->name(size) << endl;
    	} else {
    	    bytesPushed += SIZEOF_ARG;

    	    if (isRegister(_args[i]))
    		    cout << "\tpushq\t" << _args[i]->_register->name() << endl;
    	    else if (isNumber(_args[i]) || size == SIZEOF_ARG)
    		    cout << "\tpushq\t" << _args[i] << endl;
    	    else {
        		cout << "\tmov" << suffix(size) << _args[i] << ", ";
        		cout << rax->name(size) << endl;
        		cout << "\tpushq\t%rax" << endl;
    	    }
    	}
    }


    /* Call the function.  Technically, we only need to assign the number
       of floating point arguments to %eax if the function being called
       takes a variable number of arguments.  But, it never hurts. */

   if (_id->type().parameters() == nullptr)
    cout << "\tmovl\t$0, %eax" << endl;

    cout << "\tcall\t" << global_prefix << _id->name() << endl;


    /* Reclaim the space of any arguments pushed on the stack. */

    if (bytesPushed > 0)
	   cout << "\taddq\t$" << bytesPushed << ", %rsp" << endl;

    /* Save return from call. Assign a temporary to save return value. */

    assigntemp(this);
    cout << "\tmovl\t%eax, " << _operand << endl;
}


/*
 * Function: Return::generate()
 *
 * Description: Generate code for return statement.
 */

void Return::generate() {
  _expr->generate();
  cout << "\tmov\t" << _expr << ", %eax" << endl;
  cout << "\tjmp\t" << *retLbl << endl;
}


/*
 * Function:	Assignment::generate
 *
 * Description:	Generate code for an assignment statement.
 *
 *		NOT FINISHED: Only works if the right-hand side is an
 *		integer literal and the left-hand size is an integer
 *		scalar.
 */

void Assignment::generate()
{
    _left->generate();
    _right->generate();


	int lsize = _left->type().size();
	int rsize = _right->type().size();
	load(_right, getreg());
	cout << "\tmov"<<suffix(lsize) << _right->_register->name(rsize) << ", " << _left << endl;
}


/*
 * Function:	Block::generate
 *
 * Description:	Generate code for this block, which simply means we
 *		generate code for each statement within the block.
 */

void Block::generate()
{
    for (unsigned i = 0; i < _stmts.size(); i ++)
      _stmts[i]->generate();
}


/*
 * Function:	Function::generate
 *
 * Description:	Generate code for this function, which entails allocating
 *		space for local variables, then emitting our prologue, the
 *		body of the function, and the epilogue.
 */

void Function::generate()
{
    int offset = 0;
    unsigned numSpilled = _id->type().parameters()->size();
    const Symbols &symbols = _body->declarations()->symbols();
    retLbl = new Label();

    /* Assign offsets to all symbols within the scope of the function. */

    allocate(offset);


    /* Generate the prologue, body, and epilogue. */

    cout << global_prefix << _id->name() << ":" << endl;
    cout << "\tpushq\t%rbp" << endl;
    cout << "\tmovq\t%rsp, %rbp" << endl;

    if (SIMPLE_PROLOGUE) {
		offset -= align(offset);
		cout << "\tsubq\t$" << -offset << ", %rsp" << endl;
    } else {
		cout << "\tmovl\t$" << _id->name() << ".size, %eax" << endl;
		cout << "\tsubq\t%rax, %rsp" << endl;
    }

    if (numSpilled > NUM_ARGS_IN_REGS)
		numSpilled = NUM_ARGS_IN_REGS;

    for (unsigned i = 0; i < numSpilled; i ++) {
		unsigned size = symbols[i]->type().size();
		cout << "\tmov" << suffix(size) << parameters[i]->name(size);
		cout << ", " << symbols[i]->_offset << "(%rbp)" << endl;
    }

    temp_offset = offset;
    _body->generate();
    offset = temp_offset;

    cout << *retLbl << ":" << endl;

    cout << "\tmovq\t%rbp, %rsp" << endl;
    cout << "\tpopq\t%rbp" << endl;
    cout << "\tret" << endl << endl;


    /* Finish aligning the stack. */

    if (!SIMPLE_PROLOGUE) {
		offset -= align(offset);
		cout << "\t.set\t" << _id->name() << ".size, " << -offset << endl;
    }

    cout << "\t.globl\t" << global_prefix << _id->name() << endl << endl;
}


/*
 * Function:	generateGlobals
 *
 * Description:	Generate code for any global variable declarations.
 */

void generateGlobals(Scope *scope)
{
    const Symbols &symbols = scope->symbols();

  for (unsigned i = 0; i < symbols.size(); i ++)
	if (!symbols[i]->type().isFunction()) {
	    cout << "\t.comm\t" << global_prefix << symbols[i]->name() << ", ";
	    cout << symbols[i]->type().size() << endl;
	}

  for(unsigned j = 0; j < sts.size(); j++) {
        cout << sts[j];
   }
}


/*
 * Function:	Negate::generate
 *
 * Description:	Generate code for any unary - expressions.
 */

void Negate::generate() {
  cout << "#NEGATE" << endl;
  _expr->generate();
  assigntemp(this);

  cout << "\tmovl\t" << _expr << ", %eax" << endl;
  cout << "\tnegl\t" << "%eax" << endl;
  cout << "\tmovl\t %eax, " << _operand << endl;
}


/*
 * Function:	Not::generate
 *
 * Description:	Generate code for any unary ! expressions.
 */

void Not::generate() {
  cout << "#NOT" << endl;
  _expr->generate();
  assigntemp(this);

  cout << "\tmovl\t" << _expr << ", %eax" << endl;
  cout << "\tcmpl\t$0, %eax" << endl;
  cout << "\tsete\t%al" << endl;
  cout << "\tmovzbl\t%al, %eax" << endl;
  cout << "\tmovl\t %eax, " << _operand << endl;
}


/*
* Function:	Dereference::generate
*
* Description:	Generate code for any unary * expressions.
*/

void Dereference::generate() {
  cout<<"#DEREFERENCE"<<endl;
  _expr->generate();
  //assigntemp(this);
  load(_expr,getreg());
  int size = _expr->type().size();
  cout << "\tmov\t("<< _expr->_register->name(size) <<"), "<< _expr->_register->name(size) << endl;
  assign(this, _expr->_register);
}


/*
* Function:	Address::generate
*
* Description:	Generate code for any unary & expressions.
*/

void Address::generate() {
  cout << "#ADDRESS" << endl;
  _expr->generate();
  _operand = _expr->_operand;
  
  assigntemp(this);
  cout << "\tleaq\t" << _expr << ", "<<getreg() << endl;
  cout << "\tmov"<<suffix(this->type().size())<<"\t"<<getreg()<<", " << this <<endl;
}


/*
* Function:	Cast::generate
*
* Description:	Generate code for any unary () expressions.
*/

void Cast::generate()
{
	cout<<"#CAST"<<endl;
	unsigned destSize = this->type().size();
	unsigned srcSize = this->_expr->type().size();
	_expr->generate();
	load(_expr,getreg());
	string presuffix;
	if (destSize == srcSize){
		assign(this, _expr->_register);
		return;
	}
	if (destSize > srcSize){//sign extend and move
		if (srcSize == 1)
			presuffix = "b";
		else if (srcSize == 4)
			presuffix = "l";
		cout<<"\tmovs"<<presuffix<<suffix(destSize)<<_expr<<", ";
		assign(this, _expr->_register);
		cout<<this<<endl;
	}
	else{//move into smaller size
		cout<<"\tmov"<<suffix(destSize)<<_expr->_register->name(destSize)<<",  ";
		assign(this, _expr->_register);
		cout<<this<<endl;
	}
}


/*
 * Function:	Add::generate
 *
 * Description:	Generate code for any addition expressions.
 */

void Add::generate() {
  cout << "#ADD" << endl;
  _left->generate();
  _right->generate();
  assigntemp(this);
  if (_left->_register == nullptr)
    load(_left, getreg());

  cout << "\tadd\t" << _right << ", " << _left << endl;

  assign(_right, nullptr);
  assign(this, _left->_register);
}


/*
 * Function:	Subtract::generate
 *
 * Description:	Generate code for any subtraction expressions.
 */

void Subtract::generate() {
  cout << "#SUBTRACT" << endl;
  _left->generate();
  _right->generate();
  assigntemp(this);
  if (_left->_register == nullptr)
    load(_left, getreg());

  cout << "\tsub\t" << _right << ", " << _left << endl;

  assign(_right, nullptr);
  assign(this, _left->_register);
}


/*
 * Function:	Multiply::generate
 *
 * Description:	Generate code for any multiplication expressions.
 */

void Multiply::generate() {
  cout << "#MULTIPLY" << endl;
  _left->generate();
  _right->generate();
  assigntemp(this);
  if (_left->_register == nullptr)
    load(_left, getreg());

  cout << "\timul\t" << _right << ", " << _left << endl;

  assign(_right, nullptr);
  assign(this, _left->_register);
}


/*
 * Function:	Divide::generate
 *
 * Description:	Generate code for any division expressions.
 */

void Divide::generate() {
  cout << "#DIVIDE" << endl;
  _left->generate();
  _right->generate();
  assigntemp(this);
  load(_left, rax);
  load(_right, rsi);
  cout << "\tcltd" << endl;
  cout << "\tidivl\t" << _right << endl;
  assign(_right, nullptr);
  assign(this, _left->_register);
}


/*
 * Function:	Remainder::generate
 *
 * Description:	Generate code for any modulo expressions.
 */

void Remainder::generate() {
  cout << "#REMAINDER" << endl;
  _left->generate();
  _right->generate();
  assigntemp(this);
  load(_left, rax);
  load(_right, rsi);
  cout << "\tcltd" << endl;
  cout << "\tidivl\t" << _right << endl;

  assign(_right, nullptr);
  assign(this, rdx);
}


/*
 * Function:	LessThan::generate
 *
 * Description:	Generate code for binary <.
 */

void LessThan::generate() {
  cout << "#LESS THAN" << endl;
  _left->generate();
  _right->generate();
  assigntemp(this);

  cout << "\tmovl\t" << _left << ", %eax" << endl;
  cout << "\tcmpl\t" << _right << ", %eax" << endl;
  cout << "\tsetl\t%al" << endl;
  cout << "\tmovzbl\t%al, %eax" << endl;
  cout << "\tmovl\t%eax, " << _operand << endl;
}


/*
 * Function:	GreaterThan::generate
 *
 * Description:	Generate code for binary >.
 */

void GreaterThan::generate() {
  cout << "#GREATER THAN" << endl;
  _left->generate();
  _right->generate();
  assigntemp(this);

  cout << "\tmovl\t" << _left << ", %eax" << endl;
  cout << "\tcmpl\t" << _right << ", %eax" << endl;
  cout << "\tsetg\t%al" << endl;
  cout << "\tmovzbl\t%al, %eax" << endl;
  cout << "\tmovl\t%eax, " << _operand << endl;
}


/*
 * Function:	LessOrEqual::generate
 *
 * Description:	Generate code for binary <=.
 */

void LessOrEqual::generate() {
  cout << "#LESS OR EQUAL" << endl;
  _left->generate();
  _right->generate();
  assigntemp(this);

  cout << "\tmovl\t" << _left << ", %eax" << endl;
  cout << "\tcmpl\t" << _right << ", %eax" << endl;
  cout << "\tsetle\t%al" << endl;
  cout << "\tmovzbl\t%al, %eax" << endl;
  cout << "\tmovl\t%eax, " << _operand << endl;
}


/*
 * Function:	GreaterOrEqual::generate
 *
 * Description:	Generate code for binary >=.
 */

void GreaterOrEqual::generate() {
  cout << "#GREATER OR EQUAL" << endl;
  _left->generate();
  _right->generate();
  assigntemp(this);

  cout << "\tmovl\t" << _left << ", %eax" << endl;
  cout << "\tcmpl\t" << _right << ", %eax" << endl;
  cout << "\tsetge\t%al" << endl;
  cout << "\tmovzbl\t%al, %eax" << endl;
  cout << "\tmovl\t%eax, " << _operand << endl;
}


/*
 * Function:	Equal::generate
 *
 * Description:	Generate code for binary ==.
 */

void Equal::generate() {
  cout << "#EQUAL" << endl;
  _left->generate();
  _right->generate();
  assigntemp(this);

  cout << "\tmovl\t" << _left << ", %eax" << endl;
  cout << "\tcmpl\t" << _right << ", %eax" << endl;
  cout << "\tsete\t%al" << endl;
  cout << "\tmovzbl\t%al, %eax" << endl;
  cout << "\tmovl\t%eax, " << _operand << endl;
}


/*
 * Function:	NotEqual::generate
 *
 * Description:	Generate code for binary !=.
 */

void NotEqual::generate() {
  cout << "#NOT EQUAL" << endl;
  _left->generate();
  _right->generate();
  assigntemp(this);

  cout << "\tmovl\t" << _left << ", %eax" << endl;
  cout << "\tcmpl\t" << _right << ", %eax" << endl;
  cout << "\tsetne\t%al" << endl;
  cout << "\tmovzbl\t%al, %eax" << endl;
  cout << "\tmovl\t%eax, " << _operand << endl;
}


/*
 * Function:	LogicalAnd::generate
 *
 * Description:	Generate code for binary &&.
 */

void LogicalAnd::generate()
{
  cout<<"#Logical And"<<endl;
  cout << "#LOGICALAND" << endl;
  _left->generate();
  _right->generate();
  assigntemp(this);
  Label *lbl = new Label();
  if(lbl)
  {

    //left
    cout << "\tmovl\t" << _left << ", %eax" << endl;
    cout << "\tcmpl\t$0, %eax" << endl;
    cout << "\tje\t" << *lbl  << endl;
    //right
    cout << "\tmovl\t" << _right << ", %eax" << endl;
    cout << "\tcmpl\t$0, %eax" << endl;

    //LABEL
    cout << *lbl << ":" << endl;
    cout << "\tsetne\t%al" << endl;
    cout << "\tmovzbl\t%al, %eax" << endl;
    cout << "\tmovl\t%eax, " << _operand << endl;
  }
}


/*
 * Function:	LogicalOr::generate
 *
 * Description:	Generate code for binary ||.
 */

void LogicalOr::generate()
{
  cout << "#LOGICALOR" << endl;
  assigntemp(this);
  Label *lbl = new Label();
  if(lbl)
  {

    //left
    _left->generate();
    cout << "\tmovl\t" << _left << ", %eax" << endl;
    cout << "\tcmpl\t$0, %eax" << endl;
    cout << "\tjne\t" << *lbl  << endl;
    _right->generate();
    //right
    cout << "\tmovl\t" << _right << ", %eax" << endl;
    cout << "\tcmpl\t$0, %eax" << endl;

    //LABEL
    cout << *lbl << ":" << endl;
    cout << "\tsetne\t%al" << endl;
    cout << "\tmovzbl\t%al, %eax" << endl;
    cout << "\tmovl\t%eax, " << _operand << endl;
  }
}


/*
 * Function:	While::generate
 *
 * Description:	Generate code for any while loops.
 */

void While::generate() {
  cout << "#WHILE" << endl;
  Label loop, exit;

  cout << loop << ":" << endl;

  _expr->test(exit,false);
  _stmt->generate();
  release();

  cout << "\tjmp\t" << loop << endl;
  cout << exit << ":" << endl;
}


/*
 * Function:	If::generate
 *
 * Description:	Generate code for any if statements.
 */

void If::generate() {
  cout << "#IF" << endl;
  Label skip, exit;
  _expr->generate();
  _expr->test(skip,false);
  _thenStmt->generate();
  if(_elseStmt){
	cout <<"\tjmp\t"<<exit<< endl;
  }
  cout << skip << ":" << endl;
  if(_elseStmt){
	_elseStmt->generate();
	cout<<exit<<":"<<endl;
  }
}
