
#include <stdafx.h>
#include <stdio.h>
#include "testgammaunit.h"

bool testgamma(bool silent)
{
    bool result;
    double threshold;
    double v;
    double s;
    bool waserrors;
    bool gammaerrors;
    bool lngammaerrors;

    gammaerrors = false;
    lngammaerrors = false;
    waserrors = false;
    threshold = 100*ap::machineepsilon;
    
    //
    //
    //
    gammaerrors = gammaerrors||fabs(gamma(0.5)-sqrt(ap::pi()))>threshold;
    gammaerrors = gammaerrors||fabs(gamma(1.5)-0.5*sqrt(ap::pi()))>threshold;
    v = lngamma(0.5, s);
    lngammaerrors = lngammaerrors||fabs(v-log(sqrt(ap::pi())))>threshold||s!=1;
    v = lngamma(1.5, s);
    lngammaerrors = lngammaerrors||fabs(v-log(0.5*sqrt(ap::pi())))>threshold||s!=1;
    
    //
    // report
    //
    waserrors = gammaerrors||lngammaerrors;
    if( !silent )
    {
        printf("TESTING GAMMA FUNCTION\n");
        printf("GAMMA:                                   ");
        if( gammaerrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("LN GAMMA:                                ");
        if( lngammaerrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        if( waserrors )
        {
            printf("TEST FAILED\n");
        }
        else
        {
            printf("TEST PASSED\n");
        }
        printf("\n\n");
    }
    
    //
    // end
    //
    result = !waserrors;
    return result;
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testgammaunit_test_silent()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testgamma(true);
    return result;
#else
    return _i_testgammaunit_test_silent();
#endif
}


/*************************************************************************
Unit test
*************************************************************************/
bool testgammaunit_test()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testgamma(false);
    return result;
#else
    return _i_testgammaunit_test();
#endif
}



