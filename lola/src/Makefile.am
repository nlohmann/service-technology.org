# the main program - not bin_PROGRAMS as installation is handled manually
noinst_PROGRAMS = lola

# the plain sources you need to compile (no generated code)
lola_SOURCES = readnet-syntax.yy \
               readnet-lexic.ll \
               cmdline.c cmdline.h \
               net.cc net.H \
               buchi.cc buchi.H \
               formula.cc formula.H \
               stubborn.cc stubborn.H \
               graph.cc graph.H \
               symboltab.cc symboltab.H \
               symm.cc symm.H \
               check.cc check.H \
               unfold.cc unfold.H \
               dimensions.cc dimensions.H \
               path.cc path.H \
               printnet.cc printnet.H \
               tinv.cc tinv.H \
               reports.cc reports.H \
               sweep.cc sweep.H \
               structure.H \
               userconfig.H 

# additional source code
lola_EXTRASOURCES = ltl.H ltl.cc

## include a generic Makefile template to organize cleaning etc.
## must be included after the source listing and before any definition of
## BUILT_SOURCES, EXTRA_DIST, MAINTAINERCLEANFILES, or DISTCLEANFILES.
include $(top_srcdir)/m4/Makefile.src

# user configs
EXTRA_DIST += configs/boundednet/userconfig.H                    \
              configs/boundednetsymm/userconfig.H                \
              configs/boundedplace/userconfig.H                  \
              configs/bpm-liveprop/userconfig.H                  \
              configs/bpm-liveprop1/userconfig.H                 \
              configs/bpm-modelchecking/userconfig.H             \
              configs/bpm-modelchecking-naiv/userconfig.H        \
              configs/bpm-modelchecking1/userconfig.H            \
              configs/bpm-modelchecking1-naiv/userconfig.H       \
              configs/bpm-statepredicate/userconfig.H            \
              configs/bpm-statepredicate-naiv/userconfig.H       \
              configs/bpm-statepredicate1/userconfig.H           \
              configs/bpm-statepredicate1-naiv/userconfig.H      \
              configs/coverstatepredicate/userconfig.H           \
              configs/deadlock/userconfig.H                      \
              configs/deadlock1/userconfig.H                     \
              configs/deadtransition/userconfig.H                \
              configs/findpath/userconfig.H                      \
              configs/full/userconfig.H                          \
              configs/full1/userconfig.H                         \
              configs/fullcover/userconfig.H                     \
              configs/fulllimited/userconfig.H                   \
              configs/liveprop/userconfig.H                      \
              configs/liveprop1/userconfig.H                     \
              configs/modelchecking/userconfig.H                 \
              configs/none/userconfig.H                          \
              configs/prom-boundednet/userconfig.H               \
              configs/prom-deadlock/userconfig.H                 \
              configs/prom-home/userconfig.H                     \
              configs/prom-reversibility/userconfig.H            \
              configs/statepredicate/userconfig.H                \
              configs/statepredicate1/userconfig.H               \
              configs/statespace/userconfig.H                    \
              configs/statespace1/userconfig.H


#############################################################################
# Create a target "lola-xxx" for each "userconfig.H.xxx" file located in the
# "configs" directory. The created binaries are cleaned with "make clean".
#############################################################################

CONFIGS = $(patsubst $(srcdir)/configs/%/userconfig.H,lola-%,$(wildcard $(srcdir)/configs/*/userconfig.H))

# Include a file with directives to create a binary "lola-foo" for each
# configuration file "configs/foo/userconfig.H".
include Makefile.configs

MAINTAINERCLEANFILES += $(CONFIGS) cmdline.c cmdline.h
DISTCLEANFILES += $(CONFIGS)$(EXEEXT)


#############################################################################
# generate these binaries for later checks
#############################################################################

check_binaries = lola-deadlock lola-fulllimited lola-modelchecking \
                 lola-statepredicate1 lola-liveprop lola-boundednet \
                 lola-statespace

check-local: $(lola_SOURCES)
	-rm -f $(builddir)/$(check_binaries)
	$(MAKE) $(AM_MAKEFLAGS) $(check_binaries)
	$(MAKE) $(AM_MAKEFLAGS) clean


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
