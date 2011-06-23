
#ifndef _testdetldltunit_h
#define _testdetldltunit_h

#include "ap.h"
#include "ialglib.h"

#include "ldlt.h"
#include "sdet.h"


bool testdetldlt(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testdetldltunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testdetldltunit_test();


#endif
