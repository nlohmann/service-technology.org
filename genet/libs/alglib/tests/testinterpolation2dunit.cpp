
#include <stdafx.h>
#include <stdio.h>
#include "testinterpolation2dunit.h"

static void lconst(const ap::real_1d_array& c,
     const ap::real_1d_array& lx,
     const ap::real_1d_array& ly,
     int m,
     int n,
     double lstep,
     double& lc,
     double& lcx,
     double& lcy,
     double& lcxy);
static void twodnumder(const ap::real_1d_array& c,
     double x,
     double y,
     double h,
     double& f,
     double& fx,
     double& fy,
     double& fxy);
static bool testunpack(const ap::real_1d_array& c,
     const ap::real_1d_array& lx,
     const ap::real_1d_array& ly);
static bool testlintrans(const ap::real_1d_array& c,
     double ax,
     double bx,
     double ay,
     double by);

bool test2dinterpolation(bool silent)
{
    bool result;
    bool waserrors;
    bool blerrors;
    bool bcerrors;
    bool dserrors;
    bool cperrors;
    bool uperrors;
    bool lterrors;
    bool syerrors;
    bool rlerrors;
    bool rcerrors;
    int pass;
    int passcount;
    int jobtype;
    double lstep;
    double h;
    ap::real_1d_array x;
    ap::real_1d_array y;
    ap::real_1d_array c;
    ap::real_1d_array c2;
    ap::real_1d_array lx;
    ap::real_1d_array ly;
    ap::real_2d_array f;
    ap::real_2d_array fr;
    ap::real_2d_array ft;
    double ax;
    double ay;
    double bx;
    double by;
    int i;
    int j;
    int k;
    int n;
    int m;
    int n2;
    int m2;
    double err;
    double t;
    double t1;
    double t2;
    double l1;
    double l1x;
    double l1y;
    double l1xy;
    double l2;
    double l2x;
    double l2y;
    double l2xy;
    double fm;
    double f1;
    double f2;
    double f3;
    double f4;
    double v1;
    double v1x;
    double v1y;
    double v1xy;
    double v2;
    double v2x;
    double v2y;
    double v2xy;
    double mf;

    waserrors = false;
    passcount = 10;
    h = 0.00001;
    lstep = 0.001;
    blerrors = false;
    bcerrors = false;
    dserrors = false;
    cperrors = false;
    uperrors = false;
    lterrors = false;
    syerrors = false;
    rlerrors = false;
    rcerrors = false;
    
    //
    // Test: bilinear, bicubic
    //
    for(n = 2; n <= 7; n++)
    {
        for(m = 2; m <= 7; m++)
        {
            x.setbounds(0, n-1);
            y.setbounds(0, m-1);
            lx.setbounds(0, 2*n-2);
            ly.setbounds(0, 2*m-2);
            f.setbounds(0, m-1, 0, n-1);
            ft.setbounds(0, n-1, 0, m-1);
            for(pass = 1; pass <= passcount; pass++)
            {
                
                //
                // Prepare task:
                // * X and Y stores grid
                // * F stores function values
                // * LX and LY stores twice dense grid (for Lipschitz testing)
                //
                ax = -1-ap::randomreal();
                bx = +1+ap::randomreal();
                ay = -1-ap::randomreal();
                by = +1+ap::randomreal();
                for(j = 0; j <= n-1; j++)
                {
                    x(j) = 0.5*(bx+ax)-0.5*(bx-ax)*cos(ap::pi()*(2*j+1)/(2*n));
                    if( j==0 )
                    {
                        x(j) = ax;
                    }
                    if( j==n-1 )
                    {
                        x(j) = bx;
                    }
                    lx(2*j) = x(j);
                    if( j>0 )
                    {
                        lx(2*j-1) = 0.5*(x(j)+x(j-1));
                    }
                }
                for(j = 0; j <= n-1; j++)
                {
                    k = ap::randominteger(n);
                    if( k!=j )
                    {
                        t = x(j);
                        x(j) = x(k);
                        x(k) = t;
                    }
                }
                for(i = 0; i <= m-1; i++)
                {
                    y(i) = 0.5*(by+ay)-0.5*(by-ay)*cos(ap::pi()*(2*i+1)/(2*m));
                    if( i==0 )
                    {
                        y(i) = ay;
                    }
                    if( i==m-1 )
                    {
                        y(i) = by;
                    }
                    ly(2*i) = y(i);
                    if( i>0 )
                    {
                        ly(2*i-1) = 0.5*(y(i)+y(i-1));
                    }
                }
                for(i = 0; i <= m-1; i++)
                {
                    k = ap::randominteger(m);
                    if( k!=i )
                    {
                        t = y(i);
                        y(i) = y(k);
                        y(k) = t;
                    }
                }
                for(i = 0; i <= m-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        f(i,j) = exp(0.6*x(j))-exp(-0.3*y(i)+0.08*x(j))+2*cos(ap::pi()*(x(j)+1.2*y(i)))+0.1*cos(20*x(j)+15*y(i));
                    }
                }
                
                //
                // Test bilinear interpolation:
                // * interpolation at the nodes
                // * linearity
                // * continuity
                // * differentiation in the inner points
                //
                buildbilinearspline(x, y, f, m, n, c);
                err = 0;
                for(i = 0; i <= m-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        err = ap::maxreal(err, fabs(f(i,j)-splineinterpolation2d(c, x(j), y(i))));
                    }
                }
                blerrors = blerrors||err>10000*ap::machineepsilon;
                err = 0;
                for(i = 0; i <= m-2; i++)
                {
                    for(j = 0; j <= n-2; j++)
                    {
                        
                        //
                        // Test for linearity between grid points
                        // (test point - geometric center of the cell)
                        //
                        fm = splineinterpolation2d(c, lx(2*j+1), ly(2*i+1));
                        f1 = splineinterpolation2d(c, lx(2*j), ly(2*i));
                        f2 = splineinterpolation2d(c, lx(2*j+2), ly(2*i));
                        f3 = splineinterpolation2d(c, lx(2*j+2), ly(2*i+2));
                        f4 = splineinterpolation2d(c, lx(2*j), ly(2*i+2));
                        err = ap::maxreal(err, fabs(0.25*(f1+f2+f3+f4)-fm));
                    }
                }
                blerrors = blerrors||err>10000*ap::machineepsilon;
                lconst(c, lx, ly, m, n, lstep, l1, l1x, l1y, l1xy);
                lconst(c, lx, ly, m, n, lstep/3, l2, l2x, l2y, l2xy);
                blerrors = blerrors||l2/l1>1.2;
                err = 0;
                for(i = 0; i <= m-2; i++)
                {
                    for(j = 0; j <= n-2; j++)
                    {
                        splinedifferentiation2d(c, lx(2*j+1), ly(2*i+1), v1, v1x, v1y, v1xy);
                        twodnumder(c, lx(2*j+1), ly(2*i+1), h, v2, v2x, v2y, v2xy);
                        err = ap::maxreal(err, fabs(v1-v2));
                        err = ap::maxreal(err, fabs(v1x-v2x));
                        err = ap::maxreal(err, fabs(v1y-v2y));
                        err = ap::maxreal(err, fabs(v1xy-v2xy));
                    }
                }
                dserrors = dserrors||err>1.0E-3;
                uperrors = uperrors||!testunpack(c, lx, ly);
                lterrors = lterrors||!testlintrans(c, ax, bx, ay, by);
                
                //
                // Test bicubic interpolation.
                // * interpolation at the nodes
                // * smoothness
                // * differentiation
                //
                buildbicubicspline(x, y, f, m, n, c);
                err = 0;
                for(i = 0; i <= m-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        err = ap::maxreal(err, fabs(f(i,j)-splineinterpolation2d(c, x(j), y(i))));
                    }
                }
                bcerrors = bcerrors||err>10000*ap::machineepsilon;
                lconst(c, lx, ly, m, n, lstep, l1, l1x, l1y, l1xy);
                lconst(c, lx, ly, m, n, lstep/3, l2, l2x, l2y, l2xy);
                bcerrors = bcerrors||l2/l1>1.2;
                bcerrors = bcerrors||l2x/l1x>1.2;
                bcerrors = bcerrors||l2y/l1y>1.2;
                if( l2xy>0.01&&l1xy>0.01 )
                {
                    
                    //
                    // Cross-derivative continuity is tested only when
                    // bigger than 0.01. When the task size is too
                    // small, the d2F/dXdY is nearly zero and Lipschitz
                    // constant ratio is ill-conditioned.
                    //
                    bcerrors = bcerrors||l2xy/l1xy>1.2;
                }
                err = 0;
                for(i = 0; i <= 2*m-2; i++)
                {
                    for(j = 0; j <= 2*n-2; j++)
                    {
                        splinedifferentiation2d(c, lx(j), ly(i), v1, v1x, v1y, v1xy);
                        twodnumder(c, lx(j), ly(i), h, v2, v2x, v2y, v2xy);
                        err = ap::maxreal(err, fabs(v1-v2));
                        err = ap::maxreal(err, fabs(v1x-v2x));
                        err = ap::maxreal(err, fabs(v1y-v2y));
                        err = ap::maxreal(err, fabs(v1xy-v2xy));
                    }
                }
                dserrors = dserrors||err>1.0E-3;
                uperrors = uperrors||!testunpack(c, lx, ly);
                lterrors = lterrors||!testlintrans(c, ax, bx, ay, by);
                
                //
                // Copy test
                //
                if( ap::randomreal()>0.5 )
                {
                    buildbicubicspline(x, y, f, m, n, c);
                }
                else
                {
                    buildbilinearspline(x, y, f, m, n, c);
                }
                spline2dcopy(c, c2);
                err = 0;
                for(i = 1; i <= 5; i++)
                {
                    t1 = ax+(bx-ax)*ap::randomreal();
                    t2 = ay+(by-ay)*ap::randomreal();
                    err = ap::maxreal(err, fabs(splineinterpolation2d(c, t1, t2)-splineinterpolation2d(c2, t1, t2)));
                }
                cperrors = cperrors||err>10000*ap::machineepsilon;
                
                //
                // Special symmetry test
                //
                err = 0;
                for(jobtype = 0; jobtype <= 1; jobtype++)
                {
                    
                    //
                    // Prepare
                    //
                    for(i = 0; i <= m-1; i++)
                    {
                        for(j = 0; j <= n-1; j++)
                        {
                            ft(j,i) = f(i,j);
                        }
                    }
                    if( jobtype==0 )
                    {
                        buildbilinearspline(x, y, f, m, n, c);
                        buildbilinearspline(y, x, ft, n, m, c2);
                    }
                    else
                    {
                        buildbicubicspline(x, y, f, m, n, c);
                        buildbicubicspline(y, x, ft, n, m, c2);
                    }
                    
                    //
                    // Test
                    //
                    for(i = 1; i <= 10; i++)
                    {
                        t1 = ax+(bx-ax)*ap::randomreal();
                        t2 = ay+(by-ay)*ap::randomreal();
                        err = ap::maxreal(err, fabs(splineinterpolation2d(c, t1, t2)-splineinterpolation2d(c2, t2, t1)));
                    }
                }
                syerrors = syerrors||err>10000*ap::machineepsilon;
            }
        }
    }
    
    //
    // Test resample
    //
    for(m = 2; m <= 6; m++)
    {
        for(n = 2; n <= 6; n++)
        {
            f.setbounds(0, m-1, 0, n-1);
            x.setbounds(0, n-1);
            y.setbounds(0, m-1);
            for(j = 0; j <= n-1; j++)
            {
                x(j) = double(j)/double(n-1);
            }
            for(i = 0; i <= m-1; i++)
            {
                y(i) = double(i)/double(m-1);
            }
            for(i = 0; i <= m-1; i++)
            {
                for(j = 0; j <= n-1; j++)
                {
                    f(i,j) = exp(0.6*x(j))-exp(-0.3*y(i)+0.08*x(j))+2*cos(ap::pi()*(x(j)+1.2*y(i)))+0.1*cos(20*x(j)+15*y(i));
                }
            }
            for(m2 = 2; m2 <= 6; m2++)
            {
                for(n2 = 2; n2 <= 6; n2++)
                {
                    for(pass = 1; pass <= passcount; pass++)
                    {
                        for(jobtype = 0; jobtype <= 1; jobtype++)
                        {
                            if( jobtype==0 )
                            {
                                bilinearresamplecartesian(f, m, n, fr, m2, n2);
                                buildbilinearspline(x, y, f, m, n, c);
                            }
                            if( jobtype==1 )
                            {
                                bicubicresamplecartesian(f, m, n, fr, m2, n2);
                                buildbicubicspline(x, y, f, m, n, c);
                            }
                            err = 0;
                            mf = 0;
                            for(i = 0; i <= m2-1; i++)
                            {
                                for(j = 0; j <= n2-1; j++)
                                {
                                    v1 = splineinterpolation2d(c, double(j)/double(n2-1), double(i)/double(m2-1));
                                    v2 = fr(i,j);
                                    err = ap::maxreal(err, fabs(v1-v2));
                                    mf = ap::maxreal(mf, fabs(v1));
                                }
                            }
                            if( jobtype==0 )
                            {
                                rlerrors = rlerrors||err/mf>10000*ap::machineepsilon;
                            }
                            if( jobtype==1 )
                            {
                                rcerrors = rcerrors||err/mf>10000*ap::machineepsilon;
                            }
                        }
                    }
                }
            }
        }
    }
    
    //
    // report
    //
    waserrors = blerrors||bcerrors||dserrors||cperrors||uperrors||lterrors||syerrors||rlerrors||rcerrors;
    if( !silent )
    {
        printf("TESTING 2D INTERPOLATION\n");
        
        //
        // Normal tests
        //
        printf("BILINEAR TEST:                           ");
        if( blerrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("BICUBIC TEST:                            ");
        if( bcerrors )
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
        printf("SPECIAL SYMMETRY TEST:                   ");
        if( syerrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("BILINEAR RESAMPLING TEST:                ");
        if( rlerrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("BICUBIC RESAMPLING TEST:                 ");
        if( rcerrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        
        //
        // Summary
        //
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
static void lconst(const ap::real_1d_array& c,
     const ap::real_1d_array& lx,
     const ap::real_1d_array& ly,
     int m,
     int n,
     double lstep,
     double& lc,
     double& lcx,
     double& lcy,
     double& lcxy)
{
    int i;
    int j;
    double f1;
    double f2;
    double f3;
    double f4;
    double fx1;
    double fx2;
    double fx3;
    double fx4;
    double fy1;
    double fy2;
    double fy3;
    double fy4;
    double fxy1;
    double fxy2;
    double fxy3;
    double fxy4;
    double s2lstep;

    lc = 0;
    lcx = 0;
    lcy = 0;
    lcxy = 0;
    s2lstep = sqrt(double(2))*lstep;
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            
            //
            // Calculate
            //
            twodnumder(c, lx(j)-lstep/2, ly(i)-lstep/2, lstep/4, f1, fx1, fy1, fxy1);
            twodnumder(c, lx(j)+lstep/2, ly(i)-lstep/2, lstep/4, f2, fx2, fy2, fxy2);
            twodnumder(c, lx(j)+lstep/2, ly(i)+lstep/2, lstep/4, f3, fx3, fy3, fxy3);
            twodnumder(c, lx(j)-lstep/2, ly(i)+lstep/2, lstep/4, f4, fx4, fy4, fxy4);
            
            //
            // Lipschitz constant for the function itself
            //
            lc = ap::maxreal(lc, fabs((f1-f2)/lstep));
            lc = ap::maxreal(lc, fabs((f2-f3)/lstep));
            lc = ap::maxreal(lc, fabs((f3-f4)/lstep));
            lc = ap::maxreal(lc, fabs((f4-f1)/lstep));
            lc = ap::maxreal(lc, fabs((f1-f3)/s2lstep));
            lc = ap::maxreal(lc, fabs((f2-f4)/s2lstep));
            
            //
            // Lipschitz constant for the first derivative
            //
            lcx = ap::maxreal(lcx, fabs((fx1-fx2)/lstep));
            lcx = ap::maxreal(lcx, fabs((fx2-fx3)/lstep));
            lcx = ap::maxreal(lcx, fabs((fx3-fx4)/lstep));
            lcx = ap::maxreal(lcx, fabs((fx4-fx1)/lstep));
            lcx = ap::maxreal(lcx, fabs((fx1-fx3)/s2lstep));
            lcx = ap::maxreal(lcx, fabs((fx2-fx4)/s2lstep));
            
            //
            // Lipschitz constant for the first derivative
            //
            lcy = ap::maxreal(lcy, fabs((fy1-fy2)/lstep));
            lcy = ap::maxreal(lcy, fabs((fy2-fy3)/lstep));
            lcy = ap::maxreal(lcy, fabs((fy3-fy4)/lstep));
            lcy = ap::maxreal(lcy, fabs((fy4-fy1)/lstep));
            lcy = ap::maxreal(lcy, fabs((fy1-fy3)/s2lstep));
            lcy = ap::maxreal(lcy, fabs((fy2-fy4)/s2lstep));
            
            //
            // Lipschitz constant for the cross-derivative
            //
            lcxy = ap::maxreal(lcxy, fabs((fxy1-fxy2)/lstep));
            lcxy = ap::maxreal(lcxy, fabs((fxy2-fxy3)/lstep));
            lcxy = ap::maxreal(lcxy, fabs((fxy3-fxy4)/lstep));
            lcxy = ap::maxreal(lcxy, fabs((fxy4-fxy1)/lstep));
            lcxy = ap::maxreal(lcxy, fabs((fxy1-fxy3)/s2lstep));
            lcxy = ap::maxreal(lcxy, fabs((fxy2-fxy4)/s2lstep));
        }
    }
}


/*************************************************************************
Numerical differentiation.
*************************************************************************/
static void twodnumder(const ap::real_1d_array& c,
     double x,
     double y,
     double h,
     double& f,
     double& fx,
     double& fy,
     double& fxy)
{

    f = splineinterpolation2d(c, x, y);
    fx = (splineinterpolation2d(c, x+h, y)-splineinterpolation2d(c, x-h, y))/(2*h);
    fy = (splineinterpolation2d(c, x, y+h)-splineinterpolation2d(c, x, y-h))/(2*h);
    fxy = (splineinterpolation2d(c, x+h, y+h)-splineinterpolation2d(c, x-h, y+h)-splineinterpolation2d(c, x+h, y-h)+splineinterpolation2d(c, x-h, y-h))/ap::sqr(2*h);
}


/*************************************************************************
Unpack test
*************************************************************************/
static bool testunpack(const ap::real_1d_array& c,
     const ap::real_1d_array& lx,
     const ap::real_1d_array& ly)
{
    bool result;
    int i;
    int j;
    int n;
    int m;
    int ci;
    int cj;
    int p;
    double err;
    double tx;
    double ty;
    double v1;
    double v2;
    int pass;
    int passcount;
    ap::real_2d_array tbl;

    passcount = 20;
    err = 0;
    splineunpack2d(c, m, n, tbl);
    for(i = 0; i <= m-2; i++)
    {
        for(j = 0; j <= n-2; j++)
        {
            for(pass = 1; pass <= passcount; pass++)
            {
                p = (n-1)*i+j;
                tx = (0.001+0.999*ap::randomreal())*(tbl(p,1)-tbl(p,0));
                ty = (0.001+0.999*ap::randomreal())*(tbl(p,3)-tbl(p,2));
                
                //
                // Interpolation properties
                //
                v1 = 0;
                for(ci = 0; ci <= 3; ci++)
                {
                    for(cj = 0; cj <= 3; cj++)
                    {
                        v1 = v1+tbl(p,4+ci*4+cj)*pow(tx, double(ci))*pow(ty, double(cj));
                    }
                }
                v2 = splineinterpolation2d(c, tbl(p,0)+tx, tbl(p,2)+ty);
                err = ap::maxreal(err, fabs(v1-v2));
                
                //
                // Grid correctness
                //
                err = ap::maxreal(err, fabs(lx(2*j)-tbl(p,0)));
                err = ap::maxreal(err, fabs(lx(2*(j+1))-tbl(p,1)));
                err = ap::maxreal(err, fabs(ly(2*i)-tbl(p,2)));
                err = ap::maxreal(err, fabs(ly(2*(i+1))-tbl(p,3)));
            }
        }
    }
    result = err<10000*ap::machineepsilon;
    return result;
}


/*************************************************************************
LinTrans test
*************************************************************************/
static bool testlintrans(const ap::real_1d_array& c,
     double ax,
     double bx,
     double ay,
     double by)
{
    bool result;
    double err;
    double a1;
    double a2;
    double b1;
    double b2;
    double tx;
    double ty;
    double vx;
    double vy;
    double v1;
    double v2;
    int pass;
    int passcount;
    int xjob;
    int yjob;
    ap::real_1d_array c2;

    passcount = 5;
    err = 0;
    for(xjob = 0; xjob <= 1; xjob++)
    {
        for(yjob = 0; yjob <= 1; yjob++)
        {
            for(pass = 1; pass <= passcount; pass++)
            {
                
                //
                // Prepare
                //
                do
                {
                    a1 = 2*ap::randomreal()-1;
                }
                while(a1==0);
                a1 = a1*xjob;
                b1 = 2*ap::randomreal()-1;
                do
                {
                    a2 = 2*ap::randomreal()-1;
                }
                while(a2==0);
                a2 = a2*yjob;
                b2 = 2*ap::randomreal()-1;
                
                //
                // Test XY
                //
                splinecopy(c, c2);
                spline2dlintransxy(c2, a1, b1, a2, b2);
                tx = ax+ap::randomreal()*(bx-ax);
                ty = ay+ap::randomreal()*(by-ay);
                if( xjob==0 )
                {
                    tx = b1;
                    vx = ax+ap::randomreal()*(bx-ax);
                }
                else
                {
                    vx = (tx-b1)/a1;
                }
                if( yjob==0 )
                {
                    ty = b2;
                    vy = ay+ap::randomreal()*(by-ay);
                }
                else
                {
                    vy = (ty-b2)/a2;
                }
                v1 = splineinterpolation2d(c, tx, ty);
                v2 = splineinterpolation2d(c2, vx, vy);
                err = ap::maxreal(err, fabs(v1-v2));
                
                //
                // Test F
                //
                splinecopy(c, c2);
                spline2dlintransf(c2, a1, b1);
                tx = ax+ap::randomreal()*(bx-ax);
                ty = ay+ap::randomreal()*(by-ay);
                v1 = splineinterpolation2d(c, tx, ty);
                v2 = splineinterpolation2d(c2, tx, ty);
                err = ap::maxreal(err, fabs(a1*v1+b1-v2));
            }
        }
    }
    result = err<10000*ap::machineepsilon;
    return result;
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testinterpolation2dunit_test_silent()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = test2dinterpolation(true);
    return result;
#else
    return _i_testinterpolation2dunit_test_silent();
#endif
}


/*************************************************************************
Unit test
*************************************************************************/
bool testinterpolation2dunit_test()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = test2dinterpolation(false);
    return result;
#else
    return _i_testinterpolation2dunit_test();
#endif
}



