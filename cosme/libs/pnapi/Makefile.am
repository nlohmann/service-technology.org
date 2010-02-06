noinst_LIBRARIES = libpnapi.a

libpnapi_a_SOURCES = \
    parser-owfn.yy    parser-owfn-lexer.ll \
    parser-pnml.yy    parser-pnml-lexer.ll \
    parser-pn.yy parser-pn-lexer.ll \
    parser-onwd.yy    parser-onwd-lexer.ll \
    parser-lola.yy    parser-lola-lexer.ll \
    parser-sa.yy      parser-sa-lexer.ll \
    pnapi.h \
    link.h link.cc \
    component.h component.cc \
    petrinet.h petrinet.cc \
    parser.h parser.cc \
    util.h \
    formula.h formula.cc \
    condition.h condition.cc \
    petrinet-reduction.cc \
    petrinet-pn.cc \
    automaton.h automaton.cc\
    state.h state.cc \
    marking.h marking.cc \
    myio.h myio.cc io-format.cc \
    errorhandling.cc \
    Output.h Output.cc \
    interface.h interface.cc \
    port.h port.cc

# to find customer's config.h header
libpnapi_a_CPPFLAGS = -I$(top_srcdir)/src


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

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
BISON_FILES = $(filter %.yy,$(libpnapi_a_SOURCES))
BUILT_SOURCES += $(BISON_FILES:.yy=.cc)
EXTRA_DIST += $(BISON_FILES:.yy=.h)
MAINTAINERCLEANFILES += $(BISON_FILES:.yy=.h)
# DISTCLEANFILES += $(BUILT_SOURCES) $(EXTRA_DIST)
