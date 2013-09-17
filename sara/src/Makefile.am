# the main program
bin_PROGRAMS = sara

# the plain sources you need to compile (no generated code)
sara_SOURCES = main.cc sthread.cc sthread.h problem.cc problem.h reachalyzer.cc reachalyzer.h imatrix.cc imatrix.h constraint.cc constraint.h lpwrapper.cc lpwrapper.h pathfinder.cc pathfinder.h partialsolution.cc partialsolution.h jobqueue.cc jobqueue.h cmdline.c cmdline.h verbose.cc verbose.h tarjan.cc tarjan.h syntax_sara.yy lexic_sara.ll sara_lib.h JSON.cc JSON.h

# pass the sysconfdir to Sara (ugly, but see http://www.gnu.org/software/autoconf/manual/autoconf.html#Defining-Directories)
sara_CPPFLAGS = -I$(top_srcdir)/libs
# add -DSARALIB (above) for compiling a library
sara_LDADD = $(top_builddir)/libs/lp_solve_5.5/liblpsolve55.la

# only add the local Petri net API if necessary
if COMPILE_PNAPI
sara_LDADD += $(top_builddir)/libs/pnapi/libpnapi.a -lpthread
endif


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src


