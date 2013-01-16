# the main program
bin_PROGRAMS = locretia

# the plain sources you need to compile (no generated code)
locretia_SOURCES = cmdline.c cmdline.h \
                InnerMarking.h InnerMarking.cc \
		generateLog.cc generateLog.h \
		serviceAutomaton.cc serviceAutomaton.h \
                Label.h Label.cc \
                main.cc \
                Output.cc Output.h \
                syntax_graph.yy lexic_graph.ll \
                types.h \
                util.h \
                verbose.cc verbose.h

# the configuration files
sysconf_DATA = locretia.conf

# pass the sysconfdir to Locretia (ugly, but see http://www.gnu.org/software/autoconf/manual/autoconf.html#Defining-Directories)
locretia_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\"

# only add the local Petri net API if necessary
if COMPILE_PNAPI
locretia_CPPFLAGS += -I$(top_srcdir)/libs -I$(top_srcdir)/libs/pnapi
locretia_LDADD = $(top_builddir)/libs/pnapi/libpnapi.a
endif



#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
