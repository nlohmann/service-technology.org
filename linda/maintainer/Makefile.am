
#############################################################################
# variables
#############################################################################

# the name of the tarball (generic)
tarballname = $(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.gz

# the username at Gna!
gnauser = suermeli


#############################################################################
# create and a checked tarball and copy it into the maintainer directory
#############################################################################
tarballname:
	$(MAKE) $(AM_MAKEFLAGS) distcheck -C $(top_srcdir)
	cp $(top_srcdir)/$(tarballname) .




#############################################################################
# upload distribution to gna.org and set symbolic link to current version
#############################################################################
upload: tarballname
	$(MAKE) $(AM_MAKEFLAGS) $(top_srcdir)/ChangeLog -C $(top_srcdir)/doc
	$(MAKE) $(AM_MAKEFLAGS) pdf -C $(top_srcdir)
	- gpg --detach $(tarballname)
	scp -C $(tarballname) $(tarballname).sig $(top_srcdir)/ChangeLog $(top_srcdir)/doc/$(PACKAGE_TARNAME).pdf $(gnauser)@download.gna.org:/upload/service-tech/$(PACKAGE_TARNAME)
    echo "cd /upload/service-tech/$(PACKAGE_TARNAME)" > sftp-script
    echo "-rm $(PACKAGE_TARNAME).tar.gz" >> sftp-script
    echo "ln $(tarballname) $(PACKAGE_TARNAME).tar.gz" >> sftp-script
    echo "exit" >> sftp-script
    sftp -b sftp-script $(gnauser)@download.gna.org


#############################################################################
# organize cleaning
#############################################################################
CLEANFILES = -fr $(PACKAGE_TARNAME)-$(PACKAGE_VERSION) \
	     $(tarballname) $(tarballname).sig ChangeLog $(PACKAGE).pdf \
	     $(PACKAGE_TARNAME)_$(PARACKE_VERSION)* *.deb *.rpm \
	     $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)* sftp-script


#############################################################################
# binary builds
#############################################################################

bin:
	$(MAKE) bin-`uname` TARGETS="all"
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

bin-CYGWIN_NT-5.1:
	-$(MAKE) bin-generic TARGETS="$(TARGETS)" ARCH="win32" EXTRA_CFLAGS="-mno-cygwin -U _WIN32"

bin-Darwin:
	-$(MAKE) bin-generic TARGETS="$(TARGETS)" ARCH="mac-universal" EXTRA_CONFFLAGS="--disable-dependency-tracking" EXTRA_CFLAGS="-isysroot /Developer/SDKs/MacOSX10.5.sdk -arch ppc -arch i386"
	-$(MAKE) bin-generic TARGETS="$(TARGETS)" ARCH="mac-universal-64" EXTRA_CONFFLAGS="--disable-dependency-tracking" EXTRA_CFLAGS="-isysroot /Developer/SDKs/MacOSX10.5.sdk -arch ppc -arch i386 -m64"

bin-generic:
	rm -fr local
	mkdir local
	$(MAKE) --directory=$(top_srcdir) distclean
	cd $(top_srcdir); ./configure --disable-assert --prefix=$(abs_srcdir)/local --docdir=$(abs_srcdir)/local/doc sysconfdir=$(abs_srcdir)/local/bin $(EXTRA_CONFFLAGS)
	$(MAKE) --directory=$(top_srcdir) $(TARGETS) install CXXFLAGS="-O3 $(EXTRA_CFLAGS)" CFLAGS="-O3 $(EXTRA_CFLAGS)" LDFLAGS="-O3 $(EXTRA_CFLAGS)"
	$(MAKE) --directory=$(top_srcdir) install-html install-pdf AM_MAKEINFOHTMLFLAGS="--no-split"
	rm -fr local/share
	-cp $(top_srcdir)/README $(top_srcdir)/COPYING $(top_srcdir)/REQUIREMENTS $(top_srcdir)/AUTHORS $(top_srcdir)/NEWS local
	cd local ; zip -r $(abs_top_srcdir)/$(PACKAGE)-$(PACKAGE_VERSION)-$(ARCH).zip *
