noinst_LIBRARIES = libpnapi.a

libpnapi_a_SOURCES = \
	pnapi.h \
	component.h component.cc \
	petrinet.h petrinet.cc \
	parser.h parser.cc \
	parser-owfn.yy parser-owfn-lexer.ll \
	parser-owfn.h parser-owfn.cc parser-owfn-lexer.cc \
	parser-petrify.yy parser-petrify-lexer.ll \
	parser-petrify.h parser-petrify.cc parser-petrify-lexer.cc \
	util.h util.cc \
	formula.h formula.cc \
	petrinet-output.cc \
	petrinet-reduction.cc \
	petrinet-petrify.cc \
	automaton.h automaton.cc \
	state.h state.cc

# flex option to make the ylwrap script work
AM_LFLAGS = -olex.yy.c

# show all warnings
AM_CXXFLAGS = -Wall

# clean additional (generated) files
MAINTAINERCLEANFILES = parser-owfn.h parser-onwd.h parser-petrify.h
