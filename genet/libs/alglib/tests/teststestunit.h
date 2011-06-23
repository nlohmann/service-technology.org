
#ifndef _teststestunit_h
#define _teststestunit_h

#include "ap.h"
#include "ialglib.h"

#include "gammaf.h"
#include "normaldistr.h"
#include "ibetaf.h"
#include "nearunityunit.h"
#include "binomialdistr.h"
#include "stest.h"


bool testsigntest(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool teststestunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool teststestunit_test();


#endif
