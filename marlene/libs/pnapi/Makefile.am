
noinst_LIBRARIES = libpnapi.a

libpnapi_a_SOURCES = \
                parser-owfn.yy    parser-owfn-lexer.ll \
                parser-petrify.yy parser-petrify-lexer.ll \
                parser-onwd.yy    parser-onwd-lexer.ll \
                parser-lola.yy    parser-lola-lexer.ll \
                parser-sa.yy      parser-sa-lexer.ll \
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
                myio.h myio.cc io-format.cc \
		errorhandling.cc

# to find Marlene's config.h header
libpnapi_a_CPPFLAGS = -I$(top_srcdir)/src

BUILT_SOURCES = parser-owfn.cc parser-petrify.cc parser-onwd.cc parser-lola.cc \
		parser-sa.cc

EXTRA_DIST = parser-owfn.h parser-petrify.h parser-onwd.h parser-lola.h \
	     parser-sa.h

# clean additional (generated) files
DISTCLEANFILES = parser-owfn.h parser-owfn.cc parser-owfn-lexer.cc \
		 parser-onwd.h parser-onwd.cc parser-onwd-lexer.cc \
		 parser-petrify.h parser-petrify.cc parser-petrify-lexer.cc \
		 parser-lola.h parser-lola.cc parser-lola-lexer.cc \
		 parser-sa.h parser-sa.cc parser-sa-lexer.cc

