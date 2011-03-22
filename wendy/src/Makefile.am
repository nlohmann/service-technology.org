# the main program
bin_PROGRAMS = wendy

# the plain sources you need to compile (no generated code)
wendy_SOURCES = AnnotationLivelockOG.cc AnnotationLivelockOG.h \
                Clause.cc Clause.h \
                cmdline.c cmdline.h \
                CompositeMarking.cc CompositeMarking.h \
                Cover.h Cover.cc \
                Diagnosis.h Diagnosis.cc \
                InnerMarking.h InnerMarking.cc \
                InterfaceMarking.h InterfaceMarking.cc \
                Knowledge.h Knowledge.cc \
                Label.h Label.cc \
                LivelockOperatingGuideline.cc LivelockOperatingGuideline.h \
                main.cc \
                Output.cc Output.h \
                PossibleSendEvents.h PossibleSendEvents.cc \
                Queue.h Queue.cc \
                InternalReduction.h InternalReduction.cc \
                Results.h Results.cc \
                SCSHandler.cc SCSHandler.h \
                StoredKnowledge.h StoredKnowledge.cc \
                syntax_cover.yy lexic_cover.ll \
                syntax_graph.yy lexic_graph.ll \
                syntax_reducedgraph.yy lexic_reducedgraph.ll \
                types.h \
                util.h \
                verbose.cc verbose.h

# the configuration files
sysconf_DATA = wendy.conf

# pass the sysconfdir to Wendy (ugly, but see http://www.gnu.org/software/autoconf/manual/autoconf.html#Defining-Directories)
wendy_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\"

# only add the local Petri net API if necessary
if COMPILE_PNAPI
wendy_CPPFLAGS += -I$(top_srcdir)/libs -I$(top_srcdir)/libs/pnapi
wendy_LDADD = $(top_builddir)/libs/pnapi/libpnapi.a
endif


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
