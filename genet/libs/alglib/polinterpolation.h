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

#ifndef _polinterpolation_h
#define _polinterpolation_h

#include "ap.h"
#include "ialglib.h"

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
     double t);


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
     double t);


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
     double t);


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
     double t);


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
     double t);


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
     double& d2p);


/*************************************************************************
Obsolete algorithm, replaced by NevilleInterpolation.
*************************************************************************/
double lagrangeinterpolate(int n,
     const ap::real_1d_array& x,
     ap::real_1d_array f,
     double t);


/*************************************************************************
Obsolete algorithm, replaced by NevilleInterpolationWithDerivative
*************************************************************************/
void lagrangederivative(int n,
     const ap::real_1d_array& x,
     ap::real_1d_array f,
     double t,
     double& p,
     double& dp);


#endif
