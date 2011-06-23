
#include <stdafx.h>
#include <stdio.h>
#include "testlinsolve.h"

bool testsolvesystem(bool silent)
{
    bool result;
    int n;
    int maxn;
    int i;
    int j;
    int passcount;
    int pass;
    bool waserrors;
    bool wfailed;
    double err;
    double v;
    double threshold;
    ap::real_2d_array a;
    ap::real_1d_array tx;
    ap::real_1d_array x;
    ap::real_1d_array b;
    ap::integer_1d_array p;

    err = 0;
    wfailed = false;
    waserrors = false;
    maxn = 30;
    passcount = 10;
    threshold = 1.0E-8;
    
    //
    // N = 1 .. 30
    //
    for(n = 1; n <= maxn; n++)
    {
        a.setbounds(0, n-1, 0, n-1);
        tx.setbounds(0, n-1);
        b.setbounds(0, n-1);
        for(pass = 1; pass <= passcount; pass++)
        {
            
            //
            // init A, TX
            //
            for(i = 0; i <= n-1; i++)
            {
                for(j = 0; j <= n-1; j++)
                {
                    a(i,j) = 2*ap::randomreal()-1;
                }
            }
            a(ap::randominteger(n),ap::randominteger(n)) = 10;
            a(ap::randominteger(n),ap::randominteger(n)) = 7;
            for(i = 0; i <= n-1; i++)
            {
                tx(i) = 2*ap::randomreal()-1;
            }
            for(i = 0; i <= n-1; i++)
            {
                v = ap::vdotproduct(&a(i, 0), &tx(0), ap::vlen(0,n-1));
                b(i) = v;
            }
            
            //
            // solve and test normal
            //
            if( !rmatrixsolve(a, b, n, x) )
            {
                wfailed = true;
                continue;
            }
            for(i = 0; i <= n-1; i++)
            {
                err = ap::maxreal(err, fabs(tx(i)-x(i)));
            }
            
            //
            // solve and test LU
            //
            rmatrixlu(a, n, n, p);
            if( !rmatrixlusolve(a, p, b, n, x) )
            {
                wfailed = true;
                continue;
            }
            for(i = 0; i <= n-1; i++)
            {
                err = ap::maxreal(err, fabs(tx(i)-x(i)));
            }
        }
    }
    
    //
    // report
    //
    waserrors = err>threshold||wfailed;
    if( !silent )
    {
        printf("TESTING LINSOLVE\n");
        printf("Error:                                   %5.3le\n",
            double(err));
        printf("Always succeeded:                        ");
        if( !wfailed )
        {
            printf("YES\n");
        }
        else
        {
            printf("NO\n");
        }
        printf("Threshold:                               %5.3le\n",
            double(threshold));
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
    result = !waserrors;
    return result;
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testlinsolve_test_silent()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testsolvesystem(true);
    return result;
#else
    return _i_testlinsolve_test_silent();
#endif
}


/*************************************************************************
Unit test
*************************************************************************/
bool testlinsolve_test()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testsolvesystem(false);
    return result;
#else
    return _i_testlinsolve_test();
#endif
}



