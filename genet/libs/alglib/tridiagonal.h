/*************************************************************************
Copyright (c) 1992-2007 The University of Tennessee.  All rights reserved.

Contributors:
    * Sergey Bochkanov (ALGLIB project). Translation from FORTRAN to
      pseudocode.

See subroutines comments for additional copyrights.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

- Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer listed
  in this license in the documentation and/or other materials
  provided with the distribution.

- Neither the name of the copyright holders nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*************************************************************************/

#ifndef _tridiagonal_h
#define _tridiagonal_h

#include "ap.h"
#include "ialglib.h"

#include "sblas.h"
#include "reflections.h"


/*************************************************************************
Reduction of a symmetric matrix which is given by its higher or lower
triangular part to a tridiagonal matrix using orthogonal similarity
transformation: Q'*A*Q=T.

Input parameters:
    A       -   matrix to be transformed
                array with elements [0..N-1, 0..N-1].
    N       -   size of matrix A.
    IsUpper -   storage format. If IsUpper = True, then matrix A is given
                by its upper triangle, and the lower triangle is not used
                and not modified by the algorithm, and vice versa
                if IsUpper = False.

Output parameters:
    A       -   matrices T and Q in  compact form (see lower)
    Tau     -   array of factors which are forming matrices H(i)
                array with elements [0..N-2].
    D       -   main diagonal of symmetric matrix T.
                array with elements [0..N-1].
    E       -   secondary diagonal of symmetric matrix T.
                array with elements [0..N-2].


  If IsUpper=True, the matrix Q is represented as a product of elementary
  reflectors

     Q = H(n-2) . . . H(2) H(0).

  Each H(i) has the form

     H(i) = I - tau * v * v'

  where tau is a real scalar, and v is a real vector with
  v(i+1:n-1) = 0, v(i) = 1, v(0:i-1) is stored on exit in
  A(0:i-1,i+1), and tau in TAU(i).

  If IsUpper=False, the matrix Q is represented as a product of elementary
  reflectors

     Q = H(0) H(2) . . . H(n-2).

  Each H(i) has the form

     H(i) = I - tau * v * v'

  where tau is a real scalar, and v is a real vector with
  v(0:i) = 0, v(i+1) = 1, v(i+2:n-1) is stored on exit in A(i+2:n-1,i),
  and tau in TAU(i).

  The contents of A on exit are illustrated by the following examples
  with n = 5:

  if UPLO = 'U':                       if UPLO = 'L':

    (  d   e   v1  v2  v3 )              (  d                  )
    (      d   e   v2  v3 )              (  e   d              )
    (          d   e   v3 )              (  v0  e   d          )
    (              d   e  )              (  v0  v1  e   d      )
    (                  d  )              (  v0  v1  v2  e   d  )

  where d and e denote diagonal and off-diagonal elements of T, and vi
  denotes an element of the vector defining H(i).

  -- LAPACK routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     October 31, 1992
*************************************************************************/
void smatrixtd(ap::real_2d_array& a,
     int n,
     bool isupper,
     ap::real_1d_array& tau,
     ap::real_1d_array& d,
     ap::real_1d_array& e);


/*************************************************************************
Unpacking matrix Q which reduces symmetric matrix to a tridiagonal
form.

Input parameters:
    A       -   the result of a SMatrixTD subroutine
    N       -   size of matrix A.
    IsUpper -   storage format (a parameter of SMatrixTD subroutine)
    Tau     -   the result of a SMatrixTD subroutine

Output parameters:
    Q       -   transformation matrix.
                array with elements [0..N-1, 0..N-1].

  -- ALGLIB --
     Copyright 2005-2008 by Bochkanov Sergey
*************************************************************************/
void smatrixtdunpackq(const ap::real_2d_array& a,
     const int& n,
     const bool& isupper,
     const ap::real_1d_array& tau,
     ap::real_2d_array& q);


/*************************************************************************
Obsolete 1-based subroutine
*************************************************************************/
void totridiagonal(ap::real_2d_array& a,
     int n,
     bool isupper,
     ap::real_1d_array& tau,
     ap::real_1d_array& d,
     ap::real_1d_array& e);


/*************************************************************************
Obsolete 1-based subroutine
*************************************************************************/
void unpackqfromtridiagonal(const ap::real_2d_array& a,
     const int& n,
     const bool& isupper,
     const ap::real_1d_array& tau,
     ap::real_2d_array& q);


#endif
