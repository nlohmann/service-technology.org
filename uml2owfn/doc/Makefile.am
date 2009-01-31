info_TEXINFOS = uml2owfn.texi

uml2owfn_TEXINFOS = fdl.texi cmdline.out ChangeLog.texi

TEXINFO_TEX = texinfo.tex

EXTRA_DIST = gendocs.sh gendocs_template

MAINTAINERCLEANFILES = cmdline.out

all: ../ChangeLog

#############################################################################
# skip texi2dvi and dvips (avoid bug under cygwin)
#############################################################################
#TEXI2DVI = echo
#DVIPS = echo

#############################################################################
# run "uml2owfn --help" to include output in documentation
#############################################################################
cmdline.out: $(top_srcdir)/src/options.cc $(top_srcdir)/src/options.h $(top_srcdir)/configure.ac
	$(top_srcdir)/src/uml2owfn --help > cmdline.out

#############################################################################
# create ASCII ChangeLog/NEWS out of Texinfo (8 is height of header)
#
# The first SED command locates the ChangeLog part in the manual by finding
# the text the directly precedes and directly follows the changelog contents.
# Adjust these framing text passages to your needs.
#############################################################################
../ChangeLog: ChangeLog.texi
	$(MAKEINFO) -o ChangeLog.txt --no-split --no-headers --fill-column=81 uml2owfn.texi
	$(SED) -n '/as they have been included in this release./,/The most recent change log/w ChangeLog2.txt' ChangeLog.txt
	$(SED) '1,2d' ChangeLog2.txt | $(SED) -e '$$d' | $(SED) -e '$$d' | $(SED) 's/^   //' > ChangeLog.txt
	echo rm ChangeLog2.txt
	head -n8 ../README > ../ChangeLog
	cat ChangeLog.txt >> ../ChangeLog
	head -n8 ../README > ../NEWS
	head -n `($(GREP) -n -m2 "^Version " ChangeLog.txt | $(SED) -n '2p' | $(SED) -e 's/:/ /' | $(AWK) '{print $$1-1}')` ChangeLog.txt >> ../NEWS
	echo " " >> ../NEWS
	echo "See file ChangeLog <http://service-technology.org/files/$(PACKAGE)/ChangeLog>" >> ../NEWS
	echo "for complete version history." >> ../NEWS
	echo " " >> ../NEWS
	rm ChangeLog.txt


#############################################################################
# run the "gendocs.sh" script to create a manual in several formats
#############################################################################
manual: $(info_TEXINFOS)
	./gendocs.sh $(PACKAGE) "$(PACKAGE_NAME)"
	