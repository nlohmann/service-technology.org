info_TEXINFOS = bpel2owfn.texi

bpel2owfn_TEXINFOS = gpl.texi \
		     images/bpel2owfn.eps \
		     images/bpel2owfn.pdf \
		     images/bpel2owfn.png \
		     images/tools4bpel.eps \
		     images/tools4bpel.pdf \
		     images/tools4bpel.png


# rules for the installation manual
install_TEXINFOS = install.texi

CLEANFILES = install.aux \
	     install.dvi \
	     install.ky \
	     install.pdf \
	     install.toc \
	     install.vr \
	     install.cp \
	     install.fn \
	     install.log \
	     install.pg \
	     install.tmp \
	     install.tp


# rules to create the distribution documentation
hu-createdistdocs: install.texi
	$(MAKEINFO) $< -oinstall.txt --no-split --no-headers --no-number-section
	sed -n '/BEGIN OF README/,/END OF README/w ../README' install.txt
	sed -n '/BEGIN OF ChangeLog/,/END OF ChangeLog/w ../ChangeLog' install.txt
	sed -n '/BEGIN OF AUTHORS/,/END OF AUTHORS/w ../AUTHORS' install.txt
	sed -n '/BEGIN OF INSTALL/,/END OF INSTALL/w ../INSTALL' install.txt
	rm install.txt

