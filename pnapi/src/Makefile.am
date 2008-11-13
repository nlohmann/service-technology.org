lib_LIBRARIES = lib_pnapi.a

lib_pnapi_a_SOURCES = \
	petrinet-input-owfn-parser.yy \
	petrinet-input-owfn-lexer.ll \
	petrinet-input-owfn-parser.h petrinet-input-owfn-parser.cc \
	petrinet-input-owfn-lexer.cc \
	pnapi.h pnapi.cc \
	petrinet.h petrinet.cc \
	helpers.h helpers.cc \
	formula.h formula.cc \
	options.h typedefs.h \
	petrinet-output.cc \
	petrinet-reduction.cc

# flex option to make the ylwrap script work
AM_LFLAGS = -olex.yy.c

# clean additional (generated) files
MAINTAINERCLEANFILES = petrinet-input-owfn-parser.h
