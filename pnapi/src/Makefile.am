lib_LTLIBRARIES = libpnapi.la

# [CUSTOMER_B_SRC] this line is needed to build a customer Makefile. Do not remove!
libpnapi_la_SOURCES = \
    parser-lola.yy    parser-lola-lexer.ll \
    parser-owfn.yy    parser-owfn-lexer.ll \
    parser-pn.yy      parser-pn-lexer.ll \
    parser-pnml.yy    parser-pnml-lexer.ll \
    parser-sa.yy      parser-sa-lexer.ll \
    pnapi.h pnapi.cc \
    automaton.h automaton.cc\
    component.h component.cc \
    condition.h condition.cc \
    exception.h exception.cc \
    formula.h formula.cc \
    interface.h interface.cc \
    marking.h marking.cc \
    myio.h myio.cc io-format.cc \
    Output.h Output.cc \
    parser.h parser.cc \
    petrinet-pn.cc \
    petrinet-reduction.cc \
    petrinet.h petrinet.cc \
    port.h port.cc \
    state.h state.cc \
    util.h \
    verbose.h verbose.cc
# [CUSTOMER_E_SRC] this line is needed to build a customer Makefile. Do not remove!


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
VERINFO = -version-info 4:1:0

libpnapi_la_LDFLAGS = $(VERINFO) -no-undefined
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libpnapi.pc
includedir = @includedir@/pnapi

include_HEADERS = $(filter %.h,$(libpnapi_la_SOURCES))

EXTRA_DIST = Makefile.am.customer
BUILT_SOURCES = Makefile.am.customer

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
BISON_FILES = $(filter %.yy,$(libpnapi_la_SOURCES))
BUILT_SOURCES += $(BISON_FILES:.yy=.cc)
EXTRA_DIST += $(BISON_FILES:.yy=.h)
MAINTAINERCLEANFILES += $(BISON_FILES:.yy=.h)
# [CUSTOMER_E_GEN] this line is needed to build a customer Makefile. Do not remove!

#----------------------------------------#
# GENERIC: updating Makefile.am.customer #
#----------------------------------------#
Makefile.am.customer: Makefile.am
	@$(top_srcdir)/utils/make_customer.sh $@ $<

