bin_PROGRAMS = stanca

stanca_SOURCES = main.cc config.h \
			 STFormula.h STFormula.cc \
                 cmdline.c cmdline.h \
                 verbose.h verbose.cc \
                 Output.h Output.cc

stanca_CPPFLAGS = -I$(top_srcdir)/libs/pnapi \
                   -I$(top_srcdir)/libs/minisat
stanca_LDADD = $(top_builddir)/libs/pnapi/libpnapi.a \
				$(top_builddir)/libs/minisat/libminisat.a




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
BISON_FILES = $(filter %.yy,$(stanca_SOURCES))
BUILT_SOURCES += $(BISON_FILES:.yy=.cc)
EXTRA_DIST += $(BISON_FILES:.yy=.h)
MAINTAINERCLEANFILES += $(BISON_FILES:.yy=.h)

#----------------------------------------------------------------#
# GENERIC: copy the content of the config.log file to a C header #
#----------------------------------------------------------------#
config-log.h: $(top_builddir)/config.log
	echo "/// the content of the file 'config.log' generated by the configure script" > $@
	echo "#define CONFIG_LOG \\" >> $@
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
GENGETOPT_FILES = cmdline.ggo
BUILT_SOURCES += $(GENGETOPT_FILES:.ggo=.c)
EXTRA_DIST += $(GENGETOPT_FILES)
MAINTAINERCLEANFILES += $(GENGETOPT_FILES:.ggo=.c) $(GENGETOPT_FILES:.ggo=.h)
