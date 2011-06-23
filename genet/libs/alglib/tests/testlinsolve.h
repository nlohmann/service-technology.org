
#ifndef _testlinsolve_h
#define _testlinsolve_h

#include "ap.h"
#include "ialglib.h"

#include "lu.h"
#include "rsolve.h"


bool testsolvesystem(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testlinsolve_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testlinsolve_test();


#endif
