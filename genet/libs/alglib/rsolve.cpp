/*************************************************************************
This file is a part of ALGLIB project.

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
#include "rsolve.h"

/*************************************************************************
Solving a system of linear equations with a system matrix given by its
LU decomposition.

The algorithm solves a system of linear equations whose matrix is given by
its LU decomposition. In case of a singular matrix, the algorithm  returns
False.

The algorithm solves systems with a square matrix only.

Input parameters:
    A       -   LU decomposition of a system matrix in compact  form  (the
                result of the RMatrixLU subroutine).
    Pivots  -   row permutation table (the result of a
                RMatrixLU subroutine).
    B       -   right side of a system.
                Array whose index ranges within [0..N-1].
    N       -   size of matrix A.

Output parameters:
    X       -   solution of a system.
                Array whose index ranges within [0..N-1].

Result:
    True, if the matrix is not singular.
    False, if the matrux is singular. In this case, X doesn't contain a
solution.

  -- ALGLIB --
     Copyright 2005-2008 by Bochkanov Sergey
*************************************************************************/
bool rmatrixlusolve(const ap::real_2d_array& a,
     const ap::integer_1d_array& pivots,
     ap::real_1d_array b,
     int n,
     ap::real_1d_array& x)
{
    bool result;
    ap::real_1d_array y;
    int i;
    int j;
    double v;

    y.setbounds(0, n-1);
    x.setbounds(0, n-1);
    result = true;
    for(i = 0; i <= n-1; i++)
    {
        if( a(i,i)==0 )
        {
            result = false;
            return result;
        }
    }
    
    //
    // pivots
    //
    for(i = 0; i <= n-1; i++)
    {
        if( pivots(i)!=i )
        {
            v = b(i);
            b(i) = b(pivots(i));
            b(pivots(i)) = v;
        }
    }
    
    //
    // Ly = b
    //
    y(0) = b(0);
    for(i = 1; i <= n-1; i++)
    {
        v = ap::vdotproduct(&a(i, 0), &y(0), ap::vlen(0,i-1));
        y(i) = b(i)-v;
    }
    
    //
    // Ux = y
    //
    x(n-1) = y(n-1)/a(n-1,n-1);
    for(i = n-2; i >= 0; i--)
    {
        v = ap::vdotproduct(&a(i, i+1), &x(i+1), ap::vlen(i+1,n-1));
        x(i) = (y(i)-v)/a(i,i);
    }
    return result;
}


/*************************************************************************
Solving a system of linear equations.

The algorithm solves a system of linear equations by using the
LU decomposition. The algorithm solves systems with a square matrix only.

Input parameters:
    A   -   system matrix.
            Array whose indexes range within [0..N-1, 0..N-1].
    B   -   right side of a system.
            Array whose indexes range within [0..N-1].
    N   -   size of matrix A.

Output parameters:
    X   -   solution of a system.
            Array whose index ranges within [0..N-1].

Result:
    True, if the matrix is not singular.
    False, if the matrix is singular. In this case, X doesn't contain a
solution.

  -- ALGLIB --
     Copyright 2005-2008 by Bochkanov Sergey
*************************************************************************/
bool rmatrixsolve(ap::real_2d_array a,
     ap::real_1d_array b,
     int n,
     ap::real_1d_array& x)
{
    bool result;
    ap::integer_1d_array pivots;
    int i;

    rmatrixlu(a, n, n, pivots);
    result = rmatrixlusolve(a, pivots, b, n, x);
    return result;
}


/*************************************************************************
Obsolete 1-based subroutine
*************************************************************************/
bool solvesystemlu(const ap::real_2d_array& a,
     const ap::integer_1d_array& pivots,
     ap::real_1d_array b,
     int n,
     ap::real_1d_array& x)
{
    bool result;
    ap::real_1d_array y;
    int i;
    int j;
    double v;
    int ip1;
    int im1;

    y.setbounds(1, n);
    x.setbounds(1, n);
    result = true;
    for(i = 1; i <= n; i++)
    {
        if( a(i,i)==0 )
        {
            result = false;
            return result;
        }
    }
    
    //
    // pivots
    //
    for(i = 1; i <= n; i++)
    {
        if( pivots(i)!=i )
        {
            v = b(i);
            b(i) = b(pivots(i));
            b(pivots(i)) = v;
        }
    }
    
    //
    // Ly = b
    //
    y(1) = b(1);
    for(i = 2; i <= n; i++)
    {
        im1 = i-1;
        v = ap::vdotproduct(&a(i, 1), &y(1), ap::vlen(1,im1));
        y(i) = b(i)-v;
    }
    
    //
    // Ux = y
    //
    x(n) = y(n)/a(n,n);
    for(i = n-1; i >= 1; i--)
    {
        ip1 = i+1;
        v = ap::vdotproduct(&a(i, ip1), &x(ip1), ap::vlen(ip1,n));
        x(i) = (y(i)-v)/a(i,i);
    }
    return result;
}


/*************************************************************************
Obsolete 1-based subroutine
*************************************************************************/
bool solvesystem(ap::real_2d_array a,
     ap::real_1d_array b,
     int n,
     ap::real_1d_array& x)
{
    bool result;
    ap::integer_1d_array pivots;
    int i;

    ludecomposition(a, n, n, pivots);
    result = solvesystemlu(a, pivots, b, n, x);
    return result;
}



