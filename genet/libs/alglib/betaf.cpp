/*************************************************************************
Cephes Math Library Release 2.8:  June, 2000
Copyright by Stephen L. Moshier

Contributors:
    * Sergey Bochkanov (ALGLIB project). Translation from C to
      pseudocode.

See subroutines comments for additional copyrights.

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
#include "betaf.h"

/*************************************************************************
Beta function


                  -     -
                 | (a) | (b)
beta( a, b )  =  -----------.
                    -
                   | (a+b)

For large arguments the logarithm of the function is
evaluated using lgam(), then exponentiated.

ACCURACY:

                     Relative error:
arithmetic   domain     # trials      peak         rms
   IEEE       0,30       30000       8.1e-14     1.1e-14

Cephes Math Library Release 2.0:  April, 1987
Copyright 1984, 1987 by Stephen L. Moshier
*************************************************************************/
double beta(double a, double b)
{
    double result;
    double y;
    double sg;
    double s;

    sg = 1;
    ap::ap_error::make_assertion(a>0||a!=ap::ifloor(a), "Overflow in Beta");
    ap::ap_error::make_assertion(b>0||b!=ap::ifloor(b), "Overflow in Beta");
    y = a+b;
    if( fabs(y)>171.624376956302725 )
    {
        y = lngamma(y, s);
        sg = sg*s;
        y = lngamma(b, s)-y;
        sg = sg*s;
        y = lngamma(a, s)+y;
        sg = sg*s;
        ap::ap_error::make_assertion(y<=log(ap::maxrealnumber), "Overflow in Beta");
        result = sg*exp(y);
        return result;
    }
    y = stdgamma(y);
    ap::ap_error::make_assertion(y!=0, "Overflow in Beta");
    if( a>b )
    {
        y = stdgamma(a)/y;
        y = y*stdgamma(b);
    }
    else
    {
        y = stdgamma(b)/y;
        y = y*stdgamma(a);
    }
    result = y;
    return result;
}



