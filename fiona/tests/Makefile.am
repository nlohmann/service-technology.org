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

TESTS = syntax.sh sequences.sh bddtest.sh extrema.sh \
		messages.sh cyclic.sh matching.sh samples.sh


TESTS_ENVIRONMENT = testdir=$(top_srcdir)/tests PATH=".:../src:$(srcdir):$$PATH"

MEMCHECK_ENVIRONMENT = $(TESTS_ENVIRONMENT) memcheck=yes

EXTRA_DIST = $(TESTS) memcheck_helper.sh \
             cyclic/*.owfn \
             extrema/*.owfn \
             messages/*.owfn \
             samples/*.owfn \
             sequence_suite/*.owfn \
             matching/*.owfn matching/*.og \
             syntax/*.owfn \
             bdd_ref/*.owfn bdd_ref/*.cudd \
             philosophers/*.owfn philosophers/*.owfn.dot \
             philosophers/*.owfn.png


# Generate check targets from $(TESTS). That is done by removing '.sh' from
# every word in $(TESTS) and precede them by 'check-'. So one of many check
# targets would be check-samples if samples.sh was in $(TESTS).
SINGLE_CHECK_TARGETS = $(TESTS:%.sh=check-%)

# All single check targets. For example check-samples.
# The script belonging to the current check target is inferred from the
# name of the target by removing the initial 'check-' and adding '.sh' to
# the end.
$(SINGLE_CHECK_TARGETS):
	(cd .. && $(MAKE) $(AM_MAKEFLAGS) all)
	-$(TESTS_ENVIRONMENT) ./$(@:check-%=%.sh)

# Global memcheck target. Runs all tests under valgrind.
memcheck:
	(cd .. && $(MAKE) $(AM_MAKEFLAGS) all)
	$(MAKE) $(AM_MAKEFLAGS) $(MEMCHECK_ENVIRONMENT) check

# Create single memcheck target analogous to single check targets
SINGLE_MEMCHECK_TARGETS = $(TESTS:%.sh=memcheck-%)
$(SINGLE_MEMCHECK_TARGETS):
	(cd .. && $(MAKE) $(AM_MAKEFLAGS) all)
	-$(MEMCHECK_ENVIRONMENT) ./$(@:memcheck-%=%.sh)
