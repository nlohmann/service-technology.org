
#ifndef _teststudentttestsunit_h
#define _teststudentttestsunit_h

#include "ap.h"
#include "ialglib.h"

#include "gammaf.h"
#include "normaldistr.h"
#include "ibetaf.h"
#include "studenttdistr.h"
#include "studentttests.h"


bool teststudentt(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool teststudentttestsunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool teststudentttestsunit_test();


#endif
