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

#include "options.h"
#include "classes.h"

using namespace std;


void transition::add_consume(string str)
{
	consumes = new place(str, consumes);
}

void transition::add_produce(string str)
{
	produces = new place(str, produces);
}


void transition::del_produces()
{
	place *ptr;
	place *help;

	ptr = produces;
	while(ptr != NULL)
	{
		help = ptr;
		ptr = ptr->next;
		delete(help);
	}

	produces = NULL;
}

void transition::del_consumes()
{
	place *ptr;
	place *help;

	ptr = consumes;
	while(ptr != NULL)
	{
		help = ptr;
		ptr = ptr->next;
		delete(help);
	}

	consumes = NULL;
}

void transition::del_lists()
{
	place *ptr;
	place *help;

	ptr = produces;
	while(ptr != NULL)
	{
		help = ptr;
		ptr = ptr->next;
		delete(help);
	}
	ptr = consumes;
	while(ptr != NULL)
	{
		help = ptr;
		ptr = ptr->next;
		delete(help);
	}

	produces = NULL;
	consumes = NULL;
}

void transition::add_bpel(int bpel_id)
{
	bpel_code = new bpel(bpel_id, bpel_code);
}

void transition::add_bpel(int bpel_id, string name)
{
	bpel_code = new bpel(bpel_id, bpel_code, name);
}

void transition::add_last_bpel(int bpel_id, string name)
{
	bpel *tmp;
	
	tmp = bpel_code;

	if(tmp == NULL)
	{
		bpel_code = new bpel(bpel_id, bpel_code, name);
	}else
	{
		while(tmp->next != NULL)
		{
			tmp = tmp->next;
		}
		tmp->next = new bpel(bpel_id, NULL, name);
	}
}


void transition::append_bpel(bpel *list)
{
	bpel *help;

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

void transition::out()
{
	place *tmpplaces;
	cout << endl << "Transition: '" << name << "'" << " (dfs: " << dfs << ", lowlink: " << lowlink << ")" << endl;
	tmpplaces = consumes;
	cout << "   Consumes: ";
	while(tmpplaces != NULL)
	{
		cout << "'" << tmpplaces->name << "' ";
		tmpplaces = tmpplaces->next;
	}
	cout << endl << "   Produces: ";
	tmpplaces = produces;
	while(tmpplaces != NULL)
	{
		cout << "'" << tmpplaces->name << "' ";
		tmpplaces = tmpplaces->next;
	}
	cout << endl;
	if(bpel_code != NULL)
		bpel_code->out();
	if(next != NULL)
		next->out();
}

void transition::pure_out()
{
	cout << "Transition: '" << name << "'" << endl;;
	if(next != NULL)
		next->pure_out();
}

void transition::owfn_out()
{
	place *tmpplaces;
	cout << endl << "TRANSITION " << name << endl << "CONSUME" << endl;
	tmpplaces = consumes;
	if(tmpplaces == NULL)
	{
		cout << ";" << endl << "PRODUCE" << endl;
	}
	while(tmpplaces != NULL)
	{
		cout << tmpplaces->name << " : 1";
		
		if(tmpplaces->next != NULL)
		{
			cout << "," << endl;
		}else
		{
			cout << ";" << endl << "PRODUCE" << endl;
		}
		
		tmpplaces = tmpplaces->next;
	}

	tmpplaces = produces;
	if(tmpplaces == NULL)
	{
		cout << ";" << endl;
	}
	while(tmpplaces != NULL)
	{
		cout << tmpplaces->name << " : 1";
		
		if(tmpplaces->next != NULL)
		{
			cout << "," << endl;
		}else
		{
			cout << ";" << endl;
		}
		
		tmpplaces = tmpplaces->next;
	}

	if(next != NULL)
		next->owfn_out();
}

void transition::list_out()
{
	place *tmpplaces;
	cout << endl << "Transition: '" << name << "'" << endl;
	tmpplaces = consumes;
	cout << "   Consumes: ";
	while(tmpplaces != NULL)
	{
		cout << "'" << tmpplaces->name << "' ";
		tmpplaces = tmpplaces->next;
	}
	cout << endl << "   Produces: ";
	tmpplaces = produces;
	while(tmpplaces != NULL)
	{
		cout << "'" << tmpplaces->name << "' ";
		tmpplaces = tmpplaces->next;
	}
	cout << endl;
}



void transitionlist::out()
{
	cout << "Transition: " << transitionptr->name << endl;
	if(next != NULL)
		next->out();
}

void transitionlist::list_out()
{
	transitionptr->list_out();
	if(next != NULL)
		next->list_out();
}


