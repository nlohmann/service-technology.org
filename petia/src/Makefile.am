# the main program
bin_PROGRAMS = petia

# the plain sources you need to compile (no generated code)
petia_SOURCES = main.cc Output.cc Output.h verbose.cc verbose.h inveqrel.cc inveqrel.h lpwrapper.cc lpwrapper.h cmdline.c cmdline.h

# pass the sysconfdir to Petia (ugly, but see http://www.gnu.org/software/autoconf/manual/autoconf.html#Defining-Directories)
petia_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\" -I$(top_srcdir)/libs
petia_LDADD = $(top_builddir)/libs/lp_solve/liblpsolve55.la

# only add the local Petri net API if necessary
if COMPILE_PNAPI
petia_LDADD += $(top_builddir)/libs/pnapi/libpnapi.a
endif


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
