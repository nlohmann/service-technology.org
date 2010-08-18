bin_PROGRAMS = tess

tess_SOURCES = og_syntax.yy og_lexer.ll \
               main.cc config.h \
               Node.h Node.cc \
               Formula.h Formula.cc \
               Formula_toString.cc \
               Formula_toDNF.cc \
               Graph.h Graph.cc \
               TestCase.h TestCase.cc \
               helpers.h helpers.cc \
               types.h \
               cmdline.c cmdline.h \
               verbose.h verbose.cc \
               Output.h Output.cc

check_PROGRAMS = testtess

testtess_SOURCES = testmain.cc config.h \
                   Node.h Node.cc \
                   Formula.h Formula.cc \
                   Formula_toString.cc \
                   Formula_toDNF.cc \
                   Graph.h Graph.cc \
                   TestCase.h TestCase.cc \
                   helpers.h helpers.cc \
                   types.h \
                   testFormula.h testFormula.cc \
                   testTestCase.h testTestCase.cc \
                   cmdline.c cmdline.h \
                   verbose.h verbose.cc \
                   Output.h Output.cc


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
