
############################################################################
# All tests that are expected to succeed (the default case) 
############################################################################
TESTS = cmdline_test.sh parser_run.sh parser_filter_run.sh compiler_allformats.sh compiler_soundness.sh

############################################################################
# All tests that are expected to fail (must be a subset of TESTS above)
# e.g., XFAIL_TESTS = compiler_allformats.sh 
############################################################################
XFAIL_TESTS = compiler_allformats.sh


# Notice: If you add environment variables here, you have to provide defaults
# defaults.sh, too.
TESTS_ENVIRONMENT = testdir=$(srcdir) builddir=$(builddir) MKDIR_P="$(MKDIR_P)" PATH=.:../src:$$PATH

CLEANFILES = *.lola \
		*.task \
		*.xml \
		*.dot \
		*.owfn \
		*.png \
		uml2owfn.exe.stackdump \
		core \
		result.txt \
		*.csv \
		soundness/check_TestSuite_soundness.sh \
		soundness/TestSuite_soundness.BOMcncpt1__ProcessPinDisjoint_sound_ifStop.lola \
		soundness/TestSuite_soundness.BOMcncpt1__ProcessPinDisjointStopEnd_sound_stop.lola \
		soundness/TestSuite_soundness.BOMcncpt1__TaskCallPinDisjointProc_sound_stop.lola \
		soundness/TestSuite_soundness.BOMcncpt1__TaskPinDisjointProc_sound_stop.lola \
		soundness/TestSuite_soundness.BOMcncpt__ForkJoin_sound.lola \
		soundness/TestSuite_soundness.BOMcncpt__InclusiveOr2_sound.lola \
		soundness/TestSuite_soundness.BOMcncpt__InclusiveOr_sound_nonsafe.lola \
		soundness/TestSuite_soundness.BOMcncpt__ProcessSimple_sound.lola \
		soundness/TestSuite_soundness.BOMcncpt__Repository_sound.lola \
		soundness/TestSuite_soundness.StpvsEnd__StopvsEnd_1_nonsound.lola \
		soundness/TestSuite_soundness.StpvsEnd__StopvsEnd_1b_sound_nonsafe.lola \
		soundness/TestSuite_soundness.StpvsEnd__StopvsEnd_2_sound_ifStop.lola
		

EXTRA_DIST = $(TESTS) \
		 	compiler/TestSuite.xml \
		 	soundness/TestSuite_soundness.xml \
		 	soundness/TestSuite_soundness.txt
