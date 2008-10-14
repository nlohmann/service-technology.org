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
		 cmdline.c cmdline.h \
		 $(aspected_sources)

EXTRA_rachel_SOURCES = $(unaspected_sources)

BUILT_SOURCES = cmdline.c syntax_og.cc


################################################################
# organize flags
################################################################

# initial setting necessary to enable "+=" statements later
rachel_LDFLAGS  =
rachel_CFLAGS   =
rachel_CXXFLAGS =

# flags to build Mac Universal binary
if ENABLE_UNIVERSAL
  rachel_LDFLAGS  += -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch ppc -arch i386 -mmacosx-version-min=10.4
  rachel_CFLAGS   += -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch ppc -arch i386
  rachel_CXXFLAGS += -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch ppc -arch i386
endif

# flags to build Windows MinGW binary
if ENABLE_WIN32
  rachel_LDFLAGS  += -mno-cygwin -U _WIN32
  rachel_CFLAGS   += -mno-cygwin -U _WIN32
  rachel_CXXFLAGS += -mno-cygwin -U _WIN32
endif

# flags to build a 64 bit binary
if ENABLE_64BIT
  rachel_LDFLAGS  += -m64
  rachel_CFLAGS   += -m64
  rachel_CXXFLAGS += -m64
endif


################################################################
# organize cleaning, and distribution
################################################################

EXTRA_DIST = syntax_og.h cmdline.ggo rachel.conf Doxydetails \
	     $(aspects)

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
	$(GENGETOPT) --input=cmdline.ggo --long-help --include-getopt --show-required --conf-parser
