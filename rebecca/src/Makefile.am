bin_PROGRAMS = rebecca

rebecca_SOURCES = syntax_chor.yy lexic_chor.ll \
                  main.cc \
                  verbose.cc verbose.h \
                  cmdline.c cmdline.h

BUILT_SOURCES = cmdline.c syntax_chor.cc config-log.h
EXTRA_DIST = syntax_chor.h cmdline.ggo
MAINTAINERCLEANFILES = syntax_chor.h cmdline.c cmdline.h
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


doxygen:
	cd ../doc ; doxygen
