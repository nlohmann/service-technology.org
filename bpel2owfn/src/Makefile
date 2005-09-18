.PHONY: all clean

# Tools
YACC     = bison
LEX      = flex
CXX      = g++
CC       = g++

# Flags
YFLAGS   = -d -y -l -v
CXXFLAGS = -Wno-unused -O3

all: bpel2owfn


bpel2owfn: bpel-syntax.o bpel-lexic.o main.o
	$(CXX) $(CXXFLAGS) -o $@ $^


main.o: main.c main.h
	$(CXX) $(CXXFLAGS) -c -o $@ $(@:.o=.c)


###############################################################################
# The Parser
bpel-lexic.o: bpel-lexic.c
	$(CXX) $(CXXFLAGS) -c -o $@ $(@:.o=.c)

bpel-lexic.c: bpel-lexic.l 
	$(LEX) -o$@ $^

bpel-syntax.o: bpel-syntax.c
	$(CXX) $(CXXFLAGS) -c -o $@ $(@:.o=.c)

bpel-syntax.c: bpel-syntax.y
	$(YACC) $(YFLAGS) -o $@ -d $^

###############################################################################
clean:
	@echo removing temporary files...
	@-rm -f core *~ *.bak
	@-rm -f bpel-lexic.c bpel-syntax.h bpel-syntax.c bpel-syntax.output
	@-rm -f *.o
