/*************************************************************************
Copyright (c) 2007, Sergey Bochkanov (ALGLIB project).

>>> SOURCE LICENSE >>>
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation (www.fsf.org); either version 2 of the 
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

A copy of the GNU General Public License is available at
http://www.fsf.org/licensing/licenses

>>> END OF LICENSE >>>
*************************************************************************/

#include <stdafx.h>
#include "spline2d.h"

static void bicubiccalcderivatives(const ap::real_2d_array& a,
     const ap::real_1d_array& x,
     const ap::real_1d_array& y,
     int m,
     int n,
     ap::real_2d_array& dx,
     ap::real_2d_array& dy,
     ap::real_2d_array& dxy);

/*************************************************************************
This subroutine builds bilinear spline coefficients table.

Input parameters:
    X   -   spline abscissas, array[0..N-1]
    Y   -   spline ordinates, array[0..M-1]
    F   -   function values, array[0..M-1,0..N-1]
    M,N -   grid size, M>=2, N>=2

Output parameters:
    C   -   coefficients table.  Used  by  SplineInterpolation2D and other
            subroutines from this file.

  -- ALGLIB PROJECT --
     Copyright 05.07.2007 by Bochkanov Sergey
*************************************************************************/
void buildbilinearspline(ap::real_1d_array x,
     ap::real_1d_array y,
     ap::real_2d_array f,
     int m,
     int n,
     ap::real_1d_array& c)
{
    int i;
    int j;
    int k;
    int tblsize;
    int shift;
    double t;
    ap::real_2d_array dx;
    ap::real_2d_array dy;
    ap::real_2d_array dxy;

    ap::ap_error::make_assertion(n>=2&&m>=2, "BuildBilinearSpline: N<2 or M<2!");
    
    //
    // Sort points
    //
    for(j = 0; j <= n-1; j++)
    {
        k = j;
        for(i = j+1; i <= n-1; i++)
        {
            if( x(i)<x(k) )
            {
                k = i;
            }
        }
        if( k!=j )
        {
            for(i = 0; i <= m-1; i++)
            {
                t = f(i,j);
                f(i,j) = f(i,k);
                f(i,k) = t;
            }
            t = x(j);
            x(j) = x(k);
            x(k) = t;
        }
    }
    for(i = 0; i <= m-1; i++)
    {
        k = i;
        for(j = i+1; j <= m-1; j++)
        {
            if( y(j)<y(k) )
            {
                k = j;
            }
        }
        if( k!=i )
        {
            for(j = 0; j <= n-1; j++)
            {
                t = f(i,j);
                f(i,j) = f(k,j);
                f(k,j) = t;
            }
            t = y(i);
            y(i) = y(k);
            y(k) = t;
        }
    }
    
    //
    // Fill C:
    //  C[0]            -   length(C)
    //  C[1]            -   type(C):
    //                      -1 = bilinear interpolant
    //                      -3 = general cubic spline
    //                           (see BuildBicubicSpline)
    //  C[2]:
    //      N (x count)
    //  C[3]:
    //      M (y count)
    //  C[4]...C[4+N-1]:
    //      x[i], i = 0...N-1
    //  C[4+N]...C[4+N+M-1]:
    //      y[i], i = 0...M-1
    //  C[4+N+M]...C[4+N+M+(N*M-1)]:
    //      f(i,j) table. f(0,0), f(0, 1), f(0,2) and so on...
    //
    tblsize = 4+n+m+n*m;
    c.setbounds(0, tblsize-1);
    c(0) = tblsize;
    c(1) = -1;
    c(2) = n;
    c(3) = m;
    for(i = 0; i <= n-1; i++)
    {
        c(4+i) = x(i);
    }
    for(i = 0; i <= m-1; i++)
    {
        c(4+n+i) = y(i);
    }
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            shift = i*n+j;
            c(4+n+m+shift) = f(i,j);
        }
    }
}


