SUBDIRS = src utils doc man maintainer tests

##############################################################################
# remove everything that can be rebuilt
##############################################################################
svn-clean: maintainer-clean
	rm -fr configure Makefile.in depcomp compile config.guess config.sub \
	  INSTALL missing aclocal.m4 install-sh ylwrap utils/Makefile.in \
	  tests/Makefile.in doc/Makefile.in doc/mdate-sh src/Makefile.in \
	  src/config.h.in src/readnet-syntax.h maintainer/Makefile.in \
	  man/Makefile.in
	@echo "-----------------------------------------------------"
	@echo "To recreate installation status enter"
	@echo "  autoreconf -i"
	@echo


##############################################################################
# redirect calls to make preconfigured LoLAs (e.g. lola-deadlock)
##############################################################################
CONFIGS = $(patsubst $(top_srcdir)/src/configs/userconfig.H.%,lola-%,$(wildcard $(top_srcdir)/src/configs/userconfig.H.*))

all-configs: $(CONFIGS)

$(CONFIGS):
	$(MAKE) -C $(top_builddir)/src $(AM_MAKEFLAGS) $@
