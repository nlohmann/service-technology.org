
#ifndef _testfhtunit_h
#define _testfhtunit_h

#include "ap.h"
#include "ialglib.h"

#include "ftbase.h"
#include "fft.h"
#include "fht.h"


/*************************************************************************
Test
*************************************************************************/
bool testfht(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testfhtunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testfhtunit_test();


#endif
