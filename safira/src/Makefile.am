bin_PROGRAMS = safira

safira_SOURCES = og_syntax.yy og_lexer.ll \
                 main.cc config.h \
                 Node.h Node.cc \
                 Formula.h Formula.cc \
                 Graph.h Graph.cc \
                 helpers.h helpers.cc \
                 testFormula.h testFormula.cc \
                 testGraph.h testGraph.cc \
                 testNode.h testNode.cc \
                 types.h \
                 cmdline.c cmdline.h \
                 verbose.h verbose.cc
                 
sysconf_DATA = safira.conf

all: minisat

minisat:
	$(LN_S) $(abs_top_builddir)/minisat/minisat minisat

# pass the sysconfdir to safira (ugly, but see http://www.gnu.org/software/autoconf/manual/autoconf.html#Defining-Directories)
safira_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\"

BUILT_SOURCES = cmdline.c og_syntax.cc config-log.h
EXTRA_DIST = og_syntax.h cmdline.ggo safira.conf
MAINTAINERCLEANFILES = og_syntax.h cmdline.c cmdline.h
DISTCLEANFILES = config-log.h
CLEANFILES = minisat

# copy the content of the config.log file to a C header (for debugging purposes)
config-log.h: $(top_srcdir)/config.log
	echo "#define CONFIG_LOG \\" > config-log.h
	$(SED) -e 's/\\/\\\\/g' $(top_builddir)/config.log | $(SED) -e 's/"/\\"/g' | $(AWK) '{print "\""$$0"\\n\"\\"}' >> config-log.h
	echo '""' >> config-log.h
	echo "" >> config-log.h


################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo
