# the main program
bin_PROGRAMS = lisa

# the plain sources you need to compile (no generated code)
lisa_SOURCES =  main.cc \
                lpwrapper.h lpwrapper.cc \
                verbose.cc verbose.h \
                cmdline.c cmdline.h \
                Output.cc Output.h

lisa_CPPFLAGS = -I$(top_srcdir)/libs
lisa_LDADD = $(top_builddir)/libs/lp_solve/liblpsolve55.la

# only add the local Petri net API if necessary
if COMPILE_PNAPI
lisa_LDADD += $(top_builddir)/libs/pnapi/libpnapi.a
endif


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
