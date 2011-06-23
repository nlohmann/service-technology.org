
#include <stdafx.h>
#include <stdio.h>
#include "testpolynomialinterpolationunit.h"

static double polint(const ap::real_1d_array& x,
     ap::real_1d_array f,
     int n,
     double t);

bool testpolynomialinterpolation(bool silent)
{
    bool result;
    bool waserrors;
    bool guardedaccerrors;
    bool geninterrors;
    bool nevinterrors;
    bool nevdinterrors;
    bool equidistantinterrors;
    bool chebyshev1interrors;
    bool chebyshev2interrors;
    double epstol;
    int n;
    int maxn;
    int i;
    int j;
    int pass;
    int passcount;
    double a;
    double b;
    double p;
    double dp;
    double d2p;
    double q;
    double dq;
    double d2q;
    ap::real_1d_array x;
    ap::real_1d_array y;
    ap::real_1d_array w;
    double v1;
    double v2;
    double v3;
    double h;
    double maxerr;

    waserrors = false;
    maxn = 5;
    passcount = 20;
    epstol = 1.0E+4;
    
    //
    // Testing general barycentric formula
    //
    
    //
    // Crash test: three possible types of overflow
    //
    x.setbounds(0, 2);
    y.setbounds(0, 2);
    w.setbounds(0, 2);
    x(0) = -1;
    y(0) = 1;
    w(0) = 1;
    x(1) = 0;
    y(1) = 0.002;
    w(1) = 1;
    x(2) = 1;
    y(2) = 1;
    w(2) = 1;
    v2 = barycentricinterpolation(x, y, w, 3, double(0));
    v2 = equidistantpolynomialinterpolation(-1.0, 1.0, y, 3, double(0));
    v1 = 0.5;
    while(v1>0)
    {
        v2 = barycentricinterpolation(x, y, w, 3, v1);
        v2 = equidistantpolynomialinterpolation(-1.0, 1.0, y, 3, v1);
        v1 = ap::sqr(v1);
    }
    y(1) = 200;
    v1 = 0.5;
    while(v1>0)
    {
        v2 = barycentricinterpolation(x, y, w, 3, v1);
        v2 = equidistantpolynomialinterpolation(-1.0, 1.0, y, 3, v1);
        v1 = ap::sqr(v1);
    }
    
    //
    // Accuracy test of guarded formula (near-overflow task)
    //
    x.setbounds(0, 2);
    y.setbounds(0, 2);
    w.setbounds(0, 2);
    x(0) = -1;
    y(0) = -1;
    w(0) = 1;
    x(1) = 0;
    y(1) = 0;
    w(1) = -2;
    x(2) = 1;
    y(2) = 1;
    w(2) = 1;
    v1 = 0.5;
    maxerr = 0;
    while(v1>0)
    {
        v2 = barycentricinterpolation(x, y, w, 3, v1);
        maxerr = ap::maxreal(maxerr, fabs(v2-v1)/fabs(v1));
        v2 = equidistantpolynomialinterpolation(-1.0, 1.0, y, 3, v1);
        maxerr = ap::maxreal(maxerr, fabs(v2-v1)/fabs(v1));
        v1 = ap::sqr(v1);
    }
    guardedaccerrors = maxerr>epstol*ap::machineepsilon;
    
    //
    // Testing polynomial interpolation using a
    // general barycentric formula. Test on Chebyshev nodes
    // (to avoid big condition numbers).
    //
    maxerr = 0;
    h = 0.0001;
    for(n = 1; n <= maxn; n++)
    {
        x.setbounds(0, n-1);
        y.setbounds(0, n-1);
        w.setbounds(0, n-1);
        for(pass = 1; pass <= passcount; pass++)
        {
            
            //
            // Prepare task
            //
            v1 = 0.7*ap::randomreal()+0.3;
            for(i = 0; i <= n-1; i++)
            {
                x(i) = cos(i*ap::pi()/n);
                y(i) = cos(v1*x(i))+2.3*x(i)-2;
            }
            for(i = 0; i <= n-1; i++)
            {
                w(i) = 1.0;
                for(j = 0; j <= n-1; j++)
                {
                    if( j!=i )
                    {
                        w(i) = w(i)/(x(i)-x(j));
                    }
                }
            }
            
            //
            // Test at intermediate points
            //
            v1 = -1;
            while(v1<=1.0001)
            {
                v2 = polint(x, y, n, v1);
                v3 = barycentricinterpolation(x, y, w, n, v1);
                maxerr = ap::maxreal(maxerr, fabs(v2-v3));
                v1 = v1+0.01;
            }
            
            //
            // Test at nodes
            //
            for(i = 0; i <= n-1; i++)
            {
                maxerr = ap::maxreal(maxerr, fabs(barycentricinterpolation(x, y, w, n, x(i))-y(i)));
            }
        }
    }
    geninterrors = maxerr>epstol*ap::machineepsilon;
    
    //
    // Testing polynomial interpolation using a Neville's algorithm.
    // Test on Chebyshev nodes (don't want to work with big condition numbers).
    //
    maxerr = 0;
    for(n = 1; n <= maxn; n++)
    {
        x.setbounds(0, n-1);
        y.setbounds(0, n-1);
        for(pass = 1; pass <= passcount; pass++)
        {
            
            //
            // Prepare task
            //
            v1 = 0.7*ap::randomreal()+0.3;
            for(i = 0; i <= n-1; i++)
            {
                x(i) = cos(i*ap::pi()/n);
                y(i) = cos(v1*x(i))+2.3*x(i)-2;
            }
            
            //
            // Test
            //
            v1 = -1;
            while(v1<=1.0001)
            {
                v2 = polint(x, y, n, v1);
                v3 = nevilleinterpolation(x, y, n, v1);
                maxerr = ap::maxreal(maxerr, fabs(v2-v3));
                v1 = v1+0.01;
            }
        }
    }
    nevinterrors = maxerr>epstol*ap::machineepsilon;
    
    //
    // Testing polynomial interpolation using an
    // equidistant barycentric algorithm.
    //
    maxerr = 0;
    for(n = 1; n <= maxn; n++)
    {
        x.setbounds(0, n-1);
        y.setbounds(0, n-1);
        for(pass = 1; pass <= passcount; pass++)
        {
            
            //
            // Prepare task
            //
            v1 = 0.7*ap::randomreal()+0.3;
            a = -(0.5+0.5*ap::randomreal());
            b = +(0.5+0.5*ap::randomreal());
            for(i = 0; i <= n-1; i++)
            {
                x(i) = a+(b-a)*i/ap::maxint(n-1, 1);
                y(i) = cos(v1*x(i))+2.3*x(i)-2;
            }
            
            //
            // Test
            //
            v1 = -1;
            while(v1<=1.0001)
            {
                v2 = polint(x, y, n, v1);
                v3 = equidistantpolynomialinterpolation(a, b, y, n, v1);
                maxerr = ap::maxreal(maxerr, fabs(v2-v3));
                v1 = v1+0.01;
            }
        }
    }
    equidistantinterrors = maxerr>epstol*ap::machineepsilon;
    
    //
    // Testing polynomial interpolation using
    // Chebyshev-1 barycentric algorithm.
    //
    maxerr = 0;
    for(n = 1; n <= maxn; n++)
    {
        x.setbounds(0, n-1);
        y.setbounds(0, n-1);
        for(pass = 1; pass <= passcount; pass++)
        {
            
            //
            // Prepare task
            //
            v1 = 0.7*ap::randomreal()+0.3;
            a = -(0.5+0.5*ap::randomreal());
            b = +(0.5+0.5*ap::randomreal());
            for(i = 0; i <= n-1; i++)
            {
                x(i) = 0.5*(a+b)+0.5*(b-a)*cos(ap::pi()*(2*i+1)/(2*(n-1)+2));
                y(i) = cos(v1*x(i))+2.3*x(i)-2;
            }
            
            //
            // Test
            //
            v1 = a;
            while(v1<=b)
            {
                v2 = polint(x, y, n, v1);
                v3 = chebyshev1interpolation(a, b, y, n, v1);
                maxerr = ap::maxreal(maxerr, fabs(v2-v3));
                v1 = v1+0.01;
            }
        }
    }
    chebyshev1interrors = maxerr>epstol*ap::machineepsilon;
    
    //
    // Testing polynomial interpolation using
    // Chebyshev-2 barycentric algorithm.
    //
    maxerr = 0;
    for(n = 2; n <= maxn; n++)
    {
        x.setbounds(0, n-1);
        y.setbounds(0, n-1);
        for(pass = 1; pass <= passcount; pass++)
        {
            
            //
            // Prepare task
            //
            v1 = 0.7*ap::randomreal()+0.3;
            a = -(0.5+0.5*ap::randomreal());
            b = +(0.5+0.5*ap::randomreal());
            for(i = 0; i <= n-1; i++)
            {
                x(i) = 0.5*(a+b)+0.5*(b-a)*cos(ap::pi()*i/(n-1));
                y(i) = cos(v1*x(i))+2.3*x(i)-2;
            }
            
            //
            // Test
            //
            v1 = a;
            while(v1<=b)
            {
                v2 = polint(x, y, n, v1);
                v3 = chebyshev2interpolation(a, b, y, n, v1);
                maxerr = ap::maxreal(maxerr, fabs(v2-v3));
                v1 = v1+0.01;
            }
        }
    }
    chebyshev2interrors = maxerr>epstol*ap::machineepsilon;
    
    //
    // Testing polynomial interpolation using a Neville's algorithm.
    // with derivatives. Test on Chebyshev nodes (don't want to work
    // with big condition numbers).
    //
    maxerr = 0;
    h = 0.0001;
    for(n = 1; n <= maxn; n++)
    {
        x.setbounds(0, n-1);
        y.setbounds(0, n-1);
        for(pass = 1; pass <= passcount; pass++)
        {
            
            //
            // Prepare task
            //
            v2 = 0.7*ap::randomreal()+0.3;
            for(i = 0; i <= n-1; i++)
            {
                x(i) = cos(i*ap::pi()/n);
                y(i) = cos(v2*x(i));
            }
            
            //
            // Test
            //
            v1 = -1;
            while(v1<=1.0001)
            {
                p = polint(x, y, n, v1);
                dp = (polint(x, y, n, v1+h)-polint(x, y, n, v1-h))/(2*h);
                d2p = (polint(x, y, n, v1-h)-2*polint(x, y, n, v1)+polint(x, y, n, v1+h))/ap::sqr(h);
                nevilledifferentiation(x, y, n, v1, q, dq, d2q);
                maxerr = ap::maxreal(maxerr, fabs(p-q));
                maxerr = ap::maxreal(maxerr, fabs(dp-dq));
                maxerr = ap::maxreal(maxerr, fabs(d2p-d2q));
                v1 = v1+0.01;
            }
        }
    }
    nevdinterrors = maxerr>0.001;
    
    //
    // report
    //
    waserrors = guardedaccerrors||geninterrors||nevinterrors||equidistantinterrors||chebyshev1interrors||chebyshev2interrors||nevdinterrors;
    if( !silent )
    {
        printf("TESTING POLYNOMIAL INTERPOLATION\n");
        
        //
        // Normal tests
        //
        printf("BARYCENTRIC INTERPOLATION TEST:          ");
        if( geninterrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("NEVILLE INTERPOLATON TEST:               ");
        if( nevinterrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("EQUIDISTANT INTERPOLATON TEST:           ");
        if( equidistantinterrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("CHEBYSHEV-1 INTERPOLATON TEST:           ");
        if( chebyshev1interrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("CHEBYSHEV-2 INTERPOLATON TEST:           ");
        if( chebyshev2interrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("NEVILLE DIFFERENTIATION TEST:            ");
        if( nevdinterrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("GUARDED FORMULA ACCURACY TEST:           ");
        if( guardedaccerrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        
        //
        // if we are here then crash test is passed :)
        //
        printf("CRASH TEST:                              OK\n");
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


static double polint(const ap::real_1d_array& x,
     ap::real_1d_array f,
     int n,
     double t)
{
    double result;
    int i;
    int j;

    n = n-1;
    for(j = 0; j <= n-1; j++)
    {
        for(i = j+1; i <= n; i++)
        {
            f(i) = ((t-x(j))*f(i)-(t-x(i))*f(j))/(x(i)-x(j));
        }
    }
    result = f(n);
    return result;
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testpolynomialinterpolationunit_test_silent()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testpolynomialinterpolation(true);
    return result;
#else
    return _i_testpolynomialinterpolationunit_test_silent();
#endif
}


/*************************************************************************
Unit test
*************************************************************************/
bool testpolynomialinterpolationunit_test()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testpolynomialinterpolation(false);
    return result;
#else
    return _i_testpolynomialinterpolationunit_test();
#endif
}



