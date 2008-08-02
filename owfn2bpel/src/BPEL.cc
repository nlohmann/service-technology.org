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
#include <sstream>
#include "classes.h"

using std::string;
using std::cout;
using std::endl;
using std::ofstream;
using std::stringstream;


int links_are_out;


void out_bpel(int i)
{
	switch(i)
	{
		case EMPTY:
			cout << "empty";
			break;
		case REPLY:
			cout << "reply";
			break;
		case RECEIVE:
			cout << "receive";
			break;
		case FLOW:
			cout << "flow";
			break;
		case SWITCH:
			cout << "if";
			break;
		case PICK:
			cout << "pick";
			break;
		case PROCESS:
			cout << "process";
			break;
		case WHILE:
			cout << "while";
			break;
		case SEQUENCE:
			cout << "sequence";
			break;
		case EXIT:
			cout << "exit";
			break;
		case INVOKE:
			cout << "invoke";
			break;
		case FOREACH:
			cout << "forEach";
			break;
		case SCOPE:
			cout << "scope";
			break;
		case OPAQUE:
			cout << "opaqueActivity";
			break;
	}
}

void out_bpel(int i, ofstream *bpel_file)
{
	switch(i)
	{
		case EMPTY:
			*bpel_file << "empty";
			break;
		case REPLY:
			*bpel_file << "reply";
			break;
		case RECEIVE:
			*bpel_file << "receive";
			break;
		case FLOW:
			*bpel_file << "flow";
			break;
		case SWITCH:
			*bpel_file << "if";
			break;
		case PICK:
			*bpel_file << "pick";
			break;
		case PROCESS:
			*bpel_file << "process";
			break;
		case WHILE:
			*bpel_file << "while";
			break;
		case SEQUENCE:
			*bpel_file << "sequence";
			break;
		case EXIT:
			*bpel_file << "exit";
			break;
		case INVOKE:
			*bpel_file << "invoke";
			break;
		case FOREACH:
			*bpel_file << "forEach";
			break;
		case SCOPE:
			*bpel_file << "scope";
			break;
		case OPAQUE:
			*bpel_file << "opaqueActivity";
			break;
	}
}


//constructors
BPEL::BPEL(int act, BPEL *ptr, string str) :
    activity(act),
    name(str),    
    source(NULL),
    target(NULL),
    branches(NULL),
    link_is_or(0),
    next(ptr)        
{
}



string BPEL::remove_fullstop(string str)
{
	string::size_type pos;
	string tmp;
	
	do
	{
		tmp = str;
		if((pos=str.find('.')) != string::npos)
		{
			str.swap(str.erase(pos, 1));
		}
	}while(tmp != str);
    
	return str;
}


void BPEL::delete_lists()
{
	Branch *branchptr;
	Links *del;
	
	while(source != NULL)
	{
		del = source;
		source = source->next;
		delete(del);
	}
    
	while(target != NULL)
	{
		del = target;
		target = target->next;
		delete(del);
	}
	while(branches != NULL)
	{
		branchptr = branches;
		if(branchptr->bpel_code != NULL)
		{
			branchptr->bpel_code->delete_lists();
			delete(branchptr->bpel_code);
		}
		branches = branches->next;
		delete(branches);
	}
}

void BPEL::add_branch()
{
	branches = new Branch(branches);
}

void BPEL::out()
{
	Branch *help;
	Links *linkptr;
	int branch_counter = 1;
	
	cout << "\n Activity: ";
	out_bpel(activity);
	cout << " (" << name <<")\n";
	
	if(source != NULL)
	{
		linkptr = source;
		cout << " Activity is source of links: ";
		while(linkptr != NULL)
		{
			cout << linkptr->link_id << " ";
			linkptr = linkptr->next;
		}
		cout << "\n";
	}
	
	if(target != NULL)
	{
		linkptr = target;
		cout << " Activity is target of links";
		if(link_is_or)
			cout << " (OR): ";
		else
			cout << " (AND): ";
		while(linkptr != NULL)
		{
			cout << linkptr->link_id << " ";
			linkptr = linkptr->next;
		}
		cout << "\n";
	}
    
	help = branches;
	if(help != NULL)
	{
		cout << "\n   --Branches of activity " << name <<"-->\n";
		while(help != NULL)
		{
			cout << "\n  Branch " << branch_counter << " of activity " << name <<"\n";
			if(help->bpel_code != NULL)
				help->bpel_code->out();
            
			branch_counter++;
			help = help->next;
		}
		cout << "\n   <--Branches of activity " << name <<"--\n";
	}
	if(next != NULL)
		next->out();
}

