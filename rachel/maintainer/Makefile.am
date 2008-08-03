EXTRA_DIST = debian/compat \
	     debian/control \
	     debian/copyright \
	     debian/dirs \
	     debian/docs \
	     debian/info \
	     debian/README.Debian \
	     debian/rules


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
	$(MAKE) distcheck -C $(top_srcdir)
	cp $(top_srcdir)/$(tarballname) .


#############################################################################
# upload distribution to gna.org
#############################################################################
upload: tarballname
	$(MAKE) $(top_srcdir)/ChangeLog -C $(top_srcdir)/doc
	$(MAKE) pdf -C $(top_srcdir)
	gpg --detach $(tarballname)
	scp -C $(tarballname) $(tarballname).sig $(top_srcdir)/ChangeLog $(top_srcdir)/doc/$(PACKAGE_TARNAME).pdf $(gnauser)@download.gna.org:/upload/service-tech/$(PACKAGE_TARNAME)


#############################################################################
# create Debian package (source and binary) and RPM package (binary)
#############################################################################
deb: tarballname
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
	     $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)*
