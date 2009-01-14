noinst_LIBRARIES = libpnapi.a

libpnapi_a_SOURCES = \
	parser-owfn.yy parser-petrify.yy \
	parser-owfn-lexer.ll parser-petrify-lexer.ll \
	pnapi.h \
	component.h component.cc \
	petrinet.h petrinet.cc \
	parser.h parser.cc \
	util.h util.cc \
	formula.h formula.cc \
	petrinet-output.cc \
	petrinet-reduction.cc \
	petrinet-petrify.cc \
	automaton.h automaton.cc \
	state.h state.cc

BUILT_SOURCES = parser-owfn.cc parser-petrify.cc

EXTRA_DIST = parser-owfn.h parser-petrify.h

# show all warnings
libpnapi_a_LDFLAGS  = @configured_LDFLAGS@
libpnapi_a_CXXFLAGS = @configured_CXXFLAGS@ -Wall

# clean additional (generated) files
MAINTAINERCLEANFILES = parser-owfn.h parser-onwd.h parser-petrify.h
