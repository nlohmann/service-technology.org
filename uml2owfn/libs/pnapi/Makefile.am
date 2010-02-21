noinst_LIBRARIES = libpnapi.a

libpnapi_a_SOURCES = \
    parser-lola.yy    parser-lola-lexer.ll \
    parser-onwd.yy    parser-onwd-lexer.ll \
    parser-owfn.yy    parser-owfn-lexer.ll \
    parser-pn.yy      parser-pn-lexer.ll \
    parser-pnml.yy    parser-pnml-lexer.ll \
    parser-sa.yy      parser-sa-lexer.ll \
    pnapi.h \
    Output.h Output.cc \
    automaton.h automaton.cc\
    component.h component.cc \
    condition.h condition.cc \
    errorhandling.cc \
    formula.h formula.cc \
    interface.h interface.cc \
    link.h link.cc \
    marking.h marking.cc \
    myio.h myio.cc io-format.cc \
    parser.h parser.cc \
    petrinet-pn.cc \
    petrinet-reduction.cc \
    petrinet.h petrinet.cc \
    port.h port.cc \
    state.h state.cc \
    util.h

# to find UML2oWFN's config.h header
libpnapi_a_CPPFLAGS = -I$(top_srcdir)/src

BUILT_SOURCES = parser-owfn.cc parser-onwd.cc parser-lola.cc \
		parser-sa.cc parser-pn.cc parser-pnml.cc

EXTRA_DIST = parser-owfn.h parser-onwd.h parser-lola.h \
	     parser-sa.h parser-pn.h parser-pnml.h

# clean additional (generated) files
DISTCLEANFILES = parser-owfn.h parser-owfn.cc parser-owfn-lexer.cc \
		 parser-onwd.h parser-onwd.cc parser-onwd-lexer.cc \
		 parser-lola.h parser-lola.cc parser-lola-lexer.cc \
		 parser-sa.h parser-sa.cc parser-sa-lexer.cc \
		 parser-pn.h parser-pn.cc parser-pn-lexer.cc \ 
		 parser-pnml.h parser-pnml.cc parser-pnml-lexer.cc

