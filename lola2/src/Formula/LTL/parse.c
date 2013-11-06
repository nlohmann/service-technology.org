/***** ltl2ba : parse.c *****/

/* Written by Denis Oddoux, LIAFA, France                                 */
/* Copyright (c) 2001  Denis Oddoux                                       */
/* Modified by Paul Gastin, LSV, France                                   */
/* Copyright (c) 2007  Paul Gastin                                        */
/*                                                                        */
/* This program is free software; you can redistribute it and/or modify   */
/* it under the terms of the GNU General Public License as published by   */
/* the Free Software Foundation; either version 2 of the License, or      */
/* (at your option) any later version.                                    */
/*                                                                        */
/* This program is distributed in the hope that it will be useful,        */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of         */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          */
/* GNU General Public License for more details.                           */
/*                                                                        */
/* You should have received a copy of the GNU General Public License      */
/* along with this program; if not, write to the Free Software            */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA*/
/*                                                                        */
/* Based on the translation algorithm by Gastin and Oddoux,               */
/* presented at the 13th International Conference on Computer Aided       */
/* Verification, CAV 2001, Paris, France.                                 */
/* Proceedings - LNCS 2102, pp. 53-65                                     */
/*                                                                        */
/* Send bug-reports and/or questions to Paul Gastin                       */
/* http://www.lsv.ens-cachan.fr/~gastin                                   */
/*                                                                        */
/* Some of the code in this file was taken from the Spin software         */
/* Written by Gerard J. Holzmann, Bell Laboratories, U.S.A.               */

#include "ltl2ba.h"

extern int	tl_verbose, tl_simp_log;


static tl_Node	*tl_formula(void);
static tl_Node	*tl_factor(void);
static tl_Node	*tl_level(int);

static int	prec[2][4] =
{
    { U_OPER,  V_OPER, 0, 0},  /* left associative */
    { OR, AND, IMPLIES, EQUIV, },	/* left associative */
};

static int
implies(tl_Node *a, tl_Node *b)
{
    return
        (isequal(a, b) ||
         b->ntyp == TRUE ||
         a->ntyp == FALSE ||
         (b->ntyp == AND && implies(a, b->lft) && implies(a, b->rgt)) ||
         (a->ntyp == OR && implies(a->lft, b) && implies(a->rgt, b)) ||
         (a->ntyp == AND && (implies(a->lft, b) || implies(a->rgt, b))) ||
         (b->ntyp == OR && (implies(a, b->lft) || implies(a, b->rgt))) ||
         (b->ntyp == U_OPER && implies(a, b->rgt)) ||
         (a->ntyp == V_OPER && implies(a->rgt, b)) ||
         (a->ntyp == U_OPER && implies(a->lft, b) && implies(a->rgt, b)) ||
         (b->ntyp == V_OPER && implies(a, b->lft) && implies(a, b->rgt)) ||
         ((a->ntyp == U_OPER || a->ntyp == V_OPER) && a->ntyp == b->ntyp &&
          implies(a->lft, b->lft) && implies(a->rgt, b->rgt)));
}

tl_Node *
bin_simpler(tl_Node *ptr)
{
    tl_Node *a, *b;

    if (ptr)
        switch (ptr->ntyp)
        {
        case U_OPER:
            if (ptr->rgt->ntyp == TRUE
                    ||  ptr->rgt->ntyp == FALSE
                    ||  ptr->lft->ntyp == FALSE)
            {
                ptr = ptr->rgt;
                break;
            }
            if (implies(ptr->lft, ptr->rgt)) /* NEW */
            {
                ptr = ptr->rgt;
                break;
            }
            if (ptr->lft->ntyp == U_OPER
                    &&  isequal(ptr->lft->lft, ptr->rgt))
            {
                /* (p U q) U p = (q U p) */
                ptr->lft = ptr->lft->rgt;
                break;
            }
            if (ptr->rgt->ntyp == U_OPER
                    &&  implies(ptr->lft, ptr->rgt->lft))
            {
                /* NEW */
                ptr = ptr->rgt;
                break;
            }
#ifdef NXT
            /* X p U X q == X (p U q) */
            if (ptr->rgt->ntyp == NEXT
                    &&  ptr->lft->ntyp == NEXT)
            {
                ptr = tl_nn(NEXT,
                            tl_nn(U_OPER,
                                  ptr->lft->lft,
                                  ptr->rgt->lft), ZN);
                break;
            }

            /* NEW : F X p == X F p */
            if (ptr->lft->ntyp == TRUE &&
                    ptr->rgt->ntyp == NEXT)
            {
                ptr = tl_nn(NEXT, tl_nn(U_OPER, True, ptr->rgt->lft), ZN);
                break;
            }

#endif

            /* NEW : F G F p == G F p */
            if (ptr->lft->ntyp == TRUE &&
                    ptr->rgt->ntyp == V_OPER &&
                    ptr->rgt->lft->ntyp == FALSE &&
                    ptr->rgt->rgt->ntyp == U_OPER &&
                    ptr->rgt->rgt->lft->ntyp == TRUE)
            {
                ptr = ptr->rgt;
                break;
            }

            /* NEW */
            if (ptr->lft->ntyp != TRUE &&
                    implies(push_negation(tl_nn(NOT, dupnode(ptr->rgt), ZN)),
                            ptr->lft))
            {
                ptr->lft = True;
                break;
            }
            break;
        case V_OPER:
            if (ptr->rgt->ntyp == FALSE
                    ||  ptr->rgt->ntyp == TRUE
                    ||  ptr->lft->ntyp == TRUE)
            {
                ptr = ptr->rgt;
                break;
            }
            if (implies(ptr->rgt, ptr->lft))
            {
                /* p V p = p */
                ptr = ptr->rgt;
                break;
            }
            /* F V (p V q) == F V q */
            if (ptr->lft->ntyp == FALSE
                    &&  ptr->rgt->ntyp == V_OPER)
            {
                ptr->rgt = ptr->rgt->rgt;
                break;
            }
#ifdef NXT
            /* NEW : G X p == X G p */
            if (ptr->lft->ntyp == FALSE &&
                    ptr->rgt->ntyp == NEXT)
            {
                ptr = tl_nn(NEXT, tl_nn(V_OPER, False, ptr->rgt->lft), ZN);
                break;
            }
#endif
            /* NEW : G F G p == F G p */
            if (ptr->lft->ntyp == FALSE &&
                    ptr->rgt->ntyp == U_OPER &&
                    ptr->rgt->lft->ntyp == TRUE &&
                    ptr->rgt->rgt->ntyp == V_OPER &&
                    ptr->rgt->rgt->lft->ntyp == FALSE)
            {
                ptr = ptr->rgt;
                break;
            }

            /* NEW */
            if (ptr->rgt->ntyp == V_OPER
                    &&  implies(ptr->rgt->lft, ptr->lft))
            {
                ptr = ptr->rgt;
                break;
            }

            /* NEW */
            if (ptr->lft->ntyp != FALSE &&
                    implies(ptr->lft,
                            push_negation(tl_nn(NOT, dupnode(ptr->rgt), ZN))))
            {
                ptr->lft = False;
                break;
            }
            break;
#ifdef NXT
        case NEXT:
            /* NEW : X G F p == G F p */
            if (ptr->lft->ntyp == V_OPER &&
                    ptr->lft->lft->ntyp == FALSE &&
                    ptr->lft->rgt->ntyp == U_OPER &&
                    ptr->lft->rgt->lft->ntyp == TRUE)
            {
                ptr = ptr->lft;
                break;
            }
            /* NEW : X F G p == F G p */
            if (ptr->lft->ntyp == U_OPER &&
                    ptr->lft->lft->ntyp == TRUE &&
                    ptr->lft->rgt->ntyp == V_OPER &&
                    ptr->lft->rgt->lft->ntyp == FALSE)
            {
                ptr = ptr->lft;
                break;
            }
            break;
#endif
        case IMPLIES:
            if (implies(ptr->lft, ptr->rgt))
            {
                ptr = True;
                break;
            }
            ptr = tl_nn(OR, Not(ptr->lft), ptr->rgt);
            ptr = rewrite(ptr);
            break;
        case EQUIV:
            if (implies(ptr->lft, ptr->rgt) &&
                    implies(ptr->rgt, ptr->lft))
            {
                ptr = True;
                break;
            }
            a = rewrite(tl_nn(AND,
                              dupnode(ptr->lft),
                              dupnode(ptr->rgt)));
            b = rewrite(tl_nn(AND,
                              Not(ptr->lft),
                              Not(ptr->rgt)));
            ptr = tl_nn(OR, a, b);
            ptr = rewrite(ptr);
            break;
        case AND:
            /* p && (q U p) = p */
            if (ptr->rgt->ntyp == U_OPER
                    &&  isequal(ptr->rgt->rgt, ptr->lft))
            {
                ptr = ptr->lft;
                break;
            }
            if (ptr->lft->ntyp == U_OPER
                    &&  isequal(ptr->lft->rgt, ptr->rgt))
            {
                ptr = ptr->rgt;
                break;
            }

            /* p && (q V p) == q V p */
            if (ptr->rgt->ntyp == V_OPER
                    &&  isequal(ptr->rgt->rgt, ptr->lft))
            {
                ptr = ptr->rgt;
                break;
            }
            if (ptr->lft->ntyp == V_OPER
                    &&  isequal(ptr->lft->rgt, ptr->rgt))
            {
                ptr = ptr->lft;
                break;
            }

            /* (p U q) && (r U q) = (p && r) U q*/
            if (ptr->rgt->ntyp == U_OPER
                    &&  ptr->lft->ntyp == U_OPER
                    &&  isequal(ptr->rgt->rgt, ptr->lft->rgt))
            {
                ptr = tl_nn(U_OPER,
                            tl_nn(AND, ptr->lft->lft, ptr->rgt->lft),
                            ptr->lft->rgt);
                break;
            }

            /* (p V q) && (p V r) = p V (q && r) */
            if (ptr->rgt->ntyp == V_OPER
                    &&  ptr->lft->ntyp == V_OPER
                    &&  isequal(ptr->rgt->lft, ptr->lft->lft))
            {
                ptr = tl_nn(V_OPER,
                            ptr->rgt->lft,
                            tl_nn(AND, ptr->lft->rgt, ptr->rgt->rgt));
                break;
            }
#ifdef NXT
            /* X p && X q == X (p && q) */
            if (ptr->rgt->ntyp == NEXT
                    &&  ptr->lft->ntyp == NEXT)
            {
                ptr = tl_nn(NEXT,
                            tl_nn(AND,
                                  ptr->rgt->lft,
                                  ptr->lft->lft), ZN);
                break;
            }
#endif

            /* (p V q) && (r U q) == p V q */
            if (ptr->rgt->ntyp == U_OPER
                    &&  ptr->lft->ntyp == V_OPER
                    &&  isequal(ptr->lft->rgt, ptr->rgt->rgt))
            {
                ptr = ptr->lft;
                break;
            }

            if (isequal(ptr->lft, ptr->rgt)	/* (p && p) == p */
                    ||  ptr->rgt->ntyp == FALSE	/* (p && F) == F */
                    ||  ptr->lft->ntyp == TRUE	/* (T && p) == p */
                    ||  implies(ptr->rgt, ptr->lft))/* NEW */
            {
                ptr = ptr->rgt;
                break;
            }
            if (ptr->rgt->ntyp == TRUE	/* (p && T) == p */
                    ||  ptr->lft->ntyp == FALSE	/* (F && p) == F */
                    ||  implies(ptr->lft, ptr->rgt))/* NEW */
            {
                ptr = ptr->lft;
                break;
            }

            /* NEW : F G p && F G q == F G (p && q) */
            if (ptr->lft->ntyp == U_OPER &&
                    ptr->lft->lft->ntyp == TRUE &&
                    ptr->lft->rgt->ntyp == V_OPER &&
                    ptr->lft->rgt->lft->ntyp == FALSE &&
                    ptr->rgt->ntyp == U_OPER &&
                    ptr->rgt->lft->ntyp == TRUE &&
                    ptr->rgt->rgt->ntyp == V_OPER &&
                    ptr->rgt->rgt->lft->ntyp == FALSE)
            {
                ptr = tl_nn(U_OPER, True,
                            tl_nn(V_OPER, False,
                                  tl_nn(AND, ptr->lft->rgt->rgt,
                                        ptr->rgt->rgt->rgt)));
                break;
            }

            /* NEW */
            if (implies(ptr->lft,
                        push_negation(tl_nn(NOT, dupnode(ptr->rgt), ZN)))
                    || implies(ptr->rgt,
                               push_negation(tl_nn(NOT, dupnode(ptr->lft), ZN))))
            {
                ptr = False;
                break;
            }
            break;

        case OR:
            /* p || (q U p) == q U p */
            if (ptr->rgt->ntyp == U_OPER
                    &&  isequal(ptr->rgt->rgt, ptr->lft))
            {
                ptr = ptr->rgt;
                break;
            }

            /* p || (q V p) == p */
            if (ptr->rgt->ntyp == V_OPER
                    &&  isequal(ptr->rgt->rgt, ptr->lft))
            {
                ptr = ptr->lft;
                break;
            }

            /* (p U q) || (p U r) = p U (q || r) */
            if (ptr->rgt->ntyp == U_OPER
                    &&  ptr->lft->ntyp == U_OPER
                    &&  isequal(ptr->rgt->lft, ptr->lft->lft))
            {
                ptr = tl_nn(U_OPER,
                            ptr->rgt->lft,
                            tl_nn(OR, ptr->lft->rgt, ptr->rgt->rgt));
                break;
            }

            if (isequal(ptr->lft, ptr->rgt)	/* (p || p) == p */
                    ||  ptr->rgt->ntyp == FALSE	/* (p || F) == p */
                    ||  ptr->lft->ntyp == TRUE	/* (T || p) == T */
                    ||  implies(ptr->rgt, ptr->lft))/* NEW */
            {
                ptr = ptr->lft;
                break;
            }
            if (ptr->rgt->ntyp == TRUE	/* (p || T) == T */
                    ||  ptr->lft->ntyp == FALSE	/* (F || p) == p */
                    ||  implies(ptr->lft, ptr->rgt))/* NEW */
            {
                ptr = ptr->rgt;
                break;
            }

            /* (p V q) || (r V q) = (p || r) V q */
            if (ptr->rgt->ntyp == V_OPER
                    &&  ptr->lft->ntyp == V_OPER
                    &&  isequal(ptr->lft->rgt, ptr->rgt->rgt))
            {
                ptr = tl_nn(V_OPER,
                            tl_nn(OR, ptr->lft->lft, ptr->rgt->lft),
                            ptr->rgt->rgt);
                break;
            }

            /* (p V q) || (r U q) == r U q */
            if (ptr->rgt->ntyp == U_OPER
                    &&  ptr->lft->ntyp == V_OPER
                    &&  isequal(ptr->lft->rgt, ptr->rgt->rgt))
            {
                ptr = ptr->rgt;
                break;
            }

            /* NEW : G F p || G F q == G F (p || q) */
            if (ptr->lft->ntyp == V_OPER &&
                    ptr->lft->lft->ntyp == FALSE &&
                    ptr->lft->rgt->ntyp == U_OPER &&
                    ptr->lft->rgt->lft->ntyp == TRUE &&
                    ptr->rgt->ntyp == V_OPER &&
                    ptr->rgt->lft->ntyp == FALSE &&
                    ptr->rgt->rgt->ntyp == U_OPER &&
                    ptr->rgt->rgt->lft->ntyp == TRUE)
            {
                ptr = tl_nn(V_OPER, False,
                            tl_nn(U_OPER, True,
                                  tl_nn(OR, ptr->lft->rgt->rgt,
                                        ptr->rgt->rgt->rgt)));
                break;
            }

            /* NEW */
            if (implies(push_negation(tl_nn(NOT, dupnode(ptr->rgt), ZN)),
                        ptr->lft)
                    || implies(push_negation(tl_nn(NOT, dupnode(ptr->lft), ZN)),
                               ptr->rgt))
            {
                ptr = True;
                break;
            }
            break;
        }
    return ptr;
}

static tl_Node *
bin_minimal(tl_Node *ptr)
{
    if (ptr)
        switch (ptr->ntyp)
        {
        case IMPLIES:
            return tl_nn(OR, Not(ptr->lft), ptr->rgt);
        case EQUIV:
            return tl_nn(OR,
                         tl_nn(AND, dupnode(ptr->lft), dupnode(ptr->rgt)),
                         tl_nn(AND, Not(ptr->lft), Not(ptr->rgt)));
        }
    return ptr;
}
