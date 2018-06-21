CXX		= g++ -std=c++11
CXXFLAGS	= -g -Wall
OBJS		= Scope.o Symbol.o Tree.o Type.o checker.o lexer.o parser.o generate.o
PROG		= scc

all:		$(PROG)

$(PROG):	$(OBJS)
		$(CXX) -o $(PROG) $(OBJS)

clean:;		$(RM) -f $(PROG) core *.o
