info_TEXINFOS = owfn2bpel.texi

owfn2bpel_TEXINFOS = fdl.texi cmdline.out ChangeLog.texi

TEXINFO_TEX = texinfo.tex

EXTRA_DIST = gendocs.sh gendocs_template Kleine_2007.pdf LohmannKleine_2008.pdf

MAINTAINERCLEANFILES = cmdline.out

CLEANFILES = owfn2bpel.dvi


#############################################################################
# run "owfn2bpel --help" to include output in documentation
#############################################################################
cmdline.out: $(top_srcdir)/src/cmdline.ggo
	$(top_srcdir)/src/owfn2bpel --help > cmdline.out


#############################################################################
# create ASCII ChangeLog out of Texinfo
#############################################################################
../ChangeLog: ChangeLog.texi
	$(MAKEINFO) -o ChangeLog.txt --no-split --no-headers  --fill-column=78 owfn2bpel.texi
	$(SED) -n '/procrastination/,/The most recent change log/w ChangeLog2.txt' ChangeLog.txt
	$(SED) '1,2d' ChangeLog2.txt | $(SED) -e '$$d' | $(SED) -e '$$d' > ChangeLog.txt
	rm ChangeLog2.txt
	head -n8 ../README > ../ChangeLog
	cat ChangeLog.txt >> ../ChangeLog
	rm ChangeLog.txt
