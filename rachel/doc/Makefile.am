info_TEXINFOS = rachel.texi

rachel_TEXINFOS = fdl.texi cmdline.out ChangeLog.texi

TEXINFO_TEX = texinfo.tex

EXTRA_DIST = gendocs.sh gendocs_template background.pdf

MAINTAINERCLEANFILES = cmdline.out changelog.out


#############################################################################
# run "rachel --help" to include output in documentation
#############################################################################
cmdline.out: $(top_srcdir)/src/cmdline.ggo
	$(top_srcdir)/src/rachel --help > cmdline.out


#############################################################################
# create ASCII ChangeLog out of Texinfo
#############################################################################
../ChangeLog: ChangeLog.texi
	$(MAKEINFO) -o ChangeLog.txt --no-split --no-headers rachel.texi
	$(SED) -n '/procrastination/,/The most recent change log/w ChangeLog2.txt' ChangeLog.txt
	$(SED) '1,2d' ChangeLog2.txt | $(SED) -e '$$d' | $(SED) -e '$$d' > ChangeLog.txt
	rm ChangeLog2.txt
	mv ChangeLog.txt ../ChangeLog


#############################################################################
# run the "gendocs.sh" script to create a manual in several formats
#############################################################################
manual: rachel.texi
	./gendocs.sh $(PACKAGE) "$(PACKAGE_NAME)"


wiki: ChangeLog.texi
	texi2html rachel.texi
	$(SED) -n '/procrastination/,/The most recent change log/w ChangeLog.tmp' rachel.html
	cat ChangeLog.tmp | $(SED) '1,2d' | $(SED) -e 's/<dt> <b>/<h3>/;' | $(SED) -e 's/<\/b><\/dt>/<\/h3>/;' | $(SED) -e 's/<p>/<li>/;' | $(SED) -e 's/<\/p>/<\/li>/;' | $(SED) -e 's/<dd>/<ul>/;' | $(SED) -e 's/<\/dd>/<\/ul>/;' | $(SED) -e 's/<blockquote>/<ul>/;' | $(SED) -e 's/<\/blockquote>/<\/ul>/;' > ChangeLog.htm
	rm rachel.html ChangeLog.tmp
