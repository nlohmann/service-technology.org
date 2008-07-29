/*
   oWFN2BPEL - translate open workflow net models into BPEL processes

   Copyright (C) 2008       Niels Lohmann <niels.lohmann@uni-rostock.de>
   Copyright (C) 2006, 2007 Jens Kleine <jkleine@informatik.hu-berlin.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/


#ifndef _CLASSES_H_
#define _CLASSES_H_


#include <cstring>
#include <cstdlib>

using namespace std;



class plists;
class links;
class branch;
class bpel;
class transition;
class transitionlist;
class place;
class placelist;
class owfn;



extern int *stat_array;
extern int number_of_rounds;
extern char *lola_ex;
enum activities {EMPTY, REPLY, RECEIVE, FLOW, SWITCH, PICK, PROCESS, WHILE, SEQUENCE, EXIT, INVOKE, FOREACH, SCOPE, OPAQUE};


class plists
{
	public:
	plists *next;
	string name;

//constructors

	plists()
	{
		next = NULL;
		name = "";
	}

	plists(plists *ptr)
	{
		next = ptr;
		name = "";
	}

	plists(plists *ptr, string str)
	{
		next = ptr;
		name = str;
	}
}; //end of class plists


//usage of class link: increment() once, create new instance in source and in target list with same id

class links
{
	public:
	int link_id;
	links *next;
	static int link_counter;

//constructors

	links()
	{
		link_id = link_counter;
		next = NULL;
	}

	links(links *ptr)
	{
		link_id = link_counter;
		next = ptr;
	}

	static void increment()
	{
		link_counter++;
	}
}; //end of class links


	
	
class branch
{
	public:
	bpel *bpel_code;
	branch *next;
	string pick_receive;

//constructors

	branch()
	{
		bpel_code = NULL;
		next = NULL;
		pick_receive = "";
	}

	branch(branch *ptr)
	{
		bpel_code = NULL;
		next = ptr;
		pick_receive = "";
	}
}; //end of class branch


#include "class_bpel.h"
#include "class_place.h"

class placelist
{
	public:
	place *placeptr;
	placelist *next;
	int ausgang;

//constructor

	placelist(place *plptr, placelist *listptr)
	{
		placeptr = plptr;
		next = listptr;
		ausgang = 0;
	}

	void out();

}; //end of class placelist

#include "class_transition.h"

class transitionlist
{
	public:
	transition *transitionptr;
	transitionlist *next;

//constructor

	transitionlist(transition *transptr, transitionlist *listptr)
	{
		transitionptr = transptr;
		next = listptr;
	}

	void out();
	void list_out();

}; //end of class transitionlist





class stack
{
	public:
	place *placeptr;
	transition *transptr;

	stack *next;

//constructor

	stack(place *plptr, transition *trptr, stack *listptr)
	{
		placeptr = plptr;
		transptr = trptr;
		next = listptr;
	}
}; //end of class stack

#include "class_owfn.h"

#endif
