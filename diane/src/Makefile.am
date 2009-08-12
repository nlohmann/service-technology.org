bin_PROGRAMS = diane 

diane_SOURCES =  main.cc \
                 decomposition.cc decomposition.h \
                 cmdline.c cmdline.h

# only add the local Petri net API if necessary
if COMPILE_PNAPI
diane_LDADD = $(top_builddir)/libs/pnapi/libpnapi.a
diane_CPPFLAGS = -I$(top_srcdir)/libs
endif

BUILT_SOURCES = cmdline.c config-log.h
EXTRA_DIST = cmdline.ggo
MAINTAINERCLEANFILES = cmdline.c cmdline.h
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
