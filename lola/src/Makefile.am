# the main program - not bin_PROGRAMS as installation is handled manually
noinst_PROGRAMS = lola

# the plain sources you need to compile (no generated code)
lola_SOURCES = readnet-syntax.yy \
               readnet-lexic.ll \
               cmdline.c cmdline.h \
               net.cc net.h Node.h Place.h Place.cc Transition.h Transition.cc Arc.cc Arc.h \
               Globals.h Globals.cc \
               buchi.cc buchi.h \
               formula.cc formula.h \
               stubborn.cc stubborn.h \
               graph.cc graph.h \
               symboltab.cc symboltab.h \
               symm.cc symm.h \
               check.cc check.h \
               unfold.cc unfold.h \
               dimensions.cc dimensions.h \
               path.cc path.h \
               printnet.cc printnet.h \
               tinv.cc tinv.h \
               reports.cc reports.h \
               sweep.cc sweep.h \
               structure.h \
               verbose.cc verbose.h \
               userconfig.h 

# additional source code
lola_EXTRASOURCES = ltl.h ltl.cc

## include a generic Makefile template to organize cleaning etc.
## must be included after the source listing and before any definition of
## BUILT_SOURCES, EXTRA_DIST, MAINTAINERCLEANFILES, or DISTCLEANFILES.
include $(top_srcdir)/m4/Makefile.src

# user configs
EXTRA_DIST += configs


#############################################################################
# Create a target "lola-xxx" for each "userconfig.h.xxx" file located in the
# "configs" directory. The created binaries are cleaned with "make clean".
#############################################################################

CONFIGS = $(patsubst $(srcdir)/configs/%/userconfig.h,lola-%,$(wildcard $(srcdir)/configs/*/userconfig.h))

# Include a file with directives to create a binary "lola-foo" for each
# configuration file "configs/foo/userconfig.h".
include Makefile.configs

MAINTAINERCLEANFILES += $(CONFIGS)
DISTCLEANFILES += $(CONFIGS)$(EXEEXT)


#############################################################################
# generate these binaries for later checks
#############################################################################

check_binaries = lola-deadlock lola-fulllimited lola-modelchecking \
                 lola-statepredicate1 lola-liveprop lola-boundednet \
                 lola-statespace lola-statepredicate lola-boundedplace

check-local: $(check_binaries)

CLEANFILES = $(check_binaries)


#############################################################################
# "make install" installs all binaries called "lola" or "lola-xxx" (for a
# configuration "xxx") into the configured destdir (usually "/usr/local/bin").
#############################################################################

install-am:
	list='$(CONFIGS) lola'; for binary in $$list; do \
	  if test -f "$$binary$(EXEEXT)"; then \
	    $(install_sh_PROGRAM) $$binary$(EXEEXT) $(DESTDIR)$(bindir)/`echo $$binary$(EXEEXT) | sed -e '$(program_transform_name)'`; \
	  fi \
	done;

uninstall-hook:
	list='$(CONFIGS) lola'; for binary in $$list; do \
	  rm -f $(DESTDIR)$(bindir)/`echo $$binary$(EXEEXT) | sed -e '$(program_transform_name)'`; \
	done;
