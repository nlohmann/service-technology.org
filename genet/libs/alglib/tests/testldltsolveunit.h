
#ifndef _testldltsolveunit_h
#define _testldltsolveunit_h

#include "ap.h"
#include "ialglib.h"

#include "ldlt.h"
#include "ssolve.h"


bool testldltsolve(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testldltsolveunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testldltsolveunit_test();


#endif
