############################################################################
# Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg, Dennis Reinert,   #
#                      Jan Bretschneider and Christian Gierds              #
#                                                                          #
# This file is part of Fiona.                                              #
#                                                                          #
# Fiona is free software; you can redistribute it and/or modify it         #
# under the terms of the GNU General Public License as published by the    #
# Free Software Foundation; either version 2 of the License, or (at your   #
# option) any later version.                                               #
#                                                                          #
# Fiona is distributed in the hope that it will be useful, but WITHOUT     #
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    #
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for #
# more details.                                                            #
#                                                                          #
# You should have received a copy of the GNU General Public License along  #
# with Fiona; if not, write to the Free Software Foundation, Inc., 51      #
# Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                     #
############################################################################

include $(top_srcdir)/Makefile.covcommon

TESTS = slist.sh syntax.sh extrema.sh messages.sh cyclic.sh \
        matching.sh sim_og.sh productog.sh \
        sequences.sh partner.sh countservices.sh samples.sh bddtest.sh

check_PROGRAMS = slistcheck

slistcheck_SOURCES = slistcheck.cc

slistcheck_CXXFLAGS = -I$(top_srcdir)/src

# Notice: If you add environment variables here, you have to provide defaults
# defaults.sh, too.
TESTS_ENVIRONMENT = testdir=$(srcdir) MKDIR_P="$(MKDIR_P)" \
    builddir=$(builddir) PATH=".:../src:$(srcdir):$$PATH"

MEMCHECK_ENVIRONMENT = $(TESTS_ENVIRONMENT) memcheck=yes
CHECKLOUD_ENVIRONMENT = $(TESTS_ENVIRONMENT) quiet=no

# For maximum portability we mention every file explicitly:
#   o $(wildcard *.owfn) is not portable, but requires GNU make.
#   o *.owfn fails for VPATH builds.
#   o Messing around with VPATH is ugly.
#   o More reasons at http://www.gnu.org/software/automake/manual/html_mono/automake.html#wildcards
EXTRA_DIST = $(TESTS) memcheck_helper.sh \
             $(TESTS) defaults.sh \
             cyclic/livelockInP.owfn \
             cyclic/zyklusP.owfn \
             cyclic/zyklusPmitCommit.owfn \
             cyclic/zyklusR.owfn \
             cyclic/zyklusReturn.owfn \
             extrema/all_final_marking.owfn \
             extrema/empty.owfn \
             extrema/multiple_input.owfn \
             extrema/multiple_input_multiple_output.owfn \
             extrema/multiple_output.owfn \
             extrema/no_communication.owfn \
             extrema/no_final_marking.owfn \
             extrema/no_initial_marking.owfn \
             extrema/no_initial_marking2.owfn \
             extrema/one_input.owfn \
             extrema/one_input_2.owfn \
             extrema/one_input_marked.owfn \
             extrema/one_input_one_output.owfn \
             extrema/one_output.owfn \
             extrema/one_output_marked.owfn \
             messages/messageboundtest1.owfn \
             messages/messageboundtest2.owfn \
             samples/06-03-23_BPM06_shop_sect_3.owfn \
             samples/06-03-23_BPM06_shop_sect_6.owfn \
             samples/DKE07_newshop_1.owfn \
             samples/DKE07_newshop_2.owfn \
             samples/DKE07_shop.owfn \
             samples/keesCoffee_condition.owfn \
             samples/myCoffee.owfn \
             sequence_suite/sequence10.owfn \
             sequence_suite/sequence11.owfn \
             sequence_suite/sequence12.owfn \
             sequence_suite/sequence13.owfn \
             sequence_suite/sequence14.owfn \
             sequence_suite/sequence15.owfn \
             sequence_suite/sequence16.owfn \
             sequence_suite/sequence17.owfn \
             sequence_suite/sequence18.owfn \
             sequence_suite/sequence19.owfn \
             sequence_suite/sequence20.owfn \
             sequence_suite/sequence3.owfn \
             sequence_suite/sequence4.owfn \
             sequence_suite/sequence5.owfn \
             sequence_suite/sequence6.owfn \
             sequence_suite/sequence7.owfn \
             sequence_suite/sequence8.owfn \
             sequence_suite/sequence9.owfn \
             matching/client_match_1.owfn \
             matching/client_nomatch_1.owfn \
             matching/client_nomatch_2.owfn \
             matching/client_nosupport_1.owfn \
             matching/shop.og \
             matching/shop_rednodes.og \
             syntax/max_occurrence.owfn \
             bdd_ref/phcontrol4.unf_reference.owfn \
             bdd_ref/sequence3_reference.owfn \
             bdd_ref/phcontrol4.unf_reference.owfn.OG.BDD_ANN.cudd \
             bdd_ref/phcontrol4.unf_reference.owfn.OG.BDD_MP.cudd \
             bdd_ref/sequence3_reference.owfn.OG.BDD_ANN.cudd \
             bdd_ref/sequence3_reference.owfn.OG.BDD_MP.cudd \
             philosophers/phcontrol10.unf.owfn \
             philosophers/phcontrol3.unf.owfn \
             philosophers/phcontrol4.unf.owfn \
             philosophers/phcontrol5.unf.owfn \
             philosophers/phcontrol6.unf.owfn \
             philosophers/phcontrol7.unf.owfn \
             philosophers/phcontrol8.unf.owfn \
             philosophers/phcontrol9.unf.owfn \
             philosophers/phcontrol10.unf.owfn.png \
             philosophers/phcontrol3.unf.owfn.png \
             philosophers/phcontrol4.unf.owfn.png \
             philosophers/phcontrol5.unf.owfn.png \
             philosophers/phcontrol6.unf.owfn.png \
             philosophers/phcontrol7.unf.owfn.png \
             philosophers/phcontrol8.unf.owfn.png \
             philosophers/phcontrol9.unf.owfn.png \
             productog/1.og \
             productog/2.og \
             productog/3.og \
             productog/1_X_2.expected.og \
             productog/1_X_2_X_3.expected.og \
             productog/3_X_1_X_2.expected.og \
             partner/sequence3.owfn \
             partner/sequence3-partner.expected.owfn \
             countservices/acyclic.og \
             countservices/cyclic.og \
             countservices/1.og \
             sim_og/test11.owfn \
             sim_og/test12.owfn \
             sim_og/test21.owfn \
             sim_og/test22.owfn \
             sim_og/test31.owfn \
             sim_og/test32.owfn \
             sim_og/test41.owfn \
             sim_og/test42.owfn \
             sim_og/test51.owfn \
             sim_og/test52.owfn \
             sim_og/test61.owfn \
             sim_og/test62.owfn \
             sim_og/test71.owfn \
             sim_og/test72.owfn \
             sim_og/test81.owfn \
             sim_og/test82.owfn \
             sim_og/test91.owfn \
             sim_og/test92.owfn \
             sim_og/test11.owfn.og \
             sim_og/test12.owfn.og \
             sim_og/test21.owfn.og \
             sim_og/test22.owfn.og \
             sim_og/test31.owfn.og \
             sim_og/test32.owfn.og \
             sim_og/test41.owfn.og \
             sim_og/test42.owfn.og \
             sim_og/test51.owfn.og \
             sim_og/test52.owfn.og \
             sim_og/test61.owfn.og \
             sim_og/test62.owfn.og \
             sim_og/test71.owfn.og \
             sim_og/test72.owfn.og \
             sim_og/test81.owfn.og \
             sim_og/test82.owfn.og \
             sim_og/test91.owfn.og \
             sim_og/test92.owfn.og \
             sim_og/coarse-chopped.og \
             sim_og/coarse.og \
             sim_og/compact.og

