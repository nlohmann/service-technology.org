# tell aclocal about the macros in the "m4" folder
ACLOCAL_AMFLAGS = -I m4 --install

SUBDIRS = src utils man doc tests

svn-clean: maintainer-clean
	rm -fr configure INSTALL aclocal.m4 src/config.h.in doc/mdate-sh src/config-log.h build-aux
	for DIR in $(DIST_SUBDIRS) .; do rm -f $$DIR/Makefile.in; done


##############################################################################
# redirect calls to make preconfigured LoLAs (e.g. lola-deadlock)
##############################################################################
CONFIGS = $(patsubst $(top_srcdir)/src/configs/%/userconfig.H,lola-%,$(wildcard $(top_srcdir)/src/configs/*/userconfig.H))

all-configs: $(CONFIGS)

$(CONFIGS):
	$(AM_V_GEN)$(MAKE) $(AM_MAKEFLAGS) $@ --directory=$(top_builddir)/src
