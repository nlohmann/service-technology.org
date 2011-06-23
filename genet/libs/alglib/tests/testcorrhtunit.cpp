
#include <stdafx.h>
#include <stdio.h>
#include "testcorrhtunit.h"

static void generate(int n, double med, double width, ap::real_1d_array& r);
static void generatenormal(int n,
     double mean,
     double sigma,
     ap::real_1d_array& r);

bool testcorrtest(bool silent)
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
    ap::real_1d_array lrqtbl;
    ap::real_1d_array btsigmatbl;
    ap::real_1d_array lrsigmatbl;
    ap::real_1d_array bttbl;
    ap::real_1d_array lttbl;
    ap::real_1d_array rttbl;
    double bt;
    double lt;
    double rt;
    double lpower;
    double rpower;
    double bpower;
    ap::real_1d_array x;
    ap::real_1d_array y;
    bool tmptestok;
    bool ptestok;
    bool stestok;
    bool ptestpowok;
    bool stestpowok;
    bool waserrors;
    int i;
    int j;
    int k;
    double med;
    double width;
    double v;
    double r;

    waserrors = false;
    detailedreport = true;
    passcount = 20000;
    sigmathreshold = 5;
    n = 40;
    
    //
    // Prepare place
    //
    x.setbounds(0, n-1);
    y.setbounds(0, n-1);
    
    //
    // Prepare quantiles tables for both-tail test and left/right test.
    // Note that since Spearman test statistic has discrete distribution
    // we should carefully select quantiles. Equation P(tail<=alpha)=alpha
    // holds only for some specific alpha.
    //
    qcnt = 4;
    bttbl.setbounds(0, qcnt-1);
    lttbl.setbounds(0, qcnt-1);
    rttbl.setbounds(0, qcnt-1);
    btqtbl.setbounds(0, qcnt-1);
    lrqtbl.setbounds(0, qcnt-1);
    btsigmatbl.setbounds(0, qcnt-1);
    lrsigmatbl.setbounds(0, qcnt-1);
    btqtbl(0) = 0.25;
    btqtbl(1) = 0.15;
    btqtbl(2) = 0.10;
    btqtbl(3) = 0.05;
    for(i = 0; i <= qcnt-1; i++)
    {
        lrqtbl(i) = btqtbl(i);
    }
    for(i = 0; i <= qcnt-1; i++)
    {
        btsigmatbl(i) = sqrt(btqtbl(i)*(1-btqtbl(i))/passcount);
        lrsigmatbl(i) = sqrt(lrqtbl(i)*(1-lrqtbl(i))/passcount);
    }
    
    //
    // Report header
    //
    if( !silent )
    {
        printf("TESTING CORRELATION TEST\n");
    }
    
    //
    // Pearson correlation test for validity.
    //
    
    //
    // Prepare p-table
    //
    for(i = 0; i <= qcnt-1; i++)
    {
        bttbl(i) = 0;
        lttbl(i) = 0;
        rttbl(i) = 0;
    }
    
    //
    // Fill p-table
    //
    for(pass = 1; pass <= passcount; pass++)
    {
        med = 20*ap::randomreal()-10;
        width = 1+5*ap::randomreal();
        generatenormal(n, med, width, x);
        generatenormal(n, med, width, y);
        r = pearsoncorrelation(x, y, n);
        pearsoncorrelationsignificance(r, n, bt, lt, rt);
        for(i = 0; i <= qcnt-1; i++)
        {
            if( bt<=btqtbl(i) )
            {
                bttbl(i) = bttbl(i)+double(1)/double(passcount);
            }
            if( lt<=lrqtbl(i) )
            {
                lttbl(i) = lttbl(i)+double(1)/double(passcount);
            }
            if( rt<=lrqtbl(i) )
            {
                rttbl(i) = rttbl(i)+double(1)/double(passcount);
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
        if( fabs(rttbl(i)-lrqtbl(i))/lrsigmatbl(i)>sigmathreshold )
        {
            tmptestok = false;
        }
        if( fabs(lttbl(i)-lrqtbl(i))/lrsigmatbl(i)>sigmathreshold )
        {
            tmptestok = false;
        }
    }
    ptestok = tmptestok;
    
    //
    // Report
    //
    if( !silent&&detailedreport )
    {
        printf("\nPEARSON TEST TABLE\n");
        printf("Q no.     BT err.   LT err.   RT err.   \n");
        for(i = 0; i <= qcnt-1; i++)
        {
            printf("%1ld         %3.1lf std.  %3.1lf std.  %3.1lf std.  \n",
                long(i+1),
                double(fabs(bttbl(i)-btqtbl(i))/btsigmatbl(i)),
                double(fabs(lttbl(i)-lrqtbl(i))/lrsigmatbl(i)),
                double(fabs(rttbl(i)-lrqtbl(i))/lrsigmatbl(i)));
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
    // Pearson power test
    //
    
    //
    // Prepare p-table
    //
    for(i = 0; i <= qcnt-1; i++)
    {
        bttbl(i) = 0;
        lttbl(i) = 0;
        rttbl(i) = 0;
    }
    
    //
    // Fill p-table
    //
    lpower = 0;
    rpower = 0;
    bpower = 0;
    for(pass = 1; pass <= passcount; pass++)
    {
        v = 10;
        generatenormal(n, 0.0, 1.0, x);
        generatenormal(n, 0.0, 1.0, y);
        ap::vsub(&y(0), &x(0), ap::vlen(0,n-1), v);
        r = pearsoncorrelation(x, y, n);
        pearsoncorrelationsignificance(r, n, bt, lt, rt);
        if( lt<0.05 )
        {
            lpower = lpower+double(1)/double(passcount);
        }
        generatenormal(n, 0.0, 1.0, y);
        ap::vadd(&y(0), &x(0), ap::vlen(0,n-1), v);
        r = pearsoncorrelation(x, y, n);
        pearsoncorrelationsignificance(r, n, bt, lt, rt);
        if( rt<0.05 )
        {
            rpower = rpower+double(1)/double(passcount);
        }
        generatenormal(n, 0.0, 1.0, y);
        if( ap::randomreal()>0.5 )
        {
            ap::vadd(&y(0), &x(0), ap::vlen(0,n-1), v);
        }
        else
        {
            ap::vsub(&y(0), &x(0), ap::vlen(0,n-1), v);
        }
        r = pearsoncorrelation(x, y, n);
        pearsoncorrelationsignificance(r, n, bt, lt, rt);
        if( bt<0.05 )
        {
            bpower = bpower+double(1)/double(passcount);
        }
    }
    
    //
    // Check
    //
    ptestpowok = lpower>0.95&&rpower>0.95&&bpower>0.95;
    
    //
    // Report
    //
    if( !silent&&detailedreport )
    {
        printf("\nPEARSON POWER TEST TABLE\n");
        printf("LEFT-TAIL TEST POWER:  %4.2lf\n",
            double(lpower));
        printf("RIGHT-TAIL TEST POWER: %4.2lf\n",
            double(rpower));
        printf("BOTH-TAIL TEST POWER:  %4.2lf\n",
            double(bpower));
        if( ptestpowok )
        {
            printf("TEST PASSED\n\n");
        }
        else
        {
            printf("TEST FAILED\n\n");
        }
    }
    
    //
    // Spearman correlation test for validity.
    //
    
    //
    // Prepare p-table
    //
    for(i = 0; i <= qcnt-1; i++)
    {
        bttbl(i) = 0;
        lttbl(i) = 0;
        rttbl(i) = 0;
    }
    
    //
    // Fill p-table
    //
    for(pass = 1; pass <= passcount; pass++)
    {
        med = 20*ap::randomreal()-10;
        width = 1+5*ap::randomreal();
        generate(n, med, width, x);
        generate(n, med, width, y);
        r = spearmanrankcorrelation(x, y, n);
        spearmanrankcorrelationsignificance(r, n, bt, lt, rt);
        for(i = 0; i <= qcnt-1; i++)
        {
            if( bt<=btqtbl(i) )
            {
                bttbl(i) = bttbl(i)+double(1)/double(passcount);
            }
            if( lt<=lrqtbl(i) )
            {
                lttbl(i) = lttbl(i)+double(1)/double(passcount);
            }
            if( rt<=lrqtbl(i) )
            {
                rttbl(i) = rttbl(i)+double(1)/double(passcount);
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
        if( fabs(rttbl(i)-lrqtbl(i))/lrsigmatbl(i)>sigmathreshold )
        {
            tmptestok = false;
        }
        if( fabs(lttbl(i)-lrqtbl(i))/lrsigmatbl(i)>sigmathreshold )
        {
            tmptestok = false;
        }
    }
    stestok = tmptestok;
    
    //
    // Report
    //
    if( !silent&&detailedreport )
    {
        printf("\nSPEARMAN TEST TABLE\n");
        printf("Q no.     BT err.   LT err.   RT err.   \n");
        for(i = 0; i <= qcnt-1; i++)
        {
            printf("%1ld         %3.1lf std.  %3.1lf std.  %3.1lf std.  \n",
                long(i+1),
                double(fabs(bttbl(i)-btqtbl(i))/btsigmatbl(i)),
                double(fabs(lttbl(i)-lrqtbl(i))/lrsigmatbl(i)),
                double(fabs(rttbl(i)-lrqtbl(i))/lrsigmatbl(i)));
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
    // Sperman power test
    //
    
    //
    // Prepare p-table
    //
    for(i = 0; i <= qcnt-1; i++)
    {
        bttbl(i) = 0;
        lttbl(i) = 0;
        rttbl(i) = 0;
    }
    
    //
    // Fill p-table
    //
    lpower = 0;
    rpower = 0;
    bpower = 0;
    for(pass = 1; pass <= passcount; pass++)
    {
        v = 10;
        generate(n, 0.0, 1.0, x);
        generate(n, 0.0, 1.0, y);
        ap::vsub(&y(0), &x(0), ap::vlen(0,n-1), v);
        r = spearmanrankcorrelation(x, y, n);
        spearmanrankcorrelationsignificance(r, n, bt, lt, rt);
        if( lt<0.05 )
        {
            lpower = lpower+double(1)/double(passcount);
        }
        generate(n, 0.0, 1.0, y);
        ap::vadd(&y(0), &x(0), ap::vlen(0,n-1), v);
        r = spearmanrankcorrelation(x, y, n);
        spearmanrankcorrelationsignificance(r, n, bt, lt, rt);
        if( rt<0.05 )
        {
            rpower = rpower+double(1)/double(passcount);
        }
        generate(n, 0.0, 1.0, y);
        if( ap::randomreal()>0.5 )
        {
            ap::vadd(&y(0), &x(0), ap::vlen(0,n-1), v);
        }
        else
        {
            ap::vsub(&y(0), &x(0), ap::vlen(0,n-1), v);
        }
        r = spearmanrankcorrelation(x, y, n);
        spearmanrankcorrelationsignificance(r, n, bt, lt, rt);
        if( bt<0.05 )
        {
            bpower = bpower+double(1)/double(passcount);
        }
    }
    
    //
    // Check
    //
    stestpowok = lpower>0.95&&rpower>0.95&&bpower>0.95;
    
    //
    // Report
    //
    if( !silent&&detailedreport )
    {
        printf("\nSPEARMAN POWER TEST TABLE\n");
        printf("LEFT-TAIL TEST POWER:  %4.2lf\n",
            double(lpower));
        printf("RIGHT-TAIL TEST POWER: %4.2lf\n",
            double(rpower));
        printf("BOTH-TAIL TEST POWER:  %4.2lf\n",
            double(bpower));
        if( ptestpowok )
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
    waserrors = !ptestok||!stestok||!ptestpowok||!stestpowok;
    if( !silent )
    {
        printf("PEARSON CONTINUOUS VALIDITY TEST:        ");
        if( ptestok )
        {
            printf("OK\n");
        }
        else
        {
            printf("FAILED\n");
        }
        printf("PEARSON POWER TEST:                      ");
        if( ptestpowok )
        {
            printf("OK\n");
        }
        else
        {
            printf("FAILED\n");
        }
        printf("SPEARMAN CONTINUOUS VALIDITY TEST:       ");
        if( stestok )
        {
            printf("OK\n");
        }
        else
        {
            printf("FAILED\n");
        }
        printf("SPEARMAN POWER TEST:                     ");
        if( stestpowok )
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


static void generate(int n, double med, double width, ap::real_1d_array& r)
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
            r(i) = med+3*width*v;
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
bool testcorrhtunit_test_silent()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testcorrtest(true);
    return result;
#else
    return _i_testcorrhtunit_test_silent();
#endif
}


/*************************************************************************
Unit test
*************************************************************************/
bool testcorrhtunit_test()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testcorrtest(false);
    return result;
#else
    return _i_testcorrhtunit_test();
#endif
}



