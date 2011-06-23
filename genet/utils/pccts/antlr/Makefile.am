# the main program
noinst_PROGRAMS = antlr

# the plain sources you need to compile (no generated code)
antlr_SOURCES = antlr.c \
		bits.c \
		build.c \
		dumpcycles.c \
		dumpnode.c \
		egman.c \
		err.c \
		fcache.c \
		fset.c \
		fset2.c \
		gen.c \
		generic.h \
		globals.c \
		hash.c \
		hash.h \
		lex.c \
		main.c \
		misc.c \
		mode.h \
		mrhoist.c \
		pred.c \
		proto.h \
		scan.c \
		stdpccts.h \
		syn.h \
		tokens.h \
		$(top_srcdir)/utils/pccts/support/set/set.c \
		$(top_srcdir)/utils/pccts/support/set/set.h

antlr_CPPFLAGS = -I$(top_srcdir)/utils/pccts/h -I$(top_srcdir)/utils/pccts/support/set -DUSER_ZZSYN -DZZLEXBUFSIZE=32000


EXTRA_DIST = antlr.1 parser.dlg

