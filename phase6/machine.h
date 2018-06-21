/*
 * File:	machine.h
 *
 * Description:	This file contains the values of various parameters for the
 *		target machine architecture.
 */

# define SIZEOF_CHAR 1
# define SIZEOF_INT 4
# define SIZEOF_LONG 8
# define SIZEOF_PTR 8

# define SIZEOF_ARG 8
# define NUM_ARGS_IN_REGS 6
# define INIT_ARG_OFFSET 16
# define STACK_ALIGNMENT 16

# if defined (__linux__) && defined(__x86_64__)

# define global_prefix ""
# define global_suffix ""
# define label_prefix ".L"

# elif defined (__APPLE__) && defined(__x86_64__)

# define global_prefix "_"
# define global_suffix "(%rip)"
# define label_prefix "L"

# else

# error Unsupport architecture
# endif
