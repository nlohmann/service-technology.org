SUBDIRS = src man doc tests exp maintainer

#############################################################################
# remove everything that cannot be built
#############################################################################
cvs-clean: maintainer-clean
	rm -fr COPYING INSTALL Makefile.in aclocal.m4 compile configure depcomp \
	  install-sh missing ylwrap $(DIST_ARCHIVES).sig config.sub config.guess \
	  config.h.in~
	rm -fr doc/Makefile.in doc/mdate-sh doc/manual
	rm -fr man/Makefile.in
	rm -fr src/Makefile.in src/config.h.in config.h.in~ src/doc src/.deps
	rm -fr tests/Makefile.in
	rm -fr exp/Makefile.in exp/bpm2008
	rm -fr maintainer/Makefile.in maintainer/debian/changelog maintainer/debian/README.Debian
	@echo "-----------------------------------------------------"
	@echo "To recreate installation status enter"
	@echo "  aclocal"
	@echo "  autoconf"
	@echo "  autoheader"
	@echo "  automake -ac"
	@echo
	@echo "or"
	@echo "  autoreconf -i"
	@echo


#############################################################################
# create a binary distribution
#############################################################################
bindist:
	$(MAKE) $(AM_MAKEFLAGS)
	mkdir bin
	cp src/$(PACKAGE_TARNAME) bin
	wget http://download.gna.org/service-tech/$(PACKAGE_TARNAME)/$(PACKAGE_TARNAME).pdf
	mv $(PACKAGE_TARNAME).pdf bin
	cp COPYING bin
	file bin/* > CONTENT
	mv CONTENT bin
	zip -r $(distdir)-$(build_cpu)-$(build_os).zip bin
	rm -fr bin


#############################################################################
# run experiments
#############################################################################
experiments:
	$(MAKE) $(AM_MAKEFLAGS) experiments -C exp
