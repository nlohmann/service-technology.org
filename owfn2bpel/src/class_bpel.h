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


#ifndef _CLASS_BPEL_H_
#define _CLASS_BPEL_H_

class bpel
{
	public:

	int activity;
	string name;
//	string value;	//for later extensions
	links *source;
	links *target;
	branch *branches;
	int link_is_or;
	bpel *next;

//constructors

	bpel(int act)
	{
		activity = act;
		next = NULL;
		source = NULL;
		target = NULL;
		branches = NULL;
		link_is_or = 0;
		name = "";
//		value = "";
	}

	bpel(int act, bpel *ptr)
	{
		activity = act;
		next = ptr;
		source = NULL;
		target = NULL;
		branches = NULL;
		link_is_or = 0;
		name = "";
//		value = "";
	}

	bpel(int act, bpel *ptr, string str)
	{
		activity = act;
		next = ptr;
		source = NULL;
		target = NULL;
		branches = NULL;
		link_is_or = 0;
		name = str;
//		value = "";
	}
/*
	bpel(int act, bpel *ptr, string str, string val)
	{
		activity = act;
		next = ptr;
		source = NULL;
		target = NULL;
		branches = NULL;
		link_is_or = 0;
		name = str;
		value = val;
	}
*/
//branch functions

	string remove_fullstop(string str);
	void delete_lists();
	void add_branch();
	void out();
	void links_out(ofstream *bpel_file, int in = 0);
	void code(ofstream *bpel_file, plists *ins = NULL, plists *outs = NULL, int in = 0);
	void add_source();
	void add_target();
	void add_target(int i);
	bpel *is_target();

}; //end of class bpel

#endif
