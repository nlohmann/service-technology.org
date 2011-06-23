
#ifndef _testcholeskysolveunit_h
#define _testcholeskysolveunit_h

#include "ap.h"
#include "ialglib.h"

#include "cholesky.h"
#include "spdsolve.h"


bool testcholeskysolve(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testcholeskysolveunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testcholeskysolveunit_test();


#endif