/*************************************************************************
This subroutine builds bicubic spline coefficients table.

Input parameters:
    X   -   spline abscissas, array[0..N-1]
    Y   -   spline ordinates, array[0..M-1]
    F   -   function values, array[0..M-1,0..N-1]
    M,N -   grid size, M>=2, N>=2

Output parameters:
    C   -   coefficients table.  Used  by  SplineInterpolation2D and other
            subroutines from this file.

  -- ALGLIB PROJECT --
     Copyright 05.07.2007 by Bochkanov Sergey
*************************************************************************/
void buildbicubicspline(ap::real_1d_array x,
     ap::real_1d_array y,
     ap::real_2d_array f,
     int m,
     int n,
     ap::real_1d_array& c)
{
    int i;
    int j;
    int k;
    int tblsize;
    int shift;
    double t;
    ap::real_2d_array dx;
    ap::real_2d_array dy;
    ap::real_2d_array dxy;

    ap::ap_error::make_assertion(n>=2&&m>=2, "BuildBicubicSpline: N<2 or M<2!");
    
    //
    // Sort points
    //
    for(j = 0; j <= n-1; j++)
    {
        k = j;
        for(i = j+1; i <= n-1; i++)
        {
            if( x(i)<x(k) )
            {
                k = i;
            }
        }
        if( k!=j )
        {
            for(i = 0; i <= m-1; i++)
            {
                t = f(i,j);
                f(i,j) = f(i,k);
                f(i,k) = t;
            }
            t = x(j);
            x(j) = x(k);
            x(k) = t;
        }
    }
    for(i = 0; i <= m-1; i++)
    {
        k = i;
        for(j = i+1; j <= m-1; j++)
        {
            if( y(j)<y(k) )
            {
                k = j;
            }
        }
        if( k!=i )
        {
            for(j = 0; j <= n-1; j++)
            {
                t = f(i,j);
                f(i,j) = f(k,j);
                f(k,j) = t;
            }
            t = y(i);
            y(i) = y(k);
            y(k) = t;
        }
    }
    
    //
    // Fill C:
    //  C[0]            -   length(C)
    //  C[1]            -   type(C):
    //                      -1 = bilinear interpolant
    //                           (see BuildBilinearInterpolant)
    //                      -3 = general cubic spline
    //  C[2]:
    //      N (x count)
    //  C[3]:
    //      M (y count)
    //  C[4]...C[4+N-1]:
    //      x[i], i = 0...N-1
    //  C[4+N]...C[4+N+M-1]:
    //      y[i], i = 0...M-1
    //  C[4+N+M]...C[4+N+M+(N*M-1)]:
    //      f(i,j) table. f(0,0), f(0, 1), f(0,2) and so on...
    //  C[4+N+M+N*M]...C[4+N+M+(2*N*M-1)]:
    //      df(i,j)/dx table.
    //  C[4+N+M+2*N*M]...C[4+N+M+(3*N*M-1)]:
    //      df(i,j)/dy table.
    //  C[4+N+M+3*N*M]...C[4+N+M+(4*N*M-1)]:
    //      d2f(i,j)/dxdy table.
    //
    tblsize = 4+n+m+4*n*m;
    c.setbounds(0, tblsize-1);
    c(0) = tblsize;
    c(1) = -3;
    c(2) = n;
    c(3) = m;
    for(i = 0; i <= n-1; i++)
    {
        c(4+i) = x(i);
    }
    for(i = 0; i <= m-1; i++)
    {
        c(4+n+i) = y(i);
    }
    bicubiccalcderivatives(f, x, y, m, n, dx, dy, dxy);
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            shift = i*n+j;
            c(4+n+m+shift) = f(i,j);
            c(4+n+m+n*m+shift) = dx(i,j);
            c(4+n+m+2*n*m+shift) = dy(i,j);
            c(4+n+m+3*n*m+shift) = dxy(i,j);
        }
    }
}


/*************************************************************************
This subroutine calculates the value of the bilinear or bicubic spline  at
the given point X.

Input parameters:
    C   -   coefficients table.
            Built by BuildBilinearSpline or BuildBicubicSpline.
    X, Y-   point

Result:
    S(x,y)

  -- ALGLIB PROJECT --
     Copyright 05.07.2007 by Bochkanov Sergey
*************************************************************************/
double splineinterpolation2d(const ap::real_1d_array& c, double x, double y)
{
    double result;
    double v;
    double vx;
    double vy;
    double vxy;

    splinedifferentiation2d(c, x, y, v, vx, vy, vxy);
    result = v;
    return result;
}


