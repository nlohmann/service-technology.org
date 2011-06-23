# the main program
noinst_PROGRAMS = dlg

# the plain sources you need to compile (no generated code)
dlg_SOURCES = automata.c \
	      dlg.h \
	      dlg_a.c \
	      dlg_p.c \
	      err.c \
	      main.c \
	      mode.h \
	      output.c \
	      relabel.c \
	      stdpccts.h \
	      support.c \
	      tokens.h \
	      $(top_srcdir)/utils/pccts/support/set/set.c

dlg_CPPFLAGS = -I$(top_srcdir)/utils/pccts/h -I$(top_srcdir)/utils/pccts/support/set -DUSER_ZZSYN -DZZLEXBUFSIZE=32000

EXTRA_DIST = dlg.1 parser.dlg

