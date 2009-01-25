bin_PROGRAMS = rachel

rachel_SOURCES = syntax_og.yy lexer_og.ll \
		 main.cc config.h \
		 helpers.h helpers.cc \
		 Graph.cc Graph.h \
		 Formula.cc Formula.h \
		 Action.cc Action.h \
		 costfunction.h types.h \
		 Simulation.h Matching.h \
		 EditDistance.h EditDistance.cc \
		 cmdline.c cmdline.h \
		 $(aspected_sources)

EXTRA_rachel_SOURCES = $(unaspected_sources)

BUILT_SOURCES = cmdline.c syntax_og.cc


################################################################
# inherit flags set by the configure script
################################################################

rachel_LDFLAGS  = @configured_LDFLAGS@
rachel_CFLAGS   = @configured_CFLAGS@
rachel_CXXFLAGS = @configured_CXXFLAGS@


################################################################
# organize cleaning, and distribution
################################################################

EXTRA_DIST = syntax_og.h cmdline.ggo rachel.conf $(aspects)

CLEANFILES = *.gcno *.gcda

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
if ENABLE_ASPECTS
	-$(AGPP) --config $(dir $<)puma.config --weave_only -o aspected-$(notdir $<) $<
	$(SED) -i '1 i #include <new>' aspected-$(notdir $<)
	$(SED) -i 's/new (unsigned long int/new (size_t/' aspected-$(notdir $<)
	$(SED) -i 's/new (unsigned int/new (size_t/' aspected-$(notdir $<)
	$(SED) -i 's/new (long unsigned int/new (size_t/' aspected-$(notdir $<)
else
	cp $(notdir $<) aspected-$(notdir $<)
endif


################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo
