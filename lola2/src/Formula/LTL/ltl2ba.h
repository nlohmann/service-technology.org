/***** ltl2ba : ltl2ba.h *****/

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <Formula/LTL/BuechiFromLTL.h>



tl_Node	*Canonical(tl_Node *);
tl_Node	*canonical(tl_Node *);
tl_Node	*cached(tl_Node *);
tl_Node	*dupnode(tl_Node *);
tl_Node	*getnode(tl_Node *);
tl_Node	*in_cache(tl_Node *);
tl_Node	*push_negation(tl_Node *);
tl_Node	*right_linked(tl_Node *);


tl_Symbol	*getsym(tl_Symbol *);
tl_Symbol	*DoDump(tl_Node *);

char	*emalloc(int);	

int	anywhere(int, tl_Node *, tl_Node *);
int	dump_cond(tl_Node *, tl_Node *, int);
int	isequal(tl_Node *, tl_Node *);
int	tl_Getchar(void);

void	*tl_emalloc(int);
ATrans  *emalloc_atrans();
void    free_atrans(ATrans *, int);
void    free_all_atrans();
GTrans  *emalloc_gtrans();
void    free_gtrans(GTrans *, GTrans *, int);
BTrans  *emalloc_btrans();
void    free_btrans(BTrans *, BTrans *, int);
void	a_stats(void);
void	addtrans(Graph *, char *, tl_Node *, char *);
void	cache_stats(void);
void	dump(tl_Node *);
void	Fatal(char *, char *);
void	fatal(char *, char *);
void	fsm_print(void);
void	releasenode(int, tl_Node *);
void	tfree(void *);
void	tl_explain(int);
void	tl_UnGetchar(void);
void	tl_parse(void);
void	tl_yyerror(char *);
void	trans(tl_Node *);

void    mk_alternating(tl_Node *);
void    mk_generalized();
void    mk_buchi();

ATrans *dup_trans(ATrans *);
ATrans *merge_trans(ATrans *, ATrans *);
void do_merge_trans(ATrans **, ATrans *, ATrans *);

int  *new_set(int);
int  *clear_set(int *, int);
int  *make_set(int , int);
void copy_set(int *, int *, int);
int  *dup_set(int *, int);
void merge_sets(int *, int *, int);
void do_merge_sets(int *, int *, int *, int);
int  *intersect_sets(int *, int *, int);
void add_set(int *, int);
void rem_set(int *, int);
void spin_print_set(int *, int*);
void print_set(int *, int);
int  empty_set(int *, int);
int  empty_intersect_sets(int *, int *, int);
int  same_sets(int *, int *, int);
int  included_set(int *, int *, int);
int  in_set(int *, int);
int  *list_set(int *, int);

int timeval_subtract (struct timeval *, struct timeval *, struct timeval *);

#define ZN	(tl_Node *)0
#define ZS	(tl_Symbol *)0
#define Nhash	255    	
#define True	tl_nn(TRUE,  ZN, ZN)
#define False	tl_nn(FALSE, ZN, ZN)
#define Not(a)	push_negation(tl_nn(NOT, a, ZN))
#define rewrite(n)	canonical(right_linked(n))

typedef tl_Node	*Nodeptr;
/*#define YYSTYPE	 Nodeptr*/

#define Debug(x)	{ if (0) printf(x); }
#define Debug2(x,y)	{ if (tl_verbose) printf(x,y); }
#define Dump(x)		{ if (0) dump(x); }
#define Explain(x)	{ if (tl_verbose) tl_explain(x); }

#define Assert(x, y)	{ if (!(x)) { tl_explain(y); \
			  Fatal(": assertion failed\n",(char *)0); } }
#define min(x,y)        ((x<y)?x:y)