/*************************************************************************
This subroutine calculates the value of the bilinear or bicubic spline  at
the given point X and its derivatives.

Input parameters:
    C   -   coefficients table.
            Built by BuildBilinearSpline or BuildBicubicSpline.
    X, Y-   point

Output parameters:
    F   -   S(x,y)
    FX  -   dS(x,y)/dX
    FY  -   dS(x,y)/dY
    FXY -   d2S(x,y)/dXdY

  -- ALGLIB PROJECT --
     Copyright 05.07.2007 by Bochkanov Sergey
*************************************************************************/
void splinedifferentiation2d(const ap::real_1d_array& c,
     double x,
     double y,
     double& f,
     double& fx,
     double& fy,
     double& fxy)
{
    int n;
    int m;
    double t;
    double dt;
    double u;
    double du;
    int i;
    int j;
    int ix;
    int iy;
    int l;
    int r;
    int h;
    int shift1;
    int s1;
    int s2;
    int s3;
    int s4;
    int sf;
    int sfx;
    int sfy;
    int sfxy;
    double y1;
    double y2;
    double y3;
    double y4;
    double v;
    double t0;
    double t1;
    double t2;
    double t3;
    double u0;
    double u1;
    double u2;
    double u3;

    ap::ap_error::make_assertion(ap::round(c(1))==-1||ap::round(c(1))==-3, "TwoDimensionalInterpolation: incorrect C!");
    n = ap::round(c(2));
    m = ap::round(c(3));
    
    //
    // Binary search in the [ x[0], ..., x[n-2] ] (x[n-1] is not included)
    //
    l = 4;
    r = 4+n-2+1;
    while(l!=r-1)
    {
        h = (l+r)/2;
        if( c(h)>=x )
        {
            r = h;
        }
        else
        {
            l = h;
        }
    }
    t = (x-c(l))/(c(l+1)-c(l));
    dt = 1.0/(c(l+1)-c(l));
    ix = l-4;
    
    //
    // Binary search in the [ y[0], ..., y[m-2] ] (y[m-1] is not included)
    //
    l = 4+n;
    r = 4+n+(m-2)+1;
    while(l!=r-1)
    {
        h = (l+r)/2;
        if( c(h)>=y )
        {
            r = h;
        }
        else
        {
            l = h;
        }
    }
    u = (y-c(l))/(c(l+1)-c(l));
    du = 1.0/(c(l+1)-c(l));
    iy = l-(4+n);
    
    //
    // Prepare F, dF/dX, dF/dY, d2F/dXdY
    //
    f = 0;
    fx = 0;
    fy = 0;
    fxy = 0;
    
    //
    // Bilinear interpolation
    //
    if( ap::round(c(1))==-1 )
    {
        shift1 = 4+n+m;
        y1 = c(shift1+n*iy+ix);
        y2 = c(shift1+n*iy+(ix+1));
        y3 = c(shift1+n*(iy+1)+(ix+1));
        y4 = c(shift1+n*(iy+1)+ix);
        f = (1-t)*(1-u)*y1+t*(1-u)*y2+t*u*y3+(1-t)*u*y4;
        fx = (-(1-u)*y1+(1-u)*y2+u*y3-u*y4)*dt;
        fy = (-(1-t)*y1-t*y2+t*y3+(1-t)*y4)*du;
        fxy = (y1-y2+y3-y4)*du*dt;
        return;
    }
    
    //
    // Bicubic interpolation
    //
    if( ap::round(c(1))==-3 )
    {
        
        //
        // Prepare info
        //
        t0 = 1;
        t1 = t;
        t2 = ap::sqr(t);
        t3 = t*t2;
        u0 = 1;
        u1 = u;
        u2 = ap::sqr(u);
        u3 = u*u2;
        sf = 4+n+m;
        sfx = 4+n+m+n*m;
        sfy = 4+n+m+2*n*m;
        sfxy = 4+n+m+3*n*m;
        s1 = n*iy+ix;
        s2 = n*iy+(ix+1);
        s3 = n*(iy+1)+(ix+1);
        s4 = n*(iy+1)+ix;
        
        //
        // Calculate
        //
        v = +1*c(sf+s1);
        f = f+v*t0*u0;
        v = +1*c(sfy+s1)/du;
        f = f+v*t0*u1;
        fy = fy+1*v*t0*u0*du;
        v = -3*c(sf+s1)+3*c(sf+s4)-2*c(sfy+s1)/du-1*c(sfy+s4)/du;
        f = f+v*t0*u2;
        fy = fy+2*v*t0*u1*du;
        v = +2*c(sf+s1)-2*c(sf+s4)+1*c(sfy+s1)/du+1*c(sfy+s4)/du;
        f = f+v*t0*u3;
        fy = fy+3*v*t0*u2*du;
        v = +1*c(sfx+s1)/dt;
        f = f+v*t1*u0;
        fx = fx+1*v*t0*u0*dt;
        v = +1*c(sfxy+s1)/(dt*du);
        f = f+v*t1*u1;
        fx = fx+1*v*t0*u1*dt;
        fy = fy+1*v*t1*u0*du;
        fxy = fxy+1*v*t0*u0*dt*du;
        v = -3*c(sfx+s1)/dt+3*c(sfx+s4)/dt-2*c(sfxy+s1)/(dt*du)-1*c(sfxy+s4)/(dt*du);
        f = f+v*t1*u2;
        fx = fx+1*v*t0*u2*dt;
        fy = fy+2*v*t1*u1*du;
        fxy = fxy+2*v*t0*u1*dt*du;
        v = +2*c(sfx+s1)/dt-2*c(sfx+s4)/dt+1*c(sfxy+s1)/(dt*du)+1*c(sfxy+s4)/(dt*du);
        f = f+v*t1*u3;
        fx = fx+1*v*t0*u3*dt;
        fy = fy+3*v*t1*u2*du;
        fxy = fxy+3*v*t0*u2*dt*du;
        v = -3*c(sf+s1)+3*c(sf+s2)-2*c(sfx+s1)/dt-1*c(sfx+s2)/dt;
        f = f+v*t2*u0;
        fx = fx+2*v*t1*u0*dt;
        v = -3*c(sfy+s1)/du+3*c(sfy+s2)/du-2*c(sfxy+s1)/(dt*du)-1*c(sfxy+s2)/(dt*du);
        f = f+v*t2*u1;
        fx = fx+2*v*t1*u1*dt;
        fy = fy+1*v*t2*u0*du;
        fxy = fxy+2*v*t1*u0*dt*du;
        v = +9*c(sf+s1)-9*c(sf+s2)+9*c(sf+s3)-9*c(sf+s4)+6*c(sfx+s1)/dt+3*c(sfx+s2)/dt-3*c(sfx+s3)/dt-6*c(sfx+s4)/dt+6*c(sfy+s1)/du-6*c(sfy+s2)/du-3*c(sfy+s3)/du+3*c(sfy+s4)/du+4*c(sfxy+s1)/(dt*du)+2*c(sfxy+s2)/(dt*du)+1*c(sfxy+s3)/(dt*du)+2*c(sfxy+s4)/(dt*du);
        f = f+v*t2*u2;
        fx = fx+2*v*t1*u2*dt;
        fy = fy+2*v*t2*u1*du;
        fxy = fxy+4*v*t1*u1*dt*du;
        v = -6*c(sf+s1)+6*c(sf+s2)-6*c(sf+s3)+6*c(sf+s4)-4*c(sfx+s1)/dt-2*c(sfx+s2)/dt+2*c(sfx+s3)/dt+4*c(sfx+s4)/dt-3*c(sfy+s1)/du+3*c(sfy+s2)/du+3*c(sfy+s3)/du-3*c(sfy+s4)/du-2*c(sfxy+s1)/(dt*du)-1*c(sfxy+s2)/(dt*du)-1*c(sfxy+s3)/(dt*du)-2*c(sfxy+s4)/(dt*du);
        f = f+v*t2*u3;
        fx = fx+2*v*t1*u3*dt;
        fy = fy+3*v*t2*u2*du;
        fxy = fxy+6*v*t1*u2*dt*du;
        v = +2*c(sf+s1)-2*c(sf+s2)+1*c(sfx+s1)/dt+1*c(sfx+s2)/dt;
        f = f+v*t3*u0;
        fx = fx+3*v*t2*u0*dt;
        v = +2*c(sfy+s1)/du-2*c(sfy+s2)/du+1*c(sfxy+s1)/(dt*du)+1*c(sfxy+s2)/(dt*du);
        f = f+v*t3*u1;
        fx = fx+3*v*t2*u1*dt;
        fy = fy+1*v*t3*u0*du;
        fxy = fxy+3*v*t2*u0*dt*du;
        v = -6*c(sf+s1)+6*c(sf+s2)-6*c(sf+s3)+6*c(sf+s4)-3*c(sfx+s1)/dt-3*c(sfx+s2)/dt+3*c(sfx+s3)/dt+3*c(sfx+s4)/dt-4*c(sfy+s1)/du+4*c(sfy+s2)/du+2*c(sfy+s3)/du-2*c(sfy+s4)/du-2*c(sfxy+s1)/(dt*du)-2*c(sfxy+s2)/(dt*du)-1*c(sfxy+s3)/(dt*du)-1*c(sfxy+s4)/(dt*du);
        f = f+v*t3*u2;
        fx = fx+3*v*t2*u2*dt;
        fy = fy+2*v*t3*u1*du;
        fxy = fxy+6*v*t2*u1*dt*du;
        v = +4*c(sf+s1)-4*c(sf+s2)+4*c(sf+s3)-4*c(sf+s4)+2*c(sfx+s1)/dt+2*c(sfx+s2)/dt-2*c(sfx+s3)/dt-2*c(sfx+s4)/dt+2*c(sfy+s1)/du-2*c(sfy+s2)/du-2*c(sfy+s3)/du+2*c(sfy+s4)/du+1*c(sfxy+s1)/(dt*du)+1*c(sfxy+s2)/(dt*du)+1*c(sfxy+s3)/(dt*du)+1*c(sfxy+s4)/(dt*du);
        f = f+v*t3*u3;
        fx = fx+3*v*t2*u3*dt;
        fy = fy+3*v*t3*u2*du;
        fxy = fxy+9*v*t2*u2*dt*du;
        return;
    }
}


