
#include <stdafx.h>
#include <stdio.h>
#include "testsplineinterpolationunit.h"

static void lconst(double a,
     double b,
     const ap::real_1d_array& c,
     double lstep,
     double& l0,
     double& l1,
     double& l2);
static bool testunpack(const ap::real_1d_array& c, const ap::real_1d_array& x);

bool testsplineinterpolation(bool silent)
{
    bool result;
    bool waserrors;
    bool cserrors;
    bool hserrors;
    bool aserrors;
    bool lserrors;
    bool dserrors;
    bool uperrors;
    bool cperrors;
    bool lterrors;
    bool ierrors;
    int n;
    int i;
    int k;
    int pass;
    int passcount;
    int bltype;
    int brtype;
    ap::real_1d_array x;
    ap::real_1d_array y;
    ap::real_1d_array y2;
    ap::real_1d_array d;
    ap::real_1d_array c;
    ap::real_1d_array c2;
    double a;
    double b;
    double bl;
    double br;
    double t;
    double sa;
    double sb;
    double v;
    double lstep;
    double h;
    double l10;
    double l11;
    double l12;
    double l20;
    double l21;
    double l22;
    double s;
    double ds;
    double d2s;
    double s2;
    double ds2;
    double d2s2;
    double vl;
    double vm;
    double vr;
    double err;

    waserrors = false;
    passcount = 20;
    lstep = 0.005;
    h = 0.00001;
    lserrors = false;
    cserrors = false;
    hserrors = false;
    aserrors = false;
    dserrors = false;
    cperrors = false;
    uperrors = false;
    lterrors = false;
    ierrors = false;
    
    //
    // General test: linear, cubic, Hermite, Akima
    //
    for(n = 2; n <= 8; n++)
    {
        x.setbounds(0, n-1);
        y.setbounds(0, n-1);
        d.setbounds(0, n-1);
        for(pass = 1; pass <= passcount; pass++)
        {
            
            //
            // Prepare task
            //
            a = -1-ap::randomreal();
            b = +1+ap::randomreal();
            bl = 2*ap::randomreal()-1;
            br = 2*ap::randomreal()-1;
            for(i = 0; i <= n-1; i++)
            {
                x(i) = 0.5*(b+a)+0.5*(b-a)*cos(ap::pi()*(2*i+1)/(2*n));
                if( i==0 )
                {
                    x(i) = a;
                }
                if( i==n-1 )
                {
                    x(i) = b;
                }
                y(i) = cos(1.3*ap::pi()*x(i)+0.4);
                d(i) = -1.3*ap::pi()*sin(1.3*ap::pi()*x(i)+0.4);
            }
            for(i = 0; i <= n-1; i++)
            {
                k = ap::randominteger(n);
                if( k!=i )
                {
                    t = x(i);
                    x(i) = x(k);
                    x(k) = t;
                    t = y(i);
                    y(i) = y(k);
                    y(k) = t;
                    t = d(i);
                    d(i) = d(k);
                    d(k) = t;
                }
            }
            
            //
            // Build linear spline
            // Test for general interpolation scheme properties:
            // * values at nodes
            // * continuous function
            // Test for specific properties is implemented below.
            //
            buildlinearspline(x, y, n, c);
            err = 0;
            for(i = 0; i <= n-1; i++)
            {
                err = ap::maxreal(err, fabs(y(i)-splineinterpolation(c, x(i))));
            }
            lserrors = lserrors||err>100*ap::machineepsilon;
            lconst(a, b, c, lstep, l10, l11, l12);
            lconst(a, b, c, lstep/3, l20, l21, l22);
            lserrors = lserrors||l20/l10>1.2;
            
            //
            // Build cubic spline.
            // Test for interpolation scheme properties:
            // * values at nodes
            // * boundary conditions
            // * continuous function
            // * continuous first derivative
            // * continuous second derivative
            //
            for(bltype = 0; bltype <= 2; bltype++)
            {
                for(brtype = 0; brtype <= 2; brtype++)
                {
                    buildcubicspline(x, y, n, bltype, bl, brtype, br, c);
                    err = 0;
                    for(i = 0; i <= n-1; i++)
                    {
                        err = ap::maxreal(err, fabs(y(i)-splineinterpolation(c, x(i))));
                    }
                    cserrors = cserrors||err>100*ap::machineepsilon;
                    err = 0;
                    if( bltype==0 )
                    {
                        splinedifferentiation(c, a-h, s, ds, d2s);
                        splinedifferentiation(c, a+h, s2, ds2, d2s2);
                        t = (d2s2-d2s)/(2*h);
                        err = ap::maxreal(err, fabs(t));
                    }
                    if( bltype==1 )
                    {
                        t = (splineinterpolation(c, a+h)-splineinterpolation(c, a-h))/(2*h);
                        err = ap::maxreal(err, fabs(bl-t));
                    }
                    if( bltype==2 )
                    {
                        t = (splineinterpolation(c, a+h)-2*splineinterpolation(c, a)+splineinterpolation(c, a-h))/ap::sqr(h);
                        err = ap::maxreal(err, fabs(bl-t));
                    }
                    if( brtype==0 )
                    {
                        splinedifferentiation(c, b-h, s, ds, d2s);
                        splinedifferentiation(c, b+h, s2, ds2, d2s2);
                        t = (d2s2-d2s)/(2*h);
                        err = ap::maxreal(err, fabs(t));
                    }
                    if( brtype==1 )
                    {
                        t = (splineinterpolation(c, b+h)-splineinterpolation(c, b-h))/(2*h);
                        err = ap::maxreal(err, fabs(br-t));
                    }
                    if( brtype==2 )
                    {
                        t = (splineinterpolation(c, b+h)-2*splineinterpolation(c, b)+splineinterpolation(c, b-h))/ap::sqr(h);
                        err = ap::maxreal(err, fabs(br-t));
                    }
                    cserrors = cserrors||err>1.0E-3;
                    lconst(a, b, c, lstep, l10, l11, l12);
                    lconst(a, b, c, lstep/3, l20, l21, l22);
                    cserrors = cserrors||l20/l10>1.2&&l10>1.0E-6;
                    cserrors = cserrors||l21/l11>1.2&&l11>1.0E-6;
                    cserrors = cserrors||l22/l12>1.2&&l12>1.0E-6;
                }
            }
            
            //
            // Build Hermite spline.
            // Test for interpolation scheme properties:
            // * values and derivatives at nodes
            // * continuous function
            // * continuous first derivative
            //
            buildhermitespline(x, y, d, n, c);
            err = 0;
            for(i = 0; i <= n-1; i++)
            {
                err = ap::maxreal(err, fabs(y(i)-splineinterpolation(c, x(i))));
            }
            hserrors = hserrors||err>100*ap::machineepsilon;
            err = 0;
            for(i = 0; i <= n-1; i++)
            {
                t = (splineinterpolation(c, x(i)+h)-splineinterpolation(c, x(i)-h))/(2*h);
                err = ap::maxreal(err, fabs(d(i)-t));
            }
            hserrors = hserrors||err>1.0E-3;
            lconst(a, b, c, lstep, l10, l11, l12);
            lconst(a, b, c, lstep/3, l20, l21, l22);
            hserrors = hserrors||l20/l10>1.2;
            hserrors = hserrors||l21/l11>1.2;
            
            //
            // Build Akima spline
            // Test for general interpolation scheme properties:
            // * values at nodes
            // * continuous function
            // * continuous first derivative
            // Test for specific properties is implemented below.
            //
            if( n>=5 )
            {
                buildakimaspline(x, y, n, c);
                err = 0;
                for(i = 0; i <= n-1; i++)
                {
                    err = ap::maxreal(err, fabs(y(i)-splineinterpolation(c, x(i))));
                }
                aserrors = aserrors||err>100*ap::machineepsilon;
                lconst(a, b, c, lstep, l10, l11, l12);
                lconst(a, b, c, lstep/3, l20, l21, l22);
                hserrors = hserrors||l20/l10>1.2;
                hserrors = hserrors||l21/l11>1.2;
            }
        }
    }
    
    //
    // Special linear spline test:
    // test for linearity between x[i] and x[i+1]
    //
    for(n = 2; n <= 10; n++)
    {
        x.setbounds(0, n-1);
        y.setbounds(0, n-1);
        
        //
        // Prepare task
        //
        a = -1;
        b = +1;
        for(i = 0; i <= n-1; i++)
        {
            x(i) = a+(b-a)*i/(n-1);
            y(i) = 2*ap::randomreal()-1;
        }
        buildlinearspline(x, y, n, c);
        
        //
        // Test
        //
        err = 0;
        for(k = 0; k <= n-2; k++)
        {
            a = x(k);
            b = x(k+1);
            for(pass = 1; pass <= passcount; pass++)
            {
                t = a+(b-a)*ap::randomreal();
                v = y(k)+(t-a)/(b-a)*(y(k+1)-y(k));
                err = ap::maxreal(err, fabs(splineinterpolation(c, t)-v));
            }
        }
        lserrors = lserrors||err>100*ap::machineepsilon;
    }
    
    //
    // Special Akima test: test outlier sensitivity
    // Spline value at (x[i], x[i+1]) should depend from
    // f[i-2], f[i-1], f[i], f[i+1], f[i+2], f[i+3] only.
    //
    for(n = 5; n <= 10; n++)
    {
        x.setbounds(0, n-1);
        y.setbounds(0, n-1);
        y2.setbounds(0, n-1);
        
        //
        // Prepare unperturbed Akima spline
        //
        a = -1;
        b = +1;
        for(i = 0; i <= n-1; i++)
        {
            x(i) = a+(b-a)*i/(n-1);
            y(i) = cos(1.3*ap::pi()*x(i)+0.4);
        }
        buildakimaspline(x, y, n, c);
        
        //
        // Process perturbed tasks
        //
        err = 0;
        for(k = 0; k <= n-1; k++)
        {
            ap::vmove(&y2(0), &y(0), ap::vlen(0,n-1));
            y2(k) = 5;
            buildakimaspline(x, y2, n, c2);
            
            //
            // Test left part independence
            //
            if( k-3>=1 )
            {
                a = -1;
                b = x(k-3);
                for(pass = 1; pass <= passcount; pass++)
                {
                    t = a+(b-a)*ap::randomreal();
                    err = ap::maxreal(err, fabs(splineinterpolation(c, t)-splineinterpolation(c2, t)));
                }
            }
            
            //
            // Test right part independence
            //
            if( k+3<=n-2 )
            {
                a = x(k+3);
                b = +1;
                for(pass = 1; pass <= passcount; pass++)
                {
                    t = a+(b-a)*ap::randomreal();
                    err = ap::maxreal(err, fabs(splineinterpolation(c, t)-splineinterpolation(c2, t)));
                }
            }
        }
        aserrors = aserrors||err>100*ap::machineepsilon;
    }
    
    //
    // Differentiation, unpack test
    //
    for(n = 2; n <= 10; n++)
    {
        x.setbounds(0, n-1);
        y.setbounds(0, n-1);
        
        //
        // Prepare cubic spline
        //
        a = -1-ap::randomreal();
        b = +1+ap::randomreal();
        for(i = 0; i <= n-1; i++)
        {
            x(i) = a+(b-a)*i/(n-1);
            y(i) = cos(1.3*ap::pi()*x(i)+0.4);
        }
        buildcubicspline(x, y, n, 2, 0.0, 2, 0.0, c);
        
        //
        // Test diff
        //
        err = 0;
        for(pass = 1; pass <= passcount; pass++)
        {
            t = a+(b-a)*ap::randomreal();
            splinedifferentiation(c, t, s, ds, d2s);
            vl = splineinterpolation(c, t-h);
            vm = splineinterpolation(c, t);
            vr = splineinterpolation(c, t+h);
            err = ap::maxreal(err, fabs(s-vm));
            err = ap::maxreal(err, fabs(ds-(vr-vl)/(2*h)));
            err = ap::maxreal(err, fabs(d2s-(vr-2*vm+vl)/ap::sqr(h)));
        }
        dserrors = dserrors||err>0.001;
        
        //
        // Test copy
        //
        splinecopy(c, c2);
        err = 0;
        for(pass = 1; pass <= passcount; pass++)
        {
            t = a+(b-a)*ap::randomreal();
            err = ap::maxreal(err, fabs(splineinterpolation(c, t)-splineinterpolation(c2, t)));
        }
        cperrors = cperrors||err>100*ap::machineepsilon;
        
        //
        // Test unpack
        //
        uperrors = uperrors||!testunpack(c, x);
        
        //
        // Test lin.trans.
        //
        err = 0;
        for(pass = 1; pass <= passcount; pass++)
        {
            
            //
            // LinTransX, general A
            //
            sa = 4*ap::randomreal()-2;
            sb = 2*ap::randomreal()-1;
            t = a+(b-a)*ap::randomreal();
            splinecopy(c, c2);
            splinelintransx(c2, sa, sb);
            err = ap::maxreal(err, fabs(splineinterpolation(c, t)-splineinterpolation(c2, (t-sb)/sa)));
            
            //
            // LinTransX, special case: A=0
            //
            sb = 2*ap::randomreal()-1;
            t = a+(b-a)*ap::randomreal();
            splinecopy(c, c2);
            splinelintransx(c2, double(0), sb);
            err = ap::maxreal(err, fabs(splineinterpolation(c, sb)-splineinterpolation(c2, t)));
            
            //
            // LinTransY
            //
            sa = 2*ap::randomreal()-1;
            sb = 2*ap::randomreal()-1;
            t = a+(b-a)*ap::randomreal();
            splinecopy(c, c2);
            splinelintransy(c2, sa, sb);
            err = ap::maxreal(err, fabs(sa*splineinterpolation(c, t)+sb-splineinterpolation(c2, t)));
        }
        lterrors = lterrors||err>100*ap::machineepsilon;
    }
    
    //
    // Testing integration
    //
    err = 0;
    for(n = 20; n <= 35; n++)
    {
        x.setbounds(0, n-1);
        y.setbounds(0, n-1);
        for(pass = 1; pass <= passcount; pass++)
        {
            
            //
            // Prepare cubic spline
            //
            a = -1-0.2*ap::randomreal();
            b = +1+0.2*ap::randomreal();
            for(i = 0; i <= n-1; i++)
            {
                x(i) = a+(b-a)*i/(n-1);
                y(i) = sin(ap::pi()*x(i)+0.4)+exp(x(i));
            }
            bl = ap::pi()*cos(ap::pi()*a+0.4)+exp(a);
            br = ap::pi()*cos(ap::pi()*b+0.4)+exp(b);
            buildcubicspline(x, y, n, 1, bl, 1, br, c);
            
            //
            // Test
            //
            t = a+(b-a)*ap::randomreal();
            v = -cos(ap::pi()*a+0.4)/ap::pi()+exp(a);
            v = -cos(ap::pi()*t+0.4)/ap::pi()+exp(t)-v;
            v = v-splineintegration(c, t);
            err = ap::maxreal(err, fabs(v));
        }
    }
    ierrors = ierrors||err>0.001;
    
    //
    // report
    //
    waserrors = lserrors||cserrors||hserrors||aserrors||dserrors||cperrors||uperrors||lterrors||ierrors;
    if( !silent )
    {
        printf("TESTING SPLINE INTERPOLATION\n");
        
        //
        // Normal tests
        //
        printf("LINEAR SPLINE TEST:                      ");
        if( lserrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("CUBIC SPLINE TEST:                       ");
        if( cserrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("HERMITE SPLINE TEST:                     ");
        if( hserrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("AKIMA SPLINE TEST:                       ");
        if( aserrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("DIFFERENTIATION TEST:                    ");
        if( dserrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("COPY TEST:                               ");
        if( cperrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("UNPACK TEST:                             ");
        if( uperrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("LIN.TRANS. TEST:                         ");
        if( lterrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("INTEGRATION TEST:                        ");
        if( ierrors )
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
Lipschitz constants for spline inself, first and second derivatives.
*************************************************************************/
static void lconst(double a,
     double b,
     const ap::real_1d_array& c,
     double lstep,
     double& l0,
     double& l1,
     double& l2)
{
    double t;
    double vl;
    double vm;
    double vr;
    double prevf;
    double prevd;
    double prevd2;
    double f;
    double d;
    double d2;

    l0 = 0;
    l1 = 0;
    l2 = 0;
    t = a-0.1;
    vl = splineinterpolation(c, t-2*lstep);
    vm = splineinterpolation(c, t-lstep);
    vr = splineinterpolation(c, t);
    f = vm;
    d = (vr-vl)/(2*lstep);
    d2 = (vr-2*vm+vl)/ap::sqr(lstep);
    while(t<=b+0.1)
    {
        prevf = f;
        prevd = d;
        prevd2 = d2;
        vl = vm;
        vm = vr;
        vr = splineinterpolation(c, t+lstep);
        f = vm;
        d = (vr-vl)/(2*lstep);
        d2 = (vr-2*vm+vl)/ap::sqr(lstep);
        l0 = ap::maxreal(l0, fabs((f-prevf)/lstep));
        l1 = ap::maxreal(l1, fabs((d-prevd)/lstep));
        l2 = ap::maxreal(l2, fabs((d2-prevd2)/lstep));
        t = t+lstep;
    }
}


/*************************************************************************
Lipschitz constants for spline inself, first and second derivatives.
*************************************************************************/
static bool testunpack(const ap::real_1d_array& c,
     const ap::real_1d_array& x)
{
    bool result;
    int i;
    int n;
    double err;
    double t;
    double v1;
    double v2;
    int pass;
    int passcount;
    ap::real_2d_array tbl;

    passcount = 20;
    err = 0;
    splineunpack(c, n, tbl);
    for(i = 0; i <= n-2; i++)
    {
        for(pass = 1; pass <= passcount; pass++)
        {
            t = ap::randomreal()*(tbl(i,1)-tbl(i,0));
            v1 = tbl(i,2)+t*tbl(i,3)+ap::sqr(t)*tbl(i,4)+t*ap::sqr(t)*tbl(i,5);
            v2 = splineinterpolation(c, tbl(i,0)+t);
            err = ap::maxreal(err, fabs(v1-v2));
        }
    }
    for(i = 0; i <= n-2; i++)
    {
        err = ap::maxreal(err, fabs(x(i)-tbl(i,0)));
    }
    for(i = 0; i <= n-2; i++)
    {
        err = ap::maxreal(err, fabs(x(i+1)-tbl(i,1)));
    }
    result = err<100*ap::machineepsilon;
    return result;
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testsplineinterpolationunit_test_silent()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testsplineinterpolation(true);
    return result;
#else
    return _i_testsplineinterpolationunit_test_silent();
#endif
}


/*************************************************************************
Unit test
*************************************************************************/
bool testsplineinterpolationunit_test()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testsplineinterpolation(false);
    return result;
#else
    return _i_testsplineinterpolationunit_test();
#endif
}



