bin_PROGRAMS = unfolder

unfolder_SOURCES = syntax_hlowfn_initial.yy lexic_hlowfn_initial.ll \
		   syntax_hlowfn_final.yy lexic_hlowfn_final.ll \
                   syntax_llowfn_initial.yy lexic_llowfn_initial.ll \
	 	   syntax_llowfn_final.yy lexic_llowfn_final.ll \
                   main.cc types.h

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
BISON_FILES = $(filter %.yy,$(unfolder_SOURCES))
BUILT_SOURCES += $(BISON_FILES:.yy=.cc)
EXTRA_DIST += $(BISON_FILES:.yy=.h)
MAINTAINERCLEANFILES += $(BISON_FILES:.yy=.h)
