# everything for the parser
DLG = $(top_builddir)/utils/pccts/dlg/dlg
DLG_FILE = parser.dlg
DLG_FLAGS = -C2

ANTLR = $(top_builddir)/utils/pccts/antlr/antlr
ANTLR_H = $(top_srcdir)/../utils/pccts/h
ANTLR_FLAGS = -gh -k 2 -gt

TOKENS = tokens.h
# grammar file
GRM = sor.g
# lexer file
#SCAN = DLGLexer

# files created by antlr
ANTLR_CPP = sor.c err.c
ANTLR_DIST = stdpccts.h $(DLG_FILE) $(TOKENS)
ANTLR_SPAWN = $(ANTLR_CPP) $(ANTLR_DIST)
	      

#files created by dlg
DLG_CPP = scan.c
DLG_DIST = mode.h
DLG_SPAWN = $(DLG_CPP) $(DLG_DIST)


# the main program
noinst_PROGRAMS = sor

# the plain sources you need to compile (no generated code)
sor_SOURCES = cpp.c \
	      gen.c \
	      globals.c \
	      hash.c \
	      hash.h \
	      look.c \
	      main.c \
	      proto.h \
	      sor.g \
	      sor.h \
	      sym.h \
	      $(GRM) \
	      $(ANTLR_SPAWN) \
	      $(DLG_SPAWN) \
	      $(top_srcdir)/utils/pccts/support/set/set.c

sor_CPPFLAGS = -I$(top_srcdir)/utils/pccts/h -I$(top_srcdir)/utils/pccts/support/set -DZZLEXBUFSIZE=32000 # -DUSER_ZZSYN 

BUILT_SOURCES = $(ANTLR_CPP) $(DLG_CPP)

EXTRA_DIST = $(top_srcdir)/utils/pccts/h/ast.c

$(ANTLR_SPAWN) : $(GRM)
	$(ANTLR) $(ANTLR_FLAGS) $(GRM)

$(DLG_SPAWN) : $(DLG_FILE)
	$(DLG) $(DLG_FLAGS) $(DLG_FILE) $(DLG_CPP)

DISTCLEANFILES = $(ANTLR_SPAWN) $(DLG_SPAWN)

