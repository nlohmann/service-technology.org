/***** ltl2ba : lex.c *****/

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

#include <stdlib.h>
#include <ctype.h>
#include "ltl2ba.h"

static tl_Symbol	*symtab[Nhash+1];
static int	tl_lex(void);

#define Token(y)        tl_yylval = tl_nn(y,ZN,ZN); return y

int
isalnum_(int c)
{       return (isalnum(c) || c == '_');
}

int
hash(char *s)
{       int h=0;

        while (*s)
        {       h += *s++;
                h <<= 1;
                if (h&(Nhash+1))
                        h |= 1;
        }
        return h&Nhash;
}

tl_Symbol *
tl_lookup(char *s)
{	tl_Symbol *sp;
	int h = hash(s);

	for (sp = symtab[h]; sp; sp = sp->next)
		if (strcmp(sp->name, s) == 0)
			return sp;

	sp = (tl_Symbol *) tl_emalloc(sizeof(tl_Symbol));
	sp->name = (char *) tl_emalloc(strlen(s) + 1);
	strcpy(sp->name, s);
	sp->next = symtab[h];
	symtab[h] = sp;

	return sp;
}

tl_Symbol *
getsym(tl_Symbol *s)
{	tl_Symbol *n = (tl_Symbol *) tl_emalloc(sizeof(tl_Symbol));

	n->name = s->name;
	return n;
}
