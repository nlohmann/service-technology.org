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
#include "polinterpolation.h"

/*************************************************************************
Interpolation using barycentric formula

F(t) = SUM(i=0,n-1,w[i]*f[i]/(t-x[i])) / SUM(i=0,n-1,w[i]/(t-x[i]))

Input parameters:
    X   -   interpolation nodes, array[0..N-1]
    F   -   function values, array[0..N-1]
    W   -   barycentric weights, array[0..N-1]
    N   -   nodes count, N>0
    T   -   interpolation point
    
Result:
    barycentric interpolant F(t)

  -- ALGLIB --
     Copyright 28.05.2007 by Bochkanov Sergey
*************************************************************************/
double barycentricinterpolation(const ap::real_1d_array& x,
     const ap::real_1d_array& f,
     const ap::real_1d_array& w,
     int n,
     double t)
{
    double result;
    double s1;
    double s2;
    double v;
    double threshold;
    double s;
    int i;
    int j;

    ap::ap_error::make_assertion(n>0, "BarycentricInterpolation: N<=0!");
    threshold = sqrt(ap::minrealnumber);
    
    //
    // First, decide: should we use "safe" formula (guarded
    // against overflow) or fast one?
    //
    j = 0;
    s = t-x(0);
    for(i = 1; i <= n-1; i++)
    {
        if( fabs(t-x(i))<fabs(s) )
        {
            s = t-x(i);
            j = i;
        }
    }
    if( s==0 )
    {
        result = f(j);
        return result;
    }
    if( fabs(s)>threshold )
    {
        
        //
        // use fast formula
        //
        j = -1;
        s = 1.0;
    }
    
    //
    // Calculate using safe or fast barycentric formula
    //
    s1 = 0;
    s2 = 0;
    for(i = 0; i <= n-1; i++)
    {
        if( i!=j )
        {
            v = s*w(i)/(t-x(i));
            s1 = s1+v*f(i);
            s2 = s2+v;
        }
        else
        {
            v = w(i);
            s1 = s1+v*f(i);
            s2 = s2+v;
        }
    }
    result = s1/s2;
    return result;
}


/*************************************************************************
Polynomial interpolation on the equidistant nodes using barycentric
formula. O(N) complexity.

Input parameters:
    A,B -   interpolation interval [A,B]
    F   -   function values, array[0..N-1].
            F[i] = F(A+(B-A)*i/(N-1))
    N   -   nodes count
    T   -   interpolation point

Result:
    the value of the interpolation polynomial F(t)

  -- ALGLIB --
     Copyright 28.05.2007 by Bochkanov Sergey
*************************************************************************/
double equidistantpolynomialinterpolation(double a,
     double b,
     const ap::real_1d_array& f,
     int n,
     double t)
{
    double result;
    double s1;
    double s2;
    double v;
    double threshold;
    double s;
    int i;
    int j;
    double w;
    double x;

    ap::ap_error::make_assertion(n>0, "BarycentricInterpolation: N<=0!");
    threshold = sqrt(ap::minrealnumber);
    
    //
    // Special case: N=1
    //
    if( n==1 )
    {
        result = f(0);
        return result;
    }
    
    //
    // First, decide: should we use "safe" formula (guarded
    // against overflow) or fast one?
    //
    j = 0;
    s = t-a;
    for(i = 1; i <= n-1; i++)
    {
        x = a+double(i)/double(n-1)*(b-a);
        if( fabs(t-x)<fabs(s) )
        {
            s = t-x;
            j = i;
        }
    }
    if( s==0 )
    {
        result = f(j);
        return result;
    }
    if( fabs(s)>threshold )
    {
        
        //
        // use fast formula
        //
        j = -1;
        s = 1.0;
    }
    
    //
    // Calculate using safe or fast barycentric formula
    //
    s1 = 0;
    s2 = 0;
    w = 1.0;
    for(i = 0; i <= n-1; i++)
    {
        if( i!=j )
        {
            v = s*w/(t-(a+double(i)/double(n-1)*(b-a)));
            s1 = s1+v*f(i);
            s2 = s2+v;
        }
        else
        {
            v = w;
            s1 = s1+v*f(i);
            s2 = s2+v;
        }
        w = -w*(n-1-i);
        w = w/(i+1);
    }
    result = s1/s2;
    return result;
}


