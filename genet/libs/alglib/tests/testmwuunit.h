
#ifndef _testmwuunit_h
#define _testmwuunit_h

#include "ap.h"
#include "ialglib.h"

#include "normaldistr.h"
#include "mannwhitneyu.h"


bool testmwutest(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testmwuunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testmwuunit_test();


#endif
