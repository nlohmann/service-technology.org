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
	sed -n '/procrastination/,/The most recent change log/w ChangeLog2.txt' ChangeLog.txt
	sed '1,2d' ChangeLog2.txt | sed -e '$$d' | sed -e '$$d' > ChangeLog.txt
	rm ChangeLog2.txt
	mv ChangeLog.txt ../ChangeLog


#############################################################################
# run the "gendocs.sh" script to create a manual in several formats
#############################################################################
manual: rachel.texi
	./gendocs.sh $(PACKAGE) "$(PACKAGE_NAME)"


wiki: ChangeLog.texi
	texi2html rachel.texi
	sed -n '/procrastination/,/The most recent change log/w ChangeLog.tmp' rachel.html
	cat ChangeLog.tmp | sed '1,2d' | sed -e 's/<dt> <b>/<h3>/;' | sed -e 's/<\/b><\/dt>/<\/h3>/;' | sed -e 's/<p>/<li>/;' | sed -e 's/<\/p>/<\/li>/;' | sed -e 's/<dd>/<ul>/;' | sed -e 's/<\/dd>/<\/ul>/;' | sed -e 's/<blockquote>/<ul>/;' | sed -e 's/<\/blockquote>/<\/ul>/;' > ChangeLog.htm
	rm rachel.html ChangeLog.tmp
