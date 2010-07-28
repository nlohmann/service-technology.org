bin_PROGRAMS = petri cdnf generator cphils configurator artifact
dist_noinst_SCRIPTS = process.sh make_customer.sh

noinst_PROGRAMS = sound

petri_SOURCES = petri.cc cmdline.c cmdline.h \
                Output.h Output.cc \
                verbose.h verbose.cc

generator_SOURCES = generator.cc cmdline.ggo

cdnf_SOURCES = cdnf.cc cdnf-cmdline.c cdnf-cmdline.h \
               Output.h Output.cc \
               verbose.h cdnf-verbose.cc \
               cdnf-lexic.ll cdnf-syntax.yy

cphils_SOURCES = cphils.c

configurator_SOURCES = configurator.cc \
                       configurator-cmdline.c configurator-cmdline.h \
                       Output.h Output.cc \
                       verbose.h verbose.cc

sound_SOURCES = sound.cc

artifact_SOURCES = artifact.cc \
                   artifact-cmdline.c artifact-cmdline.h \
                   Output.h Output.cc \
                   verbose.h verbose.cc


# the configuration files
sysconf_DATA = petri.conf

# pass the sysconfdir to Petri (ugly, but see http://www.gnu.org/software/autoconf/manual/autoconf.html#Defining-Directories)
petri_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\" -I$(top_srcdir)/src

petri_LDADD =  $(top_builddir)/src/libpnapi.la
generator_LDADD =  $(top_builddir)/src/libpnapi.la
cdnf_LDADD =  $(top_builddir)/src/libpnapi.la
configurator_LDADD =  $(top_builddir)/src/libpnapi.la
sound_LDADD =  $(top_builddir)/src/libpnapi.la
artifact_LDADD =  $(top_builddir)/src/libpnapi.la

AM_CPPFLAGS = -I$(top_srcdir)/src

GGOFILES = cmdline.ggo cdnf-cmdline.ggo configurator-cmdline.ggo artifact-cmdline.ggo
BUILT_SOURCES = $(GGOFILES:.ggo=.c)
EXTRA_DIST = $(GGOFILES)
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

