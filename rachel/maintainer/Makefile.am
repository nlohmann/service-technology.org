EXTRA_DIST = debian/compat \
	     debian/control \
	     debian/copyright \
	     debian/dirs \
	     debian/docs \
	     debian/info \
	     debian/README.Debian.template \
	     debian/rules

MAINTAINERCLEANFILES = debian/README.Debian

.PHONY: debian/changelog debian/README.Debian

#############################################################################
# variables
#############################################################################

# the name of the tarball (generic)
tarballname = $(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.gz

# the username at Gna!
gnauser = niels


#############################################################################
# create and a checked tarball and copy it into the maintainer directory
#############################################################################
tarballname:
	$(MAKE) $(AM_MAKEFLAGS) distcheck -C $(top_srcdir)
	cp $(top_srcdir)/$(tarballname) .


#############################################################################
# move the "PURPOSE" section from the README file to Debian's control file
#############################################################################
debian/control: ../README
	$(SED) -n '1,/Description:/ p' debian/control > debian/control~
	cat ../README | $(SED) -n '/PURPOSE/,/COPYRIGHT/ p' | $(SED) 'N;$$!P;$$!D;$$d;$$d' | $(SED) '1d' | $(SED) 's/^  //' >> debian/control~
	mv debian/control~ debian/control


#############################################################################
# add the current date to the changelog and the README.Debian file
#############################################################################
debian/changelog: debian/changelog.template
	cp debian/changelog.template debian/changelog
	-$(SED) -i "s/XXXXXXXX/`date -R`/" debian/changelog

debian/README.Debian: debian/README.Debian.template
	cp debian/README.Debian.template debian/README.Debian
	-$(SED) -i "s/XXXXXXXX/`date -R`/" debian/README.Debian


#############################################################################
# upload distribution to gna.org and set symbolic link to current version
#############################################################################
upload: tarballname
	$(MAKE) $(AM_MAKEFLAGS) $(top_srcdir)/ChangeLog -C $(top_srcdir)/doc
	$(MAKE) $(AM_MAKEFLAGS) pdf -C $(top_srcdir)
	gpg --detach $(tarballname)
	scp -C $(tarballname) $(tarballname).sig $(top_srcdir)/ChangeLog $(top_srcdir)/doc/$(PACKAGE_TARNAME).pdf $(gnauser)@download.gna.org:/upload/service-tech/$(PACKAGE_TARNAME)
	echo "cd /upload/service-tech/$(PACKAGE_TARNAME)" > sftp-script
	echo "-rm $(PACKAGE_TARNAME).tar.gz" >> sftp-script
	echo "ln $(tarballname) $(PACKAGE_TARNAME).tar.gz" >> sftp-script
	echo "exit" >> sftp-script
	sftp -b sftp-script $(gnauser)@download.gna.org


#############################################################################
# create Debian package (source and binary) and RPM package (binary)
#############################################################################
deb: tarballname debian/control debian/changelog debian/README.Debian
	tar xfz $(tarballname)
	-cp -R debian $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)
	cd $(PACKAGE_TARNAME)-$(PACKAGE_VERSION) && dpkg-buildpackage
	cd $(PACKAGE_TARNAME)-$(PACKAGE_VERSION) && dpkg-buildpackage -S
	rm -fr $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)
	fakeroot alien --to-rpm --keep-version $(PACKAGE_TARNAME)_$(PACKAGE_VERSION)*.deb


#############################################################################
# organize cleaning
#############################################################################
CLEANFILES = -fr $(PACKAGE_TARNAME)-$(PACKAGE_VERSION) \
	     $(tarballname) $(tarballname).sig ChangeLog $(PACKAGE).pdf \
	     $(PACKAGE_TARNAME)_$(PACKAGE_VERSION)* \
	     $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)* sftp-script


#############################################################################
# binary builds
#############################################################################

bindist-linux:
	touch $(top_srcdir)/dot ; chmod a+x $(top_srcdir)/dot
	cd $(top_srcdir); PATH=$$PATH:. CC=$(CC) CXX=$(CXX) ./configure --disable-assert
	$(MAKE) bindist BINDIRNAME=$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-linux-x86
	cd $(top_srcdir); PATH=$$PATH:. CC=$(CC) CXX=$(CXX) ./configure --enable-64bit --disable-assert
	$(MAKE) bindist BINDIRNAME=$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-linux-x86-64
	rm $(top_srcdir)/dot

bindist-solaris:
	touch $(top_srcdir)/dot ; chmod a+x $(top_srcdir)/dot
	cd $(top_srcdir); PATH=$$PATH:. CC=$(CC) CXX=$(CXX) ./configure --disable-assert
	$(MAKE) bindist BINDIRNAME=$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-solaris-sparc
	cd $(top_srcdir); PATH=$$PATH:. CC=$(CC) CXX=$(CXX) ./configure --enable-64bit --disable-assert
	$(MAKE) bindist BINDIRNAME=$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-solaris-sparc-64
	rm $(top_srcdir)/dot

bindist-mac:
	cd $(top_srcdir); CC=$(CC) CXX=$(CXX) ./configure --enable-universal --disable-dependency-tracking --disable-assert
	$(MAKE) bindist BINDIRNAME=$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-mac-universal
	cd $(top_srcdir); CC=$(CC) CXX=$(CXX) ./configure --enable-universal --enable-64bit --disable-dependency-tracking --disable-assert
	$(MAKE) bindist BINDIRNAME=$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-mac-universal-64

bindist-win:
	touch $(top_srcdir)/dot ; chmod a+x $(top_srcdir)/dot
	cd $(top_srcdir); PATH=$$PATH:. CC=$(CC) CXX=$(CXX) ./configure --enable-win32 --disable-assert
	$(MAKE) bindist BINDIRNAME=$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-win32	
	rm $(top_srcdir)/dot

bindist:
	$(MAKE) --directory=$(top_srcdir) CXXFLAGS=-O3 CFLAGS=-O3 LDFLAGS=-O3 clean all
	wget http://download.gna.org/service-tech/$(PACKAGE)/$(PACKAGE).pdf -q
	mkdir $(top_srcdir)/$(BINDIRNAME)
	cd $(top_srcdir); cp src/$(PACKAGE) COPYING NEWS ChangeLog AUTHORS THANKS README maintainer/$(PACKAGE).pdf $(BINDIRNAME)
	cd $(top_srcdir)/$(BINDIRNAME) ; file * > CONTENTS
	tar cfz $(top_srcdir)/$(BINDIRNAME).tar.gz $(top_srcdir)/$(BINDIRNAME)
	rm -fr $(top_srcdir)/$(BINDIRNAME)
