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

