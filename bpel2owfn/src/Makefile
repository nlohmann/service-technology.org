.PHONY: all clean

# Tools
YACC     = bison
LEX      = flex
CXX      = g++
CC       = g++
KC       = kc++

# Flags
YFLAGS   = -d -y -l -v
CXXFLAGS = -Wno-unused -O3
KFLAGS   = -b -sc -fbpel-kc-

KC_TIME = .kc_time_stamp

###############################################################################
# Main Module

all: bpel2owfn

bpel2owfn: bpel-syntax.o bpel-lexic.o main.o bpel-abstract.o bpel-kc-unpk.o\
	bpel-kc-rk.o bpel-kc-csgiok.o bpel-kc-k.o
	$(CXX) $(CXXFLAGS) -o $@ $^

main.o: main.c main.h $(KC_TIME)
	$(CXX) $(CXXFLAGS) -c -o $@ $(@:.o=.c)


###############################################################################
# The Parser
bpel-lexic.o: bpel-lexic.c
	$(CXX) $(CXXFLAGS) -c -o $@ $(@:.o=.c)

bpel-lexic.c: bpel-lexic.l $(KC_TIME)
	$(LEX) -o$@ $^

bpel-syntax.o: bpel-syntax.c $(KC_TIME)
	$(CXX) $(CXXFLAGS) -c -o $@ $(@:.o=.c)

bpel-syntax.c: bpel-syntax.y
	$(YACC) $(YFLAGS) -o $@ -d $^


##############################################################################
# Kimwitu++ Files
bpel-kc-csgiok.o: $(KC_TIME)
	$(CXX) $(CXXFLAGS) -c -o $@ $(@:.o=.c)

bpel-kc-k.o: $(KC_TIME)
	$(CXX) $(CXXFLAGS) -c -o $@ $(@:.o=.c)

bpel-kc-rk.o: $(KC_TIME)
	$(CXX) $(CXXFLAGS) -c -o $@ $(@:.o=.c)

bpel-abstract.o: $(KC_TIME)
	$(CXX) $(CXXFLAGS) -c -o $@ $(@:.o=.c)

bpel-kc-unpk.o: $(KC_TIME)
	$(CXX) $(CXXFLAGS) -c -o $@ $(@:.o=.c)

$(KC_TIME): bpel-abstract.k
	@touch $@
	$(KC) $(KFLAGS) $^


###############################################################################
clean:
	@echo removing temporary files...
	@-rm -f core *~ *.bak
	@-rm -f bpel-lexic.c bpel-syntax.h bpel-syntax.c bpel-syntax.output
	@-rm -f bpel-kc-*.c bpel-kc-*.h
	@-rm -f bpel-abstract.c bpel-abstract.h
	@-rm -f $(KC_TIME)
	@-rm -f *.o
