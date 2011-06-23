
#ifndef _testlogitunit_h
#define _testlogitunit_h

#include "ap.h"
#include "ialglib.h"

#include "descriptivestatistics.h"
#include "mlpbase.h"
#include "cholesky.h"
#include "spdsolve.h"
#include "tsort.h"
#include "bdss.h"
#include "logit.h"


bool testlogit(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testlogitunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testlogitunit_test();


#endif
