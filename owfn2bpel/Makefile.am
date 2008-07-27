SUBDIRS = src man doc tests

#############################################################################
# upload distribution to gna.org
#############################################################################
upload: distcheck pdf
	make ../ChangeLog -C doc
	gpg --detach $(DIST_ARCHIVES)
	scp $(DIST_ARCHIVES) $(DIST_ARCHIVES).sig ChangeLog doc/$(PACKAGE).pdf niels@download.gna.org:/upload/service-tech/$(PACKAGE_TARNAME)


#############################################################################
# remove everything that cannot be built
#############################################################################
cvs-clean: maintainer-clean
	rm -fr .DS_Store INSTALL Makefile.in aclocal.m4 compile configure depcomp \
	  install-sh missing ylwrap $(DIST_ARCHIVES).sig config.sub config.guess \
	  config.h.in~
	rm -fr doc/.DS_Store doc/Makefile.in doc/mdate-sh doc/figs/.DS_Store doc/manual
	rm -fr man/Makefile.in
	rm -fr src/.DS_Store src/Makefile.in src/config.h.in src/doc
	rm -fr tests/Makefile.in tests/.DS_Store
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
bin-dist:
	make
	make check > CHECKED
	-mkdir bin
	cp src/$(PACKAGE_TARNAME) bin
	wget http://download.gna.org/service-tech/$(PACKAGE_TARNAME)/$(PACKAGE_TARNAME).pdf
	mv $(PACKAGE_TARNAME).pdf bin
	cp COPYING bin
	mv CHECKED bin
	file bin/* > CONTENT
	mv CONTENT bin
	zip -r $(distdir)-$(build_cpu)-$(build_os).zip bin
	rm -fr bin
