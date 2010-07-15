EXTRA_DIST = bpm2008.tar.gz
CLEANFILES = -fr bpm2008

experiments: bpm2008

bpm2008: bpm2008.tar.gz
	tar xfz bpm2008.tar.gz
	bpm2008/runme.sh $(abs_top_srcdir)/src/rachel
