# the main program
bin_PROGRAMS = candy

# the plain sources you need to compile (no generated code)
candy_SOURCES = og-syntax.yy og-lexic.ll \
                cf-syntax.yy cf-lexic.ll \
                nf-syntax.yy nf-lexic.ll \
                Output.h Output.cc \
                verbose.h verbose.cc \
                settings.h \
                Graph.cc Graph.h \
                Node.cc Node.h \
                Formula.cc Formula.h \
                Event.h \
                main.cc \
                cmdline.c cmdline.h


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
