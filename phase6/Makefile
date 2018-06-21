CXX		= g++ -std=c++11
CXXFLAGS	= -g -Wall
OBJS		= Label.o Register.o Scope.o Symbol.o Tree.o Type.o allocator.o \
		  checker.o generator.o lexer.o parser.o
PROG		= scc

all:		$(PROG)

$(PROG):	$(OBJS)
		$(CXX) -o $(PROG) $(OBJS)

clean:;		$(RM) -f $(PROG) core *.o
