
#include <stdafx.h>
#include <stdio.h>
#include "teststudentttestsunit.h"

static void teststudenttgennormrnd(int n,
     double mean,
     double sigma,
     ap::real_1d_array& r);

bool teststudentt(bool silent)
{
    bool result;
    int pass;
    int passcount;
    int maxn;
    int n;
    int m;
    int i;
    int j;
    int qcnt;
    ap::real_1d_array x;
    ap::real_1d_array y;
    ap::real_1d_array qtbl;
    ap::real_1d_array ptbl;
    ap::real_1d_array lptbl;
    ap::real_1d_array rptbl;
    ap::real_1d_array sigmatbl;
    double bt;
    double lt;
    double rt;
    double v;
    bool waserrors;
    double sigmathreshold;

    waserrors = false;
    maxn = 1000;
    passcount = 20000;
    sigmathreshold = 6;
    x.setbounds(0, maxn-1);
    y.setbounds(0, maxn-1);
    qcnt = 8;
    ptbl.setbounds(0, qcnt-1);
    lptbl.setbounds(0, qcnt-1);
    rptbl.setbounds(0, qcnt-1);
    qtbl.setbounds(0, qcnt-1);
    sigmatbl.setbounds(0, qcnt-1);
    qtbl(0) = 0.25;
    qtbl(1) = 0.15;
    qtbl(2) = 0.10;
    qtbl(3) = 0.05;
    qtbl(4) = 0.04;
    qtbl(5) = 0.03;
    qtbl(6) = 0.02;
    qtbl(7) = 0.01;
    for(i = 0; i <= qcnt-1; i++)
    {
        sigmatbl(i) = sqrt(qtbl(i)*(1-qtbl(i))/passcount);
    }
    if( !silent )
    {
        printf("TESTING STUDENT T\n");
    }
    
    //
    // 1-sample test
    //
    if( !silent )
    {
        printf("Testing 1-sample test for 1-type errors\n");
    }
    n = 15;
    for(i = 0; i <= qcnt-1; i++)
    {
        ptbl(i) = 0;
        lptbl(i) = 0;
        rptbl(i) = 0;
    }
    for(pass = 1; pass <= passcount; pass++)
    {
        
        //
        // Both tails
        //
        teststudenttgennormrnd(n, 0.0, 1+4*ap::randomreal(), x);
        studentttest1(x, n, double(0), bt, lt, rt);
        for(i = 0; i <= qcnt-1; i++)
        {
            if( bt<=qtbl(i) )
            {
                ptbl(i) = ptbl(i)+double(1)/double(passcount);
            }
        }
        
        //
        // Left tail
        //
        teststudenttgennormrnd(n, 0.5*ap::randomreal(), 1+4*ap::randomreal(), x);
        studentttest1(x, n, double(0), bt, lt, rt);
        for(i = 0; i <= qcnt-1; i++)
        {
            if( lt<=qtbl(i) )
            {
                lptbl(i) = lptbl(i)+double(1)/double(passcount);
            }
        }
        
        //
        // Right tail
        //
        teststudenttgennormrnd(n, -0.5*ap::randomreal(), 1+4*ap::randomreal(), x);
        studentttest1(x, n, double(0), bt, lt, rt);
        for(i = 0; i <= qcnt-1; i++)
        {
            if( rt<=qtbl(i) )
            {
                rptbl(i) = rptbl(i)+double(1)/double(passcount);
            }
        }
    }
    if( !silent )
    {
        printf("Expect. Both    Left    Right\n");
        for(i = 0; i <= qcnt-1; i++)
        {
            printf("%4.1lf%%   %4.1lf%%   %4.1lf%%   %4.1lf%%   \n",
                double(qtbl(i)*100),
                double(ptbl(i)*100),
                double(lptbl(i)*100),
                double(rptbl(i)*100));
        }
    }
    for(i = 0; i <= qcnt-1; i++)
    {
        waserrors = waserrors||fabs(ptbl(i)-qtbl(i))/sigmatbl(i)>sigmathreshold;
        waserrors = waserrors||(lptbl(i)-qtbl(i))/sigmatbl(i)>sigmathreshold;
        waserrors = waserrors||(rptbl(i)-qtbl(i))/sigmatbl(i)>sigmathreshold;
    }
    
    //
    // 2-sample test
    //
    if( !silent )
    {
        printf("\n\nTesting 2-sample test for 1-type errors\n");
    }
    for(i = 0; i <= qcnt-1; i++)
    {
        ptbl(i) = 0;
        lptbl(i) = 0;
        rptbl(i) = 0;
    }
    for(pass = 1; pass <= passcount; pass++)
    {
        n = 5+ap::randominteger(20);
        m = 5+ap::randominteger(20);
        v = 1+4*ap::randomreal();
        
        //
        // Both tails
        //
        teststudenttgennormrnd(n, 0.0, v, x);
        teststudenttgennormrnd(m, 0.0, v, y);
        studentttest2(x, n, y, m, bt, lt, rt);
        for(i = 0; i <= qcnt-1; i++)
        {
            if( bt<=qtbl(i) )
            {
                ptbl(i) = ptbl(i)+double(1)/double(passcount);
            }
        }
        
        //
        // Left tail
        //
        teststudenttgennormrnd(n, 0.5*ap::randomreal(), v, x);
        teststudenttgennormrnd(m, 0.0, v, y);
        studentttest2(x, n, y, m, bt, lt, rt);
        for(i = 0; i <= qcnt-1; i++)
        {
            if( lt<=qtbl(i) )
            {
                lptbl(i) = lptbl(i)+double(1)/double(passcount);
            }
        }
        
        //
        // Right tail
        //
        teststudenttgennormrnd(n, -0.5*ap::randomreal(), v, x);
        teststudenttgennormrnd(m, 0.0, v, y);
        studentttest2(x, n, y, m, bt, lt, rt);
        for(i = 0; i <= qcnt-1; i++)
        {
            if( rt<=qtbl(i) )
            {
                rptbl(i) = rptbl(i)+double(1)/double(passcount);
            }
        }
    }
    if( !silent )
    {
        printf("Expect. Both    Left    Right\n");
        for(i = 0; i <= qcnt-1; i++)
        {
            printf("%4.1lf%%   %4.1lf%%   %4.1lf%%   %4.1lf%%   \n",
                double(qtbl(i)*100),
                double(ptbl(i)*100),
                double(lptbl(i)*100),
                double(rptbl(i)*100));
        }
    }
    for(i = 0; i <= qcnt-1; i++)
    {
        waserrors = waserrors||fabs(ptbl(i)-qtbl(i))/sigmatbl(i)>sigmathreshold;
        waserrors = waserrors||(lptbl(i)-qtbl(i))/sigmatbl(i)>sigmathreshold;
        waserrors = waserrors||(rptbl(i)-qtbl(i))/sigmatbl(i)>sigmathreshold;
    }
    
    //
    // Unequal variance test
    //
    if( !silent )
    {
        printf("\n\nTesting unequal variance test for 1-type errors\n");
    }
    for(i = 0; i <= qcnt-1; i++)
    {
        ptbl(i) = 0;
        lptbl(i) = 0;
        rptbl(i) = 0;
    }
    for(pass = 1; pass <= passcount; pass++)
    {
        n = 15+ap::randominteger(20);
        m = 15+ap::randominteger(20);
        
        //
        // Both tails
        //
        teststudenttgennormrnd(n, 0.0, 1+4*ap::randomreal(), x);
        teststudenttgennormrnd(m, 0.0, 1+4*ap::randomreal(), y);
        unequalvariancettest(x, n, y, m, bt, lt, rt);
        for(i = 0; i <= qcnt-1; i++)
        {
            if( bt<=qtbl(i) )
            {
                ptbl(i) = ptbl(i)+double(1)/double(passcount);
            }
        }
        
        //
        // Left tail
        //
        teststudenttgennormrnd(n, 0.5*ap::randomreal(), 1+4*ap::randomreal(), x);
        teststudenttgennormrnd(m, 0.0, 1+4*ap::randomreal(), y);
        unequalvariancettest(x, n, y, m, bt, lt, rt);
        for(i = 0; i <= qcnt-1; i++)
        {
            if( lt<=qtbl(i) )
            {
                lptbl(i) = lptbl(i)+double(1)/double(passcount);
            }
        }
        
        //
        // Right tail
        //
        teststudenttgennormrnd(n, -0.5*ap::randomreal(), 1+4*ap::randomreal(), x);
        teststudenttgennormrnd(m, 0.0, 1+4*ap::randomreal(), y);
        unequalvariancettest(x, n, y, m, bt, lt, rt);
        for(i = 0; i <= qcnt-1; i++)
        {
            if( rt<=qtbl(i) )
            {
                rptbl(i) = rptbl(i)+double(1)/double(passcount);
            }
        }
    }
    if( !silent )
    {
        printf("Expect. Both    Left    Right\n");
        for(i = 0; i <= qcnt-1; i++)
        {
            printf("%4.1lf%%   %4.1lf%%   %4.1lf%%   %4.1lf%%   \n",
                double(qtbl(i)*100),
                double(ptbl(i)*100),
                double(lptbl(i)*100),
                double(rptbl(i)*100));
        }
    }
    for(i = 0; i <= qcnt-1; i++)
    {
        waserrors = waserrors||fabs(ptbl(i)-qtbl(i))/sigmatbl(i)>sigmathreshold;
        waserrors = waserrors||(lptbl(i)-qtbl(i))/sigmatbl(i)>sigmathreshold;
        waserrors = waserrors||(rptbl(i)-qtbl(i))/sigmatbl(i)>sigmathreshold;
    }
    
    //
    //
    //
    if( !silent )
    {
        if( waserrors )
        {
            printf("TEST FAILED\n");
        }
        else
        {
            printf("TEST PASSED\n");
        }
    }
    result = !waserrors;
    return result;
}


static void teststudenttgennormrnd(int n,
     double mean,
     double sigma,
     ap::real_1d_array& r)
{
    int i;
    double u;
    double v;
    double s;
    double sum;

    i = 0;
    while(i<n)
    {
        u = (2*ap::randominteger(2)-1)*ap::randomreal();
        v = (2*ap::randominteger(2)-1)*ap::randomreal();
        sum = u*u+v*v;
        if( sum<1&&sum>0 )
        {
            sum = sqrt(-2*log(sum)/sum);
            if( i<n )
            {
                r(i) = sigma*u*sum+mean;
            }
            if( i+1<n )
            {
                r(i+1) = sigma*v*sum+mean;
            }
            i = i+2;
        }
    }
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool teststudentttestsunit_test_silent()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = teststudentt(true);
    return result;
#else
    return _i_teststudentttestsunit_test_silent();
#endif
}


/*************************************************************************
Unit test
*************************************************************************/
bool teststudentttestsunit_test()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = teststudentt(false);
    return result;
#else
    return _i_teststudentttestsunit_test();
#endif
}



