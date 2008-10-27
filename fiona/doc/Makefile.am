info_TEXINFOS = fiona.texi

fiona_TEXINFOS = fdl.texi ChangeLog.texi \
	figs/online_shop.owfn.eps figs/online_shop.owfn.gif \
	figs/online_shop.owfn.pdf figs/online_shop.owfn.og.eps \
	figs/online_shop.owfn.og.gif figs/online_shop.owfn.og.pdf

TEXINFO_TEX = texinfo.tex

EXTRA_DIST = gendocs.sh gendocs_template\
	docShop.owfn docShop.owfn.og fionaDocumentation.pdf

all: ../ChangeLog


#############################################################################
# create ASCII ChangeLog/NEWS out of Texinfo (8 is height of header)
#############################################################################
../ChangeLog: ChangeLog.texi
	$(MAKEINFO) -o ChangeLog.txt --no-split --no-headers --fill-column=81 fiona.texi
	$(SED) -n '/procrastination/,/The most recent change log/w ChangeLog2.txt' ChangeLog.txt
	$(SED) '1,2d' ChangeLog2.txt | $(SED) -e '$$d' | $(SED) -e '$$d' | $(SED) 's/^   //' > ChangeLog.txt
	rm ChangeLog2.txt
	head -n8 ../README > ../ChangeLog
	cat ChangeLog.txt >> ../ChangeLog
	head -n8 ../README > ../NEWS
	head -n `($(GREP) -n -m2 "^Version " ChangeLog.txt | $(SED) -n '2p' | $(SED) -e 's/:/ /' | $(AWK) '{print $$1-1}')` ChangeLog.txt >> ../NEWS
	echo "\nSee file ChangeLog <http://service-technology.org/files/$(PACKAGE)/ChangeLog> for\ncomplete version history.\n" >> ../NEWS
	rm ChangeLog.txt


#############################################################################
# run the "gendocs.sh" script to create a manual in several formats
#############################################################################
manual: $(info_TEXINFOS)
	./gendocs.sh $(PACKAGE) "$(PACKAGE_NAME)"
