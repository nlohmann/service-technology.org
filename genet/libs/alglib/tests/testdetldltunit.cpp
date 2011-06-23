
#include <stdafx.h>
#include <stdio.h>
#include "testdetldltunit.h"

static double refdet(const ap::real_2d_array& a0, int n);
static void generatematrix(ap::real_2d_array& a, int n, int task);
static void restorematrix(ap::real_2d_array& a, int n, bool upperin);

bool testdetldlt(bool silent)
{
    bool result;
    ap::real_2d_array a;
    ap::real_2d_array a2;
    ap::real_2d_array a3;
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
    double v1;
    double v2;
    double err;
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
    
    //
    // Test
    //
    for(n = 1; n <= maxn; n++)
    {
        a.setbounds(0, n-1, 0, n-1);
        a2.setbounds(0, n-1, 0, n-1);
        a3.setbounds(0, n-1, 0, n-1);
        for(mtask = 0; mtask <= 2; mtask++)
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
                    // Test 1: det(A2)
                    //
                    v1 = smatrixdet(a2, n, upperin);
                    v2 = refdet(a, n);
                    if( v2!=0 )
                    {
                        err = ap::maxreal(err, fabs((v1-v2)/v2));
                    }
                    else
                    {
                        err = ap::maxreal(err, fabs(v1));
                    }
                    
                    //
                    // Test 2: inv(LDLt(A3))
                    //
                    smatrixldlt(a3, n, upperin, p);
                    v1 = smatrixldltdet(a3, p, n, upperin);
                    v2 = refdet(a, n);
                    if( v2!=0 )
                    {
                        err = ap::maxreal(err, fabs((v1-v2)/v2));
                    }
                    else
                    {
                        err = ap::maxreal(err, fabs(v1));
                    }
                }
            }
        }
    }
    
    //
    // report
    //
    waserrors = err>threshold;
    if( !silent )
    {
        printf("TESTING LDLT DET\n");
        printf("ERROR:                                   %5.3le\n",
            double(err));
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
Reference det subroutine
*************************************************************************/
static double refdet(const ap::real_2d_array& a0, int n)
{
    double result;
    int i;
    int j;
    int k;
    int l;
    int f;
    int z;
    double t;
    double m1;
    ap::real_2d_array a;

    a.setbounds(1, n, 1, n);
    for(i = 1; i <= n; i++)
    {
        for(j = 1; j <= n; j++)
        {
            a(i,j) = a0(i-1,j-1);
        }
    }
    result = 1;
    k = 1;
    do
    {
        m1 = 0;
        i = k;
        while(i<=n)
        {
            t = a(i,k);
            if( fabs(t)>fabs(m1) )
            {
                m1 = t;
                j = i;
            }
            i = i+1;
        }
        if( fabs(m1)==0 )
        {
            result = 0;
            k = n+1;
        }
        else
        {
            if( j!=k )
            {
                result = -result;
                l = k;
                while(l<=n)
                {
                    t = a(j,l);
                    a(j,l) = a(k,l);
                    a(k,l) = t;
                    l = l+1;
                }
            }
            f = k+1;
            while(f<=n)
            {
                t = a(f,k)/m1;
                z = k+1;
                while(z<=n)
                {
                    a(f,z) = a(f,z)-t*a(k,z);
                    z = z+1;
                }
                f = f+1;
            }
            result = result*a(k,k);
        }
        k = k+1;
    }
    while(k<=n);
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
            a(i,i) = (2*ap::randominteger(2)-1)*(0.7+ap::randomreal());
        }
    }
}


static void restorematrix(ap::real_2d_array& a, int n, bool upperin)
{
    int i;
    int j;

    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            if( upperin )
            {
                if( j<i )
                {
                    a(i,j) = a(j,i);
                }
            }
            else
            {
                if( i<j )
                {
                    a(i,j) = a(j,i);
                }
            }
        }
    }
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testdetldltunit_test_silent()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testdetldlt(true);
    return result;
#else
    return _i_testdetldltunit_test_silent();
#endif
}


/*************************************************************************
Unit test
*************************************************************************/
bool testdetldltunit_test()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testdetldlt(false);
    return result;
#else
    return _i_testdetldltunit_test();
#endif
}



