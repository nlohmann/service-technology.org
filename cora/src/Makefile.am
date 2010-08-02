# the main program
bin_PROGRAMS = cora

# the plain sources you need to compile (no generated code)
cora_SOURCES = main.cc cmdline.c cmdline.h verbose.cc verbose.h

cora_LDADD = $(top_builddir)/libs/cora/libcora.a

# tell preprocessor to also look in libs folder for headers
cora_CPPFLAGS = -I$(top_srcdir)/libs

# only add the local Petri net API if necessary
if COMPILE_PNAPI
cora_LDADD += $(top_builddir)/libs/pnapi/libpnapi.a
endif


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
