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
# upload distribution to gna.org
#############################################################################
upload: tarballname
	$(MAKE) $(AM_MAKEFLAGS) $(top_srcdir)/ChangeLog -C $(top_srcdir)/doc
	$(MAKE) $(AM_MAKEFLAGS) pdf -C $(top_srcdir)
	gpg --detach $(tarballname)
	scp -C $(tarballname) $(tarballname).sig $(top_srcdir)/ChangeLog $(top_srcdir)/doc/$(PACKAGE_TARNAME).pdf $(gnauser)@download.gna.org:/upload/service-tech/$(PACKAGE_TARNAME)


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
	     $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)*