void BPEL::links_out(ofstream *bpel_file, int in)
{
    
    /*
     Links können niemals die Grenzen eines flow überschreiten, so wie sie von uns angelegt werden.
     Daher kann hier bei der Suche nach Links alles übersprungen werden, was selber ein flow ist.
     Ist eingefügt durch Zeile: if(activity != FLOW), aber ungetestet.
     Ist erst wichtig, wenn es mehr Funktionen gibt, die Links erzeugen.
     */
	Branch *help;
	Links *linkptr;
    
	string indenting = "";
	int i;
    
	for(i = 1; i <= in; i++)
		indenting = indenting + " ";
    
	//check only targets, sources would double every link id
	linkptr = target;
	while(linkptr != NULL)
	{
        
		if(links_are_out == 0)
		{
			*bpel_file << indenting << "  <links>" << endl;
			links_are_out = 1;
		}
		*bpel_file << indenting << "    <link name=\"Link_" << linkptr->link_id << "\"/>" << endl;
		linkptr = linkptr->next;
	}
    
	if(activity != FLOW)
	{
		help = branches;
		while(help != NULL)
		{
			if(help->bpel_code != NULL)
				help->bpel_code->links_out(bpel_file, in);
            
			help = help->next;
		}
	}
    
	if(next != NULL)
		next->links_out(bpel_file, in);
}

