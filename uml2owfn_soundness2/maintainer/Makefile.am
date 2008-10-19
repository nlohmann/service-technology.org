# EXTRA_DIST = 
# MAINTAINERCLEANFILES = 

# .PHONY: debian/changelog debian/README.Debian

#############################################################################
# variables
#############################################################################

# the name of the tarball (generic)
tarballname = $(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.gz

# the username at Gna!
gnauser = fahland


#############################################################################
# create and a checked tarball and copy it into the maintainer directory
#############################################################################
tarballname:
	$(MAKE) $(AM_MAKEFLAGS) distcheck -C $(top_srcdir)
	cp $(top_srcdir)/$(tarballname) .


#############################################################################
# upload distribution to gna.org
#############################################################################
upload: tarballname
	$(MAKE) $(AM_MAKEFLAGS) $(top_srcdir)/ChangeLog -C $(top_srcdir)/doc
	$(MAKE) $(AM_MAKEFLAGS) pdf -C $(top_srcdir)
	gpg --detach $(tarballname)
	scp -C $(tarballname) $(tarballname).sig $(top_srcdir)/ChangeLog $(top_srcdir)/doc/$(PACKAGE_TARNAME).pdf $(gnauser)@download.gna.org:/upload/service-tech/$(PACKAGE_TARNAME)

#############################################################################
# organize cleaning
#############################################################################
CLEANFILES = -fr $(PACKAGE_TARNAME)-$(PACKAGE_VERSION) \
	     $(tarballname) $(tarballname).sig ChangeLog $(PACKAGE).pdf \
	     $(PACKAGE_TARNAME)_$(PACKAGE_VERSION)* \
	     $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)*
