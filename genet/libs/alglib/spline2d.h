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

#ifndef _spline2d_h
#define _spline2d_h

#include "ap.h"
#include "ialglib.h"

#include "spline3.h"


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
     ap::real_1d_array& c);


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
     ap::real_1d_array& c);


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
double splineinterpolation2d(const ap::real_1d_array& c, double x, double y);


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
     double& fxy);


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
     ap::real_2d_array& tbl);


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
     double by);


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
void spline2dlintransf(ap::real_1d_array& c, double a, double b);


/*************************************************************************
This subroutine makes the copy of the spline.

Input parameters:
    C   -   coefficients table.

Output parameters:
    CC  -   spline copy

  -- ALGLIB PROJECT --
     Copyright 29.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline2dcopy(const ap::real_1d_array& c, ap::real_1d_array& cc);


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
     int newwidth);


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
     int newwidth);


/*************************************************************************
Obsolete subroutine for backwards compatibility
*************************************************************************/
void bicubicresample(int oldwidth,
     int oldheight,
     int newwidth,
     int newheight,
     const ap::real_2d_array& a,
     ap::real_2d_array& b);


/*************************************************************************
Obsolete subroutine for backwards compatibility
*************************************************************************/
void bilinearresample(int oldwidth,
     int oldheight,
     int newwidth,
     int newheight,
     const ap::real_2d_array& a,
     ap::real_2d_array& b);


#endif
