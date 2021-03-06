# configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# include file for calling Flex
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/Flex.cmake)
# include file for calling Bison
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/Bison.cmake)
# include file for calling Gengetopt
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/Gengetopt.cmake)

########### next target ###############

SET(marlene_SRCS
   marlene.h
   marlene.cc
   helper.h
   helper.cc
   adapter.h
   adapter.cc
   Output.h
   Output.cc
   macros.h
   config.h
)

ADD_FLEX_FILES(marlene_SRCS lexer_adapt_rules.ll)
ADD_BISON_FILES(marlene_SRCS parser_adapt_rules.yy)
ADD_GENGETOPT_FILES(marlene_SRCS cmdline.ggo)

add_executable(marlene ${marlene_SRCS})

target_link_libraries(marlene pnapi)

install(TARGETS marlene DESTINATION bin)


########### install files ###############

install(FILES  marlene.conf DESTINATION bin )

include_directories(. ../libs)

add_definitions(-DSYSCONFDIR="$(sysconfdir)" -DHAVE_CONFIG_H -DYY_NO_UNISTD_H)

#original Makefile.am contents follow:

#
#sysconf_DATA = marlene.conf
#
## pass the sysconfdir to Marlene
#marlene_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\"
#
## only add the local Petri net API if necessary
#if COMPILE_PNAPI
#marlene_CPPFLAGS += -I$(top_srcdir)/libs
#marlene_LDADD = $(top_builddir)/libs/pnapi/libpnapi.a
#endif
#
##############################################################################
## EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
##############################################################################
#
##---------------------------------------------------#
## GENERIC: initialization for other generic targets #
##---------------------------------------------------#
#BUILT_SOURCES =
#EXTRA_DIST =
#MAINTAINERCLEANFILES =
#DISTCLEANFILES = 
#
##--------------------------------------------------------------#
## GENERIC: organize distribution and cleaning of Bison sources #
##--------------------------------------------------------------#
#BISON_FILES = $(filter %.yy,$(marlene_SOURCES))
#BUILT_SOURCES += $(BISON_FILES:.yy=.cc)
#EXTRA_DIST += $(BISON_FILES:.yy=.h)
#MAINTAINERCLEANFILES += $(BISON_FILES:.yy=.h)
#
##----------------------------------------------------------------#
## GENERIC: copy the content of the config.log file to a C header #
##----------------------------------------------------------------#
#config-log.h: $(top_builddir)/config.log
#	echo "#define CONFIG_LOG \\" > $@
#	$(SED) -e 's/\\/\\\\/g' $< | $(SED) -e 's/"/\\"/g' | $(AWK) '{print "\""$$0"\\n\"\\"}' >> $@
#	echo '""' >> $@
#	echo "" >> $@
#BUILT_SOURCES += config-log.h
#DISTCLEANFILES += config-log.h
#
##----------------------------------------------------------#
## GENERIC: use GNU gengetopt to create command line parser #
##----------------------------------------------------------#
#.ggo.c:
#	$(GENGETOPT) --input=$<
#GENGETOPT_FILES = cmdline.ggo
#BUILT_SOURCES += $(GENGETOPT_FILES:.ggo=.c)
#EXTRA_DIST += $(GENGETOPT_FILES)
#MAINTAINERCLEANFILES += $(GENGETOPT_FILES:.ggo=.c) $(GENGETOPT_FILES:.ggo=.h)
#
#doxygen:
#	cd ../doc ; doxygen
#
