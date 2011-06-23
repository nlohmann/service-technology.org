
#ifndef _testcorrhtunit_h
#define _testcorrhtunit_h

#include "ap.h"
#include "ialglib.h"

#include "gammaf.h"
#include "normaldistr.h"
#include "ibetaf.h"
#include "studenttdistr.h"
#include "correlation.h"
#include "correlationtests.h"


bool testcorrtest(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testcorrhtunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testcorrhtunit_test();


#endif
