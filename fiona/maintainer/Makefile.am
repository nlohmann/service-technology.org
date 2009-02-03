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
# organize cleaning
#############################################################################
CLEANFILES = -fr $(PACKAGE_TARNAME)-$(PACKAGE_VERSION) \
	     $(tarballname) $(tarballname).sig ChangeLog $(PACKAGE).pdf \
	     $(PACKAGE_TARNAME)_$(PACKAGE_VERSION)* \
	     $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)* sftp-script


#############################################################################
# binary builds (outcommented --disable-assert, see task #6451)
#############################################################################

bindist-linux:
	touch $(top_srcdir)/dot ; chmod a+x $(top_srcdir)/dot
	touch $(top_srcdir)/petrify ; chmod a+x $(top_srcdir)/petrify
	cd $(top_srcdir); PATH=$$PATH:. CC=$(CC) CXX=$(CXX) ./configure --disable-assert
	$(MAKE) bindist BINDIRNAME=$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-linux-x86
	cd $(top_srcdir); PATH=$$PATH:. CC=$(CC) CXX=$(CXX) ./configure --enable-64bit --disable-assert
	$(MAKE) bindist BINDIRNAME=$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-linux-x86-64
	rm $(top_srcdir)/dot $(top_srcdir)/petrify

bindist-solaris:
	touch $(top_srcdir)/dot ; chmod a+x $(top_srcdir)/dot
	touch $(top_srcdir)/petrify ; chmod a+x $(top_srcdir)/petrify
	cd $(top_srcdir); PATH=$$PATH:. CC=$(CC) CXX=$(CXX) ./configure --disable-assert
	$(MAKE) bindist BINDIRNAME=$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-solaris-sparc
	cd $(top_srcdir); PATH=$$PATH:. CC=$(CC) CXX=$(CXX) ./configure --enable-64bit --disable-assert
	$(MAKE) bindist BINDIRNAME=$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-solaris-sparc-64
	rm $(top_srcdir)/dot $(top_srcdir)/petrify

bindist-mac:
	cd $(top_srcdir); CC=$(CC) CXX=$(CXX) ./configure --enable-universal --disable-dependency-tracking --disable-assert
	$(MAKE) bindist BINDIRNAME=$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-mac-universal
	cd $(top_srcdir); CC=$(CC) CXX=$(CXX) ./configure --enable-universal --enable-64bit --disable-dependency-tracking --disable-assert
	$(MAKE) bindist BINDIRNAME=$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-mac-universal-64

bindist-win:
	touch $(top_srcdir)/dot ; chmod a+x $(top_srcdir)/dot
	touch $(top_srcdir)/petrify-4.1 ; chmod a+x $(top_srcdir)/petrify-4.1
	cd $(top_srcdir); PATH=$$PATH:. CC=$(CC) CXX=$(CXX) ./configure --enable-win32 --disable-assert
	$(MAKE) bindist BINDIRNAME=$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-win32	
	rm $(top_srcdir)/dot $(top_srcdir)/petrify-4.1

bindist:
	$(MAKE) --directory=$(top_srcdir) CXXFLAGS=-O3 CFLAGS=-O3 LDFLAGS=-O3 clean all
	wget http://download.gna.org/service-tech/$(PACKAGE)/$(PACKAGE).pdf -q
	mkdir $(top_srcdir)/$(BINDIRNAME)
	cd $(top_srcdir); cp src/$(PACKAGE) COPYING NEWS ChangeLog AUTHORS THANKS README maintainer/$(PACKAGE).pdf $(BINDIRNAME)
	cd $(top_srcdir)/$(BINDIRNAME) ; file * > CONTENTS
	tar cfz $(top_srcdir)/$(BINDIRNAME).tar.gz $(top_srcdir)/$(BINDIRNAME)
	rm -fr $(top_srcdir)/$(BINDIRNAME)