/*************************************************************************
Polynomial interpolation on the Chebyshev nodes (first kind) using
barycentric formula. O(N) complexity.

Input parameters:
    A,B -   interpolation interval [A,B]
    F   -   function values, array[0..N-1].
            F[i] = F(0.5*(B+A) + 0.5*(B-A)*Cos(PI*(2*i+1)/(2*n)))
    N   -   nodes count
    T   -   interpolation point

Result:
    the value of the interpolation polynomial F(t)

  -- ALGLIB --
     Copyright 28.05.2007 by Bochkanov Sergey
*************************************************************************/
double chebyshev1interpolation(double a,
     double b,
     const ap::real_1d_array& f,
     int n,
     double t)
{
    double result;
    double s1;
    double s2;
    double v;
    double threshold;
    double s;
    int i;
    int j;
    double a0;
    double delta;
    double alpha;
    double beta;
    double ca;
    double sa;
    double tempc;
    double temps;
    double x;
    double w;
    double p1;

    ap::ap_error::make_assertion(n>0, "Chebyshev1Interpolation: N<=0!");
    threshold = sqrt(ap::minrealnumber);
    t = (t-0.5*(a+b))/(0.5*(b-a));
    
    //
    // Prepare information for the recurrence formula
    // used to calculate sin(pi*(2j+1)/(2n+2)) and
    // cos(pi*(2j+1)/(2n+2)):
    //
    // A0    = pi/(2n+2)
    // Delta = pi/(n+1)
    // Alpha = 2 sin^2 (Delta/2)
    // Beta  = sin(Delta)
    //
    // so that sin(..) = sin(A0+j*delta) and cos(..) = cos(A0+j*delta).
    // Then we use
    //
    // sin(x+delta) = sin(x) - (alpha*sin(x) - beta*cos(x))
    // cos(x+delta) = cos(x) - (alpha*cos(x) - beta*sin(x))
    //
    // to repeatedly calculate sin(..) and cos(..).
    //
    a0 = ap::pi()/(2*(n-1)+2);
    delta = 2*ap::pi()/(2*(n-1)+2);
    alpha = 2*ap::sqr(sin(delta/2));
    beta = sin(delta);
    
    //
    // First, decide: should we use "safe" formula (guarded
    // against overflow) or fast one?
    //
    ca = cos(a0);
    sa = sin(a0);
    j = 0;
    x = ca;
    s = t-x;
    for(i = 1; i <= n-1; i++)
    {
        
        //
        // Next X[i]
        //
        temps = sa-(alpha*sa-beta*ca);
        tempc = ca-(alpha*ca+beta*sa);
        sa = temps;
        ca = tempc;
        x = ca;
        
        //
        // Use X[i]
        //
        if( fabs(t-x)<fabs(s) )
        {
            s = t-x;
            j = i;
        }
    }
    if( s==0 )
    {
        result = f(j);
        return result;
    }
    if( fabs(s)>threshold )
    {
        
        //
        // use fast formula
        //
        j = -1;
        s = 1.0;
    }
    
    //
    // Calculate using safe or fast barycentric formula
    //
    s1 = 0;
    s2 = 0;
    ca = cos(a0);
    sa = sin(a0);
    p1 = 1.0;
    for(i = 0; i <= n-1; i++)
    {
        
        //
        // Calculate X[i], W[i]
        //
        x = ca;
        w = p1*sa;
        
        //
        // Proceed
        //
        if( i!=j )
        {
            v = s*w/(t-x);
            s1 = s1+v*f(i);
            s2 = s2+v;
        }
        else
        {
            v = w;
            s1 = s1+v*f(i);
            s2 = s2+v;
        }
        
        //
        // Next CA, SA, P1
        //
        temps = sa-(alpha*sa-beta*ca);
        tempc = ca-(alpha*ca+beta*sa);
        sa = temps;
        ca = tempc;
        p1 = -p1;
    }
    result = s1/s2;
    return result;
}


