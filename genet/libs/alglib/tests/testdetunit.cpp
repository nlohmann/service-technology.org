
#include <stdafx.h>
#include <stdio.h>
#include "testdetunit.h"

static bool deterrors;
static double threshold;

static void fillsparsea(ap::real_2d_array& a, int m, int n, double sparcity);
static void testproblem(const ap::real_2d_array& a, int n);
static void makeacopy(const ap::real_2d_array& a,
     int m,
     int n,
     ap::real_2d_array& b);
static double dettriangle(ap::real_2d_array a, const int& n);

/*************************************************************************
Main unittest subroutine
*************************************************************************/
bool testdet(bool silent)
{
    bool result;
    int maxn;
    int gpasscount;
    ap::real_2d_array a;
    int n;
    int gpass;
    int i;
    int j;
    bool waserrors;

    deterrors = false;
    waserrors = false;
    maxn = 8;
    gpasscount = 5;
    threshold = 1.0E-6;
    a.setbounds(0, maxn-1, 0, maxn-1);
    
    //
    // Different problems
    //
    for(gpass = 1; gpass <= gpasscount; gpass++)
    {
        
        //
        // zero matrix, several cases
        //
        for(i = 0; i <= maxn-1; i++)
        {
            for(j = 0; j <= maxn-1; j++)
            {
                a(i,j) = 0;
            }
        }
        for(i = 1; i <= maxn; i++)
        {
            testproblem(a, i);
        }
        
        //
        // Dense matrices
        //
        for(n = 1; n <= maxn; n++)
        {
            for(i = 0; i <= n-1; i++)
            {
                for(j = 0; j <= n-1; j++)
                {
                    a(i,j) = 2*ap::randomreal()-1;
                }
            }
            testproblem(a, n);
        }
        
        //
        // Sparse matrices, very sparse matrices, incredible sparse matrices
        //
        for(n = 1; n <= maxn; n++)
        {
            fillsparsea(a, n, n, 0.8);
            testproblem(a, n);
            fillsparsea(a, n, n, 0.9);
            testproblem(a, n);
            fillsparsea(a, n, n, 0.95);
            testproblem(a, n);
        }
    }
    
    //
    // report
    //
    waserrors = deterrors;
    if( !silent )
    {
        printf("TESTING DET\n");
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
Sparse matrix
*************************************************************************/
static void fillsparsea(ap::real_2d_array& a, int m, int n, double sparcity)
{
    int i;
    int j;

    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            if( ap::randomreal()>=sparcity )
            {
                a(i,j) = 2*ap::randomreal()-1;
            }
            else
            {
                a(i,j) = 0;
            }
        }
    }
}


/*************************************************************************
Problem testing
*************************************************************************/
static void testproblem(const ap::real_2d_array& a, int n)
{
    int i;
    int j;
    ap::real_2d_array b;
    ap::real_2d_array c;
    ap::integer_1d_array pivots;
    double v1;
    double v2;
    double ve;

    b.setbounds(1, n, 1, n);
    for(i = 1; i <= n; i++)
    {
        for(j = 1; j <= n; j++)
        {
            b(i,j) = a(i-1,j-1);
        }
    }
    ve = dettriangle(b, n);
    v1 = rmatrixdet(a, n);
    makeacopy(a, n, n, c);
    rmatrixlu(c, n, n, pivots);
    v2 = rmatrixludet(c, pivots, n);
    if( fabs(ve)>threshold )
    {
        deterrors = deterrors||fabs((v1-ve)/ve)>threshold;
        deterrors = deterrors||fabs((v2-ve)/ve)>threshold;
    }
    else
    {
        deterrors = deterrors||fabs(v1)>threshold;
        deterrors = deterrors||fabs(v2)>threshold;
    }
}


/*************************************************************************
Copy
*************************************************************************/
static void makeacopy(const ap::real_2d_array& a,
     int m,
     int n,
     ap::real_2d_array& b)
{
    int i;
    int j;

    b.setbounds(0, m-1, 0, n-1);
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            b(i,j) = a(i,j);
        }
    }
}


/*************************************************************************
Basic det subroutine
*************************************************************************/
static double dettriangle(ap::real_2d_array a, const int& n)
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


/*************************************************************************
Silent unit test
*************************************************************************/
bool testdetunit_test_silent()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testdet(true);
    return result;
#else
    return _i_testdetunit_test_silent();
#endif
}


/*************************************************************************
Unit test
*************************************************************************/
bool testdetunit_test()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testdet(false);
    return result;
#else
    return _i_testdetunit_test();
#endif
}



