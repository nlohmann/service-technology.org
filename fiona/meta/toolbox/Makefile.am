SUBDIRS = ../../bpel2owfn ../../candy ../../cosme ../../diane ../../fiona ../../linda ../../marlene ../../mia ../../owfn2bpel ../../pnapi ../../rachel ../../rebecca ../../safira ../../sayo ../../uml2owfn ../../wendy ../../yasmina

svn-clean:
	-@for sub in $(SUBDIRS); do \
		$(MAKE) svn-clean -C $$sub; \
	done;
	rm -fr AUTHORS COPYING ChangeLog INSTALL Makefile Makefile.in NEWS \
		README aclocal.m4 autom4te.cache config.log config.status \
		configure install-sh missing