SUBDIRS = libs/pnapi libs/minisat src tests man doc maintainer

# tell aclocal about the macros in the "m4" folder
ACLOCAL_AMFLAGS = -I m4 --install

#############################################################################
# remove everything that cannot be built
#############################################################################
svn-clean: maintainer-clean
	rm -fr configure Makefile.in depcomp compile INSTALL missing aclocal.m4 install-sh \
	ylwrap tests/Makefile.in src/Makefile.in src/config.h.in man/Makefile.in doc/Makefile.in \
	doc/mdate-sh libs/pnapi/Makefile.in libs/minisat/Makefile.in maintainer/Makefile.in build-aux \
	src/config.h.in src/config-log.h m4/libtool.m4 m4/lt~obsolete.m4 m4/ltoptions.m4 m4/ltsugar.m4 m4/ltversion.m4 \
		 config.guess ltmain.sh config.sub  
	@echo "-----------------------------------------------------"
	@echo "To recreate installation status enter"
	@echo "  autoreconf -i"
	@echo