/*************************************************************************
Polynomial interpolation on the Chebyshev nodes (second kind) using
barycentric formula. O(N) complexity.

Input parameters:
    A,B -   interpolation interval [A,B]
    F   -   function values, array[0..N-1].
            F[i] = F(0.5*(B+A) + 0.5*(B-A)*Cos(PI*i/(n-1)))
    N   -   nodes count
    T   -   interpolation point

Result:
    the value of the interpolation polynomial F(t)

  -- ALGLIB --
     Copyright 28.05.2007 by Bochkanov Sergey
*************************************************************************/
double chebyshev2interpolation(double a,
     double b,
     const ap::real_1d_array& f,
     int n,
     double t)
{
    double result;
    double s1;
    double s2;
    double v;
    double threshold;
    double s;
    int i;
    int j;
    double a0;
    double delta;
    double alpha;
    double beta;
    double ca;
    double sa;
    double tempc;
    double temps;
    double x;
    double w;
    double p1;

    ap::ap_error::make_assertion(n>1, "Chebyshev2Interpolation: N<=1!");
    threshold = sqrt(ap::minrealnumber);
    t = (t-0.5*(a+b))/(0.5*(b-a));
    
    //
    // Prepare information for the recurrence formula
    // used to calculate sin(pi*i/n) and
    // cos(pi*i/n):
    //
    // A0    = 0
    // Delta = pi/n
    // Alpha = 2 sin^2 (Delta/2)
    // Beta  = sin(Delta)
    //
    // so that sin(..) = sin(A0+j*delta) and cos(..) = cos(A0+j*delta).
    // Then we use
    //
    // sin(x+delta) = sin(x) - (alpha*sin(x) - beta*cos(x))
    // cos(x+delta) = cos(x) - (alpha*cos(x) - beta*sin(x))
    //
    // to repeatedly calculate sin(..) and cos(..).
    //
    a0 = 0.0;
    delta = ap::pi()/(n-1);
    alpha = 2*ap::sqr(sin(delta/2));
    beta = sin(delta);
    
    //
    // First, decide: should we use "safe" formula (guarded
    // against overflow) or fast one?
    //
    ca = cos(a0);
    sa = sin(a0);
    j = 0;
    x = ca;
    s = t-x;
    for(i = 1; i <= n-1; i++)
    {
        
        //
        // Next X[i]
        //
        temps = sa-(alpha*sa-beta*ca);
        tempc = ca-(alpha*ca+beta*sa);
        sa = temps;
        ca = tempc;
        x = ca;
        
        //
        // Use X[i]
        //
        if( fabs(t-x)<fabs(s) )
        {
            s = t-x;
            j = i;
        }
    }
    if( s==0 )
    {
        result = f(j);
        return result;
    }
    if( fabs(s)>threshold )
    {
        
        //
        // use fast formula
        //
        j = -1;
        s = 1.0;
    }
    
    //
    // Calculate using safe or fast barycentric formula
    //
    s1 = 0;
    s2 = 0;
    ca = cos(a0);
    sa = sin(a0);
    p1 = 1.0;
    for(i = 0; i <= n-1; i++)
    {
        
        //
        // Calculate X[i], W[i]
        //
        x = ca;
        if( i==0||i==n-1 )
        {
            w = 0.5*p1;
        }
        else
        {
            w = 1.0*p1;
        }
        
        //
        // Proceed
        //
        if( i!=j )
        {
            v = s*w/(t-x);
            s1 = s1+v*f(i);
            s2 = s2+v;
        }
        else
        {
            v = w;
            s1 = s1+v*f(i);
            s2 = s2+v;
        }
        
        //
        // Next CA, SA, P1
        //
        temps = sa-(alpha*sa-beta*ca);
        tempc = ca-(alpha*ca+beta*sa);
        sa = temps;
        ca = tempc;
        p1 = -p1;
    }
    result = s1/s2;
    return result;
}