void BPEL::code(ofstream *bpel_file, plists *ins, plists *outs, int in)
{
#ifdef VERBOSE
	cout << ".";
#endif
	Branch *help;
	Links *linkptr;
	int branch_counter = 1;
	string indenting = "";
	int i;
	int addition;
	int alarm = 0;
	stringstream ss;
	string number;
	string link_string;
	
    
	for(i = 1; i <= in; i++)
		indenting = indenting + " ";
	
	*bpel_file << indenting << "<";
	out_bpel(activity, bpel_file);
	*bpel_file << " name=\"Act_" << name << "\"";
	switch(activity)
	{
		case PROCESS:
			*bpel_file << endl << "  targetNamespace=\"http://docs.oasis-open.org/wsbpel/2.0/process/abstract\"" << endl;
			*bpel_file << "  xmlns=\"http://docs.oasis-open.org/wsbpel/2.0/process/abstract\"" << endl;
			*bpel_file << "  suppressJoinFailure=\"yes\"" << endl;
			*bpel_file << "  xmlns:template=\"http://docs.oasisopen.org/wsbpel/2.0/process/abstract/simple-template/2006/08\"" << endl;
			*bpel_file << "  abstractProcessProfile=\"http://docs.oasisopen.org/wsbpel/2.0/process/abstract/simple-template/2006/08\"";
			break;
		case RECEIVE:
		case REPLY:
			*bpel_file << endl << indenting << "  partnerLink=\"generic_pl\"" << endl;
			*bpel_file << indenting << "  operation=\"" << name << "\"" << endl;
			*bpel_file << indenting << "  variable=\"Var_" << remove_fullstop(name) << "\"";
			break;
		case INVOKE:
			*bpel_file << endl << indenting << "  partnerLink=\"generic_pl\"" << endl;
			*bpel_file << indenting << "  operation=\"" << name << "\"" << endl;
			*bpel_file << indenting << "  inputVariable=\"Var_" << remove_fullstop(name) << "\"";
			break;
	}
    
	if(branches == NULL && next == NULL && source == NULL && target == NULL)		//close empty tags immediately
		*bpel_file << " /";
	*bpel_file << ">" << endl;
    
	if(activity == PROCESS)
	{
		*bpel_file << indenting << "    <partnerLinks>" << endl;
		*bpel_file << indenting << "      <partnerLink name=\"generic_pl\" partnerLinkType=\"##opaque\"" << endl;
		*bpel_file << indenting << "                   myRole=\"##opaque\" partnerRole=\"##opaque\" />" << endl;
		*bpel_file << indenting << "    </partnerLinks>" << endl << endl;
        
		//list all input and output names as variable names
		if(ins != NULL || outs != NULL)
		{
			*bpel_file << indenting << "  <variables>" << endl;
			while(ins != NULL)
			{
				*bpel_file << indenting << "    <variable name=\"Var_" << remove_fullstop(ins->name) << "\" element=\"##opaque\" />" << endl;
				ins = ins->next;
			}
            
			while(outs != NULL)
			{
				*bpel_file << indenting << "    <variable name=\"Var_" << remove_fullstop(outs->name) << "\" element=\"##opaque\" />" << endl;
				outs = outs->next;
			}
            
			*bpel_file << indenting << "  </variables>" << endl << endl;
		}
	}
    
	if(activity == FLOW)
	{
		//this may be the source of errors! links of nested flows are also listed!
		//list all link names in a flow at it's beginning
		//at the moment there can't be flows within the top flow with links in them
        
		links_are_out = 0;
		help = branches;
		while(help != NULL)
		{
			if(help->bpel_code != NULL)
			{
				help->bpel_code->links_out(bpel_file, in);
			}
            
			help = help->next;
		}
		if(links_are_out == 1)
		{
			*bpel_file << indenting << "  </links>" << endl << endl;
		}
	}
    
    
	if(target != NULL)
	{
		link_string = "";
		linkptr = target;
		*bpel_file << indenting << "  <targets>" << endl;
        
        // create the string for the join condition
		while(linkptr != NULL)
		{
			ss.clear();
			ss << linkptr->link_id;
			ss >> number;
			if(link_string == "")
			{
				link_string = "$Link_" + number;
			}else
			{
				if(link_is_or == 1)
				{
					link_string = link_string + " or $Link_" + number;
				}else
				{
					link_string = link_string + " and $Link_" + number;
				}
			}
			linkptr = linkptr->next;
		}
        
        // print the join condition
		if(target->next != NULL)	//Auslassen, falls nur ein Link eingeht
		{
			*bpel_file << indenting << "    <joinCondition>" << link_string <<"</joinCondition>" << endl;
		}
        
        linkptr = target;
        
        // print all links
		while(linkptr != NULL)
		{
			ss.clear();
			ss << linkptr->link_id;
			ss >> number;
			*bpel_file << indenting << "    <target linkName=\"Link_" << linkptr->link_id << "\" />" << endl;
			linkptr = linkptr->next;
		}
        
        
		*bpel_file << indenting << "  </targets>" << endl;
	}
    
	if(source != NULL)
	{
		linkptr = source;
		*bpel_file << indenting << "  <sources>" << endl;
		while(linkptr != NULL)
		{
			*bpel_file << indenting << "    <source linkName=\"Link_" << linkptr->link_id << "\" />" << endl;
			linkptr = linkptr->next;
		}
		*bpel_file << indenting << "  </sources>" << endl;
	}
    
	if(activity == WHILE)
	{
		*bpel_file << indenting << "  <condition opaque=\"yes\" />" << endl;
	}
    
	alarm = 0;
	help = branches;
	if(help != NULL)
	{
		while(help != NULL)
		{
			addition = 2;
			if(activity == SWITCH)
			{
				if(branch_counter == 1)
				{
					addition = 4;
					*bpel_file << indenting << "  <condition opaque=\"yes\" />" << endl;
					if(	//first activity is target
                       help->bpel_code->target != NULL
                       ||
                       (	//first activity in first sequence is target
                        help->bpel_code->activity == SEQUENCE
                        && help->bpel_code->branches->bpel_code->target != NULL
                        )
                       ||
                       (	//first activity is an empty and empty is source, this means, that the target could not be melted with the empty and is a target of another link
                        help->bpel_code->activity == EMPTY
                        && help->bpel_code->next == NULL
                        && help->bpel_code->source != NULL
                        && help->bpel_code->source->next == NULL
                        )
                       )
					{
						*bpel_file << indenting << "      <!-- WARNING! -->" << endl;
						*bpel_file << indenting << "      <!-- The first activity in this branch might be target of a link. -->" << endl;
						*bpel_file << indenting << "      <!-- This is a sign that there is an additional condition linked to the execution of this branch. -->" << endl;
						*bpel_file << indenting << "      <!-- Please consider this when adding the boolean expression that chooses the executed branch. -->" << endl;
					}
					//*bpel_file << endl;
				}else
				{
					if(help->next == NULL)	//last branch
					{
						addition = 6;
						*bpel_file << indenting << "  <else>" << endl;
						if(	//first activity is target
                           help->bpel_code->target != NULL
                           ||
                           (	//first activity in first sequence is target
                            help->bpel_code->activity == SEQUENCE
                            && help->bpel_code->branches->bpel_code->target != NULL
                            )
                           ||
                           (	//first activity is an empty and empty is source, this means, that the target could not be melted with the empty and is a target of another link
                            help->bpel_code->activity == EMPTY
                            && help->bpel_code->next == NULL
                            && help->bpel_code->source != NULL
                            && help->bpel_code->source->next == NULL
                            )
                           )
						{
							*bpel_file << endl;
							*bpel_file << indenting << "        <!-- WARNING! -->" << endl;
							*bpel_file << indenting << "        <!-- The first activity in this branch might be target of a link. -->" << endl;
							*bpel_file << indenting << "        <!-- This is a sign that there is an additional condition linked to the execution of this branch. -->" << endl;
							*bpel_file << indenting << "        <!-- Please consider this when adding the boolean expression that chooses the executed branch. -->" << endl;
						}
						//*bpel_file << endl;
					}else
					{
						addition = 6;
						*bpel_file << indenting << "  <elseif>" << endl << endl;
						*bpel_file << indenting << "    <condition opaque=\"yes\" />" << endl;
						if(	//first activity is target
                           help->bpel_code->target != NULL
                           ||
                           (	//first activity in first sequence is target
                            help->bpel_code->activity == SEQUENCE
                            && help->bpel_code->branches->bpel_code->target != NULL
                            )
                           ||
                           (	//first activity is an empty and empty is source, this means, that the target could not be melted with the empty and is a target of another link
                            help->bpel_code->activity == EMPTY
                            && help->bpel_code->next == NULL
                            && help->bpel_code->source != NULL
                            && help->bpel_code->source->next == NULL
                            )
                           )
						{
							*bpel_file << indenting << "        <!-- WARNING! -->" << endl;
							*bpel_file << indenting << "        <!-- The first activity in this branch might be target of a link. -->" << endl;
							*bpel_file << indenting << "        <!-- This is a sign that there is an additional condition linked to the execution of this branch. -->" << endl;
							*bpel_file << indenting << "        <!-- Please consider this when adding the boolean expression that chooses the executed branch. -->" << endl;
						}
						//*bpel_file << endl;
					}
				}
			}
			
			if(activity == PICK)
			{
				addition = 4;
				if(help->pick_receive != "" && !alarm)
				{
					*bpel_file << indenting << "  <onMessage partnerLink=\"generic_pl\"" << endl;
					*bpel_file << indenting << "    operation=\"" << help->pick_receive << "\"" << endl;
					*bpel_file << indenting << "    variable=\"Var_" << help->pick_receive << "\">" << endl << endl;
				}
				if(help->pick_receive == "" && alarm)
				{
					*bpel_file << indenting << "  <onAlarm>" << endl;
					*bpel_file << indenting << "    <for opaque=\"yes\" />" << endl << endl;
				}	//else keine Ausgabe
			}
            //rekursiver Aufruf:
			if(activity != PICK)
			{
				if(help->bpel_code != NULL)
					help->bpel_code->code(bpel_file, NULL, NULL, in + addition);
			}else
			{
				if(help->pick_receive != "" && !alarm)
				{
					if(help->bpel_code != NULL)
						help->bpel_code->code(bpel_file, NULL, NULL, in + addition);
				}
				if(help->pick_receive == "" && alarm)
				{
					if(help->bpel_code != NULL)
						help->bpel_code->code(bpel_file, NULL, NULL, in + addition);
				}	//else kein Aufruf
			}
            
			if(activity == SWITCH && branch_counter > 1)
			{
				if(help->next == NULL)
				{
					*bpel_file << indenting << "  </else>" << endl;
				}else
				{
					*bpel_file << indenting << "  </elseif>" << endl;
				}
			}
			if(activity == PICK)
			{
				if(help->pick_receive != "" && !alarm)
				{
					*bpel_file << indenting << "  </onMessage>" << endl << endl;
				}
				if(help->pick_receive == "" && alarm)
				{
					*bpel_file << indenting << "  </onAlarm>" << endl << endl;
				}	//else keine Ausgabe
			}
            
			branch_counter++;
			help = help->next;
			
			if(activity == PICK && help == NULL && !alarm)
			{
				//erneuter Durchlauf aller branches, diesmal Ausgabe der onAlarm Zweige
				help = branches;
				alarm = 1;
			}
		}
	}
	if(branches != NULL || next != NULL || source != NULL || target != NULL)
	{
		*bpel_file << indenting << "</";
		out_bpel(activity, bpel_file);
		*bpel_file << ">" << endl << endl;
	}
	if(next != NULL)
		next->code(bpel_file, NULL, NULL, in + 2);
}


//link functions
void BPEL::add_source()
{
	source = new Links(source);
}

void BPEL::add_target()
{
	target = new Links(target);
}

void BPEL::add_target(int i)
{
	//add link at the front of the list of bpel activities
	BPEL *help;
    
	help = this;
	while(help->next != NULL)
	{
		help = help->next;
	}
	help->add_target();
}

BPEL *BPEL::is_target()
{
	//check the front of the list of bpel activities
	BPEL *help;
    
	help = this;
	while(help->next != NULL)
	{
		help = help->next;
	}
	if(help->target != NULL)
		return help;
	else
		return NULL;
}
