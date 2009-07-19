SUBDIRS = ../../bpel2owfn ../../candy ../../diane ../../fiona ../../linda ../../mia ../../owfn2bpel ../../pnapi ../../rachel ../../sayo ../../uml2owfn ../../wendy

svn-clean:
	-@for sub in $(SUBDIRS); do \
		$(MAKE) svn-clean -C $$sub; \
	done;
	rm -fr AUTHORS COPYING ChangeLog INSTALL Makefile Makefile.in NEWS \
		README aclocal.m4 autom4te.cache config.log config.status \
		configure install-sh missing