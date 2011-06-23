
#include <stdafx.h>
#include <stdio.h>
#include "testdetcholeskyunit.h"

bool testdetcholesky(bool silent)
{
    bool result;
    ap::real_2d_array l;
    ap::real_2d_array a;
    int n;
    int pass;
    int i;
    int j;
    int minij;
    bool upperin;
    bool cr;
    double v;
    double d;
    double d2;
    double err;
    bool wf;
    bool waserrors;
    int passcount;
    int maxn;
    int htask;
    double threshold;

    err = 0;
    wf = false;
    passcount = 10;
    maxn = 20;
    threshold = 1000*ap::machineepsilon;
    waserrors = false;
    
    //
    // Test
    //
    for(n = 1; n <= maxn; n++)
    {
        l.setbounds(0, n-1, 0, n-1);
        a.setbounds(0, n-1, 0, n-1);
        for(htask = 0; htask <= 1; htask++)
        {
            for(pass = 1; pass <= passcount; pass++)
            {
                upperin = htask==0;
                
                //
                // Prepare task:
                // * A contains upper (or lower) half of SPD matrix
                // * L contains its Cholesky factor (upper or lower)
                // * D contains det(A)
                //
                d = 1;
                for(i = 0; i <= n-1; i++)
                {
                    for(j = i+1; j <= n-1; j++)
                    {
                        l(i,j) = 2*ap::randomreal()-1;
                        l(j,i) = l(i,j);
                    }
                    l(i,i) = 1.1+ap::randomreal();
                    d = d*ap::sqr(l(i,i));
                }
                for(i = 0; i <= n-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        minij = ap::minint(i, j);
                        v = ap::vdotproduct(l.getrow(i, 0, minij), l.getcolumn(j, 0, minij));
                        a(i,j) = v;
                    }
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
                            }
                        }
                        else
                        {
                            if( i<j )
                            {
                                l(i,j) = 0;
                            }
                        }
                    }
                }
                
                //
                // test det(A)
                //
                d2 = spdmatrixdet(a, n, upperin);
                if( d2<0 )
                {
                    wf = true;
                    continue;
                }
                err = ap::maxreal(err, fabs(d2-d)/d);
                
                //
                // test det(cholesky(A))
                //
                d2 = spdmatrixcholeskydet(l, n);
                if( d2<0 )
                {
                    wf = true;
                    continue;
                }
                err = ap::maxreal(err, fabs(d2-d)/d);
            }
        }
    }
    
    //
    // report
    //
    waserrors = err>threshold||wf;
    if( !silent )
    {
        printf("TESTING SPD DET\n");
        printf("ERROR:                                   %5.3le\n",
            double(err));
        printf("ALWAYS SUCCEDED:                         ");
        if( wf )
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
    result = !waserrors;
    return result;
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testdetcholeskyunit_test_silent()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testdetcholesky(true);
    return result;
#else
    return _i_testdetcholeskyunit_test_silent();
#endif
}


/*************************************************************************
Unit test
*************************************************************************/
bool testdetcholeskyunit_test()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testdetcholesky(false);
    return result;
#else
    return _i_testdetcholeskyunit_test();
#endif
}



