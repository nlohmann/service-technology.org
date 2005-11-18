# last changed $Date: 2005/11/18 16:57:14 $ by $Author: nlohmann $



.PHONY: clean

# Tools
YACC     = bison
LEX      = flex
CXX      = g++
KC       = kc++

# Flags
CXXFLAGS2 = -pedantic -Wall -ansi -O3   # the most strict variant
CXXFLAGS1 = -pedantic -Wall -O3         # less stricter, but against ISO C++
CXXFLAGS  = -pedantic -Wno-unused -O3   # pedantic
CXXFLAGS0 = -Wno-unused -O3             # least strict
YFLAGS    = --defines --yacc --no-lines
KFLAGS    = --yystype --no-printdot --file-prefix=bpel-kc- --suffix=cc

# Kimwitu++ helper file for make
KC_TIME = .kc_time_stamp



###############################################################################
# Main target

bpel2owfn: $(KC_TIME) bpel-syntax.o bpel-lexic.o\
	main.o debug.o exception.o bpel-attributes.o petrinet.o helpers.o\
	bpel-abstract.o bpel-unparse-xml.o bpel-unparse-petri.o bpel-rewrite.o\
	bpel-kc-unpk.o\
	bpel-kc-rk.o bpel-kc-csgiok.o bpel-kc-k.o
	$(CXX) $(CXXFLAGS2) -o $@ $^



###############################################################################
# "Pure" C++-modules

main.o: main.cc main.h
	$(CXX) $(CXXFLAGS2) -c -o $@ $(@:.o=.cc)

bpel-attributes.o: bpel-attributes.c bpel-attributes.h
	$(CXX) $(CXXFLAGS) -c -o $@ $(@:.o=.c)

petrinet.o: petrinet.h petrinet.cc
	$(CXX) $(CXXFLAGS2) -c -o $@ $(@:.o=.cc)

debug.o: debug.h debug.cc
	$(CXX) $(CXXFLAGS2) -c -o $@ $(@:.o=.cc)

helpers.o: helpers.h helpers.cc
	$(CXX) $(CXXFLAGS2) -c -o $@ $(@:.o=.cc)

exception.o: exception.h exception.cc
	$(CXX) $(CXXFLAGS2) -c -o $@ $(@:.o=.cc)


###############################################################################
# Parser

bpel-lexic.o: bpel-lexic.c
	$(CXX) $(CXXFLAGS0) -c -o $@ $(@:.o=.c)

bpel-lexic.c: bpel-lexic.l
	$(LEX) -o$@ $<

bpel-syntax.o: bpel-syntax.c
	$(CXX) $(CXXFLAGS2) -c -o $@ $(@:.o=.c)

bpel-syntax.c: bpel-syntax.y
	$(YACC) $(YFLAGS) -o $@ -d $^



##############################################################################
# Kimwitu++ Files 1

bpel-kc-csgiok.o: $(KC_TIME)
	$(CXX) $(CXXFLAGS) -c -o $@ $(@:.o=.cc)

bpel-kc-k.o: $(KC_TIME)
	$(CXX) $(CXXFLAGS1) -c -o $@ $(@:.o=.cc)

bpel-kc-rk.o: $(KC_TIME)
	$(CXX) $(CXXFLAGS) -c -o $@ $(@:.o=.cc)

bpel-kc-unpk.o: $(KC_TIME)
	$(CXX) $(CXXFLAGS) -c -o $@ $(@:.o=.cc)



##############################################################################
# Kimwitu++ Files 2

bpel-unparse-xml.o: $(KC_TIME)
	$(CXX) $(CXXFLAGS2) -c -o $@ $(@:.o=.cc)

bpel-unparse-petri.o: $(KC_TIME)
	$(CXX) $(CXXFLAGS2) -c -o $@ $(@:.o=.cc)

bpel-rewrite.o: $(KC_TIME)
	$(CXX) $(CXXFLAGS2) -c -o $@ $(@:.o=.cc)

bpel-abstract.o: $(KC_TIME)
	$(CXX) $(CXXFLAGS2) -c -o $@ $(@:.o=.cc)

$(KC_TIME): bpel-abstract.k bpel-unparse-xml.k bpel-unparse-petri.k bpel-rewrite.k
	@touch $@
	$(KC) $(KFLAGS) $^



###############################################################################
clean:
	@echo removing temporary files...
	@-rm -f core *~ *.bak
	@-rm -f bpel-lexic.c bpel-syntax.h bpel-syntax.c
	@-rm -f bpel-kc-*.cc bpel-kc-*.h
	@-rm -f bpel-abstract.cc bpel-abstract.h
	@-rm -f bpel-unparse-xml.cc bpel-unparse-xml.h
	@-rm -f bpel-unparse-petri.cc bpel-unparse-petri.h
	@-rm -f bpel-rewrite.cc bpel-rewrite.h
	@-rm -f $(KC_TIME)
	@-rm -f *.o

veryclean: clean
	@echo removing documentation...
	@-rm -f -r doc
	@-rm -f -r doc
	@echo removing executable...
	@-rm -f bpel2owfn.exe bpel2owfn

doc: $(KC_TIME) bpel-syntax.c bpel-lexic.c
	@doxygen