/*************************************************************************
This subroutine unpacks two-dimensional spline into the coefficients table

Input parameters:
    C   -   coefficients table.

Result:
    M, N-   grid size (x-axis and y-axis)
    Tbl -   coefficients table, unpacked format,
            [0..(N-1)*(M-1)-1, 0..19].
            For I = 0...M-2, J=0..N-2:
                K =  I*(N-1)+J
                Tbl[K,0] = X[j]
                Tbl[K,1] = X[j+1]
                Tbl[K,2] = Y[i]
                Tbl[K,3] = Y[i+1]
                Tbl[K,4] = C00
                Tbl[K,5] = C01
                Tbl[K,6] = C02
                Tbl[K,7] = C03
                Tbl[K,8] = C10
                Tbl[K,9] = C11
                ...
                Tbl[K,19] = C33
            On each grid square spline is equals to:
                S(x) = SUM(c[i,j]*(x^i)*(y^j), i=0..3, j=0..3)
                t = x-x[j]
                u = y-y[i]

  -- ALGLIB PROJECT --
     Copyright 29.06.2007 by Bochkanov Sergey
*************************************************************************/
void splineunpack2d(const ap::real_1d_array& c,
     int& m,
     int& n,
     ap::real_2d_array& tbl)
{
    int i;
    int j;
    int ci;
    int cj;
    int k;
    int p;
    int shift;
    int s1;
    int s2;
    int s3;
    int s4;
    int sf;
    int sfx;
    int sfy;
    int sfxy;
    double y1;
    double y2;
    double y3;
    double y4;
    double dt;
    double du;

    ap::ap_error::make_assertion(ap::round(c(1))==-3||ap::round(c(1))==-1, "SplineUnpack2D: incorrect C!");
    n = ap::round(c(2));
    m = ap::round(c(3));
    tbl.setbounds(0, (n-1)*(m-1)-1, 0, 19);
    
    //
    // Fill
    //
    for(i = 0; i <= m-2; i++)
    {
        for(j = 0; j <= n-2; j++)
        {
            p = i*(n-1)+j;
            tbl(p,0) = c(4+j);
            tbl(p,1) = c(4+j+1);
            tbl(p,2) = c(4+n+i);
            tbl(p,3) = c(4+n+i+1);
            dt = 1/(tbl(p,1)-tbl(p,0));
            du = 1/(tbl(p,3)-tbl(p,2));
            
            //
            // Bilinear interpolation
            //
            if( ap::round(c(1))==-1 )
            {
                for(k = 4; k <= 19; k++)
                {
                    tbl(p,k) = 0;
                }
                shift = 4+n+m;
                y1 = c(shift+n*i+j);
                y2 = c(shift+n*i+(j+1));
                y3 = c(shift+n*(i+1)+(j+1));
                y4 = c(shift+n*(i+1)+j);
                tbl(p,4) = y1;
                tbl(p,4+1*4+0) = y2-y1;
                tbl(p,4+0*4+1) = y4-y1;
                tbl(p,4+1*4+1) = y3-y2-y4+y1;
            }
            
            //
            // Bicubic interpolation
            //
            if( ap::round(c(1))==-3 )
            {
                sf = 4+n+m;
                sfx = 4+n+m+n*m;
                sfy = 4+n+m+2*n*m;
                sfxy = 4+n+m+3*n*m;
                s1 = n*i+j;
                s2 = n*i+(j+1);
                s3 = n*(i+1)+(j+1);
                s4 = n*(i+1)+j;
                tbl(p,4+0*4+0) = +1*c(sf+s1);
                tbl(p,4+0*4+1) = +1*c(sfy+s1)/du;
                tbl(p,4+0*4+2) = -3*c(sf+s1)+3*c(sf+s4)-2*c(sfy+s1)/du-1*c(sfy+s4)/du;
                tbl(p,4+0*4+3) = +2*c(sf+s1)-2*c(sf+s4)+1*c(sfy+s1)/du+1*c(sfy+s4)/du;
                tbl(p,4+1*4+0) = +1*c(sfx+s1)/dt;
                tbl(p,4+1*4+1) = +1*c(sfxy+s1)/(dt*du);
                tbl(p,4+1*4+2) = -3*c(sfx+s1)/dt+3*c(sfx+s4)/dt-2*c(sfxy+s1)/(dt*du)-1*c(sfxy+s4)/(dt*du);
                tbl(p,4+1*4+3) = +2*c(sfx+s1)/dt-2*c(sfx+s4)/dt+1*c(sfxy+s1)/(dt*du)+1*c(sfxy+s4)/(dt*du);
                tbl(p,4+2*4+0) = -3*c(sf+s1)+3*c(sf+s2)-2*c(sfx+s1)/dt-1*c(sfx+s2)/dt;
                tbl(p,4+2*4+1) = -3*c(sfy+s1)/du+3*c(sfy+s2)/du-2*c(sfxy+s1)/(dt*du)-1*c(sfxy+s2)/(dt*du);
                tbl(p,4+2*4+2) = +9*c(sf+s1)-9*c(sf+s2)+9*c(sf+s3)-9*c(sf+s4)+6*c(sfx+s1)/dt+3*c(sfx+s2)/dt-3*c(sfx+s3)/dt-6*c(sfx+s4)/dt+6*c(sfy+s1)/du-6*c(sfy+s2)/du-3*c(sfy+s3)/du+3*c(sfy+s4)/du+4*c(sfxy+s1)/(dt*du)+2*c(sfxy+s2)/(dt*du)+1*c(sfxy+s3)/(dt*du)+2*c(sfxy+s4)/(dt*du);
                tbl(p,4+2*4+3) = -6*c(sf+s1)+6*c(sf+s2)-6*c(sf+s3)+6*c(sf+s4)-4*c(sfx+s1)/dt-2*c(sfx+s2)/dt+2*c(sfx+s3)/dt+4*c(sfx+s4)/dt-3*c(sfy+s1)/du+3*c(sfy+s2)/du+3*c(sfy+s3)/du-3*c(sfy+s4)/du-2*c(sfxy+s1)/(dt*du)-1*c(sfxy+s2)/(dt*du)-1*c(sfxy+s3)/(dt*du)-2*c(sfxy+s4)/(dt*du);
                tbl(p,4+3*4+0) = +2*c(sf+s1)-2*c(sf+s2)+1*c(sfx+s1)/dt+1*c(sfx+s2)/dt;
                tbl(p,4+3*4+1) = +2*c(sfy+s1)/du-2*c(sfy+s2)/du+1*c(sfxy+s1)/(dt*du)+1*c(sfxy+s2)/(dt*du);
                tbl(p,4+3*4+2) = -6*c(sf+s1)+6*c(sf+s2)-6*c(sf+s3)+6*c(sf+s4)-3*c(sfx+s1)/dt-3*c(sfx+s2)/dt+3*c(sfx+s3)/dt+3*c(sfx+s4)/dt-4*c(sfy+s1)/du+4*c(sfy+s2)/du+2*c(sfy+s3)/du-2*c(sfy+s4)/du-2*c(sfxy+s1)/(dt*du)-2*c(sfxy+s2)/(dt*du)-1*c(sfxy+s3)/(dt*du)-1*c(sfxy+s4)/(dt*du);
                tbl(p,4+3*4+3) = +4*c(sf+s1)-4*c(sf+s2)+4*c(sf+s3)-4*c(sf+s4)+2*c(sfx+s1)/dt+2*c(sfx+s2)/dt-2*c(sfx+s3)/dt-2*c(sfx+s4)/dt+2*c(sfy+s1)/du-2*c(sfy+s2)/du-2*c(sfy+s3)/du+2*c(sfy+s4)/du+1*c(sfxy+s1)/(dt*du)+1*c(sfxy+s2)/(dt*du)+1*c(sfxy+s3)/(dt*du)+1*c(sfxy+s4)/(dt*du);
            }
            
            //
            // Rescale Cij
            //
            for(ci = 0; ci <= 3; ci++)
            {
                for(cj = 0; cj <= 3; cj++)
                {
                    tbl(p,4+ci*4+cj) = tbl(p,4+ci*4+cj)*pow(dt, double(ci))*pow(du, double(cj));
                }
            }
        }
    }
}


