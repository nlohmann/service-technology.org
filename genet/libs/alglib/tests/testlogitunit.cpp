
#include <stdafx.h>
#include <stdio.h>
#include "testlogitunit.h"

static const double strongthreshold = 0.7;
static const double strongfreq = double(4)/double(5);

static void degtests(int nf, int nc, bool& degerrors);
static void stest1(int nf,
     bool& converrors,
     bool& weakcorrerrors,
     bool& strongcorrerrors,
     bool& othererrors);
static void stest2(int nf,
     int nc,
     bool& converrors,
     bool& weakcorrerrors,
     bool& strongcorrerrors,
     bool& othererrors);
static void stest3(int nf,
     bool& converrors,
     bool& weakcorrerrors,
     bool& strongcorrerrors,
     bool& othererrors);
static void addtest1(int np,
     int nf,
     int nc,
     const ap::real_2d_array& xy,
     logitmodel& w,
     bool& othererrors);
static void addtest2(int nf, int nc, bool& othererrors);
static void cvtest(bool& converrors, bool& othererrors);
static void errtest(bool& converrors, bool& othererrors);
static void testprocessing(bool& converrors, bool& procerrors);
static double rnormal();
static double rsphere(ap::real_2d_array& xy, int n, int i);
static void unsetlm(logitmodel& lm);

