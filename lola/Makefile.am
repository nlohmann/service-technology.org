SUBDIRS = src utils doc man maintainer tests

##############################################################################
# remove everything that can be rebuilt
##############################################################################
svn-clean: maintainer-clean
	-rm -f aclocal.m4 COPYING INSTALL depcomp configure Makefile.in \
	  missing ylwrap install-sh config.guess config.sub \
	  src/readnet-syntax.h src/Makefile.in src/config.H.in \
	  tests/Makefile.in doc/mdate-sh doc/Makefile.in src/.deps \
	  src/config.H.in~ maintainer/Makefile.in utils/Makefile.in \
	  utils/graph_syntax.h man/Makefile.in compile
	-rm -fr doc/doxygen doc/Doxyfile
	@echo "-----------------------------------------------------"
	@echo "To recreate installation status enter"
	@echo "  autoreconf -i"
	@echo


##############################################################################
# redirect calls to make preconfigured LoLAs (e.g. lola-deadlock)
##############################################################################
CONFIGS = $(patsubst src/configs/userconfig.H.%,lola-%,$(wildcard src/configs/userconfig.H.*))

all-configs: $(CONFIGS)

$(CONFIGS):
	cd src ; $(MAKE) $(AM_MAKEFLAGS) $@
