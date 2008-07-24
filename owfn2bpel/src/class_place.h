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


#ifndef _CLASS_PLACE_H_
#define _CLASS_PLACE_H_

class place
{
	public:

	string name;
	bpel *bpel_code;
	place *next;
	
	int dfs;
	int lowlink;

//constructors

	place(string str)
	{
		name = str;
		next = NULL;
		bpel_code = NULL;
		
		dfs = -1;
		lowlink = -1;
	}

	place(string str, place *ptr)
	{
		name = str;
		next = ptr;
		bpel_code = NULL;
		
		dfs = -1;
		lowlink = -1;
	}
	
	void append_bpel(bpel *list);
	void out();
	void pure_out();
	void owfn_out();
	void add_bpel(int bpel_id);
	void add_bpel(int bpel_id, string name);
	void add_last_bpel(int bpel_id, string name);
	
}; //end of class place

#endif
