# list all testscripts
TESTS = bug1.sh bug2.sh bug3.sh bug4.sh bug5.sh bug6.sh bug7.sh \
        net-phils.sh net-purchase.sh net-sequence.sh net-travel.sh net-zeus.sh

# list the testscripts and testfiles
EXTRA_DIST = $(TESTS) \
             bug1/bug1.owfn bug1/bug1.fiona.og \
             bug2/bug2.owfn bug2/bug2.fiona.og \
             bug3/bug3.owfn bug3/bug3.fiona.og \
             bug4/bug4.owfn bug4/bug4.fiona.og \
             bug5/bug5.owfn bug5/bug5.fiona.og \
             bug6/bug6.owfn bug6/bug6.fiona.og \
             bug7/bug7.owfn bug7/bug7.fiona.og \
             net-phils/phcontrol3.unf.owfn net-phils/phcontrol3.unf.fiona.og \
             net-purchase/PO.owfn net-purchase/PO.fiona.og \
             net-sequence/sequence10.owfn net-sequence/sequence10.fiona.og \
             net-travel/T2.owfn net-travel/T2.fiona.og \
             net-zeus/zeus.owfn net-zeus/zeus.fiona.og


#############################################################################
# everything below is generic and should not be changed
#############################################################################
# this wrapper script organizes the temp file generation
EXTRA_DIST += wrap.sh

# where to execute the tests (usually a temp folder inside the build dir)
TESTDIR = $(abs_builddir)/tmp

# variables passed to the testscripts
TESTS_ENVIRONMENT = FIONA="$(FIONA)" testdir=$(TESTDIR) testroot=$(abs_srcdir) \
                    PATH=$(abs_top_builddir)/src:$$PATH $(srcdir)/wrap.sh

# "make clean" should delete the result of the tests
CLEANFILES = -r $(TESTDIR)
