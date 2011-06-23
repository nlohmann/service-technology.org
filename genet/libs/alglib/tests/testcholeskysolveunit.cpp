
#include <stdafx.h>
#include <stdio.h>
#include "testcholeskysolveunit.h"

bool testcholeskysolve(bool silent)
{
    bool result;
    ap::real_1d_array xe;
    ap::real_1d_array x;
    ap::real_1d_array x2;
    ap::real_1d_array b;
    ap::real_2d_array l;
    ap::real_2d_array a;
    int n;
    int pass;
    int i;
    int j;
    int k;
    int minij;
    bool upperin;
    double v;
    double err;
    bool fails;
    bool waserrors;
    int passcount;
    int maxn;
    int htask;
    double threshold;

    err = 0;
    passcount = 10;
    maxn = 20;
    threshold = 100000*ap::machineepsilon;
    waserrors = false;
    fails = false;
    
    //
    // Test
    //
    for(n = 1; n <= maxn; n++)
    {
        a.setbounds(0, n-1, 0, n-1);
        l.setbounds(0, n-1, 0, n-1);
        x.setbounds(0, n-1);
        x2.setbounds(0, n-1);
        xe.setbounds(0, n-1);
        b.setbounds(0, n-1);
        for(htask = 0; htask <= 1; htask++)
        {
            for(pass = 1; pass <= passcount; pass++)
            {
                upperin = htask==0;
                
                //
                // init L
                //
                for(i = 0; i <= n-1; i++)
                {
                    for(j = i+1; j <= n-1; j++)
                    {
                        l(i,j) = ap::randomreal();
                        l(j,i) = l(i,j);
                    }
                    l(i,i) = 1.1+ap::randomreal();
                }
                
                //
                // init A, B, XE
                //
                for(i = 0; i <= n-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        minij = ap::minint(i, j);
                        v = ap::vdotproduct(l.getrow(i, 0, minij), l.getcolumn(j, 0, minij));
                        a(i,j) = v;
                        a(j,i) = v;
                    }
                }
                for(i = 0; i <= n-1; i++)
                {
                    xe(i) = 2*ap::randomreal()-1;
                }
                for(i = 0; i <= n-1; i++)
                {
                    v = ap::vdotproduct(&a(i, 0), &xe(0), ap::vlen(0,n-1));
                    b(i) = v;
                }
                for(i = 0; i <= n-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        if( upperin )
                        {
                            if( j<i )
                            {
                                l(i,j) = 0;
                                a(i,j) = 0;
                            }
                        }
                        else
                        {
                            if( i<j )
                            {
                                l(i,j) = 0;
                                a(i,j) = 0;
                            }
                        }
                    }
                }
                
                //
                // solution
                //
                if( !spdmatrixcholeskysolve(l, b, n, upperin, x) )
                {
                    fails = true;
                    continue;
                }
                if( !spdmatrixsolve(a, b, n, upperin, x2) )
                {
                    fails = true;
                    continue;
                }
                
                //
                // test
                //
                for(i = 0; i <= n-1; i++)
                {
                    err = ap::maxreal(err, fabs(x(i)-xe(i)));
                }
                for(i = 0; i <= n-1; i++)
                {
                    err = ap::maxreal(err, fabs(x2(i)-xe(i)));
                }
            }
        }
    }
    
    //
    // report
    //
    waserrors = fails||err>threshold;
    if( !silent )
    {
        printf("TESTING CHOLESKY SOLVER\n");
        printf("ERROR:                                   %5.3le\n",
            double(err));
        printf("UNEXPECTED FAIL OR SUCCESS:              ");
        if( fails )
        {
            printf("OCCURED\n");
        }
        else
        {
            printf("NONE\n");
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
    result = !waserrors;
    return result;
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testcholeskysolveunit_test_silent()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testcholeskysolve(true);
    return result;
#else
    return _i_testcholeskysolveunit_test_silent();
#endif
}


/*************************************************************************
Unit test
*************************************************************************/
bool testcholeskysolveunit_test()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testcholeskysolve(false);
    return result;
#else
    return _i_testcholeskysolveunit_test();
#endif
}