bool testlogit(bool silent)
{
    bool result;
    int nf;
    int maxnf;
    int nc;
    int maxnc;
    int passcount;
    int pass;
    bool waserrors;
    bool degerrors;
    bool converrors;
    bool othererrors;
    bool weakcorrerrors;
    bool strongcorrerrors;

    
    //
    // Primary settings
    //
    maxnf = 4;
    maxnc = 4;
    passcount = 3;
    waserrors = false;
    converrors = false;
    othererrors = false;
    degerrors = false;
    weakcorrerrors = false;
    strongcorrerrors = false;
    
    //
    // Tests
    //
    for(nf = 1; nf <= maxnf; nf++)
    {
        for(nc = 2; nc <= maxnc; nc++)
        {
            
            //
            // Degenerate tests
            //
            degtests(nf, nc, degerrors);
            
            //
            // General tests
            //
            for(pass = 1; pass <= passcount; pass++)
            {
                
                //
                // Simple test #1
                //
                if( nc==2 )
                {
                    stest1(nf, converrors, weakcorrerrors, strongcorrerrors, othererrors);
                    stest3(nf, converrors, weakcorrerrors, strongcorrerrors, othererrors);
                }
                
                //
                // Simple test #2
                //
                stest2(nf, nc, converrors, weakcorrerrors, strongcorrerrors, othererrors);
            }
            
            //
            // Additional tests
            //
            addtest2(nf, nc, othererrors);
        }
    }
    cvtest(converrors, othererrors);
    errtest(converrors, othererrors);
    testprocessing(converrors, othererrors);
    
    //
    // Final report
    //
    waserrors = weakcorrerrors||converrors||strongcorrerrors||degerrors||othererrors;
    if( !silent )
    {
        printf("LOGIT TEST\n");
        printf("TOTAL RESULTS:                           ");
        if( !waserrors )
        {
            printf("OK\n");
        }
        else
        {
            printf("FAILED\n");
        }
        printf("* CONVERGENCE:                           ");
        if( !converrors )
        {
            printf("OK\n");
        }
        else
        {
            printf("FAILED\n");
        }
        printf("* DEGENERATE CASES:                      ");
        if( !degerrors )
        {
            printf("OK\n");
        }
        else
        {
            printf("FAILED\n");
        }
        printf("* WEAK CORRELATION:                      ");
        if( !weakcorrerrors )
        {
            printf("OK\n");
        }
        else
        {
            printf("FAILED\n");
        }
        printf("* STRONG CORRELATION:                    ");
        if( !strongcorrerrors )
        {
            printf("OK\n");
        }
        else
        {
            printf("FAILED\n");
        }
        printf("* OTHER PROPERTIES:                      ");
        if( !othererrors )
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


/*************************************************************************
Degenerate tests
*************************************************************************/
static void degtests(int nf, int nc, bool& degerrors)
{
    int i;
    int j;
    int info;
    ap::real_2d_array xy;
    ap::real_2d_array e;
    ap::real_1d_array x;
    ap::real_1d_array y;
    int np;
    logitmodel w;
    mnlreport rep;

    x.setbounds(0, nf-1);
    y.setbounds(0, nc-1);
    
    //
    // Test #2
    //
    for(np = nf+2; np <= nf+nf; np++)
    {
        xy.setbounds(0, np-1, 0, nf);
        for(i = 0; i <= np-1; i++)
        {
            for(j = 0; j <= nf-1; j++)
            {
                xy(i,j) = 2*ap::randomreal()-1;
            }
            xy(i,nf) = np%nc;
        }
        mnltrainh(xy, np, nf, nc, info, w, rep);
        if( info!=1 )
        {
            degerrors = true;
            return;
        }
        for(i = 0; i <= nf-1; i++)
        {
            x(i) = 2*ap::randomreal()-1;
        }
        mnlprocess(w, x, y);
        for(i = 0; i <= nc-1; i++)
        {
            if( i==np%nc )
            {
                degerrors = degerrors||y(i)!=1;
            }
            else
            {
                degerrors = degerrors||y(i)!=0;
            }
        }
    }
}


/*************************************************************************
Simple test 1.
Two well-separated balls.
*************************************************************************/
static void stest1(int nf,
     bool& converrors,
     bool& weakcorrerrors,
     bool& strongcorrerrors,
     bool& othererrors)
{
    int i;
    int j;
    int info;
    ap::real_2d_array xy;
    ap::real_1d_array x;
    ap::real_1d_array y;
    ap::real_1d_array c;
    logitmodel w;
    int nc;
    int np;
    double s;
    double v;
    int strongcnt;
    int weakcnt;
    mnlreport rep;

    np = 2*(10+2*nf+ap::randominteger(nf));
    nc = 2;
    x.setbounds(0, nf-1);
    c.setbounds(0, nf-1);
    y.setbounds(0, nc-1);
    xy.setbounds(0, np-1, 0, nf);
    
    //
    // Fill
    //
    for(j = 0; j <= nf-1; j++)
    {
        c(j) = 2*ap::randomreal()-1;
    }
    s = ap::vdotproduct(&c(0), &c(0), ap::vlen(0,nf-1));
    s = sqrt(s);
    for(i = 0; i <= np-1; i++)
    {
        rsphere(xy, nf, i);
        v = 0.2*s;
        ap::vmul(&xy(i, 0), ap::vlen(0,nf-1), v);
        if( i%2==0 )
        {
            
            //
            // label 0
            //
            ap::vadd(&xy(i, 0), &c(0), ap::vlen(0,nf-1));
            xy(i,nf) = 0;
        }
        else
        {
            
            //
            // label 1
            //
            ap::vsub(&xy(i, 0), &c(0), ap::vlen(0,nf-1));
            xy(i,nf) = 1;
        }
    }
    
    //
    // Train
    //
    mnltrainh(xy, np, nf, 2, info, w, rep);
    if( info!=1 )
    {
        converrors = true;
        return;
    }
    
    //
    // Test
    //
    weakcnt = 0;
    strongcnt = 0;
    for(i = 0; i <= np-1; i++)
    {
        ap::vmove(&x(0), &xy(i, 0), ap::vlen(0,nf-1));
        mnlprocess(w, x, y);
        if( y(i%2)>0.5 )
        {
            weakcnt = weakcnt+1;
        }
        if( y(i%2)>strongthreshold )
        {
            strongcnt = strongcnt+1;
        }
        for(j = 0; j <= nc-1; j++)
        {
            othererrors = othererrors||y(j)<0||y(j)>1;
        }
        othererrors = othererrors||fabs(y(0)+y(1)-1)>100*ap::machineepsilon;
    }
    strongcorrerrors = strongcorrerrors||double(strongcnt)/double(np)<strongfreq;
    weakcorrerrors = weakcorrerrors||double(weakcnt)/double(np)<double(2)/double(3);
    
    //
    // Additional tests
    //
    addtest1(np, nf, nc, xy, w, othererrors);
    othererrors = othererrors||mnlclserror(w, xy, np)!=np-weakcnt;
}


/*************************************************************************
Simple test 2:
NC unit balls separated by 3 units wide gaps
*************************************************************************/
static void stest2(int nf,
     int nc,
     bool& converrors,
     bool& weakcorrerrors,
     bool& strongcorrerrors,
     bool& othererrors)
{
    int i;
    int j;
    int info;
    ap::real_2d_array xy;
    ap::real_1d_array x;
    ap::real_1d_array y;
    ap::real_1d_array c;
    logitmodel w;
    int np;
    double s;
    double v;
    int weakcnt;
    int strongcnt;
    mnlreport rep;

    np = nc*(10+2*nf+ap::randominteger(nf));
    x.setbounds(0, nf-1);
    c.setbounds(0, nf-1);
    y.setbounds(0, nc-1);
    xy.setbounds(0, np-1, 0, nf);
    
    //
    // Fill
    //
    for(j = 0; j <= nf-1; j++)
    {
        c(j) = rnormal();
    }
    s = ap::vdotproduct(&c(0), &c(0), ap::vlen(0,nf-1));
    s = 1/sqrt(s);
    ap::vmul(&c(0), ap::vlen(0,nf-1), s);
    for(i = 0; i <= np-1; i++)
    {
        rsphere(xy, nf, i);
        v = 2*(i%nc);
        ap::vadd(&xy(i, 0), &c(0), ap::vlen(0,nf-1), v);
        xy(i,nf) = i%nc;
    }
    
    //
    // Train
    //
    mnltrainh(xy, np, nf, nc, info, w, rep);
    if( info!=1 )
    {
        converrors = true;
        return;
    }
    
    //
    // Test
    //
    weakcnt = 0;
    strongcnt = 0;
    for(i = 0; i <= np-1; i++)
    {
        ap::vmove(&x(0), &xy(i, 0), ap::vlen(0,nf-1));
        mnlprocess(w, x, y);
        if( y(i%nc)>strongthreshold )
        {
            strongcnt = strongcnt+1;
        }
        if( y(i%nc)>0.5 )
        {
            weakcnt = weakcnt+1;
        }
        s = 0;
        for(j = 0; j <= nc-1; j++)
        {
            s = s+y(j);
            othererrors = othererrors||y(j)<0||y(j)>1;
        }
        othererrors = othererrors||fabs(s-1)>100*ap::machineepsilon;
    }
    strongcorrerrors = strongcorrerrors||double(strongcnt)/double(np)<strongfreq;
    weakcorrerrors = weakcorrerrors||double(weakcnt)/double(np)<double(2)/double(3);
    
    //
    // Additional tests
    //
    addtest1(np, nf, nc, xy, w, othererrors);
    othererrors = othererrors||mnlclserror(w, xy, np)!=np-weakcnt;
}


/*************************************************************************
Simple test 3.
Two badly-separated balls.
*************************************************************************/
static void stest3(int nf,
     bool& converrors,
     bool& weakcorrerrors,
     bool& strongcorrerrors,
     bool& othererrors)
{
    int i;
    int j;
    int info;
    ap::real_2d_array xy;
    ap::real_1d_array x;
    ap::real_1d_array y;
    ap::real_1d_array c;
    logitmodel w;
    int nc;
    int np;
    double s;
    double v;
    int weakcnt;
    mnlreport rep;

    np = 100;
    nc = 2;
    x.setbounds(0, nf-1);
    c.setbounds(0, nf-1);
    y.setbounds(0, nc-1);
    xy.setbounds(0, np-1, 0, nf);
    
    //
    // Fill
    //
    for(j = 0; j <= nf-1; j++)
    {
        c(j) = 2*ap::randomreal()-1;
    }
    s = ap::vdotproduct(&c(0), &c(0), ap::vlen(0,nf-1));
    s = sqrt(s);
    for(i = 0; i <= np-1; i++)
    {
        rsphere(xy, nf, i);
        v = 1.5*s;
        ap::vmul(&xy(i, 0), ap::vlen(0,nf-1), v);
        if( i%2==0 )
        {
            
            //
            // label 0
            //
            ap::vadd(&xy(i, 0), &c(0), ap::vlen(0,nf-1));
            xy(i,nf) = 0;
        }
        else
        {
            
            //
            // label 1
            //
            ap::vsub(&xy(i, 0), &c(0), ap::vlen(0,nf-1));
            xy(i,nf) = 1;
        }
    }
    
    //
    // Train
    //
    mnltrainh(xy, np, nf, 2, info, w, rep);
    if( info!=1 )
    {
        converrors = true;
        return;
    }
    
    //
    // Test
    //
    weakcnt = 0;
    for(i = 0; i <= np-1; i++)
    {
        ap::vmove(&x(0), &xy(i, 0), ap::vlen(0,nf-1));
        mnlprocess(w, x, y);
        if( y(i%2)>0.5 )
        {
            weakcnt = weakcnt+1;
        }
        for(j = 0; j <= nc-1; j++)
        {
            othererrors = othererrors||y(j)<0||y(j)>1;
        }
        othererrors = othererrors||fabs(y(0)+y(1)-1)>100*ap::machineepsilon;
    }
    weakcorrerrors = weakcorrerrors||double(weakcnt)/double(np)<double(2)/double(3);
    
    //
    // Additional tests
    //
    addtest1(np, nf, nc, xy, w, othererrors);
    othererrors = othererrors||mnlclserror(w, xy, np)!=np-weakcnt;
}


/*************************************************************************
Tests additional functions using already intiialized model.
*************************************************************************/
static void addtest1(int np,
     int nf,
     int nc,
     const ap::real_2d_array& xy,
     logitmodel& w,
     bool& othererrors)
{
    ap::real_2d_array a;
    ap::real_1d_array t;
    ap::real_1d_array x;
    ap::real_1d_array y;
    logitmodel w2;
    ap::real_1d_array y2;
    int i;
    double v;
    double s;
    int nvars;
    int nclasses;

    
    //
    // PACK/UNPACK test
    //
    t.setbounds(0, nc-1);
    y.setbounds(0, nc-1);
    y2.setbounds(0, nc-1);
    x.setbounds(0, nf-1);
    mnlunpack(w, a, nvars, nclasses);
    for(i = 0; i <= nf-1; i++)
    {
        x(i) = 2*ap::randomreal()-1;
    }
    s = 0;
    for(i = 0; i <= nc-2; i++)
    {
        v = ap::vdotproduct(&a(i, 0), &x(0), ap::vlen(0,nf-1));
        t(i) = exp(v+a(i,nf));
        s = s+t(i);
    }
    t(nc-1) = 1;
    s = s+1;
    s = 1/s;
    ap::vmul(&t(0), ap::vlen(0,nc-1), s);
    mnlprocess(w, x, y);
    for(i = 0; i <= nc-1; i++)
    {
        othererrors = othererrors||fabs(y(i)-t(i))>1000*ap::machineepsilon;
    }
    mnlpack(a, nf, nc, w2);
    mnlprocess(w2, x, y2);
    for(i = 0; i <= nc-1; i++)
    {
        othererrors = othererrors||fabs(y2(i)-t(i))>1000*ap::machineepsilon;
    }
}


/*************************************************************************
Tests additional functions.
*************************************************************************/
static void addtest2(int nf, int nc, bool& othererrors)
{
    ap::real_2d_array a;
    ap::real_2d_array xy;
    logitmodel w;
    int i;
    int j;

    a.setbounds(0, nc-2, 0, nf);
    for(i = 0; i <= nc-2; i++)
    {
        for(j = 0; j <= nf; j++)
        {
            a(i,j) = 0;
        }
    }
    mnlpack(a, nf, nc, w);
    xy.setbounds(0, nc-1, 0, nf);
    for(i = 0; i <= nc-1; i++)
    {
        for(j = 0; j <= nf-1; j++)
        {
            xy(i,j) = 2*ap::randomreal()-1;
        }
        xy(i,nf) = i;
    }
    othererrors = othererrors||fabs(mnlavgce(w, xy, nc)-log(double(nc))/log(double(2)))>1000*ap::machineepsilon;
}


/*************************************************************************
Tests additional functions.
*************************************************************************/
static void cvtest(bool& converrors, bool& othererrors)
{
    int np;
    int nf;
    int nc;
    ap::real_2d_array xy;
    int i;
    int info;
    logitmodel w;
    mnlreport rep;
    double relclsexact;
    double avgceexact;
    int ncecvcloser;
    int nthreshold;
    int pass;
    int passcount;
    int ksetup;

    return;
}


/*************************************************************************
Tests error calculation.
*************************************************************************/
static void errtest(bool& converrors, bool& othererrors)
{
    int np;
    int nf;
    int nc;
    ap::real_2d_array xytrn;
    ap::real_2d_array xytst;
    int i;
    int j;
    int info;
    logitmodel w;
    mnlreport rep;

    
    //
    // Prepare training set such that on test set consisting of 3 examples
    // we have:
    //
    // N    INPUTS      POST.PROB.
    // 0    0 0         (0.25, 0.25, 0.50) for true class 0
    // 1    1 0         (0.25, 0.25, 0.50) for true class 1
    // 2    0 1         (0.25, 0.25, 0.50) for true class 2
    //
    xytrn.setbounds(0, 11, 0, 2);
    for(i = 0; i <= 11; i++)
    {
        xytrn(i,0) = 0;
        xytrn(i,1) = 0;
        if( i>=4&&i<8 )
        {
            xytrn(i,0) = 1;
        }
        if( i>=8&&i<12 )
        {
            xytrn(i,1) = 1;
        }
        xytrn(i,2) = ap::minint(i%4, 2);
    }
    xytst.setbounds(0, 2, 0, 2);
    xytst(0,0) = 0;
    xytst(0,1) = 0;
    xytst(0,2) = 0;
    xytst(1,0) = 1;
    xytst(1,1) = 0;
    xytst(1,2) = 1;
    xytst(2,0) = 0;
    xytst(2,1) = 1;
    xytst(2,2) = 2;
    mnltrainh(xytrn, 12, 2, 3, info, w, rep);
    if( info<0 )
    {
        converrors = true;
        return;
    }
    othererrors = othererrors||fabs(mnlrelclserror(w, xytst, 3)-double(2)/double(3))>1000*ap::machineepsilon;
    othererrors = othererrors||fabs(mnlavgce(w, xytst, 3)-double(2+2+1)/double(3))>0.01;
    othererrors = othererrors||fabs(mnlrmserror(w, xytst, 3)-sqrt((2*(ap::sqr(0.75)+ap::sqr(0.25)+ap::sqr(0.5))+ap::sqr(0.25)+ap::sqr(0.25)+ap::sqr(0.50))/9))>0.01;
    othererrors = othererrors||fabs(mnlavgerror(w, xytst, 3)-(0.75+0.25+0.50+0.25+0.75+0.50+0.25+0.25+0.50)/9)>0.01;
    othererrors = othererrors||fabs(mnlavgrelerror(w, xytst, 3)-(0.75+0.75+0.50)/3)>0.01;
    
    //
    // 3. (1.00, 0.00, 0.00) for true class 0
    // 4. (0.00, 1.00, 0.00) for true class 1
    // 5. (0.00, 0.00, 1.00) for true class 2
    // 6. (0.50, 0.25, 0.25) for true class 0
    // 7. (0.25, 0.50, 0.25) for true class 1
    // 8. (0.25, 0.25, 0.50) for true class 2
    //
}


/*************************************************************************
Processing functions test
*************************************************************************/
static void testprocessing(bool& converrors, bool& procerrors)
{
    int nvars;
    int nclasses;
    logitmodel lm1;
    logitmodel lm2;
    int npoints;
    ap::real_2d_array xy;
    int pass;
    int passcount;
    int i;
    int j;
    bool allsame;
    int rlen;
    int info;
    mnlreport rep;
    ap::real_1d_array x1;
    ap::real_1d_array x2;
    ap::real_1d_array y1;
    ap::real_1d_array y2;
    ap::real_1d_array ra;
    ap::real_1d_array ra2;
    double v;

    passcount = 100;
    
    //
    // Main cycle
    //
    for(pass = 1; pass <= passcount; pass++)
    {
        
        //
        // initialize parameters
        //
        nvars = 1+ap::randominteger(3);
        nclasses = 2+ap::randominteger(3);
        
        //
        // Initialize arrays and data
        //
        npoints = 10+ap::randominteger(50);
        x1.setbounds(0, nvars-1);
        x2.setbounds(0, nvars-1);
        y1.setbounds(0, nclasses-1);
        y2.setbounds(0, nclasses-1);
        xy.setbounds(0, npoints-1, 0, nvars);
        for(i = 0; i <= npoints-1; i++)
        {
            for(j = 0; j <= nvars-1; j++)
            {
                xy(i,j) = 2*ap::randomreal()-1;
            }
            xy(i,nvars) = ap::randominteger(nclasses);
        }
        
        //
        // create forest
        //
        mnltrainh(xy, npoints, nvars, nclasses, info, lm1, rep);
        if( info<=0 )
        {
            converrors = true;
            return;
        }
        
        //
        // Same inputs leads to same outputs
        //
        for(i = 0; i <= nvars-1; i++)
        {
            x1(i) = 2*ap::randomreal()-1;
            x2(i) = x1(i);
        }
        for(i = 0; i <= nclasses-1; i++)
        {
            y1(i) = 2*ap::randomreal()-1;
            y2(i) = 2*ap::randomreal()-1;
        }
        mnlprocess(lm1, x1, y1);
        mnlprocess(lm1, x2, y2);
        allsame = true;
        for(i = 0; i <= nclasses-1; i++)
        {
            allsame = allsame&&y1(i)==y2(i);
        }
        procerrors = procerrors||!allsame;
        
        //
        // Same inputs on original forest leads to same outputs
        // on copy created using DFCopy
        //
        unsetlm(lm2);
        mnlcopy(lm1, lm2);
        for(i = 0; i <= nvars-1; i++)
        {
            x1(i) = 2*ap::randomreal()-1;
            x2(i) = x1(i);
        }
        for(i = 0; i <= nclasses-1; i++)
        {
            y1(i) = 2*ap::randomreal()-1;
            y2(i) = 2*ap::randomreal()-1;
        }
        mnlprocess(lm1, x1, y1);
        mnlprocess(lm2, x2, y2);
        allsame = true;
        for(i = 0; i <= nclasses-1; i++)
        {
            allsame = allsame&&y1(i)==y2(i);
        }
        procerrors = procerrors||!allsame;
        
        //
        // Same inputs on original forest leads to same outputs
        // on copy created using DFSerialize
        //
        unsetlm(lm2);
        ra.setbounds(0, 0);
        ra(0) = 0;
        rlen = 0;
        mnlserialize(lm1, ra, rlen);
        ra2.setbounds(0, rlen-1);
        for(i = 0; i <= rlen-1; i++)
        {
            ra2(i) = ra(i);
        }
        mnlunserialize(ra2, lm2);
        for(i = 0; i <= nvars-1; i++)
        {
            x1(i) = 2*ap::randomreal()-1;
            x2(i) = x1(i);
        }
        for(i = 0; i <= nclasses-1; i++)
        {
            y1(i) = 2*ap::randomreal()-1;
            y2(i) = 2*ap::randomreal()-1;
        }
        mnlprocess(lm1, x1, y1);
        mnlprocess(lm2, x2, y2);
        allsame = true;
        for(i = 0; i <= nclasses-1; i++)
        {
            allsame = allsame&&y1(i)==y2(i);
        }
        procerrors = procerrors||!allsame;
        
        //
        // Normalization properties
        //
        for(i = 0; i <= nvars-1; i++)
        {
            x1(i) = 2*ap::randomreal()-1;
        }
        mnlprocess(lm1, x1, y1);
        v = 0;
        for(i = 0; i <= nclasses-1; i++)
        {
            v = v+y1(i);
            procerrors = procerrors||y1(i)<0;
        }
        procerrors = procerrors||fabs(v-1)>1000*ap::machineepsilon;
    }
}


/*************************************************************************
Random normal number
*************************************************************************/
static double rnormal()
{
    double result;
    double u;
    double v;
    double s;
    double x1;
    double x2;

    while(true)
    {
        u = 2*ap::randomreal()-1;
        v = 2*ap::randomreal()-1;
        s = ap::sqr(u)+ap::sqr(v);
        if( s>0&&s<1 )
        {
            s = sqrt(-2*log(s)/s);
            x1 = u*s;
            x2 = v*s;
            break;
        }
    }
    result = x1;
    return result;
}


/*************************************************************************
Random point from sphere
*************************************************************************/
static double rsphere(ap::real_2d_array& xy, int n, int i)
{
    double result;
    int j;
    double v;

    for(j = 0; j <= n-1; j++)
    {
        xy(i,j) = rnormal();
    }
    v = ap::vdotproduct(&xy(i, 0), &xy(i, 0), ap::vlen(0,n-1));
    v = ap::randomreal()/sqrt(v);
    ap::vmul(&xy(i, 0), ap::vlen(0,n-1), v);
    return result;
}


/*************************************************************************
Unsets model
*************************************************************************/
static void unsetlm(logitmodel& lm)
{
    ap::real_2d_array xy;
    int info;
    mnlreport rep;
    int i;

    xy.setbounds(0, 5, 0, 1);
    for(i = 0; i <= 5; i++)
    {
        xy(i,0) = 0;
        xy(i,1) = 0;
    }
    mnltrainh(xy, 6, 1, 2, info, lm, rep);
    ap::ap_error::make_assertion(info>0, "");
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testlogitunit_test_silent()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testlogit(true);
    return result;
#else
    return _i_testlogitunit_test_silent();
#endif
}


/*************************************************************************
Unit test
*************************************************************************/
bool testlogitunit_test()
{
#ifndef ALGLIB_OPTIMIZED
    bool result;

    result = testlogit(false);
    return result;
#else
    return _i_testlogitunit_test();
#endif
}



