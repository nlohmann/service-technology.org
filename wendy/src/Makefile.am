# the main program
bin_PROGRAMS = wendy

# the plain sources you need to compile (no generated code)
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
                cmdline.ggo

# the configuration files
sysconf_DATA = wendy.conf

# pass the sysconfdir to Wendy (ugly, but see http://www.gnu.org/software/autoconf/manual/autoconf.html#Defining-Directories)
wendy_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\"

# only add the local Petri net API if necessary
if COMPILE_PNAPI
wendy_CPPFLAGS += -I$(top_srcdir)/libs
wendy_LDADD = $(top_builddir)/libs/pnapi/libpnapi.a
endif


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

#---------------------------------------------------#
# GENERIC: initialization for other generic targets #
#---------------------------------------------------#
BUILT_SOURCES =
EXTRA_DIST =
MAINTAINERCLEANFILES =
DISTCLEANFILES = 

#--------------------------------------------------------------#
# GENERIC: organize distribution and cleaning of Bison sources #
#--------------------------------------------------------------#
BISON_FILES = $(filter %.yy,$(wendy_SOURCES))
BUILT_SOURCES += $(BISON_FILES:.yy=.cc)
EXTRA_DIST += $(BISON_FILES:.yy=.h)
MAINTAINERCLEANFILES += $(BISON_FILES:.yy=.h)

#----------------------------------------------------------------#
# GENERIC: copy the content of the config.log file to a C header #
#----------------------------------------------------------------#
config-log.h: $(top_builddir)/config.log
	echo "#define CONFIG_LOG \\" > $@
	$(SED) -e 's/\\/\\\\/g' $< | $(SED) -e 's/"/\\"/g' | $(AWK) '{print "\""$$0"\\n\"\\"}' >> $@
	echo '""' >> $@
	echo "" >> $@
BUILT_SOURCES += config-log.h
DISTCLEANFILES += config-log.h

#----------------------------------------------------------#
# GENERIC: use GNU gengetopt to create command line parser #
#----------------------------------------------------------#
.ggo.c:
	$(GENGETOPT) --input=$<
GENGETOPT_FILES = $(filter %.ggo,$(wendy_SOURCES))
BUILT_SOURCES += $(GENGETOPT_FILES:.ggo=.c)
EXTRA_DIST += $(GENGETOPT_FILES:.ggo=.c) $(GENGETOPT_FILES:.ggo=.h)
MAINTAINERCLEANFILES += $(GENGETOPT_FILES:.ggo=.c) $(GENGETOPT_FILES:.ggo=.h)
