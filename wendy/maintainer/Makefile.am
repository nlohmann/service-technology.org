# the username at Gna! used for "upload" target (Note: your PGP key must be
# properly set up; you must be able to execute "gpg --detach"!)
gnauser = niels

# which targets should be built for the binary releases
BINTARGETS="all"


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

# version number including svn revision (without "-unreleased")
MICROVERSION = $(subst -unreleased,,$(PACKAGE_VERSION)).$(VERSION_SVN)

#-------------------#
# organize cleaning #
#-------------------#
CLEANFILES = -fr $(PACKAGE_TARNAME)-$(PACKAGE_VERSION) \
	     $(tarballname) $(tarballname).sig ChangeLog $(PACKAGE).pdf \
	     $(PACKAGE_TARNAME)_$(PARACKE_VERSION)* *.deb *.rpm \
	     $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)* sftp-script

#------------------------------------------------------------------------#
# create and a checked tarball and copy it into the maintainer directory #
#------------------------------------------------------------------------#
tarballname = $(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.gz
tarballname:
	$(MAKE) $(AM_MAKEFLAGS) distcheck -C $(top_srcdir)
	cp $(top_srcdir)/$(tarballname) .


##############
# Gna! stuff #
##############

#-------------------------------------------------------------------------#
# upload distribution to gna.org and set symbolic link to current version #
#-------------------------------------------------------------------------#
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


################
# Debian stuff #
################
EXTRA_DIST = debian/compat debian/control debian/copyright debian/dirs debian/docs
MAINTAINERCLEANFILES = debian/changelog
.PHONY: debian/changelog

#--------------------------------------------------------------------------#
# move the "PURPOSE" section from the README file to Debian's control file #
#--------------------------------------------------------------------------#
#debian/control: ../README
#	$(SED) -n '1,/Description:/ p' debian/control > debian/control~
#	cat ../README | $(SED) -n '/PURPOSE/,/COPYRIGHT/ p' | $(SED) 'N;$$!P;$$!D;$$d;$$d' | $(SED) '1d' | $(SED) 's/^  //' >> debian/control~
#	mv debian/control~ debian/control

#------------------------------------------------------------------#
# add the current date to the changelog and the README.Debian file #
#------------------------------------------------------------------#
debian/changelog: debian/changelog.template
	cp debian/changelog.template debian/changelog
	-$(SED) -i "s/XXXXXXXX/`date -R`/" debian/changelog
	-$(SED) -i "s/YYYYYYYY/$(MICROVERSION)/" debian/changelog

#--------------------------------------------------------------------#
# create Debian package (source and binary) and RPM package (binary) #
#--------------------------------------------------------------------#
deb: tarballname debian/changelog
	tar xfz $(tarballname)
	-cp -R debian $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)
	-cd $(PACKAGE_TARNAME)-$(PACKAGE_VERSION) ; CFLAGS="-O3" CXXFLAGS="-O3" LDFLAGS="-O3" dpkg-buildpackage
	-cd $(PACKAGE_TARNAME)-$(PACKAGE_VERSION) ; CFLAGS="-O3 -m64" CXXFLAGS="-O3 -m64" LDFLAGS="-O3 -m64" dpkg-buildpackage -aamd64
	-cd $(PACKAGE_TARNAME)-$(PACKAGE_VERSION) ; dpkg-buildpackage -S
	rm -fr $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)
	fakeroot alien --to-rpm --keep-version $(PACKAGE_TARNAME)_$(PACKAGE_VERSION)*.deb

#---------------------------------------------------------#
# create Debian source package and upload it to Launchpad #
#---------------------------------------------------------#
launchpad: debian/changelog
	$(MAKE) $(AM_MAKEFLAGS) dist -C $(top_srcdir)
	mv $(top_srcdir)/$(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.gz $(top_srcdir)/$(PACKAGE_TARNAME)_$(MICROVERSION).orig.tar.gz
	debuild -S -sa
	cd $(top_srcdir) ; dput st-ppa $(PACKAGE_TARNAME)_$(MICROVERSION)-1_source.changes


#################
# binary builds #
#################
bin:
	$(MAKE) bin-`uname` TARGETS=$(BINTARGETS)
	$(MAKE) --directory=$(top_srcdir) distclean
	@echo "=============================================="
	@echo "created binary distributions:"
	@cd $(abs_top_srcdir); ls -1 $(PACKAGE)*.zip 
	@echo "=============================================="

bin-SunOS:
	-$(MAKE) bin-generic TARGETS="$(TARGETS)" ARCH="solaris-sparc"
	-$(MAKE) bin-generic TARGETS="$(TARGETS)" ARCH="solaris-sparc-64" EXTRA_CFLAGS="-m64"

bin-Linux:
	-$(MAKE) bin-generic TARGETS="$(TARGETS)" ARCH="linux-x86"
	-$(MAKE) bin-generic TARGETS="$(TARGETS)" ARCH="linux-x86-64" EXTRA_CFLAGS="-m64"

bin-CYGWIN_NT-6.1: bin-CYGWIN_NT-5.1

bin-CYGWIN_NT-6.0: bin-CYGWIN_NT-5.1

bin-CYGWIN_NT-5.1:
	-$(MAKE) bin-generic TARGETS="$(TARGETS)" ARCH="win32" EXTRA_CFLAGS="-mno-cygwin -U _WIN32"

bin-Darwin:
	-$(MAKE) bin-generic TARGETS="$(TARGETS)" ARCH="mac-universal" EXTRA_CONFFLAGS="--disable-dependency-tracking" EXTRA_CFLAGS="-isysroot /Developer/SDKs/MacOSX10.5.sdk -arch ppc -arch i386"
	-$(MAKE) bin-generic TARGETS="$(TARGETS)" ARCH="mac-universal-64" EXTRA_CONFFLAGS="--disable-dependency-tracking" EXTRA_CFLAGS="-isysroot /Developer/SDKs/MacOSX10.5.sdk -arch ppc -arch i386 -m64"

bin-generic:
	rm -fr local
	mkdir local
	$(MAKE) --directory=$(top_srcdir) distclean
	cd $(top_srcdir); ./configure --disable-assert --prefix=$(abs_srcdir)/local --docdir=$(abs_srcdir)/local/doc sysconfdir=$(abs_srcdir)/local/bin $(EXTRA_CONFFLAGS) CXXFLAGS="-O3 $(EXTRA_CFLAGS)" CFLAGS="-O3 $(EXTRA_CFLAGS)" LDFLAGS="$(EXTRA_LDFLAGS)"
	$(MAKE) --directory=$(top_srcdir) $(TARGETS) install
	$(MAKE) --directory=$(top_srcdir) install-html install-pdf AM_MAKEINFOHTMLFLAGS="--no-split"
	rm -fr local/share
	-cp $(top_srcdir)/README $(top_srcdir)/COPYING $(top_srcdir)/REQUIREMENTS $(top_srcdir)/THEORY $(top_srcdir)/AUTHORS $(top_srcdir)/NEWS local
	cd local ; zip -r $(abs_top_srcdir)/$(PACKAGE)-$(PACKAGE_VERSION)-$(ARCH).zip *
