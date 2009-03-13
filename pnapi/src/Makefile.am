noinst_LIBRARIES = libpnapi.a

libpnapi_a_SOURCES = \
	parser-owfn.yy    parser-owfn-lexer.ll \
	parser-petrify.yy parser-petrify-lexer.ll \
	parser-onwd.yy    parser-onwd-lexer.ll \
	pnapi.h \
	link.h link.cc \
	component.h component.cc \
	petrinet.h petrinet.cc \
	parser.h parser.cc \
	util.h util.cc \
	formula.h formula.cc \
	condition.h condition.cc \
	petrinet-reduction.cc \
	petrinet-petrify.cc \
	automaton.h automaton.cc\
	state.h state.cc \
	marking.h marking.cc \
	io.h io.cc io-format.cc

BUILT_SOURCES = parser-owfn.cc parser-petrify.cc

EXTRA_DIST = parser-owfn.h parser-petrify.h parser-onwd.h

# get flags from configure script
AM_LDFLAGS  = @configured_LDFLAGS@
AM_CXXFLAGS = @configured_CXXFLAGS@

# show all warnings
libpnapi_a_CPPFLAGS = -Wall

# clean additional (generated) files
DISTCLEANFILES = parser-owfn.h parser-owfn.cc parser-owfn-lexer.cc \
		 parser-onwd.h parser-onwd.cc parser-onwd-lexer.cc \
		 parser-petrify.h parser-petrify.cc parser-petrify-lexer.cc
