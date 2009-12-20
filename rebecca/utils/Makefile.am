noinst_PROGRAMS = chor2dot

chor2dot_SOURCES = syntax_chor.yy lexic_chor.ll

#--------------------------------------------------------------#
# GENERIC: organize distribution and cleaning of Bison sources #
#--------------------------------------------------------------#
BISON_FILES = $(filter %.yy,$(chor2dot_SOURCES))
BUILT_SOURCES = $(BISON_FILES:.yy=.cc)
EXTRA_DIST = $(BISON_FILES:.yy=.h)
MAINTAINERCLEANFILES = $(BISON_FILES:.yy=.h)