/*************************************************************************
This subroutine performs linear transformation of the spline argument.

Input parameters:
    C       -   coefficients table.
    AX, BX  -   transformation coefficients: x = A*t + B
    AY, BY  -   transformation coefficients: y = A*u + B
Result:
    C   -   transformed spline

  -- ALGLIB PROJECT --
     Copyright 30.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline2dlintransxy(ap::real_1d_array& c,
     double ax,
     double bx,
     double ay,
     double by)
{
    int i;
    int j;
    int n;
    int m;
    double v;
    ap::real_1d_array x;
    ap::real_1d_array y;
    ap::real_2d_array f;
    int typec;

    typec = ap::round(c(1));
    ap::ap_error::make_assertion(typec==-3||typec==-1, "Spline2DLinTransXY: incorrect C!");
    n = ap::round(c(2));
    m = ap::round(c(3));
    x.setbounds(0, n-1);
    y.setbounds(0, m-1);
    f.setbounds(0, m-1, 0, n-1);
    for(j = 0; j <= n-1; j++)
    {
        x(j) = c(4+j);
    }
    for(i = 0; i <= m-1; i++)
    {
        y(i) = c(4+n+i);
    }
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            f(i,j) = c(4+n+m+i*n+j);
        }
    }
    
    //
    // Special case: AX=0 or AY=0
    //
    if( ax==0 )
    {
        for(i = 0; i <= m-1; i++)
        {
            v = splineinterpolation2d(c, bx, y(i));
            for(j = 0; j <= n-1; j++)
            {
                f(i,j) = v;
            }
        }
        if( typec==-3 )
        {
            buildbicubicspline(x, y, f, m, n, c);
        }
        if( typec==-1 )
        {
            buildbilinearspline(x, y, f, m, n, c);
        }
        ax = 1;
        bx = 0;
    }
    if( ay==0 )
    {
        for(j = 0; j <= n-1; j++)
        {
            v = splineinterpolation2d(c, x(j), by);
            for(i = 0; i <= m-1; i++)
            {
                f(i,j) = v;
            }
        }
        if( typec==-3 )
        {
            buildbicubicspline(x, y, f, m, n, c);
        }
        if( typec==-1 )
        {
            buildbilinearspline(x, y, f, m, n, c);
        }
        ay = 1;
        by = 0;
    }
    
    //
    // General case: AX<>0, AY<>0
    // Unpack, scale and pack again.
    //
    for(j = 0; j <= n-1; j++)
    {
        x(j) = (x(j)-bx)/ax;
    }
    for(i = 0; i <= m-1; i++)
    {
        y(i) = (y(i)-by)/ay;
    }
    if( typec==-3 )
    {
        buildbicubicspline(x, y, f, m, n, c);
    }
    if( typec==-1 )
    {
        buildbilinearspline(x, y, f, m, n, c);
    }
}


/*************************************************************************
This subroutine performs linear transformation of the spline.

Input parameters:
    C   -   coefficients table. Built by BuildLinearSpline,
            BuildHermiteSpline, BuildCubicSpline, BuildAkimaSpline.
    A, B-   transformation coefficients: S2(x,y) = A*S(x,y) + B
    
Output parameters:
    C   -   transformed spline

  -- ALGLIB PROJECT --
     Copyright 30.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline2dlintransf(ap::real_1d_array& c, double a, double b)
{
    int i;
    int j;
    int n;
    int m;
    double v;
    ap::real_1d_array x;
    ap::real_1d_array y;
    ap::real_2d_array f;
    int typec;

    typec = ap::round(c(1));
    ap::ap_error::make_assertion(typec==-3||typec==-1, "Spline2DLinTransXY: incorrect C!");
    n = ap::round(c(2));
    m = ap::round(c(3));
    x.setbounds(0, n-1);
    y.setbounds(0, m-1);
    f.setbounds(0, m-1, 0, n-1);
    for(j = 0; j <= n-1; j++)
    {
        x(j) = c(4+j);
    }
    for(i = 0; i <= m-1; i++)
    {
        y(i) = c(4+n+i);
    }
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            f(i,j) = a*c(4+n+m+i*n+j)+b;
        }
    }
    if( typec==-3 )
    {
        buildbicubicspline(x, y, f, m, n, c);
    }
    if( typec==-1 )
    {
        buildbilinearspline(x, y, f, m, n, c);
    }
}


/*************************************************************************
This subroutine makes the copy of the spline.

Input parameters:
    C   -   coefficients table.

Output parameters:
    CC  -   spline copy

  -- ALGLIB PROJECT --
     Copyright 29.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline2dcopy(const ap::real_1d_array& c, ap::real_1d_array& cc)
{

    splinecopy(c, cc);
}


/*************************************************************************
Bicubic spline resampling

Input parameters:
    A           -   function values at the old grid,
                    array[0..OldHeight-1, 0..OldWidth-1]
    OldHeight   -   old grid height, OldHeight>1
    OldWidth    -   old grid width, OldWidth>1
    NewHeight   -   new grid height, NewHeight>1
    NewWidth    -   new grid width, NewWidth>1
    
Output parameters:
    B           -   function values at the new grid,
                    array[0..NewHeight-1, 0..NewWidth-1]

  -- ALGLIB routine --
     15 May, 2007
     Copyright by Bochkanov Sergey
*************************************************************************/
void bicubicresamplecartesian(const ap::real_2d_array& a,
     int oldheight,
     int oldwidth,
     ap::real_2d_array& b,
     int newheight,
     int newwidth)
{
    ap::real_2d_array buf;
    ap::real_1d_array x;
    ap::real_1d_array y;
    ap::real_1d_array c;
    int i;
    int j;
    int mw;
    int mh;

    ap::ap_error::make_assertion(oldwidth>1&&oldheight>1, "BicubicResampleCartesian: width/height less than 1");
    ap::ap_error::make_assertion(newwidth>1&&newheight>1, "BicubicResampleCartesian: width/height less than 1");
    
    //
    // Prepare
    //
    mw = ap::maxint(oldwidth, newwidth);
    mh = ap::maxint(oldheight, newheight);
    b.setbounds(0, newheight-1, 0, newwidth-1);
    buf.setbounds(0, oldheight-1, 0, newwidth-1);
    x.setbounds(0, ap::maxint(mw, mh)-1);
    y.setbounds(0, ap::maxint(mw, mh)-1);
    
    //
    // Horizontal interpolation
    //
    for(i = 0; i <= oldheight-1; i++)
    {
        
        //
        // Fill X, Y
        //
        for(j = 0; j <= oldwidth-1; j++)
        {
            x(j) = double(j)/double(oldwidth-1);
            y(j) = a(i,j);
        }
        
        //
        // Interpolate and place result into temporary matrix
        //
        buildcubicspline(x, y, oldwidth, 0, 0.0, 0, 0.0, c);
        for(j = 0; j <= newwidth-1; j++)
        {
            buf(i,j) = splineinterpolation(c, double(j)/double(newwidth-1));
        }
    }
    
    //
    // Vertical interpolation
    //
    for(j = 0; j <= newwidth-1; j++)
    {
        
        //
        // Fill X, Y
        //
        for(i = 0; i <= oldheight-1; i++)
        {
            x(i) = double(i)/double(oldheight-1);
            y(i) = buf(i,j);
        }
        
        //
        // Interpolate and place result into B
        //
        buildcubicspline(x, y, oldheight, 0, 0.0, 0, 0.0, c);
        for(i = 0; i <= newheight-1; i++)
        {
            b(i,j) = splineinterpolation(c, double(i)/double(newheight-1));
        }
    }
}


