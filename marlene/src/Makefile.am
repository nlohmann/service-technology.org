bin_PROGRAMS = marlene

marlene_SOURCES = parser_adapt_rules.yy lexer_adapt_rules.ll \
		  parser_marking_information.yy lexer_marking_information.ll \
		  marlene.h marlene.cc \
		  helper.h helper.cc \
		  adapter.h adapter.cc\
		  diagnosis.h diagnosis.cc \
		  markings.h markings.cc \
		  cmdline.c cmdline.h \
		  Output.h Output.cc \
		  verbose.h verbose.cc \
		  macros.h #subversion.h 

sysconf_DATA = marlene.conf

# pass the sysconfdir to Marlene
marlene_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\"

# only add the local Petri net API if necessary
if COMPILE_PNAPI
marlene_CPPFLAGS += -I$(top_srcdir)/libs
marlene_LDADD = $(top_builddir)/libs/pnapi/libpnapi.a
endif

#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src

