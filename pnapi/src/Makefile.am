lib_LIBRARIES = lib_pnapi.a

lib_pnapi_a_SOURCES = \
	pnapi.h pnapi.cc \
	petrinet.h petrinet.cc \
	parser.h parser.cc \
	parser-owfn.yy parser-owfn-lexer.ll \
	parser-owfn.h parser-owfn.cc parser-owfn-lexer.cc \
	helpers.h helpers.cc \
	formula.h formula.cc \
	options.h typedefs.h \
	petrinet-output.cc \
	petrinet-reduction.cc

# flex option to make the ylwrap script work
AM_LFLAGS = -olex.yy.c

# clean additional (generated) files
MAINTAINERCLEANFILES = parser-owfn.h