/*************************************************************************
Bilinear spline resampling

Input parameters:
    A           -   function values at the old grid,
                    array[0..OldHeight-1, 0..OldWidth-1]
    OldHeight   -   old grid height, OldHeight>1
    OldWidth    -   old grid width, OldWidth>1
    NewHeight   -   new grid height, NewHeight>1
    NewWidth    -   new grid width, NewWidth>1

Output parameters:
    B           -   function values at the new grid,
                    array[0..NewHeight-1, 0..NewWidth-1]

  -- ALGLIB routine --
     09.07.2007
     Copyright by Bochkanov Sergey
*************************************************************************/
void bilinearresamplecartesian(const ap::real_2d_array& a,
     int oldheight,
     int oldwidth,
     ap::real_2d_array& b,
     int newheight,
     int newwidth)
{
    int i;
    int j;
    int l;
    int c;
    double t;
    double u;

    b.setbounds(0, newheight-1, 0, newwidth-1);
    for(i = 0; i <= newheight-1; i++)
    {
        for(j = 0; j <= newwidth-1; j++)
        {
            l = i*(oldheight-1)/(newheight-1);
            if( l==oldheight-1 )
            {
                l = oldheight-2;
            }
            u = double(i)/double(newheight-1)*(oldheight-1)-l;
            c = j*(oldwidth-1)/(newwidth-1);
            if( c==oldwidth-1 )
            {
                c = oldwidth-2;
            }
            t = double(j*(oldwidth-1))/double(newwidth-1)-c;
            b(i,j) = (1-t)*(1-u)*a(l,c)+t*(1-u)*a(l,c+1)+t*u*a(l+1,c+1)+(1-t)*u*a(l+1,c);
        }
    }
}


