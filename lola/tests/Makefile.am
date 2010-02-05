binaries:
	$(MAKE) $(AM_MAKEFLAGS) -s -C $(top_builddir) lola-deadlock
	$(MAKE) $(AM_MAKEFLAGS) -s -C $(top_builddir) lola-fulllimited
	$(MAKE) $(AM_MAKEFLAGS) -s -C $(top_builddir) lola-modelchecking
	$(MAKE) $(AM_MAKEFLAGS) -s -C $(top_builddir) lola-statepredicate1
	$(MAKE) $(AM_MAKEFLAGS) -s -C $(top_builddir) lola-liveprop
	$(MAKE) $(AM_MAKEFLAGS) -s -C $(top_builddir) lola-boundednet
	$(MAKE) $(AM_MAKEFLAGS) -s -C $(top_builddir) lola-statespace

TESTSUITE = $(builddir)/testsuite
EXTRA_DIST = testsuite.at wrap.sh \
		testfiles

DISTCLEANFILES = atconfig package.m4 $(TESTSUITE)

# check-local should build the binaries
check-local: atconfig $(TESTSUITE) binaries
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

clean-local:
	test ! -f '$(TESTSUITE)' || $(SHELL) '$(TESTSUITE)' --clean

$(TESTSUITE): $(srcdir)/testsuite.at
	$(AUTOM4TE) --language=autotest -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@
