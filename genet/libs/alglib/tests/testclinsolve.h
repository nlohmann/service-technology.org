
#ifndef _testclinsolve_h
#define _testclinsolve_h

#include "ap.h"
#include "ialglib.h"

#include "clu.h"
#include "csolve.h"


bool testcsolvesystem(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testclinsolve_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testclinsolve_test();


#endif
