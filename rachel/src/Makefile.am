bin_PROGRAMS = rachel

rachel_SOURCES = syntax_og.yy lexer_og.ll \
		 main.cc config.h \
		 helpers.h helpers.cc \
		 Graph.cc Graph.h \
		 Scheduler.cc Scheduler.h \
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


################################################################
# organize flags
################################################################

rachel_LDFLAGS  =
rachel_CFLAGS   =
rachel_CXXFLAGS =


# flags to use threads
#rachel_LDFLAGS  += -lpthread
#rachel_CFLAGS   += -D_REENTRANT
#rachel_CXXFLAGS += -D_REENTRANT


# flags to build Mac Universal binary
if UNIVERSAL
  rachel_LDFLAGS  += -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch ppc -arch i386 -mmacosx-version-min=10.4
  rachel_CFLAGS   += -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch ppc -arch i386
  rachel_CXXFLAGS += -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch ppc -arch i386
endif


# flags to build Windows MinGW binary
if WIN32
  rachel_LDFLAGS  += -mno-cygwin -U _WIN32
  rachel_CFLAGS   += -mno-cygwin -U _WIN32
  rachel_CXXFLAGS += -mno-cygwin -U _WIN32
endif


# flags to build a 64 bit binary
if USE64BIT
  rachel_LDFLAGS  += -m64
  rachel_CFLAGS   += -m64
  rachel_CXXFLAGS += -m64
endif


################################################################
# organize cleaning, and distribution
################################################################

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


################################################################
# include lp_solve as library
################################################################
#rachel_LDADD = lp_solve/liblpsolve55.a
#INCLUDES = -Ilp_solve
#SUBDIRS = lp_solve
