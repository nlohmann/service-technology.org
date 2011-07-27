# the main program
bin_PROGRAMS = rachel

# the plain sources you need to compile (no generated code)
rachel_SOURCES = syntax_og.yy lexer_og.ll \
		 main.cc config.h \
		 helpers.h helpers.cc \
		 Graph.cc Graph.h \
		 Formula.cc Formula.h \
		 Action.cc Action.h \
		 costfunction.h types.h \
		 Simulation.cc Simulation.h \
		 Matching.cc Matching.h \
		 EditDistance.h EditDistance.cc \
		 cmdline.c cmdline.h \
		 verbose.cc verbose.h \
		 Output.cc Output.h



#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
