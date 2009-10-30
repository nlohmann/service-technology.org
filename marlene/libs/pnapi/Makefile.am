
noinst_LIBRARIES = libpnapi.a

libpnapi_a_SOURCES = \
	parser-owfn.yy    parser-owfn-lexer.ll \
	parser-pn.yy parser-pn-lexer.ll \
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
	petrinet-pn.cc \
	automaton.h automaton.cc\
	state.h state.cc \
	marking.h marking.cc \
	myio.h myio.cc io-format.cc \
	errorhandling.cc \
	Output.h Output.cc

BUILT_SOURCES = parser-owfn.cc parser-owfn.h parser-owfn-lexer.cc \
                parser-pn.cc parser-pn.h parser-pn-lexer.cc \
		parser-onwd.cc parser-onwd.h parser-onwd-lexer.cc \
		parser-lola.cc parser-lola.h parser-lola-lexer.cc \
		parser-sa.cc parser-sa.h parser-sa-lexer.cc

EXTRA_DIST = parser-owfn.h parser-pn.h parser-onwd.h parser-lola.h \
	     parser-sa.h \
	     CMakeLists.txt

# clean additional (generated) files
DISTCLEANFILES = parser-owfn.h parser-owfn.cc parser-owfn-lexer.cc \
		 parser-onwd.h parser-onwd.cc parser-onwd-lexer.cc \
		 parser-pn.h parser-pn.cc parser-pn-lexer.cc \
		 parser-lola.h parser-lola.cc parser-lola-lexer.cc \
		 parser-sa.h parser-sa.cc parser-sa-lexer.cc


