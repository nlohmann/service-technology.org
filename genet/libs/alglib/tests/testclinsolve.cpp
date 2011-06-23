
#include <stdafx.h>
#include <stdio.h>
#include "testclinsolve.h"

bool testcsolvesystem(bool silent)
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
    ap::complex v;
    double threshold;
    ap::complex_2d_array a;
    ap::complex_1d_array tx;
    ap::complex_1d_array x;
    ap::complex_1d_array b;
    ap::integer_1d_array p;
    int i_;

    err = 0;
    wfailed = false;
    waserrors = false;
    maxn = 30;
    passcount = 10;
    threshold = 2*maxn*1000*ap::machineepsilon;
    
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
                    a(i,j).x = 2*ap::randomreal()-1;
                    a(i,j).y = 2*ap::randomreal()-1;
                }
            }
            a(ap::randominteger(n),ap::randominteger(n)) = 10;
            a(ap::randominteger(n),ap::randominteger(n)) = 7;
            for(i = 0; i <= n-1; i++)
            {
                tx(i).x = 2*ap::randomreal()-1;
                tx(i).y = 2*ap::randomreal()-1;
            }
            for(i = 0; i <= n-1; i++)
            {
                v = 0.0;
                for(i_=0; i_<=n-1;i_++)
                {
                    v += a(i,i_)*tx(i_);
                }
                b(i) = v;
            }
            
            //
            // solve and test normal
            //
            if( !cmatrixsolve(a, b, n, x) )
            {
                wfailed = true;
                continue;
            }
            for(i = 0; i <= n-1; i++)
            {
                err = ap::maxreal(err, ap::abscomplex(tx(i)-x(i)));
            }
            
            //
            // solve and test LU
            //
            cmatrixlu(a, n, n, p);
            if( !cmatrixlusolve(a, p, b, n, x) )
            {
                wfailed = true;
                continue;
            }
            for(i = 0; i <= n-1; i++)
            {
                err = ap::maxreal(err, ap::abscomplex(tx(i)-x(i)));
            }
        }
    }
    
    //
    // report
    //
    waserrors = err>threshold||wfailed;
    if( !silent )
    {
        printf("TESTING COMPLEX LINSOLVE\n");
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
bool testclinsolve_test_silent()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testcsolvesystem(true);
    return result;
#else
    return _i_testclinsolve_test_silent();
#endif
}


/*************************************************************************
Unit test
*************************************************************************/
bool testclinsolve_test()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testcsolvesystem(false);
    return result;
#else
    return _i_testclinsolve_test();
#endif
}



