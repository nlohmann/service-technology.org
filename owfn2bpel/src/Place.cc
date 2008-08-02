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


#include <fstream>
#include <iostream>
#include <string>
#include "classes.h"

using std::string;
using std::cout;
using std::endl;


//constructor
Place::Place(string str, Place *ptr) :
    name(str),
    bpel_code(NULL),
    next(ptr),
    dfs(-1),
    lowlink(-1)    
{
}


void Place::add_bpel(int bpel_id)	//fügt neue Aktivität am Ende an
{
	bpel_code = new BPEL(bpel_id, bpel_code);
}

void Place::add_bpel(int bpel_id, string name)
{
	bpel_code = new BPEL(bpel_id, bpel_code, name);
}

void Place::add_last_bpel(int bpel_id, string name)	//fügt neue Aktivität vor dem Anfang an
{
	BPEL *tmp;
	
	tmp = bpel_code;

	if(tmp == NULL)
	{
		bpel_code = new BPEL(bpel_id, bpel_code, name);
	}else
	{
		while(tmp->next != NULL)
		{
			tmp = tmp->next;
		}
		tmp->next = new BPEL(bpel_id, NULL, name);
	}
}

void Place::append_bpel(BPEL *list)
{
	BPEL *help;

	if(list != NULL)
	{
		help = list;
		while(list->next != NULL)
		{
			list = list->next;
		}
		list->next = bpel_code;
		bpel_code = help; 
	}	//else nothing to do
}

void Place::out()
{
	cout << "\nPlace: '" << name << "'" << " (dfs: " << dfs << ", lowlink: " << lowlink << ")" << endl;
	if(bpel_code != NULL)
		bpel_code->out();
	if(next != NULL)
		next->out();
}

void Place::owfn_out()
{
	cout << name;
	if(next != NULL)
	{
		cout << ", ";
		next->owfn_out();
	}else
	{
		cout << ";" << endl << endl;
	}
}

void PlaceList::out()
{
	cout << "Place: " << placeptr->name << "\n";
	if(next != NULL)
		next->out();
}