/*************************************************************************
Obsolete subroutine for backwards compatibility
*************************************************************************/
void bicubicresample(int oldwidth,
     int oldheight,
     int newwidth,
     int newheight,
     const ap::real_2d_array& a,
     ap::real_2d_array& b)
{

    bicubicresamplecartesian(a, oldheight, oldwidth, b, newheight, newwidth);
}


/*************************************************************************
Obsolete subroutine for backwards compatibility
*************************************************************************/
void bilinearresample(int oldwidth,
     int oldheight,
     int newwidth,
     int newheight,
     const ap::real_2d_array& a,
     ap::real_2d_array& b)
{

    bilinearresamplecartesian(a, oldheight, oldwidth, b, newheight, newwidth);
}


/*************************************************************************
Internal subroutine.
Calculation of the first derivatives and the cross-derivative.
*************************************************************************/
static void bicubiccalcderivatives(const ap::real_2d_array& a,
     const ap::real_1d_array& x,
     const ap::real_1d_array& y,
     int m,
     int n,
     ap::real_2d_array& dx,
     ap::real_2d_array& dy,
     ap::real_2d_array& dxy)
{
    int i;
    int j;
    int k;
    ap::real_1d_array xt;
    ap::real_1d_array ft;
    ap::real_1d_array c;
    double s;
    double ds;
    double d2s;
    double v;

    dx.setbounds(0, m-1, 0, n-1);
    dy.setbounds(0, m-1, 0, n-1);
    dxy.setbounds(0, m-1, 0, n-1);
    
    //
    // dF/dX
    //
    xt.setbounds(0, n-1);
    ft.setbounds(0, n-1);
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            xt(j) = x(j);
            ft(j) = a(i,j);
        }
        buildcubicspline(xt, ft, n, 0, 0.0, 0, 0.0, c);
        for(j = 0; j <= n-1; j++)
        {
            splinedifferentiation(c, x(j), s, ds, d2s);
            dx(i,j) = ds;
        }
    }
    
    //
    // dF/dY
    //
    xt.setbounds(0, m-1);
    ft.setbounds(0, m-1);
    for(j = 0; j <= n-1; j++)
    {
        for(i = 0; i <= m-1; i++)
        {
            xt(i) = y(i);
            ft(i) = a(i,j);
        }
        buildcubicspline(xt, ft, m, 0, 0.0, 0, 0.0, c);
        for(i = 0; i <= m-1; i++)
        {
            splinedifferentiation(c, y(i), s, ds, d2s);
            dy(i,j) = ds;
        }
    }
    
    //
    // d2F/dXdY
    //
    xt.setbounds(0, n-1);
    ft.setbounds(0, n-1);
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            xt(j) = x(j);
            ft(j) = dy(i,j);
        }
        buildcubicspline(xt, ft, n, 0, 0.0, 0, 0.0, c);
        for(j = 0; j <= n-1; j++)
        {
            splinedifferentiation(c, x(j), s, ds, d2s);
            dxy(i,j) = ds;
        }
    }
}