/*************************************************************************
Polynomial interpolation on the arbitrary nodes using Neville's algorithm.
O(N^2) complexity.

Input parameters:
    X   -   interpolation nodes, array[0..N-1].
    F   -   function values, array[0..N-1].
    N   -   nodes count
    T   -   interpolation point

Result:
    the value of the interpolation polynomial F(t)

  -- ALGLIB --
     Copyright 28.05.2007 by Bochkanov Sergey
*************************************************************************/
double nevilleinterpolation(const ap::real_1d_array& x,
     ap::real_1d_array f,
     int n,
     double t)
{
    double result;
    int m;
    int i;

    n = n-1;
    for(m = 1; m <= n; m++)
    {
        for(i = 0; i <= n-m; i++)
        {
            f(i) = ((t-x(i+m))*f(i)+(x(i)-t)*f(i+1))/(x(i)-x(i+m));
        }
    }
    result = f(0);
    return result;
}


/*************************************************************************
Polynomial interpolation on the arbitrary nodes using Neville's algorithm.
O(N^2) complexity. Subroutine  returns  the  value  of  the  interpolation
polynomial, the first and the second derivative.

Input parameters:
    X   -   interpolation nodes, array[0..N-1].
    F   -   function values, array[0..N-1].
    N   -   nodes count
    T   -   interpolation point

Output parameters:
    P   -   the value of the interpolation polynomial F(t)
    DP  -   the first derivative of the interpolation polynomial dF(t)/dt
    D2P -   the second derivative of the interpolation polynomial d2F(t)/dt2

  -- ALGLIB --
     Copyright 28.05.2007 by Bochkanov Sergey
*************************************************************************/
void nevilledifferentiation(const ap::real_1d_array& x,
     ap::real_1d_array f,
     int n,
     double t,
     double& p,
     double& dp,
     double& d2p)
{
    int m;
    int i;
    ap::real_1d_array df;
    ap::real_1d_array d2f;

    n = n-1;
    df.setbounds(0, n);
    d2f.setbounds(0, n);
    for(i = 0; i <= n; i++)
    {
        d2f(i) = 0;
        df(i) = 0;
    }
    for(m = 1; m <= n; m++)
    {
        for(i = 0; i <= n-m; i++)
        {
            d2f(i) = ((t-x(i+m))*d2f(i)+(x(i)-t)*d2f(i+1)+2*df(i)-2*df(i+1))/(x(i)-x(i+m));
            df(i) = ((t-x(i+m))*df(i)+f(i)+(x(i)-t)*df(i+1)-f(i+1))/(x(i)-x(i+m));
            f(i) = ((t-x(i+m))*f(i)+(x(i)-t)*f(i+1))/(x(i)-x(i+m));
        }
    }
    p = f(0);
    dp = df(0);
    d2p = d2f(0);
}


/*************************************************************************
Obsolete algorithm, replaced by NevilleInterpolation.
*************************************************************************/
double lagrangeinterpolate(int n,
     const ap::real_1d_array& x,
     ap::real_1d_array f,
     double t)
{
    double result;

    result = nevilleinterpolation(x, f, n, t);
    return result;
}


/*************************************************************************
Obsolete algorithm, replaced by NevilleInterpolationWithDerivative
*************************************************************************/
void lagrangederivative(int n,
     const ap::real_1d_array& x,
     ap::real_1d_array f,
     double t,
     double& p,
     double& dp)
{
    double d2p;

    nevilledifferentiation(x, f, n, t, p, dp, d2p);
}



