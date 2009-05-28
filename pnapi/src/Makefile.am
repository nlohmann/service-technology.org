lib_LTLIBRARIES = libpnapi.la

libpnapi_la_SOURCES = \
	parser-owfn.yy    parser-owfn-lexer.ll \
	parser-petrify.yy parser-petrify-lexer.ll \
	parser-onwd.yy    parser-onwd-lexer.ll \
	parser-lola.yy    parser-lola-lexer.ll \
	parser-sa.yy	  parser-sa-lexer.ll \
	pnapi.h \
	link.h link.cc \
	component.h component.cc \
	petrinet.h petrinet.cc \
	parser.h parser.cc \
	util.h util.cc \
	formula.h formula.cc \
	condition.h condition.cc \
	petrinet-reduction.cc \
	petrinet-petrify.h petrinet-petrify.cc \
	automaton.h automaton.cc\
	state.h state.cc \
	marking.h marking.cc \
	io.h io.cc io-format.cc

BUILT_SOURCES = parser-owfn.cc parser-petrify.cc parser-onwd.cc parser-lola.cc \
		parser-sa.cc

EXTRA_DIST = parser-owfn.h parser-petrify.h parser-onwd.h parser-lola.h \
	     parser-sa.h

# get flags from configure script
AM_LDFLAGS  = @configured_LDFLAGS@
AM_CXXFLAGS = @configured_CXXFLAGS@

# show all warnings
libpnapi_a_CPPFLAGS = -Wall

# information for libtool
includedir = @includedir@/pnapi

include_HEADERS = pnapi.h io.h condition.h petrinet.h \
		  automaton.h formula.h component.h \
		  parser.h util.h state.h \
		  marking.h link.h \
		  parser-owfn.h parser-petrify.h \
		  parser-onwd.h parser-lola.h \
		  parser-sa.h

# clean additional (generated) files
DISTCLEANFILES = parser-owfn.h parser-owfn.cc parser-owfn-lexer.cc \
		 parser-onwd.h parser-onwd.cc parser-onwd-lexer.cc \
		 parser-petrify.h parser-petrify.cc parser-petrify-lexer.cc \
		 parser-lola.h parser-lola.cc parser-lola-lexer.cc \
		 parser-sa.h parser-sa.cc parser-sa-lexer.cc
