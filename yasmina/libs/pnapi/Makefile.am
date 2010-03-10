##################################
# This file has been generated   #
# automaticall by compiling      #
# Petri Net API 3.01-unreleased #
#                                #
# You MUST NOT change this file. #
##################################

noinst_LIBRARIES = libpnapi.a

# [CUSTOMER_B_SRC] this line is needed to build a customer Makefile. Do not remove!
libpnapi_a_SOURCES = \
    parser-lola.yy    parser-lola-lexer.ll \
    parser-onwd.yy    parser-onwd-lexer.ll \
    parser-owfn.yy    parser-owfn-lexer.ll \
    parser-pn.yy      parser-pn-lexer.ll \
    parser-pnml.yy    parser-pnml-lexer.ll \
    parser-sa.yy      parser-sa-lexer.ll \
    pnapi.h pnapi.cc \
    Output.h Output.cc \
    verbose.h verbose.cc \
    automaton.h automaton.cc\
    component.h component.cc \
    condition.h condition.cc \
    errorhandling.cc \
    formula.h formula.cc \
    interface.h interface.cc \
    link.h link.cc \
    marking.h marking.cc \
    myio.h myio.cc io-format.cc \
    parser.h parser.cc \
    petrinet-pn.cc \
    petrinet-reduction.cc \
    petrinet.h petrinet.cc \
    port.h port.cc \
    state.h state.cc \
    util.h
# [CUSTOMER_E_SRC] this line is needed to build a customer Makefile. Do not remove!

# to find customer's config.h header
libpnapi_a_CPPFLAGS = -I$(top_srcdir)/src


EXTRA_DIST =
BUILT_SOURCES =
# [CUSTOMER_B_GEN] this line is needed to build a customer Makefile. Do not remove!
#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

#---------------------------------------------------#
# GENERIC: initialization for other generic targets #
#---------------------------------------------------#
MAINTAINERCLEANFILES =
DISTCLEANFILES = 

#--------------------------------------------------------------#
# GENERIC: organize distribution and cleaning of Bison sources #
#--------------------------------------------------------------#
BISON_FILES = $(filter %.yy,$(libpnapi_a_SOURCES))
BUILT_SOURCES += $(BISON_FILES:.yy=.cc)
EXTRA_DIST += $(BISON_FILES:.yy=.h)
MAINTAINERCLEANFILES += $(BISON_FILES:.yy=.h)
# [CUSTOMER_E_GEN] this line is needed to build a customer Makefile. Do not remove!
