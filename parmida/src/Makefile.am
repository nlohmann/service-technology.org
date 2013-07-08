# the main program
bin_PROGRAMS = parmida

# the plain sources you need to compile (no generated code)
parmida_SOURCES = \
                cmdline.c cmdline.h \
				verbose.cc verbose.h \
				imatrix.cc imatrix.h \
				rules.cc rules.h \
				facts.cc facts.h \
				Socket.cc Socket.h \
				Runtime.cc Runtime.h \
				JSON.cc JSON.h \
                main.cc

parmida_LDADD = -lpthread
# only add the local Petri net API if necessary
if COMPILE_PNAPI
parmida_CPPFLAGS = -I$(top_srcdir)/libs
parmida_LDADD += $(top_builddir)/libs/pnapi/libpnapi.a
endif

#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src

