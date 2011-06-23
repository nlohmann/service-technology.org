
#include <stdafx.h>
#include <stdio.h>
#include "testldltsolveunit.h"

static void generatematrix(ap::real_2d_array& a, int n, int task);

bool testldltsolve(bool silent)
{
    bool result;
    ap::real_2d_array a;
    ap::real_2d_array a2;
    ap::real_2d_array a3;
    ap::real_1d_array xe;
    ap::real_1d_array x;
    ap::real_1d_array b;
    ap::integer_1d_array p;
    int n;
    int pass;
    int mtask;
    int i;
    int j;
    int k;
    int minij;
    bool upperin;
    bool cr;
    double v;
    double err;
    bool fails;
    bool waserrors;
    int passcount;
    int maxn;
    int htask;
    double threshold;

    err = 0;
    passcount = 100;
    maxn = 20;
    threshold = 10000000*ap::machineepsilon;
    waserrors = false;
    fails = false;
    
    //
    // Test
    //
    for(n = 1; n <= maxn; n++)
    {
        a.setbounds(0, n-1, 0, n-1);
        a2.setbounds(0, n-1, 0, n-1);
        a3.setbounds(0, n-1, 0, n-1);
        xe.setbounds(0, n-1);
        x.setbounds(0, n-1);
        b.setbounds(0, n-1);
        for(mtask = 2; mtask <= 2; mtask++)
        {
            for(htask = 0; htask <= 1; htask++)
            {
                for(pass = 1; pass <= passcount; pass++)
                {
                    upperin = htask==0;
                    
                    //
                    // Prepare task:
                    // * A contains symmetric matrix
                    // * A2, A3 contains its upper (or lower) half
                    //
                    generatematrix(a, n, mtask);
                    for(i = 0; i <= n-1; i++)
                    {
                        for(j = 0; j <= n-1; j++)
                        {
                            a2(i,j) = a(i,j);
                            a3(i,j) = a(i,j);
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
                                    a2(i,j) = 0;
                                    a3(i,j) = 0;
                                }
                            }
                            else
                            {
                                if( i<j )
                                {
                                    a2(i,j) = 0;
                                    a3(i,j) = 0;
                                }
                            }
                        }
                    }
                    
                    //
                    // Prepare XE, B
                    //
                    for(i = 0; i <= n-1; i++)
                    {
                        xe(i) = 2*ap::randomreal()-1;
                    }
                    for(i = 0; i <= n-1; i++)
                    {
                        v = ap::vdotproduct(&a(i, 0), &xe(0), ap::vlen(0,n-1));
                        b(i) = v;
                    }
                    
                    //
                    // solve(A):
                    // 1. MTask=0 means zero matrix, must always fail
                    // 2. MTask=1 means sparse matrix, can fail, can succeed
                    // 3. MTask=2 means dense matrix, must succeed
                    //
                    x.setbounds(0, 0);
                    cr = smatrixsolve(a2, b, n, upperin, x);
                    if( mtask==0 )
                    {
                        fails = fails||cr;
                    }
                    if( mtask==1 )
                    {
                        if( cr )
                        {
                            for(i = 0; i <= n-1; i++)
                            {
                                err = ap::maxreal(err, fabs(x(i)-xe(i)));
                            }
                        }
                    }
                    if( mtask==2 )
                    {
                        if( cr )
                        {
                            for(i = 0; i <= n-1; i++)
                            {
                                err = ap::maxreal(err, fabs(x(i)-xe(i)));
                            }
                        }
                        else
                        {
                            fails = true;
                        }
                    }
                    
                    //
                    // solve(LDLT(A)):
                    // 1. MTask=0 means zero matrix, must always fail
                    // 2. MTask=1 means sparse matrix, can fail, can succeed
                    // 3. MTask=2 means dense matrix, must succeed
                    //
                    x.setbounds(0, 0);
                    smatrixldlt(a3, n, upperin, p);
                    cr = smatrixldltsolve(a3, p, b, n, upperin, x);
                    if( mtask==0 )
                    {
                        fails = fails||cr;
                    }
                    if( mtask==1 )
                    {
                        if( cr )
                        {
                            for(i = 0; i <= n-1; i++)
                            {
                                err = ap::maxreal(err, fabs(x(i)-xe(i)));
                            }
                        }
                    }
                    if( mtask==2 )
                    {
                        if( cr )
                        {
                            for(i = 0; i <= n-1; i++)
                            {
                                err = ap::maxreal(err, fabs(x(i)-xe(i)));
                            }
                        }
                        else
                        {
                            fails = true;
                        }
                    }
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
        printf("TESTING LDLT SOLVER\n");
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


static void generatematrix(ap::real_2d_array& a, int n, int task)
{
    int i;
    int j;

    if( task==0 )
    {
        
        //
        // Zero matrix
        //
        for(i = 0; i <= n-1; i++)
        {
            for(j = 0; j <= n-1; j++)
            {
                a(i,j) = 0;
            }
        }
    }
    if( task==1 )
    {
        
        //
        // Sparse matrix
        //
        for(i = 0; i <= n-1; i++)
        {
            for(j = i+1; j <= n-1; j++)
            {
                if( ap::randomreal()>0.95 )
                {
                    a(i,j) = 2*ap::randomreal()-1;
                }
                else
                {
                    a(i,j) = 0;
                }
                a(j,i) = a(i,j);
            }
            if( ap::randomreal()>0.95 )
            {
                a(i,i) = (2*ap::randominteger(2)-1)*(0.8+ap::randomreal());
            }
            else
            {
                a(i,i) = 0;
            }
        }
    }
    if( task==2 )
    {
        
        //
        // Dense matrix
        //
        for(i = 0; i <= n-1; i++)
        {
            for(j = i+1; j <= n-1; j++)
            {
                a(i,j) = 2*ap::randomreal()-1;
                a(j,i) = a(i,j);
            }
            a(i,i) = (2*ap::randominteger(2)-1)*(0.8+ap::randomreal());
        }
    }
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testldltsolveunit_test_silent()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testldltsolve(true);
    return result;
#else
    return _i_testldltsolveunit_test_silent();
#endif
}


/*************************************************************************
Unit test
*************************************************************************/
bool testldltsolveunit_test()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testldltsolve(false);
    return result;
#else
    return _i_testldltsolveunit_test();
#endif
}



