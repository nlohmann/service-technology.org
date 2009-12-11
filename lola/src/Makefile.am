.SUFFIXES = .ggo

# not bin_PROGRAMS as installation is handled manually
noinst_PROGRAMS = lola

#######################
# the list of sources #
#######################

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
               sweep.cc sweep.H \
               structure.H \
               userconfig.H 

lola_EXTRASOURCES = ltl.H ltl.cc

EXTRA_DIST = readnet-syntax.h cmdline.ggo \
             configs/userconfig.H.boundednet \
             configs/userconfig.H.bpm-liveprop \
             configs/userconfig.H.bpm-liveprop1 \
             configs/userconfig.H.bpm-modelchecking \
             configs/userconfig.H.bpm-modelchecking1 \
             configs/userconfig.H.bpm-statepredicate \
             configs/userconfig.H.bpm-statepredicate1 \
             configs/userconfig.H.deadlock \
             configs/userconfig.H.deadlock1 \
             configs/userconfig.H.full \
             configs/userconfig.H.full1 \
             configs/userconfig.H.fulllimited \
             configs/userconfig.H.liveprop \
             configs/userconfig.H.liveprop1 \
             configs/userconfig.H.modelchecking \
             configs/userconfig.H.none \
             configs/userconfig.H.statepredicate \
             configs/userconfig.H.statepredicate1 \
             configs/userconfig.H.statespace 


################################################################
# use GNU gengetopt to create command line parser
################################################################
.ggo.c:
	$(AM_V_GEN)$(GENGETOPT) --input=$< -F $(@:.c=)


#############################################################################
# Create a target "lola-xxx" for each "userconfig.H.xxx" file located in the
# "configs" directory. The created binaries are cleaned with "make clean".
#############################################################################

CONFIGS = $(patsubst $(srcdir)/configs/userconfig.H.%,lola-%,$(wildcard $(srcdir)/configs/userconfig.H.*))

$(CONFIGS): $@
	$(AM_V_at)$(MAKE) -s clean-am
	$(AM_V_at)chmod u+w $(srcdir)/userconfig.H
	$(AM_V_at)cp $(srcdir)/userconfig.H $(builddir)/userconfig.H~
	$(AM_V_at)cp $(srcdir)/configs/userconfig.H.$(patsubst lola-%,%,$@) $(srcdir)/userconfig.H
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) CPPFLAGS=-DSTANDARDCONFIG all-am
	$(AM_V_GEN)mv $(builddir)/lola$(EXEEXT) $(builddir)/$@$(EXEEXT)
	$(AM_V_at)cp $(builddir)/userconfig.H~ $(srcdir)/userconfig.H
	$(AM_V_at)chmod u+w $(builddir)/userconfig.H~
	$(AM_V_at)rm $(builddir)/userconfig.H~

MAINTAINERCLEANFILES = $(CONFIGS) cmdline.c cmdline.h
DISTCLEANFILES = $(CONFIGS)$(EXEEXT)


#############################################################################
# "make install" installs all binaries called "lola" or "lola-xxx" (for a
# configuration "xxx") into the configured destdir (usually "/usr/local/bin").
#############################################################################

install-am:
	list='$(CONFIGS) lola'; for binary in $$list; do \
	  if test -f "$$binary"; then \
	    $(install_sh_PROGRAM) $$binary $(DESTDIR)$(bindir)/$$binary; \
	  fi \
	done;

uninstall-hook:
	list='$(CONFIGS) lola'; for binary in $$list; do \
	  rm -f $(DESTDIR)$(bindir)/$$binary; \
	done;
