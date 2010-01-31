bin_PROGRAMS = lola2config

lola2config_SOURCES = output_syntax.yy output_lexic.ll main.cc

#--------------------------------------------------------------#
# GENERIC: organize distribution and cleaning of Bison sources #
#--------------------------------------------------------------#
BISON_FILES = $(filter %.yy,$(lola2config_SOURCES))
BUILT_SOURCES = $(BISON_FILES:.yy=.cc)
EXTRA_DIST = $(BISON_FILES:.yy=.h)
MAINTAINERCLEANFILES = $(BISON_FILES:.yy=.h)
