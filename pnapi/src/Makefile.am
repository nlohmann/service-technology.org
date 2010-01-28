lib_LTLIBRARIES = libpnapi.la

libpnapi_la_SOURCES = \
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


# Versioning rules ( C:R:A )
#
# 1. Start with version 0:0:0.
# 2. If any of the sources have changed, increment R. This is a new revision
#    of the current interface.
# 3. If the interface has changed, increment C and set R to 0. This is the
#    first revision of a new interface.
# 4. If the new interface is a superset of the previous interface
#    (that is, if the previous interface has not been broken by the
#    changes in this new release), increment A. This release is backwards
#    compatible with the previous release.
# 5. If the new interface has removed elements with respect to the
#    previous interface, then backward compatibility is broken; set A to 0.
#    This release has a new, but backwards incompatible interface.
#
# For more info see section 6.3 of the GNU Libtool Manual.
VERINFO = -version-info 0:0:0

libpnapi_la_LDFLAGS = $(VERINFO) -no-undefined
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libpnapi.pc
includedir = @includedir@/pnapi

include_HEADERS = $(filter %.h,$(libpnapi_la_SOURCES))

EXTRA_DIST = Makefile.am.customer

#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

#---------------------------------------------------#
# GENERIC: initialization for other generic targets #
#---------------------------------------------------#
BUILT_SOURCES =
MAINTAINERCLEANFILES =
DISTCLEANFILES = 

#--------------------------------------------------------------#
# GENERIC: organize distribution and cleaning of Bison sources #
#--------------------------------------------------------------#
BISON_FILES = $(filter %.yy,$(libpnapi_la_SOURCES))
BUILT_SOURCES += $(BISON_FILES:.yy=.cc)
EXTRA_DIST += $(BISON_FILES:.yy=.h)
MAINTAINERCLEANFILES += $(BISON_FILES:.yy=.h)
