SUBDIRS = unfolder

bin_PROGRAMS = petri sa2owfn sa2sm cdnf generator cphils configurator

petri_SOURCES = petri.cc cmdline.c cmdline.h \
                Output.h Output.cc \
                verbose.h verbose.cc

generator_SOURCES = generator.cc cmdline.ggo

sa2owfn_SOURCES = sa2owfn.cc

sa2sm_SOURCES = sa2sm.cc

cdnf_SOURCES = cdnf.cc cdnf-cmdline.c cdnf-cmdline.h \
               Output.h Output.cc \
               verbose.h cdnf-verbose.cc \
               cdnf-lexic.ll cdnf-syntax.yy

cphils_SOURCES = cphils.c

configurator_SOURCES = configurator.cc \
                       configurator-cmdline.c configurator-cmdline.h \
                       Output.h Output.cc \
                       verbose.h verbose.cc

petri_LDADD =  $(top_builddir)/src/libpnapi.la
generator_LDADD =  $(top_builddir)/src/libpnapi.la
sa2owfn_LDADD =  $(top_builddir)/src/libpnapi.la
sa2sm_LDADD =  $(top_builddir)/src/libpnapi.la
cdnf_LDADD =  $(top_builddir)/src/libpnapi.la
configurator_LDADD =  $(top_builddir)/src/libpnapi.la

AM_CPPFLAGS = -I$(top_srcdir)/src

GGOFILES = cmdline.ggo cdnf-cmdline.ggo configurator-cmdline.ggo
BUILT_SOURCES = $(GGOFILES:.ggo=.c)
EXTRA_DIST = $(GGOFILES) make_customer.sh
DISTCLEANFILES = $(GGOFILES:.ggo=.c) $(GGOFILES:.ggo=.h)

################################################################
# use GNU gengetopt to create command line parser
################################################################
%.c: %.ggo
	$(AM_V_GEN)$(GENGETOPT) --input=$(srcdir)/$< --set-version=$(VERSION) --file-name=$(@:.c=)


#---------------------------------------------------#
# GENERIC: initialization for other generic targets #
#---------------------------------------------------#
MAINTAINERCLEANFILES =


#--------------------------------------------------------------#
# GENERIC: organize distribution and cleaning of Bison sources #
#--------------------------------------------------------------#
BISON_FILES = $(filter %.yy,$(cdnf_SOURCES))
BUILT_SOURCES += $(BISON_FILES:.yy=.cc)
EXTRA_DIST += $(BISON_FILES:.yy=.h)
MAINTAINERCLEANFILES += $(BISON_FILES:.yy=.h)

