
#include <stdafx.h>
#include <stdio.h>
#include "testjbunit.h"

static void generatenonnormal1(int n,
     double med,
     double width,
     ap::real_1d_array& r);
static void generatenonnormal2(int n,
     double med,
     double width,
     ap::real_1d_array& r);
static void generatenormal(int n,
     double mean,
     double sigma,
     ap::real_1d_array& r);

bool testjbtest(bool silent)
{
    bool result;
    int passcount;
    bool detailedreport;
    double sigmathreshold;
    int n;
    int pass;
    int dtask;
    int npos;
    int ncnt;
    ap::integer_1d_array ntbl;
    int qcnt;
    ap::real_1d_array btqtbl;
    ap::real_1d_array btsigmatbl;
    ap::real_1d_array bttbl;
    double bt;
    double bpower;
    ap::real_1d_array x;
    bool tmptestok;
    bool ctestok;
    bool powertestok;
    bool waserrors;
    int i;
    int j;
    int k;
    double mean;
    double sigma;
    double v;

    waserrors = false;
    detailedreport = true;
    passcount = 20000;
    sigmathreshold = 5;
    n = 100+ap::randominteger(50);
    
    //
    // Prepare place
    //
    x.setbounds(0, n-1);
    
    //
    // Prepare quantiles tables for both-tail test and left/right test.
    // Note that since sign test statistic has discrete distribution
    // we should carefully select quantiles. Equation P(tail<=alpha)=alpha
    // holds only for some specific alpha.
    //
    qcnt = 5;
    bttbl.setbounds(0, qcnt-1);
    btqtbl.setbounds(0, qcnt-1);
    btsigmatbl.setbounds(0, qcnt-1);
    btqtbl(0) = 0.25;
    btqtbl(1) = 0.15;
    btqtbl(2) = 0.10;
    btqtbl(3) = 0.05;
    btqtbl(4) = 0.01;
    for(i = 0; i <= qcnt-1; i++)
    {
        btsigmatbl(i) = sqrt(btqtbl(i)*(1-btqtbl(i))/passcount);
    }
    
    //
    // Report header
    //
    if( !silent )
    {
        printf("TESTING JARQUE BERA TEST\n");
    }
    
    //
    // Continuous distribuiton test for validity.
    //
    
    //
    // Prepare p-table
    //
    for(i = 0; i <= qcnt-1; i++)
    {
        bttbl(i) = 0;
    }
    
    //
    // Fill p-table
    //
    for(pass = 1; pass <= passcount; pass++)
    {
        mean = 20*ap::randomreal()-10;
        sigma = 1+2*ap::randomreal();
        generatenormal(n, mean, sigma, x);
        jarqueberatest(x, n, bt);
        for(i = 0; i <= qcnt-1; i++)
        {
            if( bt<=btqtbl(i) )
            {
                bttbl(i) = bttbl(i)+double(1)/double(passcount);
            }
        }
    }
    
    //
    // Check
    //
    tmptestok = true;
    for(i = 0; i <= qcnt-1; i++)
    {
        if( fabs(bttbl(i)-btqtbl(i))/btsigmatbl(i)>sigmathreshold )
        {
            tmptestok = false;
        }
    }
    ctestok = tmptestok;
    
    //
    // Report
    //
    if( !silent&&detailedreport )
    {
        printf("\nCONTINUOUS TEST TABLE\n");
        printf("Q no.     err.\n");
        for(i = 0; i <= qcnt-1; i++)
        {
            printf("%1ld         %3.1lf std.\n",
                long(i+1),
                double(fabs(bttbl(i)-btqtbl(i))/btsigmatbl(i)));
        }
        if( tmptestok )
        {
            printf("TEST PASSED\n\n");
        }
        else
        {
            printf("TEST FAILED\n\n");
        }
    }
    
    //
    // Power test
    //
    
    //
    // Prepare p-table
    //
    for(i = 0; i <= qcnt-1; i++)
    {
        bttbl(i) = 0;
    }
    
    //
    // Fill p-table
    //
    bpower = 0;
    for(pass = 1; pass <= passcount; pass++)
    {
        if( pass%2==0 )
        {
            generatenonnormal1(n, 20*ap::randomreal()-10, double(5), x);
        }
        else
        {
            generatenonnormal2(n, 20*ap::randomreal()-10, double(5), x);
        }
        jarqueberatest(x, n, bt);
        if( bt<0.05 )
        {
            bpower = bpower+double(1)/double(passcount);
        }
    }
    
    //
    // Check
    //
    powertestok = bpower>0.95;
    
    //
    // Report
    //
    if( !silent&&detailedreport )
    {
        printf("\nPOWER TEST TABLE\n");
        printf("TEST POWER:  %4.2lf\n",
            double(bpower));
        if( powertestok )
        {
            printf("TEST PASSED\n\n");
        }
        else
        {
            printf("TEST FAILED\n\n");
        }
    }
    
    //
    // Final report
    //
    waserrors = !ctestok||!powertestok;
    if( !silent )
    {
        printf("CONTINUOUS VALIDITY TEST:                ");
        if( ctestok )
        {
            printf("OK\n");
        }
        else
        {
            printf("FAILED\n");
        }
        printf("POWER TEST:                              ");
        if( powertestok )
        {
            printf("OK\n");
        }
        else
        {
            printf("FAILED\n");
        }
        if( waserrors )
        {
            printf("TEST SUMMARY: FAILED\n");
        }
        else
        {
            printf("TEST SUMMARY: PASSED\n");
        }
        printf("\n\n");
    }
    result = !waserrors;
    return result;
}


static void generatenonnormal1(int n,
     double med,
     double width,
     ap::real_1d_array& r)
{
    int i;
    double v;

    for(i = 0; i <= n-1; i++)
    {
        r(i) = med+width*(2*ap::randominteger(2)-1);
    }
}


static void generatenonnormal2(int n,
     double med,
     double width,
     ap::real_1d_array& r)
{
    int i;
    double v;

    for(i = 0; i <= n-1; i++)
    {
        v = 2*ap::randomreal()-1;
        if( v>0 )
        {
            r(i) = med+width*v;
        }
        else
        {
            r(i) = med+2*width*v;
        }
    }
}


static void generatenormal(int n,
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
bool testjbunit_test_silent()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testjbtest(true);
    return result;
#else
    return _i_testjbunit_test_silent();
#endif
}


/*************************************************************************
Unit test
*************************************************************************/
bool testjbunit_test()
{
    bool result;

    result = testjbtest(false);
    return result;
}



