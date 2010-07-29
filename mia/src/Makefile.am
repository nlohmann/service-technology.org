# the main program
bin_PROGRAMS = mia

# the plain sources you need to compile (no generated code)
mia_SOURCES = syntax_graph.yy lexic_graph.ll \
              syntax_im.yy lexic_im.ll \
              syntax_mi.yy lexic_mi.ll \
              main.cc \
              verbose.cc verbose.h \
              cmdline.c cmdline.h \
              Output.cc Output.h

# the configuration files
sysconf_DATA = mia.conf

# pass the sysconfdir to Wendy (ugly, but see http://www.gnu.org/software/autoconf/manual/autoconf.html#Defining-Directories)
mia_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\"

# only add the local Petri net API if necessary
if COMPILE_PNAPI
mia_CPPFLAGS += -I$(top_srcdir)/libs -I$(top_srcdir)/libs/pnapi
mia_LDADD = $(top_builddir)/libs/pnapi/libpnapi.a
endif


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
