#if FROMSVN
#.PHONY : subversion.h
#endif

AM_CXXFLAGS=@configured_CXXFLAGS@
AM_CFLAGS=@configured_CFLAGS@
AM_LDFLAGS=@configured_LDFLAGS@

bin_PROGRAMS = marlene

marlene_SOURCES = parser_adapt_rules.yy lexer_adapt_rules.ll \
		  marlene.h marlene.cc \
		  helper.h helper.cc \
		  adapter.h adapter.cc\
		  cmdline.c cmdline.h \
		  macros.h #subversion.h 

sysconf_DATA = marlene.conf

# pass the sysconfdir to Marlene
marlene_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\"

# only add the local Petri net API if necessary
if COMPILE_PNAPI
marlene_CPPFLAGS += -I$(top_srcdir)/libs
marlene_LDADD = $(top_builddir)/libs/pnapi/libpnapi.a
endif


BUILT_SOURCES = parser_adapt_rules.cc lexer_adapt_rules.cc \
		subversion.h cmdline.c

EXTRA_DIST = parser_adapt_rules.h cmdline.ggo subversion.h

MAINTAINERCLEANFILES = parser_adapt_rules.h parser_adapt_rules.cc \
		       lexer_adapt_rules.cc \
		       cmdline.c cmdline.h

################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo

# automatically add SVN revision to a header file
if FROMSVN
svnrevision = \#define SVNREV \"`svnversion -c`\"

subversion.h: $(marlene_SOURCES)
	@echo "#ifndef SUBVERSION_H" > subversion.h
	@echo "#define SUBVERSION_H" >> subversion.h
	@echo "$(svnrevision)" >> subversion.h
	@echo "#endif" >>subversion.h
else
svnrevision =

subversion.h:
	@echo "#ifndef SUBVERSION_H" > subversion.h
	@echo "#define SUBVERSION_H" >> subversion.h
	@echo "#endif" >>subversion.h
endif

doxygen:
	cd ../doc ; doxygen