# Global check-loud target. Runs all tests. Output files are generated.
check-loud:
	$(MAKE) $(AM_MAKEFLAGS) $(CHECKLOUD_ENVIRONMENT) check


# Generate check targets from $(TESTS). That is done by removing '.sh' from
# every word in $(TESTS) and precede them by 'check-'. So one of many check
# targets would be check-samples if samples.sh was in $(TESTS).
SINGLE_CHECK_TARGETS = $(TESTS:%.sh=check-%)

# The same for loud (unquiet, output files generating)
SINGLE_CHECKLOUD_TARGETS = $(TESTS:%.sh=check-%-loud)

# All single check targets. For example check-samples.
# The script belonging to the current check target is inferred from the
# name of the target by removing the initial 'check-' and adding '.sh' to
# the end.
$(SINGLE_CHECK_TARGETS):
	-$(TESTS_ENVIRONMENT) ./$(@:check-%=%.sh)

$(SINGLE_CHECKLOUD_TARGETS):
	-$(CHECKLOUD_ENVIRONMENT) ./$(@:check-%-loud=%.sh)

# Global memcheck target. Runs all tests under valgrind.
memcheck:
	$(MAKE) $(AM_MAKEFLAGS) $(MEMCHECK_ENVIRONMENT) check

# Create single memcheck target analogous to single check targets
SINGLE_MEMCHECK_TARGETS = $(TESTS:%.sh=memcheck-%)
$(SINGLE_MEMCHECK_TARGETS):
	-$(MEMCHECK_ENVIRONMENT) ./$(@:memcheck-%=%.sh)

COV_RUNFILES=cov-run-*.covexp
COV_DBFILE=cov-db
COV_ALLFILES=$(COV_RUNFILES) $(COV_DBFILE)

# Coverage check
covcheck: covrunclean
	$(MAKE) $(AM_MAKEFLAGS) CXX="$(CXXCOV)" $(check_PROGRAMS)
	$(MAKE) $(AM_MAKEFLAGS) cleandeps
	-$(MAKE) $(AM_MAKEFLAGS) check
	covmerge.exe $(COV_RUNFILES) > $(COV_DBFILE)
	gen_html $(COV_DBFILE)
	$(MAKE) $(AM_MAKEFLAGS) covrunclean

covrunclean:
	-test -z "$(COV_ALLFILES)" || rm -f $(COV_ALLFILES)

clean-local: covrunclean cleandeps

DISTCLEAN_FILES = productog/1_X_2.output.og \
                  productog/1_X_2.output.out \
                  productog/1_X_2.output.png \
                  productog/1_X_2_X_3.output.og \
                  productog/1_X_2_X_3.output.out \
                  productog/1_X_2_X_3.output.png \
                  productog/3_X_1_X_2.output.og \
                  productog/3_X_1_X_2.output.out \
                  productog/3_X_1_X_2.output.png \
                  partner/sequence3.owfn \
                  partner/sequence3.owfn.OG.out \
                  partner/sequence3.owfn.OG.png \
                  partner/sequence3.owfn.og \
                  partner/sequence3.owfn.OG.stg \
                  partner/sequence3.owfn.OG.pn \
                  partner/sequence3-partner.owfn \
                  partner/sequence3-partner.dot \
                  partner/sequence3-partner.png \
                  petrify.log

distclean-local:
	-test -d coverage_html/ && rm -rf coverage_html/
	rm -rf $(DISTCLEAN_FILES)    
   

