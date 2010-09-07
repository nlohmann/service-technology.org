# the main program
bin_PROGRAMS = owfn2sara

# the plain sources you need to compile (no generated code)
owfn2sara_SOURCES = owfn2sara.cc problem.h problem.cc verbose.h verbose.cc cmdline.h cmdline.c

# pass the sysconfdir to Sara (ugly, but see http://www.gnu.org/software/autoconf/manual/autoconf.html#Defining-Directories)
owfn2sara_CPPFLAGS = -I$(top_srcdir)/libs
# owfn2sara_LDADD = $(top_builddir)/libs/lp_solve/liblpsolve55.la

# only add the local Petri net API if necessary
if COMPILE_PNAPI
owfn2sara_LDADD = $(top_builddir)/libs/pnapi/libpnapi.a
endif


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
