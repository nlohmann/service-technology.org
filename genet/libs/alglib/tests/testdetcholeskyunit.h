
#ifndef _testdetcholeskyunit_h
#define _testdetcholeskyunit_h

#include "ap.h"
#include "ialglib.h"

#include "cholesky.h"
#include "spddet.h"


bool testdetcholesky(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testdetcholeskyunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testdetcholeskyunit_test();


#endif
