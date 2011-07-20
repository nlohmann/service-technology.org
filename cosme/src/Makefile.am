# the main program
bin_PROGRAMS = cosme

# the plain sources you need to compile (no generated code)
cosme_SOURCES = og-syntax.yy og-lexic.ll \
                graph-syntax.yy graph-lexic.ll \
                util.h \
                main.cc types.h \
                Results.cc Results.h \	
                BitSetC.cc BitSetC.h \
                Label.cc Label.h \
                OperatingGuideline.cc OperatingGuideline.h \
                Service.cc Service.h\
                RBTreeStack.cc RBTreeStack.h \
                cmdline.c cmdline.h \
                Output.cc Output.h \
                verbose.cc verbose.h

# the configuration files
sysconf_DATA = cosme.conf

# pass the sysconfdir to Cosme (ugly, but see http://www.gnu.org/software/autoconf/manual/autoconf.html#Defining-Directories)
cosme_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\"

# only add the local Petri net API if necessary
if COMPILE_PNAPI
cosme_CPPFLAGS += -I$(top_srcdir)/libs -I$(top_srcdir)/libs/pnapi
cosme_LDADD = $(top_builddir)/libs/pnapi/libpnapi.a
endif


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
