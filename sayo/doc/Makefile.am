info_TEXINFOS = sayo.texi

sayo_TEXINFOS = fdl.texi cmdline.out ChangeLog.texi

TEXINFO_TEX = texinfo.tex \
              examples/2bits.og \
              examples/2bits.interface \
              examples/2bits.nodes \
              examples/coffee.owfn \
              examples/coffee.og \
              examples/coffee.sa \
              examples/coffee.pv.owfn.part1 \
              examples/coffee.pv.owfn.part2 

EXTRA_DIST = cmdline.out \
             examples/2bits.og \
             examples/2bits.interface \
             examples/2bits.nodes \
             examples/coffee.owfn \
             examples/coffee.og \
             examples/coffee.sa \
             examples/coffee.pv.owfn.part1 \
             examples/coffee.pv.owfn.part2 

MAINTAINERCLEANFILES = cmdline.out

all: ../ChangeLog


#############################################################################
# run "sayo --full" to include output in documentation
#############################################################################
cmdline.out: $(top_srcdir)/src/cmdline.ggo $(top_srcdir)/configure.ac
	$(top_srcdir)/src/sayo --help > cmdline.out


#############################################################################
# create ASCII ChangeLog/NEWS out of Texinfo (9 is height of header)
#############################################################################
../ChangeLog: ChangeLog.texi
	$(MAKEINFO) -o ChangeLog.txt --no-split --no-headers --fill-column=81 sayo.texi
	$(SED) -n '/procrastination/,/The most recent change log/w ChangeLog2.txt' ChangeLog.txt
	$(SED) '1,2d' ChangeLog2.txt | $(SED) -e '$$d' | $(SED) -e '$$d' | $(SED) 's/^   //' > ChangeLog.txt
	rm ChangeLog2.txt
	head -n9 ../README > ../ChangeLog
	cat ChangeLog.txt >> ../ChangeLog
	head -n9 ../README > ../NEWS
	head -n `($(GREP) -n -m2 "^Version " ChangeLog.txt | $(SED) -n '2p' | $(SED) -e 's/:/ /' | $(AWK) '{print $$1-1}')` ChangeLog.txt >> ../NEWS
	echo "\nSee file ChangeLog <http://service-technology.org/files/$(PACKAGE)/ChangeLog> for\ncomplete version history.\n" >> ../NEWS
	rm ChangeLog.txt
