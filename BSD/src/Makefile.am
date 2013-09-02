# the main program
bin_PROGRAMS = BSD

# the plain sources you need to compile (no generated code)
BSD_SOURCES = cmdline.c cmdline.h \
                InnerMarking.h InnerMarking.cc \
                BSD.cc BSD.h \
                openNet.cc openNet.h \
                Label.h Label.cc \
                main.cc \
                Output.cc Output.h \
                syntax_graph.yy lexic_graph.ll \
                types.h \
                util.h \
                verbose.cc verbose.h

# the configuration files
sysconf_DATA = BSD.conf

# pass the sysconfdir to BSD (ugly, but see http://www.gnu.org/software/autoconf/manual/autoconf.html#Defining-Directories)
BSD_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\"

# only add the local Petri net API if necessary
if COMPILE_PNAPI
BSD_CPPFLAGS += -I$(top_srcdir)/libs -I$(top_srcdir)/libs/pnapi
BSD_LDADD = $(top_builddir)/libs/pnapi/libpnapi.a
endif



#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
