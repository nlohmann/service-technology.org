bin_PROGRAMS = wendy

wendy_SOURCES = syntax_graph.yy lexic_graph.ll \
		syntax_cover.yy lexic_cover.ll \
                main.cc types.h \
                FullMarking.h FullMarking.cc \
                InterfaceMarking.h InterfaceMarking.cc \
                Label.h Label.cc \
                InnerMarking.h InnerMarking.cc \
                Knowledge.h Knowledge.cc \
                StoredKnowledge.h StoredKnowledge.cc \
		Cover.h Cover.cc \
                verbose.cc verbose.h \
                cmdline.c cmdline.h

sysconf_DATA = wendy.conf

# pass the sysconfdir to Wendy (ugly, but see http://www.gnu.org/software/autoconf/manual/autoconf.html#Defining-Directories)
wendy_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\"

# only add the local Petri net API if necessary
if COMPILE_PNAPI
wendy_CPPFLAGS += -I$(top_srcdir)/libs
wendy_LDADD =  $(top_builddir)/libs/pnapi/libpnapi.a
endif

BUILT_SOURCES = cmdline.c syntax_graph.cc \
		syntax_cover.cc config-log.h 
EXTRA_DIST = syntax_graph.h syntax_cover.h cmdline.ggo
MAINTAINERCLEANFILES = syntax_graph.h syntax_cover.h \
		       cmdline.c cmdline.h
DISTCLEANFILES = config-log.h

# copy the content of the config.log file to a C header (for debugging purposes)
config-log.h: $(top_srcdir)/config.log
	echo "#define CONFIG_LOG \\" > config-log.h
	$(SED) -e 's/\\/\\\\/g' $(top_builddir)/config.log | $(SED) -e 's/"/\\"/g' | $(AWK) '{print "\""$$0"\\n\"\\"}' >> config-log.h
	echo '""' >> config-log.h
	echo "" >> config-log.h


################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo
