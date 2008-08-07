bin_PROGRAMS = rachel

rachel_SOURCES = syntax_og.yy lexer_og.ll \
		 main.cc config.h \
		 helpers.h helpers.cc \
		 Graph.cc Graph.h \
		 Formula.cc Formula.h \
		 Action.cc Action.h \
		 costfunction.h \
		 Simulation.h Matching.h \
		 EditDistance.h EditDistance.cc \
		 LP.h LP.cc \
		 cmdline.c cmdline.h \
		 getopt.h getopt.c getopt1.c \
		 $(aspected_sources)

EXTRA_rachel_SOURCES = $(unaspected_sources)

BUILT_SOURCES = cmdline.c syntax_og.cc

EXTRA_DIST = syntax_og.h cmdline.ggo simmatch.conf Doxydetails \
	     $(aspects) \
	     lp_solve.tar.gz

MAINTAINERCLEANFILES = syntax_og.cc syntax_og.h lexer_og.cc \
		       cmdline.c cmdline.h puma.config \
		       $(aspected_sources)


################################################################
# use AspectC++ to create and weave in aspects
################################################################
aspects = Tracer.ah
aspected_sources = aspected-Simulation.cc aspected-Matching.cc
unaspected_sources = Simulation.cc Matching.cc

# Since AspectC++ does use "unsigned int" instead of the more portable "size_t",
# we have to run an edit script to fix the generated code. Thus bug has been
# filed at http://www.aspectc.org/bugzilla/show_bug.cgi?id=362
# if "./configure --disable-aspects" was used, the original sources are just copied
# and ag++ is not invoked at all
aspected-%.cc: %.cc $(aspects)
if ASPECTS
	-$(AGPP) --config $(dir $<)puma.config --weave_only -o aspected-$(notdir $<) $<
	$(SED) -i '1 i #include <new>' aspected-$(notdir $<)
	$(SED) -i 's/new (unsigned long int/new (size_t/' aspected-$(notdir $<)
	$(SED) -i 's/new (long unsigned int/new (size_t/' aspected-$(notdir $<)
else
	cp $(notdir $<) aspected-$(notdir $<)
endif


################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo --long-help --include-getopt --show-required --conf-parser


################################################################
# include lp_solve as library
################################################################
#rachel_LDADD = lp_solve/liblpsolve55.a
#INCLUDES = -Ilp_solve
#SUBDIRS = lp_solve
