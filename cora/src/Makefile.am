# the main program
bin_PROGRAMS = cora

# the plain sources you need to compile (no generated code)
cora_SOURCES = main.cc cora.cc cora.h pnloader.cc pnloader.h imatrix.cc imatrix.h cmdline.c cmdline.h verbose.cc verbose.h extmarking.cc extmarking.h cnode.cc cnode.h rnode.cc rnode.h covergraph.cc covergraph.h stubbornset.cc stubbornset.h

cora_CPPFLAGS = -I$(top_srcdir)/libs

# only add the local Petri net API if necessary
if COMPILE_PNAPI
cora_LDADD = $(top_builddir)/libs/pnapi/libpnapi.a
endif


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
