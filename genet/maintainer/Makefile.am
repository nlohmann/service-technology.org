EXTRA_DIST = debian/compat \
	     debian/control \
	     debian/copyright \
	     debian/dirs \
	     debian/docs

MAINTAINERCLEANFILES = debian/README.Debian

.PHONY: debian/changelog debian/README.Debian win64 win32


#############################################################################
# variables
#############################################################################

# the name of the tarball (generic)
tarballname = $(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.gz


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
# create Debian package (source and binary) and RPM package (binary)
#############################################################################
deb: tarballname debian/control debian/changelog debian/README.Debian
	tar xfz $(tarballname)
	-cp -R debian $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)
	chmod a+x $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)/debian/rules
	-cd $(PACKAGE_TARNAME)-$(PACKAGE_VERSION) ; CFLAGS="-O3" CXXFLAGS="-O3" LDFLAGS="-O3" dpkg-buildpackage
	-cd $(PACKAGE_TARNAME)-$(PACKAGE_VERSION) ; CFLAGS="-O3 -m64" CXXFLAGS="-O3 -m64" LDFLAGS="-O3 -m64" dpkg-buildpackage -aamd64
	-cd $(PACKAGE_TARNAME)-$(PACKAGE_VERSION) ; dpkg-buildpackage -S
	rm -fr $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)
	fakeroot alien --to-rpm --keep-version $(PACKAGE_TARNAME)_$(PACKAGE_VERSION)*.deb


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
	-$(MAKE) bin-generic TARGETS="$(TARGETS)" ARCH="solaris-`uname -p`"
#	-$(MAKE) bin-generic TARGETS="$(TARGETS)" ARCH="solaris-sparc-64" EXTRA_CFLAGS="-m64"

bin-Linux:
	-$(MAKE) bin-generic TARGETS="$(TARGETS)" ARCH="linux-`uname -p`"
#	-$(MAKE) bin-generic TARGETS="$(TARGETS)" ARCH="linux-x86-64" EXTRA_CFLAGS="-m64"

bin-CYGWIN_NT-5.1:
	-$(MAKE) bin-generic TARGETS="$(TARGETS)" ARCH="win32-`uname -m`" # EXTRA_CFLAGS="-mno-cygwin -U _WIN32"
	cp `which cygwin1.dll` local/bin
	cd local ; zip -r $(abs_top_srcdir)/$(PACKAGE)-$(PACKAGE_VERSION)-win32-`uname -m`.zip *

# Vista is not different from XP besides version number
bin-CYGWIN_NT-6.0: bin-CYGWIN_NT-5.1

bin-CYGWIN_NT-6.1: bin-CYGWIN_NT-5.1

bin-CYGWIN_NT-6.1-WOW64: bin-CYGWIN_NT-5.1

bin-Darwin:
	-$(MAKE) bin-generic TARGETS="$(TARGETS)" ARCH="mac-universal" EXTRA_CONFFLAGS="--disable-dependency-tracking" EXTRA_CFLAGS="-isysroot /Developer/SDKs/MacOSX10.5.sdk -arch ppc -arch i386"
#	-$(MAKE) bin-generic TARGETS="$(TARGETS)" ARCH="mac-universal-64" EXTRA_CONFFLAGS="--disable-dependency-tracking" EXTRA_CFLAGS="-isysroot /Developer/SDKs/MacOSX10.5.sdk -arch ppc -arch i386 -m64"

bin-generic:
	rm -fr local
	mkdir local
	$(MAKE) --directory=$(top_srcdir) distclean
	cd $(top_srcdir); CXXFLAGS="-O3 $(EXTRA_CFLAGS)" CFLAGS="-O3 $(EXTRA_CFLAGS)" LDFLAGS="$(EXTRA_LDFLAGS)" ./configure --disable-assert --prefix=$(abs_srcdir)/local --docdir=$(abs_srcdir)/local/doc sysconfdir=$(abs_srcdir)/local/bin $(EXTRA_CONFFLAGS)
	$(MAKE) --directory=$(top_srcdir) $(TARGETS) install
	$(MAKE) --directory=$(top_srcdir) install-html install-pdf AM_MAKEINFOHTMLFLAGS="--no-split"
	rm -fr local/share
	-cp $(top_srcdir)/README $(top_srcdir)/COPYING $(top_srcdir)/REQUIREMENTS $(top_srcdir)/AUTHORS $(top_srcdir)/NEWS local
	cd local ; zip -r $(abs_top_srcdir)/$(PACKAGE)-$(PACKAGE_VERSION)-$(ARCH).zip *

win64:
	$(MKDIR_P) win64
	$(MAKE) --directory=$(top_srcdir) clean
	cd .. ; ./configure --host=x86_64-w64-mingw32 CPPFLAGS="-I/usr/lib/jvm/java-6-sun-1.6.0.24/include -I/usr/lib/jvm/java-6-sun-1.6.0.24/include/linux" CXXFLAGS="-O3 -static-libgcc -static-libstdc++ -I/usr/lib/jvm/java-6-sun-1.6.0.24/include -I/usr/lib/jvm/java-6-sun-1.6.0.24/include/linux" CFLAGS="-O3 -static-libgcc -I/usr/lib/jvm/java-6-sun-1.6.0.24/include -I/usr/lib/jvm/java-6-sun-1.6.0.24/include/linux" LDFLAGS="-O3 -static-libgcc -static-libstdc++ -Wl,--allow-multiple-definition -Wl,--enable-auto-import" --prefix=$(abs_builddir)/win64
	$(MAKE) --directory=$(top_srcdir)
	$(MAKE) --directory=$(top_srcdir) install
	cd win64 ; zip -r $(abs_top_srcdir)/$(PACKAGE)-$(PACKAGE_VERSION)-win-x64.zip *
  
win32:
	$(MKDIR_P) win32
	$(MAKE) --directory=$(top_srcdir) clean
	cd .. ; ./configure --host=i586-mingw32msvc CPPFLAGS="-I/usr/lib/jvm/java-6-sun-1.6.0.24/include -I/usr/lib/jvm/java-6-sun-1.6.0.24/include/linux" CXXFLAGS="-O3 -static-libgcc -I/usr/lib/jvm/java-6-sun-1.6.0.24/include -I/usr/lib/jvm/java-6-sun-1.6.0.24/include/linux" CFLAGS="-O3 -static-libgcc -I/usr/lib/jvm/java-6-sun-1.6.0.24/include -I/usr/lib/jvm/java-6-sun-1.6.0.24/include/linux" LDFLAGS="-O3 -static-libgcc -static-libstdc++ -Wl,--allow-multiple-definition -Wl,--enable-auto-import" --prefix=$(abs_builddir)/win32
	$(MAKE) --directory=$(top_srcdir)
	$(MAKE) --directory=$(top_srcdir) install
	cd win32 ; zip -r $(abs_top_srcdir)/$(PACKAGE)-$(PACKAGE_VERSION)-win-x86.zip *
  

