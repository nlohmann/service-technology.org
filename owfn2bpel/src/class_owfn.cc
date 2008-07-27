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
#include <sstream>
#include <string>
#include <libgen.h>
#include <cassert>

#include "options.h"
#include "classes.h"
#include "cmdline.h"

using namespace std;

// variable holding the information from the command line
extern gengetopt_args_info args_info;




bpel *owfn::copy_bpel(bpel *bpel_code)
{
//Warning: This function copies the links with their own id! Links can only be used once!
	bpel *new_code;
	bpel *returned_code;
	branch *branches;
	links *tmp;

	new_code = NULL;

	if(bpel_code != NULL)
	{
		returned_code = copy_bpel(bpel_code->next);
		
		new_code = new bpel(bpel_code->activity, returned_code, bpel_code->name);

		new_code->link_is_or = bpel_code->link_is_or;

		tmp = bpel_code->target;
		while(tmp != NULL)
		{
			new_code->add_target();
			new_code->target->link_id = tmp->link_id;
			
			tmp = tmp->next;
		}
		tmp = bpel_code->source;
		while(tmp != NULL)
		{
			new_code->add_source();
			new_code->source->link_id = tmp->link_id;
			
			tmp = tmp->next;
		}

		if(bpel_code->branches != NULL)
		{
			branches = bpel_code->branches;
			while(branches != NULL)
			{
				new_code->add_branch();
				new_code->branches->bpel_code = copy_bpel(branches->bpel_code);
				branches = branches->next;
			}
		}
	}

	return new_code;
}

void owfn::copy_links(bpel *bpel_code)
{
	//Ändert die ids aller Links
	int i;
	int stop;

	stop = links::link_counter;

	for(i = 1; i <= stop; i++)
	{
		global_link_id = 0;
		search_links(bpel_code, i);
	}
}

void owfn::search_links(bpel *bpel_code, int start)
{
	branch *branches;
	links *tmp;

	if(bpel_code != NULL)
	{
		tmp = bpel_code->source;
		while(tmp != NULL)
		{
			if(tmp->link_id == start)
			{
				if(global_link_id == 0)
				{
					links::increment();
					global_link_id = links::link_counter;
				}
				tmp->link_id = global_link_id;
			}
			tmp = tmp->next;
		}

		tmp = bpel_code->target;
		while(tmp != NULL)
		{
			if(tmp->link_id == start)
			{
				if(global_link_id == 0)
				{
					links::increment();
					global_link_id = links::link_counter;
				}
				tmp->link_id = global_link_id;
			}
			tmp = tmp->next;
		}

		search_links(bpel_code->next, start);

		if(bpel_code->branches != NULL)
		{
			branches = bpel_code->branches;
			while(branches != NULL)
			{
				search_links(branches->bpel_code, start);
				branches = branches->next;
			}
		}
	}
}


void owfn::copy_place(place *place_to_copy, int n)
{
	string placename;
	string number;
	stringstream ss;
	
	ss << n;
	ss >> number;
	
	placename = place_to_copy->name + "_C" + number;
	add_place(placename);

	places->bpel_code = copy_bpel(place_to_copy->bpel_code);
	copy_links(places->bpel_code);
}


void owfn::copy_transition(transition *transition_to_copy, int n)
{
//Warning: The copied transition expects copied place names!

	place *lists;
	string transname;
	string placename;
	string number;
	stringstream ss;
	
	ss << n;
	ss >> number;
	
	transname = transition_to_copy->name + "_C" + number;
	transitions = new transition(transname, transitions);

	transitions->bpel_code = copy_bpel(transition_to_copy->bpel_code);
	copy_links(transitions->bpel_code);

	lists = transition_to_copy->consumes;
	while(lists != NULL)
	{
		placename = lists->name + "_C" + number;
		transitions->add_consume(placename);
		lists = lists->next;
	}
	lists = transition_to_copy->produces;
	while(lists != NULL)
	{
		placename = lists->name + "_C" + number;
		transitions->add_produce(placename);
		lists = lists->next;
	}
}


void owfn::copy_transition_once(transition *transition_to_copy, int n)
{
//doesn't change name in the consume list
//doesn't change name in the produce list

	place *lists;
	string transname;
	string number;
	stringstream ss;
	
	ss << n;
	ss >> number;
	
	transname = transition_to_copy->name + "_C" + number;
	transitions = new transition(transname, transitions);

	transitions->bpel_code = copy_bpel(transition_to_copy->bpel_code);
	copy_links(transitions->bpel_code);

	lists = transition_to_copy->consumes;
	while(lists != NULL)
	{
		transitions->add_consume(lists->name);
		lists = lists->next;
	}
	lists = transition_to_copy->produces;
	while(lists != NULL)
	{
		transitions->add_produce(lists->name);
		lists = lists->next;
	}
}

void owfn::rename_copy(place *place, int n)
{
	string number;
	stringstream ss;
	
	ss << n;
	ss >> number;
	
	place->name = place->name + "_C" + number;
}

place *owfn::return_placeptr(string placename)
{
	place *placeptr;
	
	placeptr = places;
	while(placeptr != NULL)
	{
		if(placeptr->name == placename)
		{
			return placeptr;
		}
		placeptr = placeptr->next;
	}
	
	return NULL;
}


void owfn::add_place(string name)
{
	places = new place(name, places);
}

void owfn::add_initial(string name)
{
	initials = new place(name, initials);;
}

void owfn::add_final(string name)
{
	//TODO: This function: str name, int marking (mehrere?)
	finals = new place(name, finals);;
}

void owfn::add_input(string name)
{
	inputs = new place(name, inputs);
}

void owfn::add_output(string name)
{
	outputs = new place(name, outputs);
}



void owfn::write_bpel()
{
	bpel_file.open(outfile, ios::out);

	if(!bpel_file)
	{
		cout << "Error 1 in function 'write_bpel': BPEL file '" << outfile << "' could not be opened!" << endl;
		exit(EXIT_FAILURE);
	}

#ifdef VERBOSE
	cout << "BPEL file opened...\n";
#endif

	place *input;
	plists *inlist;
	inlist = NULL;
	input = inputs;

	while(input != NULL)
	{

		inlist = new plists(inlist, input->name);
		input = input->next;
	}

	place *output;
	plists *outlist;
	outlist = NULL;
	output = outputs;
	while(output != NULL)
	{
		outlist = new plists(outlist, output->name);
		output = output->next;
	}

#ifdef VERBOSE
	cout << " writing: ";
#endif
	final_code->code(&bpel_file, inlist, outlist, 0);

	bpel_file.close();
#ifdef VERBOSE
	cout << "\nBPEL file closed.\n";
#endif
}


void owfn::read_owfn()
{
	char keyword[16];

	//open file
	owfn_file.open(file, ios::in);
	if(!owfn_file)
	{
		cout << "Error 1 in function 'read_owfn': oWFN file '" << file << "' not found!\n";
		exit(EXIT_FAILURE);
	}

#ifdef VERBOSE
	cout << "oWFN opened...\n";
#endif

	strcpy(keyword, "INTERNAL");
	if(find_key(keyword))
	{
#ifdef VERBOSE
		cout << " Storing places...\n";
#endif

		store(&owfn::add_place);	//call with function name
	}else
	{
		cout << "Error 2 in function 'read_owfn': Internal places were not found in oWFN file! There needs to be at least one place in the oWFN to generate BPEL code.\n";
		exit(EXIT_FAILURE);
	}

	owfn_file.clear();
	owfn_file.seekg(0);

	strcpy(keyword, "INPUT");
	if(find_key(keyword))
	{
#ifdef VERBOSE
		cout << " Storing input places...\n";
#endif
		store(&owfn::add_input);	//call with function name
	}else
	{
#ifdef VERBOSE
		cout << " Warning: Net in oWFN file has no input places!\n";
#endif
	}

	owfn_file.clear();
	owfn_file.seekg(0);

	strcpy(keyword, "OUTPUT");
	if(find_key(keyword))
	{
#ifdef VERBOSE
		cout << " Storing output places...\n";
#endif
		store(&owfn::add_output);	//call with function name
	}else
	{
#ifdef VERBOSE
		cout << " Warning: Net in oWFN file has no output places!\n";
#endif
	}

	owfn_file.clear();
	owfn_file.seekg(0);

	strcpy(keyword, "INITIALMARKING");
	if(find_key(keyword))
	{
#ifdef VERBOSE
		cout << " Storing initial marking...\n";
#endif
		store(&owfn::add_initial);	//call with function name
	}else
	{
		cout << "Error 3 in function 'read_owfn': Initial marking was not found in oWFN file! There needs to be at least one place marked in the oWFN to generate BPEL code.\n";
		exit(EXIT_FAILURE);
	}

	owfn_file.clear();
	owfn_file.seekg(0);

	strcpy(keyword, "FINALMARKING");
	if(find_key(keyword))
	{
#ifdef VERBOSE
		cout << " Storing final marking...\n";
#endif
		store(&owfn::add_final);	//call with function name
	}else
	{
		cout << "Error 4 in function 'read_owfn': Final marking was not found in oWFN file! There needs to be at least one final marking in the oWFN to generate BPEL code.\n";
		exit(EXIT_FAILURE);
	}

	owfn_file.clear();
	owfn_file.seekg(0);

	strcpy(keyword, "TRANSITION");
	int streamptr;

	while(find_key(keyword))
	{
		streamptr = owfn_file.tellg();
		owfn_file.clear();
		store_transition();
		owfn_file.seekg(streamptr);	//reset position, if not only every third transition gets read
	}

	//close file
	owfn_file.close();
#ifdef VERBOSE
	cout << "oWFN closed...\n";
#endif
}

void owfn::store_transition()
{
	char keyword[16];
	string buffer, name;
	int streamptr;
	
	streamptr = owfn_file.tellg();
//first the name
	owfn_file >> buffer;
	name = remove_whitespace(buffer);
	transitions = new transition(name, transitions);

//consumed places go second

	owfn_file.clear();
	owfn_file.seekg(streamptr);

	strcpy(keyword, "CONSUME");
	if(find_key(keyword))
	{
		store(&owfn::add_consume);	//call with function name
	}

	owfn_file.clear();
	owfn_file.seekg(streamptr);

//produce places
	strcpy(keyword, "PRODUCE");
	if(find_key(keyword))
	{
		store(&owfn::add_produce);	//call with function name
	}
}


void owfn::add_consume(string str)
{
	//transitions points to the last added transition
	transitions->add_consume(str);
}

void owfn::add_produce(string str)
{
	//transitions points to the last added transition
	transitions->add_produce(str);
}


void owfn::store(void (owfn::*add_f)(string))
{
	string buffer;
	string::size_type loc, tmp_loc;
	int stopper = 1;	// ; found, stop scanning
	int indicator;		// nothing found, don't save
	int comment = 0;
	int new_word = 1;

	while(owfn_file >> buffer && stopper)
	{
		//remove comments
		if(comment)
		{
			loc = buffer.find('}', 0);
			if(loc != string::npos)
			{
				//end of comment without whitespace
				buffer.erase(0, loc + 1);
				indicator = 1;	//if string is longer there might be something
				comment = 0;
			}else
			{
				//we are still somewhere in the comment
				comment = 1;
				indicator = 0;
				buffer.erase();
			}
		}
		loc = buffer.find('{', 0);
		if(loc != string::npos)
		{
			if(loc == 0)	//Comment starts
			{
				loc = buffer.find('}', 0);
				if(loc != string::npos)
				{
					//end of comment without whitespace
					buffer.erase(0, loc + 1);
					indicator = 1;	//if string is longer there might be something
				}else
				{
					//we are somewhere in the comment
					comment = 1;
					indicator = 0;
					buffer.erase();
				}
			}else		//something{comment
			{
				tmp_loc = buffer.find('}', 0);
				if(tmp_loc != string::npos)
				{
					//end of comment without whitespace
					buffer.erase(loc, tmp_loc + 1);
					indicator = 1;	//if string is longer there might be something left
				}else
				{
					//we are somewhere in the comment
					comment = 1;
					indicator = 1;	//save start
					buffer.erase(loc);
				}
			}
		}
		//comments removed

		loc = buffer.find(';', 0);
		if(loc != string::npos)
		{
			stopper = 0;
		}
		buffer = remove_whitespace(buffer);
		while(!buffer.empty())
		{
			buffer = remove_whitespace(buffer);
			loc = buffer.find(',', 0);
			if(loc != string::npos)
			{
				if(new_word)
				{
					tmp_loc = buffer.find(':', 0);
					if(tmp_loc != string::npos)
					{
						buffer.erase(tmp_loc);
					}
					(*this.*add_f)(buffer.substr(0, loc));	//add object to list
				}else
				{
					//ignore word but save next
					new_word = 1;
				}
				//TODO: Kommentare und Werte finden und mit speichern
				buffer = buffer.substr(loc + 1);	//+ 1 to remove ,
			}else
			{
				loc = buffer.find(';', 0);
				if(loc != string::npos)
				{
					//remove everything behind the ;
					buffer.erase(loc);

					if(new_word)
					{
						if(!buffer.empty())
						{
							tmp_loc = buffer.find(':', 0);
							if(tmp_loc != string::npos)
							{
								buffer.erase(tmp_loc);
							}
							(*this.*add_f)(buffer);	//add object to list
						}
					}else
					{
						//ignore word but save next
						new_word = 1;
					}
				}else
				{
					if(new_word)
					{
						//erstes wort wird gespeichert, alles dahinter bis zum , oder ; ignoriert
						//just one name, save it
						if(!buffer.empty())
						{
							tmp_loc = buffer.find(':', 0);
							if(tmp_loc != string::npos)
							{
								buffer.erase(tmp_loc);
							}
							(*this.*add_f)(buffer);	//add object to list
							new_word = 0;
						}
					}
				}
				
				//TODO: Kommentare finden und mit speichern
				buffer.erase();
			}
		}
	}
}


int owfn::find_key(char *keyword)
{
	string buffer;
	string::size_type loc;

	while(owfn_file >> buffer)
	{
		loc = buffer.find(keyword, 0);
		if(loc != string::npos)
		//keyword found
			return 1;
	}
	return 0;
}

string owfn::remove_whitespace(string str)
{
	char const *delim = " \t\r\n";

	string::size_type loc = str.find_first_not_of(delim);
	str.erase(0, loc);

	loc = str.find_last_not_of(delim);
	str.erase(loc + 1);

	return str;
}

string owfn::remove_delims(string str)
{
	char const *delim = ";,";

	string::size_type loc = str.find_first_not_of(delim);
	str.erase(0, loc);

	loc = str.find_last_not_of(delim);
	str.erase(loc + 1);

	return str;
}










//Net transforming functions



void owfn::intrans_oneinitial()
{
#ifdef VERBOSE
	cout << " looking for a single initial marking...\n";
#endif

	transition *transptr;
	place *placeptr;
	int found = 0;
	string new_place_name = "replacement_initial_place";
	string new_trans_name = "replacement_initial_transition";

	if(initials != NULL)
	{
		if(initials->next == NULL)
		{
			transptr = transitions;
			while(transptr != NULL && !found)
			{
				placeptr = transptr->produces;
				while(placeptr != NULL && !found)
				{
					if(placeptr->name == initials->name)
					{
						found = 1;
					}
					placeptr = placeptr->next;
				}
				
				transptr = transptr->next;
			}
		}
	}

	if(found)	//transitions produce on initial place (must be part of a circle)
	{
		add_place(new_place_name);
		transitions = new transition(new_trans_name, transitions);
		transitions->add_consume(new_place_name);
		transitions->add_produce(initials->name);
		transitions->add_bpel(EMPTY, transitions->name);
		delete(initials);
		initials = NULL;
		add_initial(new_place_name);
#ifdef VERBOSE
	cout << "  initial place on a circle found and replaced.\n";
#endif
	}
#ifdef VERBOSE
	else
	{
		cout << " no initial place on a circle found.\n";
	}
#endif

}

void owfn::intrans_initialmarkings()
{
#ifdef VERBOSE
	cout << " looking for initial markings...\n";
#endif
	place *initialptr;
	place *deleteptr;
	string new_place_name = "replacement_initial_place";
	string new_trans_name = "replacement_initial_transition";

	if(initials != NULL)
	{
		if(initials->next != NULL)		//More than one initial place
		{
			add_place(new_place_name);
			transitions = new transition(new_trans_name, transitions);
			transitions->add_consume(new_place_name);
	
			initialptr = initials;
			while(initialptr != NULL)
			{
				transitions->add_produce(initialptr->name);
				deleteptr = initialptr;
				initialptr = initialptr->next;
				delete(deleteptr);
			}
			initials = NULL;
			add_initial(new_place_name);
#ifdef VERBOSE
		cout << "  more than one initial place found and replaced.\n";
#endif
		}
#ifdef VERBOSE
		else
		{
			cout << " not more than one initial place found.\n";
		}
#endif
	}
}






int owfn::inrepl_ins()
{
#ifdef VERBOSE
	cout << " looking for receiving transitions...\n";
#endif
	int return_value = 0;
	transition *transptr;
	place *placeptr;
	place *inputptr;
	place *prevptr;
	bpel *newbpel;
	branch *newbranch;
	int counter;
	int nocounter;
	int repeat;

	transptr = transitions;

	while(transptr != NULL)					//check all transitions
	{
		counter = 0;
		nocounter = 0;
		placeptr = transptr->consumes;
		while(placeptr != NULL)				//check all input places
		{
			inputptr = inputs;
			while(inputptr != NULL)			//compare to all input places
			{
				if(placeptr->name == inputptr->name)
				{
					counter++;
					break;
				}
				
				inputptr = inputptr->next;
			}

			placeptr = placeptr->next;		//next place in consumes list
		}

		placeptr = transptr->produces;
		while(placeptr != NULL)				//check all output places
		{
			inputptr = outputs;
			while(inputptr != NULL)			//compare to all output places
			{
				if(placeptr->name == inputptr->name)
				{
					nocounter++;
					break;
				}
				
				inputptr = inputptr->next;
			}

			placeptr = placeptr->next;		//next place in produces list
		}

		if(counter > 1 && nocounter == 0)					//transition with more than one in/output place found
		{
			transptr->add_bpel(FLOW, transptr->name);	//add flow

			//repeat searches
			placeptr = transptr->consumes;
			prevptr = NULL;
			while(placeptr != NULL)				//check all input places
			{
				repeat = 0;
				inputptr = inputs;
				while(inputptr != NULL)			//compare to all input places
				{
					if(placeptr->name == inputptr->name)
					{
						//add a receive branch for every input place
						transptr->bpel_code->add_branch();
						newbranch = transptr->bpel_code->branches;
						newbpel = new bpel(RECEIVE, NULL, inputptr->name);
						newbranch->bpel_code = newbpel;
						
						//remove input place from consumes list
						if(prevptr == NULL)	//first element of list
						{
							transptr->consumes = transptr->consumes->next;
						}else
						{
							prevptr->next = placeptr->next;
						}
						delete(placeptr);
						repeat = 1;

						break;
					}

					inputptr = inputptr->next;
				}

				if(!repeat)
				{
					prevptr = placeptr;
					if(placeptr != NULL)
						placeptr = placeptr->next;		//next place in produces list
				}else
				{
					prevptr = NULL;
					placeptr = transptr->consumes;
				}

			}
			return_value++;
		}

		transptr = transptr->next;				//next transition
	}

#ifdef VERBOSE
	cout << "  " << return_value << " transition(s) found and changed.\n";
#endif
#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " transition(s) found and changed.\n";
	}
#endif
	return return_value;
}




int owfn::inrepl_outs()
{
#ifdef VERBOSE
	cout << " looking for sending transitions...\n";
#endif
	int return_value = 0;
	transition *transptr;
	place *placeptr;
	place *inputptr;
	place *prevptr;
	bpel *newbpel;
	branch *newbranch;
	int counter;
	int nocounter;
	int repeat;

	transptr = transitions;

	while(transptr != NULL)					//check all transitions
	{
		counter = 0;
		nocounter = 0;
		placeptr = transptr->consumes;
		while(placeptr != NULL)				//check all input places
		{
			inputptr = inputs;
			while(inputptr != NULL)			//compare to all input places
			{
				if(placeptr->name == inputptr->name)
				{
					nocounter++;
					break;
				}
				
				inputptr = inputptr->next;
			}

			placeptr = placeptr->next;		//next place in consumes list
		}

		placeptr = transptr->produces;
		while(placeptr != NULL)				//check all output places
		{
			inputptr = outputs;
			while(inputptr != NULL)			//compare to all output places
			{
				if(placeptr->name == inputptr->name)
				{
					counter++;
					break;
				}
				
				inputptr = inputptr->next;
			}

			placeptr = placeptr->next;		//next place in produces list
		}

		if(counter > 1 && nocounter == 0)		//transition with more than one in/output place found
		{
			transptr->add_bpel(FLOW, transptr->name);	//add flow

			//repeat searches
			placeptr = transptr->produces;
			prevptr = NULL;
			while(placeptr != NULL)				//check all input places
			{
				repeat = 0;
				inputptr = outputs;
				while(inputptr != NULL)			//compare to all output places
				{
					if(placeptr->name == inputptr->name)
					{
						//add a invoke branch for every output place
						transptr->bpel_code->add_branch();
						newbranch = transptr->bpel_code->branches;
						newbpel = new bpel(INVOKE, NULL, inputptr->name);
						newbranch->bpel_code = newbpel;
						
						//remove output place from produces list
						if(prevptr == NULL)	//first element of list
						{
							transptr->produces = transptr->produces->next;
						}else
						{
							prevptr->next = placeptr->next;
						}
						delete(placeptr);
						repeat = 1;

						break;
					}

					inputptr = inputptr->next;
				}

				if(!repeat)
				{
					prevptr = placeptr;
					if(placeptr != NULL)
						placeptr = placeptr->next;		//next place in produces list
				}else
				{
					prevptr = NULL;
					placeptr = transptr->produces;
				}
			}
			return_value++;
		}

		transptr = transptr->next;				//next transition
	}

#ifdef VERBOSE
	cout << "  " << return_value << " transition(s) found and changed.\n";
#endif
#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " transition(s) found and changed.\n";
	}
#endif
	return return_value;
}


int owfn::inrepl_inout()
{
#ifdef VERBOSE
	cout << " looking for receiving and sending transitions...\n";
#endif
	int return_value = 0;
	transition *transptr;
	place *placeptr;
	place *inputptr;
	place *prevptr;
	bpel *newbpel;
	branch *newbranch;
	int counterin;
	int counterout;
	int repeat;

	transptr = transitions;

	while(transptr != NULL)					//check all transitions
	{
		counterin = 0;
		placeptr = transptr->consumes;
		while(placeptr != NULL)				//check all input places
		{
			inputptr = inputs;
			while(inputptr != NULL)			//compare to all input places
			{
				if(placeptr->name == inputptr->name)
				{
					counterin++;
					break;
				}
				
				inputptr = inputptr->next;
			}

			placeptr = placeptr->next;		//next place in consumes list
		}

		counterout = 0;
		placeptr = transptr->produces;
		while(placeptr != NULL)				//check all output places
		{
			inputptr = outputs;
			while(inputptr != NULL)			//compare to all output places
			{
				if(placeptr->name == inputptr->name)
				{
					counterout++;
					break;
				}
				
				inputptr = inputptr->next;
			}

			placeptr = placeptr->next;		//next place in produces list
		}

		if(counterin > 1 && counterout > 1)					//transition with more than one in/output place found
		{
			transptr->add_bpel(FLOW, "input_flow_at_" + transptr->name);	//add flow

			//repeat searches
			placeptr = transptr->consumes;
			prevptr = NULL;
			while(placeptr != NULL)				//check all input places
			{
				repeat = 0;
				inputptr = inputs;
				while(inputptr != NULL)			//compare to all input places
				{
					if(placeptr->name == inputptr->name)
					{
						//add a receive branch for every input place
						transptr->bpel_code->add_branch();
						newbranch = transptr->bpel_code->branches;
						newbpel = new bpel(RECEIVE, NULL, inputptr->name);
						newbranch->bpel_code = newbpel;
						
						//remove input place from consumes list
						if(prevptr == NULL)	//first element of list
						{
							transptr->consumes = transptr->consumes->next;
						}else
						{
							prevptr->next = placeptr->next;
						}
						delete(placeptr);
						repeat = 1;

						break;
					}

					inputptr = inputptr->next;
				}
				if(!repeat)
				{
					prevptr = placeptr;
					if(placeptr != NULL)
						placeptr = placeptr->next;		//next place in produces list
				}else
				{
					prevptr = NULL;
					placeptr = transptr->consumes;
				}
			}

			transptr->add_bpel(FLOW, "output_flow_at_" + transptr->name);	//add flow

			placeptr = transptr->produces;
			prevptr = NULL;
			while(placeptr != NULL)				//check all input places
			{
				repeat = 0;
				inputptr = outputs;
				while(inputptr != NULL)			//compare to all output places
				{
					if(placeptr->name == inputptr->name)
					{
						//add a invoke branch for every output place
						transptr->bpel_code->add_branch();
						newbranch = transptr->bpel_code->branches;
						newbpel = new bpel(INVOKE, NULL, inputptr->name);
						newbranch->bpel_code = newbpel;
						
						//remove output place from produces list
						if(prevptr == NULL)	//first element of list
						{
							transptr->produces = transptr->produces->next;
						}else
						{
							prevptr->next = placeptr->next;
						}
						delete(placeptr);
						repeat = 1;

						break;
					}

					inputptr = inputptr->next;
				}
				if(!repeat)
				{
					prevptr = placeptr;
					if(placeptr != NULL)
						placeptr = placeptr->next;		//next place in produces list
				}else
				{
					prevptr = NULL;
					placeptr = transptr->produces;
				}
			}
			return_value++;
		}
		if(counterin > 1 && counterout == 1)
		{
			transptr->add_bpel(FLOW, transptr->name);	//add flow

			//repeat searches
			placeptr = transptr->consumes;
			prevptr = NULL;
			while(placeptr != NULL)				//check all input places
			{
				repeat = 0;
				inputptr = inputs;
				while(inputptr != NULL)			//compare to all input places
				{
					if(placeptr->name == inputptr->name)
					{
						//add a receive branch for every input place
						transptr->bpel_code->add_branch();
						newbranch = transptr->bpel_code->branches;
						newbpel = new bpel(RECEIVE, NULL, inputptr->name);
						newbranch->bpel_code = newbpel;
						
						//remove input place from consumes list
						if(prevptr == NULL)	//first element of list
						{
							transptr->consumes = transptr->consumes->next;
						}else
						{
							prevptr->next = placeptr->next;
						}
						delete(placeptr);
						repeat = 1;

						break;
					}

					inputptr = inputptr->next;
				}
				if(!repeat)
				{
					prevptr = placeptr;
					if(placeptr != NULL)
						placeptr = placeptr->next;		//next place in produces list
				}else
				{
					prevptr = NULL;
					placeptr = transptr->consumes;
				}
			}
			placeptr = transptr->produces;
			prevptr = NULL;
			repeat = 0;
			while(placeptr != NULL && !repeat)				//check all input places
			{
				repeat = 0;
				inputptr = outputs;
				while(inputptr != NULL && !repeat)			//compare to all output places
				{
					if(placeptr->name == inputptr->name)
					{
						//add an invoke
						transptr->add_bpel(INVOKE, inputptr->name);
						
						//remove output place from produces list
						if(prevptr == NULL)	//first element of list
						{
							transptr->produces = transptr->produces->next;
						}else
						{
							prevptr->next = placeptr->next;
						}
						delete(placeptr);
						repeat = 1;	//there is only one invoke, so stop here

						break;
					}

					inputptr = inputptr->next;
				}
				if(!repeat)
				{
					prevptr = placeptr;
					if(placeptr != NULL)
						placeptr = placeptr->next;		//next place in produces list
				}else
				{
					prevptr = NULL;
					placeptr = transptr->produces;
				}
			}
			return_value++;
		}
		if(counterin == 1 && counterout > 1)
		{
			//repeat searches
			placeptr = transptr->consumes;
			prevptr = NULL;
			repeat = 0;
			while(placeptr != NULL && !repeat)				//check all input places
			{
				repeat = 0;
				inputptr = inputs;
				while(inputptr != NULL && !repeat)			//compare to all input places
				{
					if(placeptr->name == inputptr->name)
					{
						//add a receive
						transptr->add_bpel(RECEIVE, inputptr->name);
						
						//remove input place from consumes list
						if(prevptr == NULL)	//first element of list
						{
							transptr->consumes = transptr->consumes->next;
						}else
						{
							prevptr->next = placeptr->next;
						}
						delete(placeptr);
						repeat = 1;	//stop

						break;
					}

					inputptr = inputptr->next;
				}
				if(!repeat)
				{
					prevptr = placeptr;
					if(placeptr != NULL)
						placeptr = placeptr->next;		//next place in produces list
				}else
				{
					prevptr = NULL;
					placeptr = transptr->consumes;
				}
			}
			transptr->add_bpel(FLOW, transptr->name);	//add flow
			placeptr = transptr->produces;
			prevptr = NULL;
			while(placeptr != NULL)				//check all input places
			{
				repeat = 0;
				inputptr = outputs;
				while(inputptr != NULL)			//compare to all output places
				{
					if(placeptr->name == inputptr->name)
					{
						//add a invoke branch for every output place
						transptr->bpel_code->add_branch();
						newbranch = transptr->bpel_code->branches;
						newbpel = new bpel(INVOKE, NULL, inputptr->name);
						newbranch->bpel_code = newbpel;
						
						//remove output place from produces list
						if(prevptr == NULL)	//first element of list
						{
							transptr->produces = transptr->produces->next;
						}else
						{
							prevptr->next = placeptr->next;
						}
						delete(placeptr);
						repeat = 1;

						break;
					}

					inputptr = inputptr->next;
				}
				if(!repeat)
				{
					prevptr = placeptr;
					if(placeptr != NULL)
						placeptr = placeptr->next;		//next place in produces list
				}else
				{
					prevptr = NULL;
					placeptr = transptr->produces;
				}
			}
			return_value++;
		}
		if(counterin == 1 && counterout == 1)
		{
			//repeat searches
			placeptr = transptr->consumes;
			prevptr = NULL;
			repeat = 0;
			while(placeptr != NULL && !repeat)				//check all input places
			{
				repeat = 0;
				inputptr = inputs;
				while(inputptr != NULL && !repeat)			//compare to all input places
				{
					if(placeptr->name == inputptr->name)
					{
						//add a receive
						transptr->add_bpel(RECEIVE, inputptr->name);
						
						//remove input place from consumes list
						if(prevptr == NULL)	//first element of list
						{
							transptr->consumes = transptr->consumes->next;
						}else
						{
							prevptr->next = placeptr->next;
						}
						delete(placeptr);
						repeat = 1;	//stop

						break;
					}

					inputptr = inputptr->next;
				}
				if(!repeat)
				{
					prevptr = placeptr;
					if(placeptr != NULL)
						placeptr = placeptr->next;		//next place in produces list
				}else
				{
					prevptr = NULL;
					placeptr = transptr->consumes;
				}
			}
			placeptr = transptr->produces;
			prevptr = NULL;
			repeat = 0;
			while(placeptr != NULL && !repeat)				//check all input places
			{
				repeat = 0;
				inputptr = outputs;
				while(inputptr != NULL && !repeat)			//compare to all output places
				{
					if(placeptr->name == inputptr->name)
					{
						//add an invoke
						transptr->add_bpel(INVOKE, inputptr->name);
						
						//remove output place from produces list
						if(prevptr == NULL)	//first element of list
						{
							transptr->produces = transptr->produces->next;
						}else
						{
							prevptr->next = placeptr->next;
						}
						delete(placeptr);
						repeat = 1;	//there is only one invoke, so stop here

						break;
					}

					inputptr = inputptr->next;
				}
				if(!repeat)
				{
					prevptr = placeptr;
					if(placeptr != NULL)
						placeptr = placeptr->next;		//next place in produces list
				}else
				{
					prevptr = NULL;
					placeptr = transptr->produces;
				}
			}
			return_value++;
		}

		transptr = transptr->next;				//next transition
	}

#ifdef VERBOSE
	cout << "  " << return_value << " transition(s) found and changed.\n";
#endif
#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " transition(s) found and changed.\n";
	}
#endif
	return return_value;
}


int owfn::inrepl_inputs()
{
#ifdef VERBOSE
	cout << " looking for single receiving transitions...\n";
#endif
	int return_value = 0;
	transition *transptr;
	place *prevptr;
	place *placeptr;
	place *inputptr;
	place *nextplace;

	transptr = transitions;

	while(transptr != NULL)					//check all transitions
	{
		prevptr = NULL;
		placeptr = transptr->consumes;
		while(placeptr != NULL)				//check all input places
		{
			nextplace = placeptr->next;
			inputptr = inputs;
			while(inputptr != NULL)			//compare to all input places
			{
				if(placeptr->name == inputptr->name)
				{
					transptr->add_bpel(RECEIVE, placeptr->name);	//add RECEIVE and name it like the input place
					if(prevptr == NULL)	//first element of list
					{
						transptr->consumes = transptr->consumes->next;
					}else
					{
						prevptr->next = placeptr->next;
					}
					delete(placeptr);
					placeptr = prevptr;
					return_value++;
					break;
				}
				
				inputptr = inputptr->next;
			}

			prevptr = placeptr;
			if(placeptr != NULL)
				placeptr = nextplace;		//next place in consumes list
		}

		transptr = transptr->next;				//next transition
	}

#ifdef VERBOSE
	cout << "  " << return_value << " transition(s) found and changed.\n";
#endif
#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " transition(s) found and changed.\n";
	}
#endif
	return return_value;
}



int owfn::inrepl_outputs()
{
#ifdef VERBOSE
	cout << " looking for single sending transitions...\n";
#endif
	int return_value = 0;
	transition *transptr;
	place *prevptr;
	place *placeptr;
	place *inputptr;
	place *nextplace;

	transptr = transitions;

	while(transptr != NULL)					//check all transitions
	{
		prevptr = NULL;
		placeptr = transptr->produces;
		while(placeptr != NULL)				//check all output places
		{
			nextplace = placeptr->next;
			inputptr = outputs;
			while(inputptr != NULL)			//compare to all output places
			{
				if(placeptr->name == inputptr->name)
				{
					transptr->add_bpel(INVOKE, placeptr->name);	//add INVOKE and name it like the output place
					//remove output place from produces list
					if(prevptr == NULL)	//first element of list
					{
						transptr->produces = transptr->produces->next;
					}else
					{
						prevptr->next = placeptr->next;
					}
					delete(placeptr);
					placeptr = prevptr;
					return_value++;
					break;
				}
				
				inputptr = inputptr->next;
			}

			prevptr = placeptr;
			if(placeptr != NULL)
				placeptr = nextplace;		//next place in produces list
		}
		transptr = transptr->next;				//next transition
	}

#ifdef VERBOSE
	cout << "  " << return_value << " transition(s) found and changed.\n";
#endif
#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " transition(s) found and changed.\n";
	}
#endif
	return return_value;
}


int owfn::reoccrepl_linplaces()
{
#ifdef VERBOSE
	cout << " looking for linear rows (transition core)...\n";
#endif
	
	int return_value = 0;
	int check;
	//Places A and C, Transition B, 1) A* = {B}, 2) B* = {C}, 3) *B = {A}, 4) *C = {B}
	place *placeA;
	place *placeC;
	transition *transitionB;
	transition *transptr;
	transition *transdel;
	transition *transprev;
	transition *next;		//needed to delete transition B
	transition *update;
	place *updateplace;
	place *placeptr;
	place *placeprev;
	string placename;

	transitionB = transitions;

	while(transitionB != NULL)
	{
		placeA = NULL;
		placeC = NULL;
		check = 1;
		next = transitionB->next;
		
		if(transitionB->consumes != NULL)		//there is at least one place
		{
			if(transitionB->consumes->next == NULL)	//but not a second, 2) *B = {A}
			{
				placeA = transitionB->consumes;
				if(transitionB->produces != NULL)	//one place
				{
					if(transitionB->produces->next == NULL)	//not a second 3) B* = {C}
					{
						placeC = transitionB->produces;
						transptr = transitions;
						while(transptr != NULL && check)
						{
							if(transptr != transitionB)
							{
								placeptr = transptr->consumes;		//1)
								while(placeptr != NULL && check)
								{
									if(placeptr->name == placeA->name)
									{
										check = 0;
									}
									placeptr = placeptr->next;
								}
								placeptr = transptr->produces;		//4)
								while(placeptr != NULL && check)
								{
									if(placeptr->name == placeC->name)
									{
										check = 0;
									}
									placeptr = placeptr->next;
								}
							}
							transptr = transptr->next;
						}
						//all checked
						if(check && placeA->name != placeC->name)	//1) and 4) and no double arc
						{
							//A, B, C -> Place(ABC)
							//create new place
							placename = placeA->name + "_" + transitionB->name + "_" + placeC->name;
							add_place(placename);

#ifdef DEBUG
							cout << "   A = " << placeA->name << ", B = " << transitionB->name << ", C = " << placeC->name << " -> " << placename << "\n";
#endif

							//add bpel code of A, B and C to new place
							placeptr = places;
							placeprev = NULL;
							while(placeptr != NULL && placeptr->name != placeA->name)
							{
								placeprev = placeptr;
								placeptr = placeptr->next;
							}
							if(placeptr == NULL)
							{
								cout << "Error 1 in function 'reoccrepl_linplaces': Place '" << placeA->name << "' vanished. This should not happen...\n";
								exit(EXIT_FAILURE);
							}
							places->append_bpel(placeptr->bpel_code);
							//delete place
							if(placeprev == NULL)	//first element of list
							{
								places = places->next;
							}else
							{
								placeprev->next = placeptr->next;
							}
							delete(placeptr);

							transdel = transitions;
							transprev = NULL;
							while(transdel != NULL && transdel->name != transitionB->name)
							{
								transprev = transdel;
								transdel = transdel->next;
							}
							if(transdel == NULL)
							{
								cout << "Error 2 in function 'reoccrepl_linplaces': Transition '" << transitionB->name << "' vanished. This should not happen...\n";
								exit(EXIT_FAILURE);
							}
							if(transdel->bpel_code != NULL)
							{
								places->append_bpel(transdel->bpel_code);
							}else
							{
								places->add_bpel(OPAQUE, transdel->name);
							}

							placeptr = places;
							placeprev = NULL;
							while(placeptr != NULL && placeptr->name != placeC->name)
							{
								placeprev = placeptr;
								placeptr = placeptr->next;
							}
							if(placeptr == NULL)
							{
								cout << "Error 3 in function 'reoccrepl_linplaces': Place '" << placeC->name << "' vanished. This should not happen...\n";
								exit(EXIT_FAILURE);
							}
							places->append_bpel(placeptr->bpel_code);
							//delete place
							if(placeprev == NULL)	//first element of list
							{
								places = places->next;
							}else
							{
								placeprev->next = placeptr->next;
							}
							delete(placeptr);

							//change consumers of C and producers of A to the new place
							update = transitions;
							while(update != NULL)
							{
								updateplace = update->consumes;
								while(updateplace != NULL)
								{
									if(updateplace->name == placeC->name)
									{
										updateplace->name = placename;
									}
									updateplace = updateplace->next;
								}
								update = update->next;
							}
							update = transitions;
							while(update != NULL)
							{
								updateplace = update->produces;
								while(updateplace != NULL)
								{
									if(updateplace->name == placeA->name)
									{
										updateplace->name = placename;
									}
									updateplace = updateplace->next;
								}
								update = update->next;
							}

							//delete transition (now because placeC links to a structure in transition
							if(transprev == NULL)	//first element of list
							{
								transitions = transitions->next;
							}else
							{
								transprev->next = transdel->next;
							}
							transdel->del_lists();
							delete(transdel);
							return_value++;
						}
					}
				}
			}
		}
		transitionB = next;
	}

#ifdef VERBOSE
	cout << "  " << return_value << " linear row(s) found and reduced.\n";
#endif
#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " linear row(s) found and reduced.\n";
	}
#endif
	return return_value;
}


int owfn::reoccrepl_lintrans()
{
#ifdef VERBOSE
	cout << " looking for linear rows (place core)...\n";
#endif
	
	int return_value = 0;
	int check;
	//Transitions A and C, Place B, 1) A* = {B}, 2) B* = {C}, 3) *B = {A}, 4) *C = {B}
	transition *transitionA;
	place *placeB;
	transition *transitionC;
	transition *transptr;
	transition *transprev;
	transition *transprevA;
	transition *transprevC;
	transition *help;
	transition *nextA;		//needed to delete transition A
	place *placeptr;
	place *placeprev;
	place *plist;
	string transitionname;
	int trans_counter;

	transitionA = transitions;
	transprevA = NULL;

	while(transitionA != NULL)
	{
		transitionC = NULL;
		placeB = NULL;
		check = 1;
		nextA = transitionA->next;

		if(transitionA->produces != NULL)		//there is at least one place
		{
			if(transitionA->produces->next == NULL)	//but not a second, 1) A* = {B}
			{
				placeB = transitionA->produces;
				transptr = transitions;
				transprev = NULL;
				trans_counter = 0;
				while(transptr != NULL)
				{
					if(transptr->consumes != NULL)
					{
						placeptr = transptr->consumes;
						while(placeptr != NULL)
						{
							if(placeptr->name == placeB->name)
							{
								//transptr consumes from B: a possible C found
								transitionC = transptr;
								trans_counter++;
							}
							placeptr = placeptr->next;
						}
					}
					transprev = transptr;
					transptr = transptr->next;
				}
				if(trans_counter == 1 && transitionA != transitionC)		//just one transition consumes from B 2) B* = {C} and no double arcs
				{
					if(transitionC->consumes->next == NULL)		//4) *C = {B}
					{
						trans_counter = 0;
						transptr = transitions;
						while(transptr != NULL)
						{
							if(transptr->produces != NULL)
							{
								placeptr = transptr->produces;
								while(placeptr != NULL)
								{
									if(placeptr->name == placeB->name)
									{
										trans_counter++;
									}
									placeptr = placeptr->next;
								}
							}
							transptr = transptr->next;
						}
						if(trans_counter == 1)			//3) *B = {A}
						{
							//A, B, C -> Transition(ABC)
							//create new transition
							transitionname = transitionA->name + "_" + placeB->name + "_" + transitionC->name;
							transitions = new transition(transitionname, transitions);

#ifdef DEBUG
							cout << "   A = " << transitionA->name << ", B = " << placeB->name << ", C = " << transitionC->name << " -> " << transitionname << "\n";
#endif

							//add consume and produce list
							plist = transitionA->consumes;
							while(plist != NULL)
							{
								transitions->add_consume(plist->name);
								plist = plist->next;
							}
							plist = transitionC->produces;
							while(plist != NULL)
							{
								transitions->add_produce(plist->name);
								plist = plist->next;
							}

							//add bpel code of A, B and C to new transition

							if(transitionA->bpel_code != NULL)
							{
								transitions->append_bpel(transitionA->bpel_code);
							}else
							{
								transitions->add_bpel(OPAQUE, transitionA->name);
							}

							placeptr = places;
							placeprev = NULL;
							while(placeptr != NULL && placeptr->name != placeB->name)
							{
								placeprev = placeptr;
								placeptr = placeptr->next;
							}
							if(placeptr == NULL)
							{
								cout << "Error 1 of function 'reoccrepl_lintrans': Place '" << placeB->name << "' vanished. This should not happen...\n";
								exit(EXIT_FAILURE);
							}
							transitions->append_bpel(placeptr->bpel_code);
							//delete place
							if(placeprev == NULL)	//first element of list
							{
								places = places->next;
							}else
							{
								placeprev->next = placeptr->next;
							}
							delete(placeptr);

							if(transitionC->bpel_code != NULL)
							{
								transitions->append_bpel(transitionC->bpel_code);
							}else
							{
								transitions->add_bpel(OPAQUE, transitionC->name);
							}

							help = transitions;
							transprevA = NULL;
							while(help != NULL && help->name != transitionA->name)
							{
								transprevA = help;
								help = help->next;
							}
							if(help == NULL)
							{
								cout << "Error 2 of function 'reoccrepl_lintrans': Transition '" << transitionC->name << "' vanished. This should not happen...\n";
								exit(EXIT_FAILURE);
							}

							//delete transition A
							if(transprevA == NULL)	//first element of list
							{
								transitions = transitions->next;
							}else
							{
								transprevA->next = transitionA->next;
							}
							transitionA->del_lists();
							delete(transitionA);

							if(nextA == transitionC)
							{
								nextA = transitionC->next;	//C gets deleted now
							}

							help = transitions;
							transprevC = NULL;
							while(help != NULL && help->name != transitionC->name)
							{
								transprevC = help;
								help = help->next;
							}
							if(help == NULL)
							{
								cout << "Error 3 of function 'reoccrepl_lintrans': Transition '" << transitionC->name << "' vanished. This should not happen...\n";
								exit(EXIT_FAILURE);
							}
							//delete transition C
							if(transprevC == NULL)	//first element of list
							{
								transitions = transitions->next;
							}else
							{
								transprevC->next = transitionC->next;
							}
							transitionC->del_lists();

							delete(transitionC);
							return_value++;
						}
					}
				}
			}
		}
		transitionA = nextA;
	}

#ifdef VERBOSE
	cout << "  " << return_value << " linear row(s) found and reduced.\n";
#endif
#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " linear row(s) found and reduced.\n";
	}
#endif
	return return_value;
}


int owfn::reoccrepl_endplaces()
{
#ifdef VERBOSE
	cout << " looking for linear rows with open end (transition core)...\n";
#endif

	int return_value = 0;
	int check;
	transition *transptr;
	transition *prev;
	transition *next;
	transition *other;
	transition *update;
	place *placeA;
	place *placeptr;
	place *updateplace;
	string newname;
	
	prev = NULL;
	transptr = transitions;
	while(transptr != NULL)
	{
		next = transptr->next;
		if(transptr->produces == NULL)				//transition does not produce
		{
			if(transptr->consumes != NULL)			//at least one place to consume
			{
				if(transptr->consumes->next == NULL)	//only one
				{
					placeA = transptr->consumes;
					other = transitions;
					check = 1;
					while(other != NULL && check)
					{
						if(other != transptr)
						{
							placeptr = other->consumes;
							while(placeptr != NULL && check)
							{
								if(placeptr->name == placeA->name)
								{
									check = 0;
								}
								placeptr = placeptr->next;
							}
						}
						other = other->next;
					}
					if(check)
					{
						newname = placeA->name + "_" + transptr->name;
#ifdef DEBUG
						cout << "   A = " << placeA->name << ", B = " << transptr->name << " -> " << newname << "\n";
#endif
						//find place A in place list
						placeptr = places;
						while(placeptr != NULL && placeptr->name != placeA->name)
						{
							placeptr = placeptr->next;
						}
						if(placeptr == NULL)
						{
							cout << "Error: Place '" << placeA->name << "' vanished. This should not happen...\n";
							exit(EXIT_FAILURE);
						}
						if(transptr->bpel_code != NULL)
						{
							placeptr->append_bpel(transptr->bpel_code);
						}else
						{
							placeptr->add_bpel(OPAQUE, transptr->name);
						}
						update = transitions;
						while(update != NULL)
						{
							updateplace = update->produces;
							while(updateplace != NULL)
							{
								if(updateplace->name == placeA->name)
								{
									updateplace->name = newname;
								}
								updateplace = updateplace->next;
							}
							update = update->next;
						}
						placeptr->name = newname;
						//delete transition
						if(prev == NULL)	//first element of list
						{
							transitions = transitions->next;
						}else
						{
							prev->next = transptr->next;
						}
						transptr->del_lists();
						delete(transptr);
						transptr = prev;
						return_value++;
					}
				}
			}
		}

		prev = transptr;
		transptr = next;
	}

#ifdef VERBOSE
	cout << "  " << return_value << " linear row(s) found and reduced.\n";
#endif
#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " linear row(s) found and reduced.\n";
	}
#endif
	return return_value;
}


int owfn::reoccrepl_endtrans()
{
#ifdef VERBOSE
	cout << " looking for linear rows with open end (place core)...\n";
#endif

	int return_value = 0;
	int check;
	transition *transptr;
	transition *othertrans;
	place *otherplaces;
	place *placeptr;
	place *next;
	place *prev;
	string newname;
	
	prev = NULL;
	placeptr = places;
	while(placeptr != NULL)
	{
		next = placeptr->next;
		transptr = transitions;
		while(transptr != NULL && placeptr != NULL)
		{
			if(transptr->produces != NULL)			//transition produces places
			{
				if(transptr->produces->next == NULL)	//just one place
				{
					if(transptr->produces->name == placeptr->name)	//place A
					{
						//and no other transition consumes and produces place A
						check = 1;
						othertrans = transitions;
						while(othertrans != NULL && check)
						{
							if(othertrans != transptr)
							{
								otherplaces = othertrans->consumes;
								while(otherplaces != NULL && check)
								{
									if(otherplaces->name == placeptr->name)
									{
										check = 0;
									}
									otherplaces = otherplaces->next;
								}
								otherplaces = othertrans->produces;
								while(otherplaces != NULL && check)
								{
									if(otherplaces->name == placeptr->name)
									{
										check = 0;
									}
									otherplaces = otherplaces->next;
								}
							}

							othertrans = othertrans->next;
						}

						if(check)
						{
							newname = transptr->name + "_" + placeptr->name;
#ifdef DEBUG
							cout << "   A = " << transptr->name << ", B = " << placeptr->name << " -> " << newname << "\n";
#endif
							if(transptr->bpel_code == NULL)
							{
								transptr->add_bpel(OPAQUE, transptr->name);
							}
							//append bpel code of place to transition
							transptr->append_bpel(placeptr->bpel_code);
							//remove place from produce list of transition
							transptr->produces = NULL;
							transptr->name = newname;
							//delete place
							if(prev == NULL)	//first element of list
							{
								places = places->next;
							}else
							{
								prev->next = placeptr->next;
							}
							delete(placeptr);
							placeptr = prev;
							return_value++;
						}
					}
				}
			}
			transptr = transptr->next;
		}

		prev = placeptr;
		placeptr = next;
	}

#ifdef VERBOSE
	cout << "  " << return_value << " linear row(s) found and reduced.\n";
#endif
#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " linear row(s) found and reduced.\n";
	}
#endif
	return return_value;
}



int owfn::reoccrepl_switch()
{
#ifdef VERBOSE
	cout << " looking for ifs...\n";
#endif

	int return_value = 0;
	place *placeptr;
	transition *transptr;
	transition *transprev;
	string A;
	string B;
	string transname;
	int found;
	bpel *help;
	bpel *list;
	branch *newbranch;

	transitionlist *translist;
	transitionlist *translistptr;
	transitionlist *tlist;
	transitionlist *tlistptr;

	placeptr = places;
	while(placeptr != NULL)
	{
		found = 0;
		A = placeptr->name;
		transptr = transitions;
		translist = NULL;
		while(transptr != NULL)
		{
			//consumes only from A and produces only on one place that we'll later might call B
			if(transptr->consumes != NULL && transptr->consumes->next == NULL && transptr->consumes->name == A && transptr->produces != NULL && transptr->produces->next == NULL && transptr->produces->name != A)
			{
				translist = new transitionlist(transptr, translist);
			}
			transptr = transptr->next;
		}

		if(translist != NULL && translist->next != NULL)
		{
			//find a possible B (if there is more than one it will be found the next time this function is called)
			tlistptr = translist;
			tlist = NULL;
			while(!found && tlistptr != NULL)
			{
				while(tlist != NULL)
				{
					translistptr = tlist;
					tlist = tlist->next;
					delete(translistptr);
				}

				//try all the places transitions in translist produce on
				B = tlistptr->transitionptr->produces->name;

				translistptr = translist;
				while(translistptr != NULL)
				{
					if(translistptr->transitionptr->produces->name == B)
					{
						tlist = new transitionlist(translistptr->transitionptr, tlist);
					}
					translistptr = translistptr->next;
				}
				
				//more than one transition produces on the new B
				if(tlist != NULL && tlist->next != NULL)
				{
					found = 1;
				}
				
				tlistptr = tlistptr->next;
			}

			if(found)
			{
				transname = "if_" + A + "_to_" + B;
#ifdef DEBUG
				cout << "   If from " << A << " to " << B << " -> " << transname << "\n";
#endif
				//create new transition
				transitions = new transition(transname, transitions);
				transitions->add_consume(A);
				transitions->add_produce(B);
				transitions->add_bpel(SWITCH, transname);

				//add branches for every transition
				translistptr = tlist;
				while(translistptr != NULL)
				{
					transptr = transitions;
					transprev = NULL;
					while(transptr != NULL && transptr != translistptr->transitionptr)
					{
						transprev = transptr;
						transptr = transptr->next;
					}
					if(transptr != NULL)
					{
						//add branch and bpel
						transitions->bpel_code->add_branch();
						newbranch = transitions->bpel_code->branches;
						list = transptr->bpel_code;
			
						if(list != NULL)
						{
							help = list;
							while(list->next != NULL)
							{
								list = list->next;
							}
							list->next = newbranch->bpel_code;
							newbranch->bpel_code = help; 
						}else
						{
							newbranch->bpel_code = new bpel(OPAQUE, NULL, transptr->name);
						}

						//delete transition
						if(transprev == NULL)	//first element of list
						{//this should not happen! the first transition is the one we just created!
							transitions = transitions->next;
						}else
						{
							transprev->next = transptr->next;
						}
						transptr->del_lists();
						delete(transptr);
					}else
					{
						cout << "Error 1 in function 'reoccrepl_switch': Transition '" << translistptr->transitionptr->name << "' vanished. This should not happen...\n";
						exit(EXIT_FAILURE);
					}
					translistptr = translistptr->next;
				}
			}
		}

		while(translist != NULL)
		{
			translistptr = translist;
			translist = translist->next;
			delete(translistptr);
		}

		if(found)
		{
			return_value++;
			placeptr = places;	//restart
		}else
		{
			placeptr = placeptr->next;
		}
	}



#ifdef VERBOSE
	cout << "  " << return_value << " if(s) found and reduced.\n";
#endif
#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " if(s) found and reduced.\n";
	}
#endif
	return return_value;
}


int owfn::reoccrepl_switchshort()
{
#ifdef VERBOSE
	cout << " looking for short ifs...\n";
#endif

	int return_value = 0;
	place *placeptr;
	place *next;
	place *prev;
	place *prod;
	transition *transptr;
	transition *transprev;
	transition *transnext;
	string placename;
	string newplace;
	int found;
	int check;
	bpel *help;
	bpel *list;
	branch *newbranch;
	
	prev = NULL;
	placeptr = places;
	while(placeptr != NULL)
	{
		next = placeptr->next;

		placename = "";
		found = 0;
		check = 1;
		transptr = transitions;
		while(transptr != NULL && check)
		{
			if(transptr->consumes != NULL)		//one place
			{
				if(transptr->consumes->next == NULL)	//exactly one
				{
					if(transptr->consumes->name == placeptr->name)	//the right one
					{
						if(transptr->produces != NULL)	//one place
						{
							if(transptr->produces->next == NULL)	//exactly one
							{
								if(found)
								{
									if(placename != transptr->produces->name)
									{
										check = 0;
									}else
									{
										found++;	//there needs to be more than one transition
									}
								}else
								{
									placename = transptr->produces->name;
									found = 1;
								}
							}else					//more than one, place useless
							{
								check = 0;
							}
						}else				//no place
						{
							check = 0;
						}
					}
				}
			}

			transptr = transptr->next;
		}

		if(check && found > 1)
		{
			//no other transitions may produce on found final place of switch
			//and no other transition may consume from starting place of switch
			transptr = transitions;
			while(transptr != NULL)
			{
				if((transptr->consumes == NULL) || (transptr->consumes->next != NULL) || (transptr->consumes->name != placeptr->name) || (transptr->produces == NULL) || (transptr->produces->next != NULL) || (placename != transptr->produces->name))
				{
					prod = transptr->produces;
					while(prod != NULL)
					{
						if(prod->name == placename)
						{
							check = 0;
						}
						prod = prod->next;
					}
					prod = transptr->consumes;
					while(prod != NULL)
					{
						if(prod->name == placeptr->name)
						{
							check = 0;
						}
						prod = prod->next;
					}
				}
				transptr = transptr->next;
			}
			if(check)
			{
				//create new place
				newplace = "if_" + placeptr->name + "_to_" + placename;
				add_place(newplace);
#ifdef DEBUG
				cout << "   If from " << placeptr->name << " to " << placename << " -> " << newplace << "\n";
#endif
				//add bpel code of old place, new switch and old place
				places->append_bpel(placeptr->bpel_code);
				places->add_bpel(SWITCH, newplace);

				//add branches for every transition
				transptr = transitions;
				transprev = NULL;
				while(transptr != NULL)
				{
					transnext = transptr->next;
					if((transptr->consumes != NULL) && (transptr->consumes->next == NULL) && (transptr->consumes->name == placeptr->name) && (transptr->produces != NULL) && (transptr->produces->next == NULL) && (placename == transptr->produces->name))
					{
						//add branch and bpel
						places->bpel_code->add_branch();
						newbranch = places->bpel_code->branches;
						list = transptr->bpel_code;
			
						if(list != NULL)
						{
							help = list;
							while(list->next != NULL)
							{
								list = list->next;
							}
							list->next = newbranch->bpel_code;
							newbranch->bpel_code = help; 
						}else
						{
							newbranch->bpel_code = new bpel(OPAQUE, NULL, transptr->name);
						}
	
						//delete transition
						if(transprev == NULL)	//first element of list
						{
							transitions = transitions->next;
						}else
						{
							transprev->next = transptr->next;
						}
						transptr->del_lists();
						delete(transptr);
						transptr = transprev;
					}
					transprev = transptr;
					transptr = transnext;
				}

				//find place
				place *secondplace;
				place *placeprev;
				placeprev = NULL;
				secondplace = places;
				while(secondplace != NULL && secondplace->name != placename)
				{
					placeprev = secondplace;
					secondplace = secondplace->next;
				}
				if(secondplace == NULL)
				{
					cout << "Error: Place '" << placename << "' vanished. This should not happen...\n";
					exit(EXIT_FAILURE);
				}
				places->append_bpel(secondplace->bpel_code);
				//update consume and produce list of new place

				transptr = transitions;
				while(transptr != NULL)
				{
					prod = transptr->produces;
					while(prod != NULL)
					{
						if(prod->name == placeptr->name)
						{
							prod->name = newplace;
						}
						if(prod->name == secondplace->name)
						{
							prod->name = newplace;
						}
						prod = prod->next;
					}
					prod = transptr->consumes;
					while(prod != NULL)
					{
						if(prod->name == placeptr->name)
						{
							prod->name = newplace;
						}
						if(prod->name == secondplace->name)
						{
							prod->name = newplace;
						}
						prod = prod->next;
					}
					transptr = transptr->next;
				}

				//find place
				place *firstplace;
				prev = NULL;
				firstplace = places;
				while(firstplace != NULL && firstplace->name != placeptr->name)
				{
					prev = firstplace;
					firstplace = firstplace->next;
				}
				if(firstplace == NULL)
				{
					cout << "Error: Place '" << placeptr->name << "' vanished. This should not happen...\n";
					exit(EXIT_FAILURE);
				}

				//delete first place
				if(prev == NULL)	//first element of list
				{
					places = places->next;
				}else
				{
					prev->next = firstplace->next;
				}
				delete(firstplace);

				//delete secondplace place
				placeprev = NULL;
				secondplace = places;
				while(secondplace != NULL && secondplace->name != placename)
				{
					placeprev = secondplace;
					secondplace = secondplace->next;
				}
				if(secondplace == NULL)
				{
					cout << "Error: Place '" << placename << "' vanished. This should not happen...\n";
					exit(EXIT_FAILURE);
				}
				if(placeprev == NULL)	//first element of list
				{
					places = places->next;
				}else
				{
					placeprev->next = secondplace->next;
				}
				delete(secondplace);

				placeptr = NULL;
				next = places;
				return_value++;
			}
		}
		
		prev = placeptr;
		placeptr = next;
	}



#ifdef VERBOSE
	cout << "  " << return_value << " if(s) found and reduced.\n";
#endif
#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " if(s) found and reduced.\n";
	}
#endif
	return return_value;
}


int owfn::reoccrepl_switchshortopenend()
{
#ifdef VERBOSE
	cout << " looking for short ifs with open ends...\n";
#endif

	int return_value = 0;
	place *placeptr;
	place *next;
	place *prev;
	place *prod;
	transition *transptr;
	transition *transprev;
	transition *transnext;
	string placename;
	string newplace;
	int found;
	int check;
	bpel *help;
	bpel *list;
	branch *newbranch;

	placeptr = places;
	while(placeptr != NULL)
	{
		next = placeptr->next;

		placename = "";
		found = 0;
		check = 1;
		transptr = transitions;
		while(transptr != NULL && check)
		{
			if(transptr->consumes != NULL)		//one place
			{
				if(transptr->consumes->next == NULL)	//exactly one
				{
					if(transptr->consumes->name == placeptr->name)	//the right one
					{
						if(transptr->produces == NULL)	//no place
						{
							found++;	//there needs to be more than one transition
						}else				//no place
						{
							check = 0;
						}
					}
				}
			}

			transptr = transptr->next;
		}

		if(check && found > 1)
		{
			//no other transition may consume from starting place of switch
			transptr = transitions;
			while(transptr != NULL)
			{
				if((transptr->consumes == NULL) || (transptr->consumes->next != NULL) || (transptr->consumes->name != placeptr->name) || (transptr->produces != NULL))
				{
					prod = transptr->consumes;
					while(prod != NULL)
					{
						if(prod->name == placeptr->name)
						{
							check = 0;
						}
						prod = prod->next;
					}
				}
				transptr = transptr->next;
			}
			if(check)
			{
				//create new place
				newplace = "if_" + placeptr->name + "_open_end";
				add_place(newplace);
#ifdef DEBUG
				cout << "   If from " << placeptr->name << " with open end -> " << newplace << "\n";
#endif

				//add bpel code of old place and new switch
				places->append_bpel(placeptr->bpel_code);
				places->add_bpel(SWITCH, newplace);
	
				//add branches for every transition
				transptr = transitions;
				transprev = NULL;
				while(transptr != NULL)
				{
					transnext = transptr->next;
					if((transptr->consumes != NULL) && (transptr->consumes->next == NULL) && (transptr->consumes->name == placeptr->name) && (transptr->produces == NULL))
					{
						//add branch and bpel
						places->bpel_code->add_branch();
						newbranch = places->bpel_code->branches;
						list = transptr->bpel_code;
			
						if(list != NULL)
						{
							help = list;
							while(list->next != NULL)
							{
								list = list->next;
							}
							list->next = newbranch->bpel_code;
							newbranch->bpel_code = help; 
						}else
						{
							newbranch->bpel_code = new bpel(OPAQUE, NULL, transptr->name);
						}
	
						//delete transition
						if(transprev == NULL)	//first element of list
						{
							transitions = transitions->next;
						}else
						{
							transprev->next = transptr->next;
						}
						transptr->del_lists();
						delete(transptr);
						transptr = transprev;
					}
					transprev = transptr;
					transptr = transnext;
				}
	
				//update consume and produce list of new place
	
				transptr = transitions;
				while(transptr != NULL)
				{
					prod = transptr->produces;
					while(prod != NULL)
					{
						if(prod->name == placeptr->name)
						{
							prod->name = newplace;
						}
						prod = prod->next;
					}
					prod = transptr->consumes;
					while(prod != NULL)
					{
						if(prod->name == placeptr->name)
						{
							prod->name = newplace;
						}
						prod = prod->next;
					}
					transptr = transptr->next;
				}

				prod = places;
				prev = NULL;
				while(prod != NULL && prod->name != placeptr->name)
				{
					prev = prod;
					prod = prod->next;
				}
				if(prod == NULL)
				{
					cout << "Error 1 in function 'reoccrepl_switchshortopenend': Place '" << placeptr->name << "' vanished. This should not happen...\n";
					exit(EXIT_FAILURE);
				}

				//delete first place
				if(prev == NULL)	//first element of list
				{
					places = places->next;
				}else
				{
					prev->next = placeptr->next;
				}
				delete(placeptr);
				next = places;
	
				return_value++;
			}
		}

		placeptr = next;
	}



#ifdef VERBOSE
	cout << "  " << return_value << " if(s) found and reduced.\n";
#endif
#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " if(s) found and reduced.\n";
	}
#endif
	return return_value;
}




int owfn::reoccrepl_circleshort()
{
#ifdef VERBOSE
	cout << " looking for short circles...\n";
#endif

	int return_value = 0;
	int restart;
	transition *transptr;
	transition *transprev;
	place *placeptr;
	bpel *tmpptr;
	bpel *help;

	transprev = NULL;
	transptr = transitions;
	while(transptr != NULL)
	{
		restart = 0;
		if(transptr->produces != NULL && transptr->consumes != NULL && transptr->produces->next == NULL && transptr->consumes->next == NULL && transptr->produces->name == transptr->consumes->name)
		{
#ifdef DEBUG
			cout << "   Circle from " << transptr->consumes->name << " to " << transptr->name << "\n";
#endif
			
			//find the place
			placeptr = places;
			while(placeptr != NULL && placeptr->name != transptr->consumes->name)
			{
				placeptr = placeptr->next;
			}
			if(placeptr == NULL)
			{
				cout << "Error 1 in function 'reoccrepl_circleshort': Place '" << transptr->consumes->name << "' vanished. This should not happen...\n";
				exit(EXIT_FAILURE);
			}
			//don't copy a while as the last activity at a place
			//copy place bpel code
			help = placeptr->bpel_code;
			while(help != NULL && help->activity == WHILE)
			{
				help = help->next;
			}
			tmpptr = copy_bpel(help);
			copy_links(tmpptr);

			//add while
			placeptr->add_bpel(WHILE, transptr->consumes->name);
			placeptr->bpel_code->add_branch();

			//add place bpel code
			placeptr->bpel_code->branches->bpel_code = tmpptr;

			//add transition bpel code
			if(transptr->bpel_code == NULL)
			{
				transptr->add_bpel(OPAQUE, transptr->name);
			}
			if(tmpptr != NULL)
			{
				while(tmpptr->next != NULL)
				{
					tmpptr = tmpptr->next;
				}
			 	tmpptr->next = transptr->bpel_code;	//tmpptr still points at the list starting at placeptr->bpel_code->branches->bpel_code
			}else
			{
				placeptr->bpel_code->branches->bpel_code = transptr->bpel_code;
			}
			if(transprev == NULL)
			{
				transitions = transitions->next;
			}else
			{
				transprev->next = transptr->next;
			}
			delete(transptr);
			restart = 1;
			
			return_value++;
		}

		if(restart)
		{
			transprev = NULL;
			transptr = transitions;
		}else
		{
			transprev = transptr;
			if(transptr != NULL)
			{
				transptr = transptr->next;
			}
		}
	}


#ifdef VERBOSE
	cout << "  " << return_value << " circle(s) found and reduced.\n";
#endif
#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " circle(s) found and reduced.\n";
	}
#endif
	return return_value;
}




int owfn::reoccrepl_flowshortopenend()
{
#ifdef VERBOSE
	cout << " looking for short flows with open ends...\n";
#endif

	int return_value = 0;
	int check;
	transition *transptr;
	transition *alltrans;
	place *placeptr;
	place *prev;
	place *prod;
	place *plist;
	branch *newbranch;
	bpel *list;
	bpel *help;
	
	transptr = transitions;
	
	while(transptr != NULL)
	{
		check = 1;
		if(transptr->produces != NULL && transptr->produces->next != NULL)	//transition produces on 2 places or more
		{
			prod = transptr->produces;
			while(prod != NULL && check)		//check if all places have no consumers and no other producer
			{
				alltrans = transitions;
				while(alltrans != NULL && check)
				{
					plist = alltrans->consumes;
					while(plist != NULL && check)	//check if all consumer lists don't include the place
					{
						if(plist->name == prod->name)
						{
							check = 0;
						}
						plist = plist->next;
					}

					if(alltrans != transptr)
					{
						plist = alltrans->produces;
						while(plist != NULL && check)	//check if all producer lists don't include the place
						{
							if(plist->name == prod->name)
							{
								check = 0;
							}
							plist = plist->next;
						}
					}
					alltrans = alltrans->next;
				}
				prod = prod->next;
			}
			if(check)
			{
#ifdef DEBUG
				cout << "   Flow at " << transptr->name << " with open end\n";
#endif

				//add flow to transition bpel
				transptr->add_bpel(FLOW, transptr->name);
				
				//add branch for every place, delete places
				prod = transptr->produces;
				while(prod != NULL)
				{
					transptr->bpel_code->add_branch();
					newbranch = transptr->bpel_code->branches;

					placeptr = places;
					prev = NULL;
					while(placeptr != NULL && placeptr->name != prod->name)
					{
						prev = placeptr;
						placeptr = placeptr->next;
					}
					if(placeptr == NULL)
					{
						cout << "Error: Place '" << prod->name << "' vanished. This should not happen...\n";
						exit(EXIT_FAILURE);
					}
					list = placeptr->bpel_code;

					if(list != NULL)
					{
						help = list;
						while(list->next != NULL)
						{
							list = list->next;
						}
						list->next = newbranch->bpel_code;
						newbranch->bpel_code = help;
					}else
					{
						newbranch->bpel_code = new bpel(OPAQUE, NULL, placeptr->name);
					}

					//delete place
					if(prev == NULL)	//first element of list
					{
						places = places->next;
					}else
					{
						prev->next = placeptr->next;
					}
					delete(placeptr);
					
					prod = prod->next;
				}
				
				//clear produce list of transition
				transptr->del_produces();

				return_value++;
			}
		}
		transptr = transptr->next;
	}


#ifdef VERBOSE
	cout << "  " << return_value << " flow(s) found and reduced.\n";
#endif
#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " flow(s) found and reduced.\n";
	}
#endif
	return return_value;
}





int owfn::reoccrepl_flowshort()
{
#ifdef VERBOSE
	cout << " looking for short flows...\n";
#endif

	int return_value = 0;
	int counter;
	int check;
	int found;
	transition *transptr;
	transition *alltrans;
	transition *transA;
	transition *transB;
	transition *transprev;
	place *placeptr;
	string finalplace;
	place *prev;
	place *prod;
	place *plist;
	branch *newbranch;
	bpel *list;
	bpel *help;
	transitionlist *translist;
	transitionlist *translistptr;

	transptr = transitions;


	while(transptr != NULL)
	{
		translist = NULL;
		check = 1;
		if(transptr->produces != NULL && transptr->produces->next != NULL)	//transition produces on 2 places or more
		{
			prod = transptr->produces;
			while(prod != NULL && check)		//check if all places have one or none consumer and no other producer
			{
				counter = 0;
				alltrans = transitions;
				while(alltrans != NULL && check)
				{
					if(alltrans != transptr)
					{
						plist = alltrans->produces;
						while(plist != NULL && check)	//check if all producer lists don't include the place
						{
							if(plist->name == prod->name)
							{
								check = 0;
							}
							plist = plist->next;
						}
					}
					
					plist = alltrans->consumes;
					while(plist != NULL && check)	//check how many consumer lists include the place
					{
						if(plist->name == prod->name)
						{
							counter++;
							translist = new transitionlist(alltrans, translist);
						}
						plist = plist->next;
					}

					alltrans = alltrans->next;
				}

				if(counter > 1)		//there may be only one or none transition consuming from the places
				{
					check = 0;
				}

				prod = prod->next;
			}
		}

		if(check && translist != NULL)		//found at least one transition (if there is none another function will handle this flow)
		{
			//check if all transitions are the same
			found = 0;
			translistptr = translist;
			while(translistptr != NULL && check)
			{
				if(translistptr->transitionptr != translist->transitionptr)
				{
					check = 0;
				}
				translistptr = translistptr->next;
			}

			if(check)
			{
				transA = transptr;
				transB = translist->transitionptr;

				//check if that transition B consumes only on place in the production list of transition A
				placeptr = transB->consumes;
				while(placeptr != NULL)
				{
					found = 0;
					prod = transA->produces;
					while(prod != NULL)
					{
						if(prod->name == placeptr->name)
						{
							found = 1;
						}
						prod = prod->next;
					}
					if(!found)
					{
						check = 0;
					}
					placeptr = placeptr->next;
				}
				
				if(check && transA != transB)
				{
#ifdef DEBUG
					cout << "   Flow from " << transA->name << " to " << transB->name << "\n";
#endif

					//add flow, add branches, delete places, add bpel of transition B, copy produce list of trans B to A, delete B
					//add flow to transition
					transA->add_bpel(FLOW, transA->name);
				
					//add branch for every place, delete places
					prod = transA->produces;
					while(prod != NULL)
					{
						transA->bpel_code->add_branch();
						newbranch = transA->bpel_code->branches;

						placeptr = places;
						prev = NULL;
						while(placeptr != NULL && placeptr->name != prod->name)
						{
							prev = placeptr;
							placeptr = placeptr->next;
						}
						if(placeptr == NULL)
						{
							cout << "Error: Place '" << prod->name << "' vanished. This should not happen...\n";
							exit(EXIT_FAILURE);
						}
						list = placeptr->bpel_code;

						if(list != NULL)
						{
							help = list;
							while(list->next != NULL)
							{
								list = list->next;
							}
							list->next = newbranch->bpel_code;
							newbranch->bpel_code = help; 
						}else
						{
							newbranch->bpel_code = new bpel(OPAQUE, NULL, placeptr->name);
						}
					
						//delete place
						if(prev == NULL)	//first element of list
						{
							places = places->next;
						}else
						{
							prev->next = placeptr->next;
						}
						delete(placeptr);
							
						prod = prod->next;
					}
				
					//clear produce list of transitionA
					transA->del_produces();
					transA->produces = transB->produces;
					
					transA->append_bpel(transB->bpel_code);
					//delete transition B
					alltrans = transitions;
					transprev = NULL;
					while(alltrans != NULL && alltrans->name != transB->name)
					{
						transprev = alltrans;
						alltrans = alltrans->next;
					}
					if(alltrans == NULL)
					{
						cout << "Error: Transition '" << transB->name << "' vanished. This should not happen...\n";
						exit(EXIT_FAILURE);
					}
					if(transprev == NULL)	//first element of list
					{
						transitions = transitions->next;
					}else
					{
						transprev->next = alltrans->next;
					}
					alltrans->del_consumes();	//do not delete produces, reused by trans A
					delete(alltrans);

					return_value++;
				}
			}
		}

		//free memory of temporary lists
		while(translist != NULL)
		{
			translistptr = translist;
			translist = translist->next;
			delete(translistptr);
		}
		transptr = transptr->next;
	}

#ifdef VERBOSE
	cout << "  " << return_value << " flow(s) found and reduced.\n";
#endif
#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " flow(s) found and reduced.\n";
	}
#endif
	return return_value;
}



//final transformations

int owfn::somethingleft()
{
	if(places != NULL && places->next != NULL)	//more than one place
	{
		return 1;
	}else
	{
		if(transitions != NULL && transitions->next != NULL) //more than one transition
		{
			return 1;
		}else
		{
			if(places != NULL && transitions != NULL)	//place and transition
			{
				return 1;
			}else				//only one place or one transition
			{
				return 0;
			}
		}
	}
}



int owfn::reoccrepl_exfc_A()
{
#ifdef VERBOSE
	cout << " looking for re-structuring possibilities A...\n";
#endif

	int return_value = 0;
	transition *transptr;
	transition *alltrans;
	place *con;
//	place *prod;
	place *comparelist;
	int cont;
	transitionlist *translist;
	transitionlist *translistptr;
	
	transptr = transitions;
	while(transptr != NULL)
	{
		//if transition consumes from more than one place, it's a candidate
		if(transptr->consumes != NULL && transptr->consumes->next != NULL)
		{
			//find transitions with exactly the same consume list
			translist = NULL;
			alltrans = transitions;
			while(alltrans != NULL)
			{
				//check if the transition consumes from all the same places
				cont = 1;	//as long as cont(inue) equals 1, no missmatch has been found
				comparelist = transptr->consumes;
				while(cont && comparelist != NULL)
				{
					con = alltrans->consumes;
					while(cont && con != NULL && con->name != comparelist->name)
					{
						con = con->next;
					}
					if(con == NULL)		//same place not found
					{
						cont = 0;
					}
					comparelist = comparelist->next;
				}
				//check if the transition consumes from no other place
				con = alltrans->consumes;
				while(cont && con != NULL)
				{
					comparelist = transptr->consumes;
					while(cont && comparelist != NULL && con->name != comparelist->name)
					{
						comparelist = comparelist->next;
					}
					if(comparelist == NULL)		//same place not found
					{
						cont = 0;
					}
					con = con->next;
				}
				if(cont)
				{
					//add alltrans to list of transitions with equal consume lists
					translist = new transitionlist(alltrans, translist);
				}
				alltrans = alltrans->next;
			}

			//if we have found more than one transition, we take a closer look now
			if(translist != NULL && translist->next != NULL)
			{

				//test if no place on the consume list is on the produce list of any of the transitions
				cont = 1;

				//we will unravel all the transitions and places now
				return_value++;

				//create a new transition
				transitions = new transition("transition_for_cross_over", transitions);
				transitions->add_bpel(EMPTY, transitions->name);

				//add all the places from the consume list to the new transitions consume list
				comparelist = transptr->consumes;
				while(comparelist != NULL)
				{
					transitions->add_consume(comparelist->name);
					comparelist = comparelist->next;
				}
				
				//create a new place
				places = new place("place_for_cross_over", places);
				
				//add new place to new transitions produce list
				transitions->add_produce(places->name);

				//delete consume lists from all transitions
				//and add new place to all transitions consume lists
				translistptr = translist;
				while(translistptr != NULL)
				{
					translistptr->transitionptr->del_consumes();
					translistptr->transitionptr->add_consume(places->name);
					translistptr = translistptr->next;
				}
			}

			//delete old translist
			while(translist != NULL)
			{
				translistptr = translist;
				translist = translist->next;
				delete(translistptr);
			}
		}
		
		transptr = transptr->next;
	}

#ifdef VERBOSE
	cout << "  " << return_value << " re-structuring possibility/ies found and re-structured.\n";
#endif
#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " re-structuring possibility/ies found and re-structured.\n";
	}
#endif
	return return_value;
}



int owfn::reoccrepl_exfc_B()
{
#ifdef VERBOSE
	cout << " looking for re-structuring possibilities B...\n";
#endif

	int return_value = 0;
	transition *transptr;
	transition *alltrans;
	place *con;
//	place *prod;
	place *comparelist;
	int cont;
	transitionlist *translist;
	transitionlist *translistptr;
	
	transptr = transitions;
	while(transptr != NULL)
	{
		//if transition produces on more than one place, it's a candidate
		if(transptr->produces != NULL && transptr->produces->next != NULL)
		{
			//find transitions with exactly the same produce list
			translist = NULL;
			alltrans = transitions;
			while(alltrans != NULL)
			{
				//check if the transition produce on all the same places
				cont = 1;	//as long as cont(inue) equals 1, no missmatch has been found
				comparelist = transptr->produces;
				while(cont && comparelist != NULL)
				{
					con = alltrans->produces;
					while(cont && con != NULL && con->name != comparelist->name)
					{
						con = con->next;
					}
					if(con == NULL)		//same place not found
					{
						cont = 0;
					}
					comparelist = comparelist->next;
				}
				//check if the transition produces on no other place
				con = alltrans->produces;
				while(cont && con != NULL)
				{
					comparelist = transptr->produces;
					while(cont && comparelist != NULL && con->name != comparelist->name)
					{
						comparelist = comparelist->next;
					}
					if(comparelist == NULL)		//same place not found
					{
						cont = 0;
					}
					con = con->next;
				}
				if(cont)
				{
					//add alltrans to list of transitions with equal consume lists
					translist = new transitionlist(alltrans, translist);
				}
				alltrans = alltrans->next;
			}

			//if we have found more than one transition, we take a closer look now
			if(translist != NULL && translist->next != NULL)
			{

				//we will unravel all the transitions and places now
				return_value++;

				//create a new transition
				transitions = new transition("transition_for_cross_over", transitions);
				transitions->add_bpel(EMPTY, transitions->name);
				
				//add all the places from the produce list to the new transitions produce list
				comparelist = transptr->produces;
				while(comparelist != NULL)
				{
					transitions->add_produce(comparelist->name);
					comparelist = comparelist->next;
				}
				
				//create a new place
				places = new place("place_for_cross_over", places);
				
				//add new place to new transitions consume list
				transitions->add_consume(places->name);

				//delete produce lists from all transitions
				//and add new place to all transitions produce lists
				translistptr = translist;
				while(translistptr != NULL)
				{
					translistptr->transitionptr->del_produces();
					translistptr->transitionptr->add_produce(places->name);
					translistptr = translistptr->next;
				}
			}

			//delete old translist
			while(translist != NULL)
			{
				translistptr = translist;
				translist = translist->next;
				delete(translistptr);
			}
		}
		
		transptr = transptr->next;
	}

#ifdef VERBOSE
	cout << "  " << return_value << " re-structuring possibility/ies found and re-structured.\n";
#endif
#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " re-structuring possibility/ies found and re-structured.\n";
	}
#endif
	return return_value;
}


void owfn::create_B(place *start)
{
	transition *alltrans;
	place *con;
	place *prod;
	place *placeptr;
	placelist *placelistptr;
	transitionlist *translistptr;
	int seen;

	alltrans = transitions;

	while(alltrans != NULL)
	{
		con = alltrans->consumes;
		while(con != NULL)
		{
			if(con->name == start->name)
			{
				//check if we already know this transition
				seen = 0;
				translistptr = trans_in_B;
				while(translistptr != NULL)
				{
					if(translistptr->transitionptr->name == alltrans->name)
					{
						seen = 1;
					}
					translistptr = translistptr->next;
				}
				if(!seen)
				{
					trans_in_B = new transitionlist(alltrans, trans_in_B);
					prod = alltrans->produces;
					while(prod != NULL)
					{
						//check if we produce on a place that we have already in our list
						seen = 0;
						placelistptr = places_in_B;
						while(placelistptr != NULL)
						{
							if(placelistptr->placeptr->name == prod->name)
							{
								seen = 1;
							}
							placelistptr = placelistptr->next;
						}
						
						if(!seen)	//if we already saw this place we don't add it again
						{
							placeptr = places;
							while(placeptr != NULL && placeptr->name != prod->name)
							{
								placeptr = placeptr->next;
							}
							if(placeptr == NULL)
							{
								cout << alltrans->name << " soll drauf prod." << endl;
								cout << "Error 1 in function 'create_B': Place '" << prod->name << "' vanished. This should not happen...\n";
								exit(EXIT_FAILURE);
							}
							places_in_B = new placelist(placeptr, places_in_B);
							create_B(placeptr);
						}
						prod = prod->next;
					}
				}
			}
			con = con->next;
		}
		alltrans = alltrans->next;
	}
}


int owfn::reoccrepl_copy_A()
{
#ifdef VERBOSE
	cout << " looking for places that get marked multiple times...\n";
#endif

	int return_value = 0;
	int i;
	int counter;
	int circle;
	int stop;
	int hit = 0;
	
	place *next_place_to_copy;
	transition *next_trans_to_copy;
	
	place *allplaces;		//to find X
	place *X;
	place *prod;
	place *con;
	place *prev;
	place *placeptr;
	transition *alltrans;
	transitionlist *producers;	//*X
	transitionlist *translistptr;
//	transitionlist *trans_in_B;	//globaly defined as part of the owfn
//	placelist *places_in_B;
	placelist *placelistptr;


	allplaces = places;
	while(allplaces != NULL && !hit)	//if we hit something we stop, this function is in conflict with others
	{
		counter = 0;
		alltrans = transitions;
		producers = NULL;
		while(alltrans != NULL)
		{
			prod = alltrans->produces;
			while(prod != NULL)
			{
				if(prod->name == allplaces->name)
				{
					counter++;
					producers = new transitionlist(alltrans, producers);
				}
				prod = prod->next;
			}
			alltrans = alltrans->next;
		}

		places_in_B = NULL;
		trans_in_B = NULL;

		if(counter > 1)
		{
			//allplaces is a candidate, more than one transition produces on it
			X = allplaces;
			//create lists of places and transitions in ((X*)*)*...
			create_B(X);
			
			//check if X was added to places_in_B
			circle = 0;
			placelistptr = places_in_B;
			while(placelistptr != NULL && !circle)
			{
				if(placelistptr->placeptr->name == X->name)
				{
					circle = 1;
				}
				placelistptr = placelistptr->next;
			}
				
			if(!circle)
			{
				if(places_in_B == NULL && trans_in_B == NULL)
				{
					//both lists are empty, there is nothing, that has to be checked
					//X is the only place to copy
#ifdef DEBUG
					cout << "   copying place " << X->name << endl;
#endif
					return_value++;
					hit = 1;
					
					//copy the last copy to create unique place and transition names (i.e. place_A_, place_A__, ...)
					translistptr = producers;
					next_place_to_copy = X;
					for(i = 1; i < counter; i++)			//create counter-1 copies
					{
						copy_place(next_place_to_copy, i);
						//next_place_to_copy = places;		//point to newly created place
						prod = translistptr->transitionptr->produces;
						while(prod != NULL && prod->name != X->name)
						{
							prod = prod->next;
						}
						if(prod == NULL)
						{
							cout << "Error 1 in function 'reoccrepl_copy': Place '" << X->name << "' vanished. This should not happen...\n";
							exit(EXIT_FAILURE);
						}
						//let the produce list point to the newly created copy
						prod->name = places->name;
						if(translistptr->next != NULL)
						{
							translistptr = translistptr->next;
						}else
						{
							cout << "Error 2 in function 'reoccrepl_copy': List of transitions is shorter than counted. This should not happen...\n";
							exit(EXIT_FAILURE);
						}
					}
					//rename X
					prod = translistptr->transitionptr->produces;
					while(prod != NULL && prod->name != X->name)
					{
						prod = prod->next;
					}
					if(prod == NULL)
					{
						cout << "Error 1 in function 'reoccrepl_copy': Place '" << X->name << "' vanished. This should not happen...\n";
						exit(EXIT_FAILURE);
					}
					prod->name = X->name + "_C0";
					X->name = X->name + "_C0";
				}else
				{
					//check if there are more arcs from A to B
					//change: save the ones that have to be copied
					//change: stop here, too!
					//stop at the other ones
					
					//1. check if no transition
					//that is not in trans_in_B
					//produces on a place in places_in_B
					alltrans = transitions;
					stop = 0;
					while(alltrans != NULL && !stop)
					{
						translistptr = trans_in_B;
						while(translistptr != NULL && translistptr->transitionptr != alltrans)
						{
							translistptr = translistptr->next;
						}
						if(translistptr == NULL)
						{
							prod = alltrans->produces;
							while(prod != NULL && !stop)
							{
								placelistptr = places_in_B;
								while(placelistptr != NULL && placelistptr->placeptr->name != prod->name)
								{
									placelistptr = placelistptr->next;
								}
								if(placelistptr != NULL)
								{
									//there is another place that has to be found first
									stop = 1;
								}
								prod = prod->next;
							}
						}
						alltrans = alltrans->next;
					}
					if(!stop)
					{
						//2. create a list of all places
						//that are not in places_in_B
						//and get consumed by transitions in trans_in_B

						translistptr = trans_in_B;
						while(translistptr != NULL)
						{
							con = translistptr->transitionptr->consumes;
							prev = NULL;
							while(!stop && con != NULL)
							{
								placelistptr = places_in_B;
								while(placelistptr != NULL && placelistptr->placeptr->name != con->name)
								{
									placelistptr = placelistptr->next;
								}
								if(placelistptr == NULL && con->name != X->name)	//arc from a place that is not in B
								{
									stop = 1;
								}else
								{
									prev = con;
									con = con->next;
								}
							}
							translistptr = translistptr->next;
						}
						
						if(!stop)
						{
							//now copy all of B
							//copy the last copy to create unique place and transition names (i.e. place_A_, place_A__, ...)
							//add X to the list of places that get copied
							
							//copy X
							//set arcs to X
#ifdef DEBUG
							cout << "   copying place " << X->name << " and following net" << endl;
#endif
							translistptr = producers;
							next_place_to_copy = X;
							for(i = 1; i < counter; i++)			//create counter-1 copies
							{
								copy_place(next_place_to_copy, i);
								//next_place_to_copy = places;		//point to newly created place
								prod = translistptr->transitionptr->produces;
								while(prod != NULL && prod->name != X->name)
								{
									prod = prod->next;
								}
								if(prod == NULL)
								{
									cout << "Error 3 in function 'reoccrepl_copy': Place '" << X->name << "' vanished. This should not happen...\n";
									exit(EXIT_FAILURE);
								}
								//let the produce list point to the newly created copy
								prod->name = places->name;
								if(translistptr->next != NULL)
								{
									translistptr = translistptr->next;
								}else
								{
									cout << "Error 4 in function 'reoccrepl_copy': List of transitions is shorter than counted. This should not happen...\n";
									exit(EXIT_FAILURE);
								}
							}
							//rename X
							prod = translistptr->transitionptr->produces;
							while(prod != NULL && prod->name != X->name)
							{
								prod = prod->next;
							}
							if(prod == NULL)
							{
								cout << "Error 5 in function 'reoccrepl_copy': Place '" << X->name << "' vanished. This should not happen...\n";
								exit(EXIT_FAILURE);
							}
							prod->name = X->name + "_C0";
							X->name = X->name + "_C0";
							
							//copy all other places
							placelistptr = places_in_B;
							while(placelistptr != NULL)
							{
								next_place_to_copy = placelistptr->placeptr;
								for(i = 1; i < counter; i++)			//create counter-1 copies
								{
									copy_place(next_place_to_copy, i);
									//next_place_to_copy = places;		//point to newly created place
								}
								//rename the original
								placeptr = places;
								while(placeptr != NULL && placeptr->name != placelistptr->placeptr->name)
								{
									placeptr = placeptr->next;
								}
								if(placeptr == NULL)
								{
									cout << "Error 6 in function 'reoccrepl_copy': Place '" << placelistptr->placeptr->name << "' vanished. This should not happen...\n";
									exit(EXIT_FAILURE);
								}else
								{
									placeptr->name = placeptr->name + "_C0";
								}
								placelistptr = placelistptr->next;
							}
	
							//copy all transitions
							translistptr = trans_in_B;
							while(translistptr != NULL)
							{
								next_trans_to_copy = translistptr->transitionptr;
								
								for(i = 1; i < counter; i++)			//create counter-1 copies
								{
									copy_transition(next_trans_to_copy, i);
									//next_trans_to_copy = transitions;	//point to newly created transition
								}
								
								//rename the original
								translistptr->transitionptr->name = translistptr->transitionptr->name + "_C0";
								//change consume and produce lists
								prod = translistptr->transitionptr->produces;
								while(prod != NULL)
								{
									prod->name = prod->name + "_C0";
									prod = prod->next;
								}
								con = translistptr->transitionptr->consumes;
								while(con != NULL)
								{
									con->name = con->name + "_C0";
									con = con->next;
								}
								
								translistptr = translistptr->next;
							}
	
							return_value++;
							hit = 1;
						}	//else Bogen von A nach B
					}	//else Platz weiter außen
				}
			}	//else Kreis
		}

		//delete old translist
		while(producers != NULL)
		{
			translistptr = producers;
			producers = producers->next;
			delete(translistptr);
		}
		//delete old translist
		while(trans_in_B != NULL)
		{
			translistptr = trans_in_B;
			trans_in_B = trans_in_B->next;
			delete(translistptr);
		}

		//delete old placelist
		while(places_in_B != NULL)
		{
			placelistptr = places_in_B;
			places_in_B = places_in_B->next;
			delete(placelistptr);
		}

		allplaces = allplaces->next;
	}


#ifdef VERBOSE
	cout << "  " << return_value << " place(s) found and copied.\n";
#endif

#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " place(s) found and copied.\n";
	}
#endif

	return return_value;
}

int owfn::reoccrepl_copy_B()
{
#ifdef VERBOSE
	cout << " looking for places that get marked multiple times...\n";
#endif

	int return_value = 0;
	int i;
	int counter;
	int circle;
	int stop;
	int hit = 0;
	
	place *next_place_to_copy;
	transition *next_trans_to_copy;
	
	place *allplaces;		//to find X
	place *X;
	place *prod;
	place *con;
	place *prev;
//	place *placeptr;
	transition *alltrans;
	transition *tmp;
	transitionlist *producers;	//*X
	transitionlist *translistptr;
//	transitionlist *trans_in_B;	//globaly defined as part of the owfn
//	placelist *places_in_B;
	placelist *placelistptr;
	transitionlist *end_of_arc;
	placelist *arc_into_B;
	transitionlist *tlp;
	placelist *plp;

	allplaces = places;
	while(allplaces != NULL && !hit)	//if we hit something we stop, this function is in conflict with others
	{
		counter = 0;
		alltrans = transitions;
		producers = NULL;
		while(alltrans != NULL)
		{
			prod = alltrans->produces;
			while(prod != NULL)
			{
				if(prod->name == allplaces->name)
				{
					counter++;
					producers = new transitionlist(alltrans, producers);
				}
				prod = prod->next;
			}
			alltrans = alltrans->next;
		}

		places_in_B = NULL;
		trans_in_B = NULL;
		end_of_arc = NULL;
		arc_into_B = NULL;

		if(counter > 1)
		{
			//allplaces is a candidate, more than one transition produces on it
			X = allplaces;
			//create lists of places and transitions in ((X*)*)*...
			create_B(X);
			
			//check if X was added to places_in_B
			circle = 0;
			placelistptr = places_in_B;
			while(placelistptr != NULL && !circle)
			{
				if(placelistptr->placeptr->name == X->name)
				{
					circle = 1;
				}
				placelistptr = placelistptr->next;
			}
				
			if(!circle)
			{
				if(places_in_B == NULL && trans_in_B == NULL)
				{
					//both lists are empty, there is nothing, that has to be checked
					//X is the only place to copy
#ifdef DEBUG
					cout << "   copying place " << X->name << endl;
#endif
					return_value++;
					hit = 1;
					
					//copy the last copy to create unique place and transition names (i.e. place_A_, place_A__, ...)
					translistptr = producers;
					next_place_to_copy = X;
					for(i = 1; i < counter; i++)			//create counter-1 copies
					{
						copy_place(next_place_to_copy, i);
						//next_place_to_copy = places;		//point to newly created place
						prod = translistptr->transitionptr->produces;
						while(prod != NULL && prod->name != X->name)
						{
							prod = prod->next;
						}
						if(prod == NULL)
						{
							cout << "Error 1 in function 'reoccrepl_copy': Place '" << X->name << "' vanished. This should not happen...\n";
							exit(EXIT_FAILURE);
						}
						//let the produce list point to the newly created copy
						prod->name = places->name;
						if(translistptr->next != NULL)
						{
							translistptr = translistptr->next;
						}else
						{
							cout << "Error 2 in function 'reoccrepl_copy': List of transitions is shorter than counted. This should not happen...\n";
							exit(EXIT_FAILURE);
						}
					}
					//rename X
					prod = translistptr->transitionptr->produces;
					while(prod != NULL && prod->name != X->name)
					{
						prod = prod->next;
					}
					if(prod == NULL)
					{
						cout << "Error 1 in function 'reoccrepl_copy': Place '" << X->name << "' vanished. This should not happen...\n";
						exit(EXIT_FAILURE);
					}
					prod->name = X->name + "_C0";
					X->name = X->name + "_C0";
				}else
				{
					//check if there are more arcs from A to B
					//save the ones that have to be copied
					//stop at the other ones
					
					//1. check if no transition
					//that is not in trans_in_B
					//produces on a place in places_in_B
					alltrans = transitions;
					stop = 0;
					while(alltrans != NULL && !stop)
					{
						translistptr = trans_in_B;
						while(translistptr != NULL && translistptr->transitionptr != alltrans)
						{
							translistptr = translistptr->next;
						}
						if(translistptr == NULL)
						{
							prod = alltrans->produces;
							while(prod != NULL && !stop)
							{
								placelistptr = places_in_B;
								while(placelistptr != NULL && placelistptr->placeptr->name != prod->name)
								{
									placelistptr = placelistptr->next;
								}
								if(placelistptr != NULL)
								{
									//there is another place that has to be found first
									stop = 1;
								}
								prod = prod->next;
							}
						}
						alltrans = alltrans->next;
					}
					if(!stop)
					{
						//2. create a list of all places
						//that are not in places_in_B
						//and get consumed by transitions in trans_in_B

						translistptr = trans_in_B;
						while(translistptr != NULL)
						{
							con = translistptr->transitionptr->consumes;
							prev = NULL;
							while(con != NULL)
							{
								placelistptr = places_in_B;
								while(placelistptr != NULL && placelistptr->placeptr->name != con->name)
								{
									placelistptr = placelistptr->next;
								}
								if(placelistptr == NULL && con->name != X->name)	//arc from a place that is not in B
								{
									end_of_arc = new transitionlist(translistptr->transitionptr, end_of_arc);
									//this is not the real place but a copy, we need only the name, so it is ok
									arc_into_B = new placelist(con, arc_into_B);
									//remove but not delete the place from the consume list now and add it later to all copies
								//this is stupid and resulted from copying the function
									if(prev == NULL)
									{
										translistptr->transitionptr->consumes = translistptr->transitionptr->consumes->next;
									}else
									{
										prev->next = con->next;
									}
									//reset the search
									prev = NULL;
									con = translistptr->transitionptr->consumes;
								}else
								{
									prev = con;
									con = con->next;
								}
							}
							translistptr = translistptr->next;
						}

						//change: now copy all of B
						//change: copy X and first transitions
						//copy the last copy to create unique place and transition names (i.e. place_A_, place_A__, ...)
						//add X to the list of places that get copied
						
						//copy X
						//set arcs to X
#ifdef DEBUG
						cout << "   copying place " << X->name << " and first transitions" << endl;
#endif
						translistptr = producers;
						next_place_to_copy = X;
						for(i = 1; i < counter; i++)			//create counter-1 copies
						{
							copy_place(next_place_to_copy, i);
							//next_place_to_copy = places;		//point to newly created place
							prod = translistptr->transitionptr->produces;
							while(prod != NULL && prod->name != X->name)
							{
								prod = prod->next;
							}
							if(prod == NULL)
							{
								cout << "Error 3 in function 'reoccrepl_copy': Place '" << X->name << "' vanished. This should not happen...\n";
								exit(EXIT_FAILURE);
							}
							//let the produce list point to the newly created copy
							prod->name = places->name;
							if(translistptr->next != NULL)
							{
								translistptr = translistptr->next;
							}else
							{
								cout << "Error 4 in function 'reoccrepl_copy': List of transitions is shorter than counted. This should not happen...\n";
								exit(EXIT_FAILURE);
							}
						}
						//rename X
						prod = translistptr->transitionptr->produces;
						while(prod != NULL && prod->name != X->name)
						{
							prod = prod->next;
						}
						if(prod == NULL)
						{
							cout << "Error 5 in function 'reoccrepl_copy': Place '" << X->name << "' vanished. This should not happen...\n";
							exit(EXIT_FAILURE);
						}
						prod->name = X->name + "_C0";
						X->name = X->name + "_C0";

						//copy all transitions
						translistptr = trans_in_B;
						while(translistptr != NULL)
						{
							next_trans_to_copy = translistptr->transitionptr;
							//only copy transitions that consume from X
							con = translistptr->transitionptr->consumes;
							while(con != NULL && con->name + "_C0" != X->name)
							{
								con = con->next;
							}
							if(con != NULL)
							{
								tlp = end_of_arc;
								plp = arc_into_B;
								while(tlp != NULL && tlp->transitionptr != next_trans_to_copy)
								{
									tlp = tlp->next;
									plp = plp->next;
								}
								if(tlp != NULL)		//transition is end of an arc from A
								{
									tmp = next_trans_to_copy;			//add place to consume list after this transition has been copied WITHOUT the place on the list
									for(i = 1; i < counter; i++)			//create counter-1 copies
									{
										copy_transition_once(next_trans_to_copy, i);
										transitions->add_consume(plp->placeptr->name);
										con = transitions->consumes;
										while(con != NULL)
										{
											if(con->name + "_C0" == X->name)
											{
												rename_copy(con, i);
											}
											con = con->next;
										}
									}
									//rename the original
									translistptr->transitionptr->name = translistptr->transitionptr->name + "_C0";
									//change consume and produce lists

									con = translistptr->transitionptr->consumes;
									while(con != NULL)
									{
										if(con->name + "_C0" == X->name)
										{
											con->name = con->name + "_C0";
										}
										con = con->next;
									}
									
									tmp->add_consume(plp->placeptr->name);
								}else
								{
									for(i = 1; i < counter; i++)			//create counter-1 copies
									{
										copy_transition_once(next_trans_to_copy, i);
										con = transitions->consumes;
										while(con != NULL)
										{
											if(con->name + "_C0" == X->name)
											{
												rename_copy(con, i);
											}
											con = con->next;
										}
									}
									
									//rename the original
									translistptr->transitionptr->name = translistptr->transitionptr->name + "_C0";
									//change consume and produce lists

									con = translistptr->transitionptr->consumes;
									while(con != NULL)
									{
										if(con->name + "_C0" == X->name)
										{
											con->name = con->name + "_C0";
										}
										con = con->next;
									}
								}
							}else
							{
								//restore consume list that might have been damaged
								//this is a bad way to do it, it shouldn't have been damaged at all
								tlp = end_of_arc;
								plp = arc_into_B;
								while(tlp != NULL && tlp->transitionptr != next_trans_to_copy)
								{
									tlp = tlp->next;
									plp = plp->next;
								}
								if(tlp != NULL)		//transition is end of an arc from A
								{
									next_trans_to_copy->add_consume(plp->placeptr->name);
								}
								//else nothing to repair
							}
							translistptr = translistptr->next;
						}

						return_value++;
						hit = 1;
					}	//else Platz weiter außen
				}
			}	//else Kreis
		}

		//delete old translist
		while(producers != NULL)
		{
			translistptr = producers;
			producers = producers->next;
			delete(translistptr);
		}
		//delete old translist
		while(trans_in_B != NULL)
		{
			translistptr = trans_in_B;
			trans_in_B = trans_in_B->next;
			delete(translistptr);
		}

		//delete old placelist
		while(places_in_B != NULL)
		{
			placelistptr = places_in_B;
			places_in_B = places_in_B->next;
			delete(placelistptr);
		}

		//delete old translist
		while(end_of_arc != NULL)
		{
			translistptr = end_of_arc;
			end_of_arc = end_of_arc->next;
			delete(translistptr);
		}

		//delete old placelist
		while(arc_into_B != NULL)
		{
			placelistptr = arc_into_B;
			arc_into_B = arc_into_B->next;
			delete(placelistptr);
		}

		allplaces = allplaces->next;
	}


#ifdef VERBOSE
	cout << "  " << return_value << " place(s) found and copied.\n";
#endif

#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " place(s) found and copied.\n";
	}
#endif
	return return_value;
}


void owfn::final_empty()
{
	transition *transptr;
	place *placeptr;
	
	transptr = transitions;
	while(transptr != NULL)
	{
		if(transptr->bpel_code == NULL)
		{
			transptr->add_bpel(EMPTY, transptr->name);
			transptr->add_bpel(OPAQUE, transptr->name);
			transptr->add_bpel(EMPTY, transptr->name);
		}else
		{
			transptr->add_bpel(EMPTY, transptr->name);
			transptr->add_last_bpel(EMPTY, transptr->name);
		}
		transptr = transptr->next;
	}

	placeptr = places;
	while(placeptr != NULL)
	{
		if(placeptr->bpel_code == NULL)
		{
			placeptr->add_bpel(EMPTY, placeptr->name);
		}else
		{
			placeptr->add_bpel(EMPTY, placeptr->name);
			placeptr->add_last_bpel(EMPTY, placeptr->name);
		}
		placeptr = placeptr->next;
	}
}


void owfn::final_links()
{
	transition *transptr;
	place *plist;
	place *placeptr;
	bpel *tmp;
	
	transptr = transitions;
	while(transptr != NULL)
	{
		plist = transptr->consumes;
		while(plist != NULL)
		{
			placeptr = places;
			while(placeptr != NULL && placeptr->name != plist->name)
			{
				placeptr = placeptr->next;
			}
			if(placeptr == NULL)
			{
				cout << "Error 1 in function 'final_links': Place '" << plist->name << "' vanished. This should not happen...\n";
				exit(EXIT_FAILURE);
			}

			//turn arc from place to transition into a link
			links::increment();
			placeptr->bpel_code->add_source();
			transptr->bpel_code->add_target(1);


			plist = plist->next;
		}
		
		plist = transptr->produces;
		while(plist != NULL)
		{
			placeptr = places;
			while(placeptr != NULL && placeptr->name != plist->name)
			{
				placeptr = placeptr->next;
			}
			if(placeptr == NULL)
			{
				cout << "Error 2 in function 'final_links': Place '" << plist->name << "' vanished. This should not happen...\n";
				exit(EXIT_FAILURE);
			}

			//falls der Platz bereits Ziel von Links ist, an dieser Stelle das OR Flag setzen
			if((tmp = placeptr->bpel_code->is_target()) != NULL)
			{
				tmp->link_is_or = 1;
			}

			//turn arc from transition to place into a link
			links::increment();
			placeptr->bpel_code->add_target(1);
			transptr->bpel_code->add_source();

			plist = plist->next;
		}
		
		transptr = transptr->next;
	}
}


void owfn::final_create_flow()
{
	transition *transptr;
	place *placeptr;
	branch *newbranch;
	bpel *list;
	bpel *help;
	
	//create new flow
	final_code = new bpel(FLOW, final_code, "top_flow");

	//add branch for every transition
	transptr = transitions;
	while(transptr != NULL)
	{
		final_code->add_branch();
		newbranch = final_code->branches;

		list = transptr->bpel_code;

		if(list != NULL)
		{
			help = list;
			while(list->next != NULL)
			{
				list = list->next;
			}
			list->next = newbranch->bpel_code;
			newbranch->bpel_code = help; 
		}	//else nothing to do

		transptr = transptr->next;
	}

	//add branch for every place
	placeptr = places;
	while(placeptr != NULL)
	{
		final_code->add_branch();
		newbranch = final_code->branches;

		list = placeptr->bpel_code;

		if(list != NULL)
		{
			help = list;
			while(list->next != NULL)
			{
				list = list->next;
			}
			list->next = newbranch->bpel_code;
			newbranch->bpel_code = help; 
		}	//else nothing to do

		placeptr = placeptr->next;
	}
}


void owfn::final_switches()
{
	place *placeptr;
	place *con;
	place *con_prev;
	transition *transptr;
	string placename;
	string newplace;
	int found;
	
	placeptr = places;
	while(placeptr != NULL)
	{
		found = 0;
		transptr = transitions;
		while(transptr != NULL)
		{
			con = transptr->consumes;
			while(con != NULL)
			{
				if(con->name == placeptr->name)
				{
					found++;	//there needs to be more than one transition
				}
				con = con->next;
			}

			transptr = transptr->next;
		}

		if(found > 1)
		{
			//add  switch
			placeptr->add_bpel(SWITCH, placeptr->name);

			//add branches for every transition
			transptr = transitions;
			while(transptr != NULL)
			{
				con = transptr->consumes;
				con_prev = NULL;
				while(con != NULL)
				{
					if(con->name == placeptr->name)
					{
						placeptr->bpel_code->add_branch();
						placeptr->bpel_code->branches->bpel_code = new bpel(EMPTY, NULL, "empty_after_switch");
						//create link arc
						links::increment();
						placeptr->bpel_code->branches->bpel_code->add_source();
						transptr->bpel_code->add_target(1);
						//remove old arc
						if(con_prev == NULL)
						{
							transptr->consumes = transptr->consumes->next;
						}else
						{
							con_prev->next = con->next;
						}
						delete(con);
						con = NULL;	//end of search, there can only be one arc
					}else
					{
						con_prev = con;
						con = con->next;
					}
				}
	
				transptr = transptr->next;
			}
		}
		placeptr = placeptr->next;
	}
}


void owfn::final_create_final(char *name)
{
	string help;
	bpel *final;
    // use "basename()" to strip directory prefix
	help = "process_" + string(basename(name));
    cerr << help << endl;
	final = new bpel(PROCESS, NULL, help);

	final->add_branch();
	
	if(final_code != NULL)	//flow was created
	{
		final->branches->bpel_code = final_code;	//flow becomes innerActivity of process
	}else
	{
		if(places != NULL)
		{
			final->branches->bpel_code = places->bpel_code;	//bpel_code of last remaining place
		}else
		{
			final->branches->bpel_code = transitions->bpel_code;	//bpel_code of last remaining transition
		}
	}
	final_code = final;
}


void owfn::finish_sequence(branch *start)
{
	bpel *sequence;
	bpel *bpelptr;

	if(start->bpel_code->next != NULL)		//two activities in a row
	{

		//new sequence, including all bpels as branches
		//but what links to it?

		sequence = new bpel(SEQUENCE, NULL, "sequence");

		bpelptr = start->bpel_code;
		while(bpelptr != NULL)
		{
			//add branch to sequence
			sequence->add_branch();
			sequence->branches->bpel_code = bpelptr;
			bpelptr = bpelptr->next;
			sequence->branches->bpel_code->next = NULL;
		}
		
		start->bpel_code = sequence;
	}

	if(start->bpel_code->branches != NULL)
	{
		finish_sequence(start->bpel_code->branches);
	}

	if(start->next != NULL)
	{
		finish_sequence(start->next);
	}
}


void owfn::finish_links(bpel *start)
{
	branch *help;
	branch *prev;
	branch *source_branch;
	branch *tmp;
	branch *tmp_b;
	branch *tmp_prev;
	bpel *target_bpel;
	int id;
	int cont;

	if(start->activity == FLOW)
	{
		//look at all first activities of the branches
		help = start->branches;
		while(help != NULL)
		{
			cont = 1;
			//test the "first" bpel code, pointer points to last
			target_bpel = help->bpel_code;
			while(target_bpel->next != NULL)	//find last
			{
				target_bpel = target_bpel->next;
			}

			if(target_bpel->target != NULL && target_bpel->target->next == NULL)	//activity is target of one link
			{
				id = target_bpel->target->link_id;
				//find source of link
				prev = NULL;
				source_branch = start->branches;
				while(source_branch != NULL && cont)
				{
					if(source_branch->bpel_code != NULL && source_branch->bpel_code->source != NULL && source_branch->bpel_code->source->next == NULL && source_branch->bpel_code->source->link_id == id) 	//source and target fullfill all requirements
					{
#ifdef DEBUG
	cout << "   removing link: " << id << " from " << source_branch->bpel_code->name << " to " << target_bpel->name << ".\n";
#endif
						//pair found
						//append bpel
						target_bpel->next = source_branch->bpel_code;
						//remove link
						target_bpel->target = NULL;
						source_branch->bpel_code->source = NULL;
						//remove empty branch
						if(prev == NULL)
						{
							start->branches = source_branch->next;
						}else
						{
							prev->next = source_branch->next;
						}
						delete(source_branch);
						source_branch = prev;
						
						cont = 0;
					}else
					{	//if the last activity of a branch is a switch, check it's branches
						if(source_branch->bpel_code != NULL && source_branch->bpel_code->activity == SWITCH) 	//source and target fullfill all requirements
						{
							tmp = source_branch->bpel_code->branches;
							while(tmp != NULL)
							{
								if(tmp->bpel_code != NULL && tmp->bpel_code->source != NULL && tmp->bpel_code->source->next == NULL && tmp->bpel_code->source->link_id == id) 	//source and target fullfill all requirements
								{
#ifdef DEBUG
	cout << "   removing link: " << id << " from " << tmp->bpel_code->name << " to " << target_bpel->name << ".\n";
#endif
									//pair found
									//remove link
									target_bpel->target = NULL;
									tmp->bpel_code->source = NULL;
									//append bpel
									target_bpel->next = tmp->bpel_code;
									tmp->bpel_code = help->bpel_code;	//new head of list

									//remove empty target branch! (source branch in if above)
									if(start->branches == help)
									{
										start->branches = start->branches->next;
									}else
									{
										tmp_prev = NULL;
										tmp_b = start->branches;
										while(tmp_b != NULL && tmp_b != help)
										{
											tmp_prev = tmp_b;
											tmp_b = tmp_b->next;
										}
										tmp_prev->next = help->next;
									}
									delete(help);
									
									cont = 0;
								}
								if(tmp != NULL)
									tmp = tmp->next;
							}
						}
					}
					prev = source_branch;
					if(source_branch != NULL)
						source_branch = source_branch->next;
				}
			}
			if(help != NULL)
				help = help->next;
			if(!cont)	//restart search until nothing changes
				help = start->branches;
		}
	}

	if(start->next != NULL)
	{
		finish_links(start->next);
	}

	help = start->branches;
	while(help != NULL)
	{
		finish_links(help->bpel_code);
		help = help->next;
	}
}


void owfn::finish_pick(bpel *start)
{
	branch *help;
	bpel *last_bpel;
	bpel *prev_bpel;
	int rec_found;

	if(start->activity == SWITCH)
	{
		//look at all first activities of the branches
		rec_found = 0;
		help = start->branches;
		while(help != NULL)
		{
			//test the "first" bpel code, pointer points to last
			last_bpel = help->bpel_code;
			prev_bpel = NULL;
			while(last_bpel->next != NULL)	//find last
			{
				prev_bpel = last_bpel;
				last_bpel = last_bpel->next;
			}

			if(last_bpel->activity == RECEIVE && last_bpel->target == NULL)		//must be receive and not target of a link
			{
				rec_found++;
				//one is enough to start converting
				//add name of receive to pick list
				help->pick_receive = last_bpel->name;
				//remove receive
				if(prev_bpel == NULL)
				{
					//empty branch
					last_bpel->activity = EMPTY;
				}else
				{
					if(last_bpel->source == NULL)
					{
						prev_bpel->next = NULL;
						delete(last_bpel);
					}else
					{
						last_bpel->activity = EMPTY;	//keep outgoing links at an empty activity
					}
				}
			}

			help = help->next;
		}
		
		if(rec_found > 0)
		{
			//turn switch into a pick
			start->activity = PICK;
#ifdef DEBUG
	cout << "   changing if: " << start->name << " to pick with " << rec_found << " onMessageEvents.\n";
#endif
		}
	}

	if(start->next != NULL)
	{
		finish_pick(start->next);
	}
	
	help = start->branches;
	while(help != NULL)
	{
		finish_pick(help->bpel_code);
		help = help->next;
	}
}


void owfn::finish_while(bpel *start)
{
	bpel *bpelptr;
	bpel *newif;
	bpel *newwhile;
	bpel *tmp;
	bpel *prev;
	branch *help;

	help = start->branches;

	while(help != NULL)
	{
		bpelptr = help->bpel_code;
		prev = NULL;
		while(bpelptr != NULL)
		{
			if(bpelptr->activity == WHILE && bpelptr->next != NULL && bpelptr->next->activity == WHILE && bpelptr->next->source == NULL && bpelptr->next->target == NULL && bpelptr->next->source == NULL && bpelptr->next->target == NULL)
			{
				//create new if
				newif = new bpel(SWITCH, NULL, "multiple_whiles_if");
				//add the content of all whiles as branches to the new if
				tmp = bpelptr;
				while(tmp != NULL && tmp->activity == WHILE && tmp->target == NULL && tmp->source == NULL)
				{
					newif->add_branch();
					newif->branches->bpel_code = tmp->branches->bpel_code;
					tmp = tmp->next;
				}
				//create new while
				newwhile = new bpel(WHILE, NULL, "multiple_whiles_while");
				newwhile->add_branch();
				newwhile->branches->bpel_code = newif;
				//insert new if
				newwhile->next = tmp;
				if(prev == NULL)
				{
					help->bpel_code = newwhile;
				}else
				{
					prev->next = newwhile;
				}
				
#ifdef DEBUG
				cout << "   merging while: " << bpelptr->name << ".\n";
#endif
				bpelptr = help->bpel_code;
				prev = NULL;
			}else
			{
				prev = bpelptr;
				bpelptr = bpelptr->next;
			}
		}
		help = help->next;
	}
	//call this function for every branch and the next activity
	if(start->next != NULL)
	{
		finish_while(start->next);
	}

	help = start->branches;
	while(help != NULL)
	{
		if(help->bpel_code != NULL)
		{
			finish_while(help->bpel_code);
		}else
		{
			cout << "Error 1 in function 'finish_even_more_empties': An empty branch has been created. This should not happen...\n";
			exit(EXIT_FAILURE);
		}
		help = help->next;
	}
}



void owfn::finish_if(bpel *start)
{
	branch *help;
	branch *last;
	branch *prev;

	if(start->activity == SWITCH)
	{
		//look at all first activities of the branches
		help = start->branches;
		prev = NULL;
		while(help != NULL)
		{
			if(help->bpel_code->activity == SWITCH && help->bpel_code->target == NULL && help->bpel_code->next == NULL)		//must be if and not target of a link
			{
#ifdef DEBUG
	cout << "   merging branches with if: " << start->name << endl;
#endif
				last = start->branches;
				while(last != NULL && last->next != NULL)
				{
					last = last->next;
				}
				//add branches of help if to start if
				last->next = help->bpel_code->branches;
				//remove branch with help if
				if(prev == NULL)
				{
					start->branches = start->branches->next;
				}else
				{
					prev->next = help->next;
				}

				delete(help->bpel_code);
				delete(help);

				help = start->branches;
				prev = NULL;
			}else
			{
				prev = help;
				help = help->next;
			}
		}
	}

	if(start->next != NULL)
	{
		finish_if(start->next);
	}
	
	help = start->branches;
	while(help != NULL)
	{
		finish_if(help->bpel_code);
		help = help->next;
	}
}


void owfn::finish_flow(bpel *start)
{
	branch *help;
	branch *newbranches;
	branch *prev;
	int cont;

	if(start->activity == FLOW)
	{
		//look at all first activities of the branches
		help = start->branches;
		prev = NULL;
		while(help != NULL)
		{
			cont = 1;
			//test if the only bpel code is a flow
			if(help->bpel_code->activity == FLOW && help->bpel_code->source == NULL && help->bpel_code->target == NULL && help->bpel_code->next == NULL)	//activity is flow, has to be merged
			{
#ifdef DEBUG
	cout << "   adding branches of sub-flow at: " << help->bpel_code->name << " with branches of flow at: " << start->name << ".\n";
#endif

				//append branches
				newbranches = start->branches;		//can't be NULL, because we are in one branch
				while(newbranches->next != NULL)
				{
					newbranches = newbranches->next;
				}
				newbranches->next = help->bpel_code->branches;
				//remove bpel code
				delete(help->bpel_code);
				//remove branch
				if(prev == NULL)
				{
					start->branches = help->next;
				}else
				{
					prev->next = help->next;
				}
				delete(help);
				help = NULL;
				cont = 0;
			}

			prev = help;
			if(help != NULL)
				help = help->next;
			if(!cont)	//restart search until nothing changes
			{
				help = start->branches;
			}
		}
	}

	if(start->next != NULL)
	{
		finish_flow(start->next);
	}
	
	help = start->branches;
	while(help != NULL)
	{
		finish_flow(help->bpel_code);
		help = help->next;
	}
}


void owfn::finish_empties(bpel *start)
{
	branch *help;
	bpel *bpel_c;
	bpel *prev;
	bpel *tmp;

	//remove empties from the top row of activities of all the own branches
	help = start->branches;

	while(help != NULL)
	{
		//looking at the start of bpel code at branch help
		bpel_c = help->bpel_code;
		if(bpel_c->next != NULL)	//don't remove branches with just one activity
		{
			prev = NULL;
			while(bpel_c != NULL)
			{
				//remove all empties that are not source or taget of a link
				if(bpel_c->activity == EMPTY && bpel_c->target == NULL && bpel_c->source == NULL)
				{
					if(prev == NULL)	//first element
					{
						if(bpel_c->next != NULL)	//only element! can't remove that!
						{
#ifdef DEBUG
	cout << "   removing empty: " << bpel_c->name << ".\n";
#endif
							tmp = help->bpel_code;
							help->bpel_code = help->bpel_code->next;
							delete(tmp);
							prev = NULL;
							bpel_c = help->bpel_code;
						}else
						{
							bpel_c = NULL;
						}
					}else
					{
#ifdef DEBUG
	cout << "   removing empty: " << bpel_c->name << ".\n";
#endif
						prev->next = bpel_c->next;
						delete(bpel_c);
						bpel_c = prev->next;
					}
				}else
				{
					prev = bpel_c;
					bpel_c = bpel_c->next;
				}
			}
		}
		help = help->next;
	}
	//call this function for every branch and the next activity
	if(start->next != NULL)
	{
		finish_empties(start->next);
	}

	help = start->branches;
	while(help != NULL)
	{
		if(help->bpel_code != NULL)
		{
			finish_empties(help->bpel_code);
		}else
		{
			cout << "Error 1 in function 'finish_empties': An empty branch has been created. This should not happen...\n";
			exit(EXIT_FAILURE);
		}
		help = help->next;
	}
}

void owfn::finish_more_empties(bpel *start)
{
	bpel *bpelptr;
	bpel *prev;
	branch *help;
	links *tmp;

	help = start->branches;

	while(help != NULL)
	{
		bpelptr = help->bpel_code;
		prev = NULL;
		while(bpelptr != NULL)
		{
			if(bpelptr->next != NULL && bpelptr->activity == EMPTY && bpelptr->target == NULL)
			{
				//append source links
				if(bpelptr->next->source == NULL)
				{
					bpelptr->next->source = bpelptr->source;
				}else
				{
					tmp = bpelptr->next->source;
					while(tmp->next != NULL)
					{
						tmp = tmp->next;
					}
					tmp->next = bpelptr->source;
				}
				//delete empty
#ifdef DEBUG
				cout << "   removing empty: " << bpelptr->name << ".\n";
#endif
				if(prev != NULL)
				{
					prev->next = bpelptr->next;
				}else
				{
					help->bpel_code = bpelptr->next;
				}
				delete(bpelptr);
				bpelptr = help->bpel_code;
				prev = NULL;
			}else
			{
				prev = bpelptr;
				bpelptr = bpelptr->next;
			}
		}

		help = help->next;
	}
	//call this function for every branch and the next activity
	if(start->next != NULL)
	{
		finish_more_empties(start->next);
	}

	help = start->branches;
	while(help != NULL)
	{
		if(help->bpel_code != NULL)
		{
			finish_more_empties(help->bpel_code);
		}else
		{
			cout << "Error 1 in function 'finish_more_empties': An empty branch has been created. This should not happen...\n";
			exit(EXIT_FAILURE);
		}
		help = help->next;
	}
}


void owfn::finish_even_more_empties(bpel *start)
{
	bpel *bpelptr;
	bpel *del;
	branch *help;

	help = start->branches;

	while(help != NULL)
	{
		bpelptr = help->bpel_code;
		while(bpelptr != NULL)
		{
			if(bpelptr->next != NULL && bpelptr->next->activity == EMPTY && bpelptr->next->source == NULL && bpelptr->target == NULL)
			{
				//append target links
				if(bpelptr->next->target != NULL)
				{
					bpelptr->target = bpelptr->next->target;
					//Aktivierungsbedingung auf die Aktivität übertragen
					if(bpelptr->next->link_is_or == 1)
					{
						bpelptr->link_is_or = 1;
					}
				}
				//delete empty
#ifdef DEBUG
				cout << "   removing empty: " << bpelptr->next->name << ".\n";
#endif
				del = bpelptr->next;
				bpelptr->next = bpelptr->next->next;
				delete(del);
			}
			bpelptr = bpelptr->next;
		}
		help = help->next;
	}
	//call this function for every branch and the next activity
	if(start->next != NULL)
	{
		finish_even_more_empties(start->next);
	}

	help = start->branches;
	while(help != NULL)
	{
		if(help->bpel_code != NULL)
		{
			finish_even_more_empties(help->bpel_code);
		}else
		{
			cout << "Error 1 in function 'finish_even_more_empties': An empty branch has been created. This should not happen...\n";
			exit(EXIT_FAILURE);
		}
		help = help->next;
	}
}




void owfn::finish_opaques(bpel *start)
{
	branch *help;
	bpel *bpel_c;
	bpel *prev;
	bpel *tmp;

	//remove opaques from the top row of activities of all the own branches
	help = start->branches;

	while(help != NULL)
	{
		//looking at the start of bpel code at branch help
		bpel_c = help->bpel_code;
		if(bpel_c->next != NULL)	//don't remove branches with just one activity
		{
			prev = NULL;
			while(bpel_c != NULL)
			{
				//remove all opaques that are not source or taget of a link
				if(bpel_c->activity == OPAQUE && bpel_c->target == NULL && bpel_c->source == NULL)
				{
					if(prev == NULL)	//first element
					{
						if(bpel_c->next != NULL)	//only element! can't remove that!
						{
#ifdef DEBUG
	cout << "   removing opaqueActivity: " << bpel_c->name << ".\n";
#endif
							tmp = help->bpel_code;
							help->bpel_code = help->bpel_code->next;
							delete(tmp);
							prev = NULL;
							bpel_c = help->bpel_code;
						}else
						{
							bpel_c = NULL;
						}
					}else
					{
#ifdef DEBUG
	cout << "   removing opaqueActivity: " << bpel_c->name << ".\n";
#endif
						prev->next = bpel_c->next;
						delete(bpel_c);
						bpel_c = prev->next;
					}
				}else
				{
					prev = bpel_c;
					bpel_c = bpel_c->next;
				}
			}
		}
		help = help->next;
	}
	//call this function for every branch and the next activity
	if(start->next != NULL)
	{
		finish_opaques(start->next);
	}

	help = start->branches;
	while(help != NULL)
	{
		if(help->bpel_code != NULL)
		{
			finish_opaques(help->bpel_code);
		}else
		{
			cout << "Error 1 in function 'finish_opaques': An empty branch has been created. This should not happen...\n";
			exit(EXIT_FAILURE);
		}
		help = help->next;
	}
}

void owfn::finish_more_opaques(bpel *start)
{
	bpel *bpelptr;
	bpel *prev;
	branch *help;
	links *tmp;

	help = start->branches;

	while(help != NULL)
	{
		bpelptr = help->bpel_code;
		prev = NULL;
		while(bpelptr != NULL)
		{
			if(bpelptr->next != NULL && bpelptr->activity == OPAQUE && bpelptr->target == NULL)
			{
				//append source links
				if(bpelptr->next->source == NULL)
				{
					bpelptr->next->source = bpelptr->source;
				}else
				{
					tmp = bpelptr->next->source;
					while(tmp->next != NULL)
					{
						tmp = tmp->next;
					}
					tmp->next = bpelptr->source;
				}
				//delete opaque
#ifdef DEBUG
				cout << "   removing opaqueActivity: " << bpelptr->name << ".\n";
#endif
				if(prev != NULL)
				{
					prev->next = bpelptr->next;
				}else
				{
					help->bpel_code = bpelptr->next;
				}
				delete(bpelptr);
				bpelptr = help->bpel_code;
				prev = NULL;
			}else
			{
				prev = bpelptr;
				bpelptr = bpelptr->next;
			}
		}

		help = help->next;
	}
	//call this function for every branch and the next activity
	if(start->next != NULL)
	{
		finish_more_opaques(start->next);
	}

	help = start->branches;
	while(help != NULL)
	{
		if(help->bpel_code != NULL)
		{
			finish_more_opaques(help->bpel_code);
		}else
		{
			cout << "Error 1 in function 'finish_more_opaques': An empty branch has been created. This should not happen...\n";
			exit(EXIT_FAILURE);
		}
		help = help->next;
	}
}


void owfn::finish_even_more_opaques(bpel *start)
{
	bpel *bpelptr;
	bpel *del;
	branch *help;

	help = start->branches;

	while(help != NULL)
	{
		bpelptr = help->bpel_code;
		while(bpelptr != NULL)
		{
			if(bpelptr->next != NULL && bpelptr->next->activity == OPAQUE && bpelptr->next->source == NULL && bpelptr->target == NULL)
			{
				//append target links
				if(bpelptr->next->target != NULL)
				{
					bpelptr->target = bpelptr->next->target;
					//Aktivierungsbedingung auf die Aktivität übertragen
					if(bpelptr->next->link_is_or == 1)
					{
						bpelptr->link_is_or = 1;
					}
				}
				//delete opaque
#ifdef DEBUG
				cout << "   removing opaqueActivity: " << bpelptr->next->name << ".\n";
#endif
				del = bpelptr->next;
				bpelptr->next = bpelptr->next->next;
				delete(del);
			}
			bpelptr = bpelptr->next;
		}
		help = help->next;
	}
	//call this function for every branch and the next activity
	if(start->next != NULL)
	{
		finish_even_more_opaques(start->next);
	}

	help = start->branches;
	while(help != NULL)
	{
		if(help->bpel_code != NULL)
		{
			finish_even_more_opaques(help->bpel_code);
		}else
		{
			cout << "Error 1 in function 'finish_even_more_opaques': An empty branch has been created. This should not happen...\n";
			exit(EXIT_FAILURE);
		}
		help = help->next;
	}
}




int owfn::reoccrepl_circlemultexits()
{
#ifdef VERBOSE
	cout << " looking for circles with multiple exits...\n";
#endif
	place *start;
	place *checkplace;
	place *tmpplace;
	place *placeptr;
	place *realplace;
	place *prevplace;
	place *con;
	transitionlist *translistptr;
	transitionlist *list_of_trans;
	transitionlist *trans_of_circle;
	transitionlist *delptr;
	transitionlist *tmplist;
	transitionlist *tlptr;
	transition *firsttrans;
	transition *transptr;
	transition *deltrans;
	transition *prev;
	int test1;
	int test2;
	int return_value = 0;
	bpel *help;
	bpel *tmpptr;
	bpel *kopie;
	bpel *new_bpel;

	//test every place as a possible start of a circle
	start = places;
	while(start != NULL)
	{
		//check if the start place has in going and out going arcs
		transptr = transitions;
		test1 = test2 = 0;
		list_of_trans = NULL;

		while(transptr != NULL)
		{
			if(test1 == 0)
			{
				tmpplace = transptr->consumes;
				while(tmpplace != NULL)
				{
					if(tmpplace->name == start->name)
					{
						test1 = 1;
					}
					tmpplace = tmpplace->next;
				}
			}
	
			tmpplace = transptr->produces;
			while(tmpplace != NULL)
			{
				if(tmpplace->name == start->name)
				{
					list_of_trans = new transitionlist(transptr, list_of_trans);
					test2 = 1;
				}
				tmpplace = tmpplace->next;
			}

			transptr = transptr->next;
		}

		if(test1 == 1 && test2 == 1)
		{
			//list_of_trans = Transitionen im Vorbereich
			//look at all the transitions that produce on the possible start place
			translistptr = list_of_trans;
			while(translistptr != NULL && start != NULL)
			{
				trans_of_circle = NULL;
				firsttrans = translistptr->transitionptr;
				//remove trans that consume from more than one place or produce on more than one place
				//firsttrans hat nur einen Platz im Vorbereich und einen Platz im Nachbereich
				if(firsttrans->consumes != NULL && firsttrans->consumes->next == NULL && firsttrans->produces != NULL && firsttrans->produces->next == NULL)
				{
					//remove trans where other trans consume from the same places
					if(firsttrans->consumes->name != start->name)
					{
						//walk through the entire circle and save all trans
						checkplace = firsttrans->consumes;
						while(checkplace != NULL && checkplace->name != start->name)
						{
							transptr = transitions;
							test1 = 0;
							while(transptr != NULL)
							{
								tmpplace = transptr->produces;
								while(tmpplace != NULL)
								{
									if(tmpplace->name == checkplace->name)
									{
										test1++;
									}
									tmpplace = tmpplace->next;
								}
								transptr = transptr->next;
							}

							if(test1 == 1)		//checkplace hat genau eine Transition im Vorbereich
							{
								//find that one place again and test it or find no place at all and abort
								test2 = 0;
								transptr = transitions;
								while(transptr != NULL && test2 == 0)
								{
									//checks only first entries in the produce list, if the place comes later it is not found but also not wanted
									if(transptr->produces != NULL && transptr->produces->name == checkplace->name)
									{
										test2 = 1;
									}else
									{
										transptr = transptr->next;
									}
								}

								if(transptr != NULL && transptr->consumes != NULL && transptr->consumes->next == NULL && transptr->produces != NULL && transptr->produces->next == NULL)
								{
									//add transition to circle and set checkplace to the next place to check
									trans_of_circle = new transitionlist(transptr, trans_of_circle);
									checkplace = transptr->consumes;
								}else
								{
									checkplace = NULL;
								}
							}else
							{
								checkplace = NULL;
							}
						}

						if(checkplace != NULL)
						{
#ifdef DEBUG
							cout << "   Circle from " << start->name << " to " << firsttrans->name << "\n";
#endif

							//restructure the circle
							//add while
							//don't copy a while as the last activity at a place
							//copy place bpel code
							help = start->bpel_code;
							while(help != NULL && help->activity == WHILE)
							{
								help = help->next;
							}
							tmpptr = copy_bpel(help);
							copy_links(tmpptr);

							//add while
							start->add_bpel(WHILE, start->name);
							start->bpel_code->add_branch();
//Rückwärts anbringen des BPEL Codes
							//add place bpel code
							start->bpel_code->branches->bpel_code = tmpptr;
							//add transition bpel code of the last transition of the circle
							if(firsttrans->bpel_code == NULL)
							{
								firsttrans->add_bpel(OPAQUE, firsttrans->name);
							}

							kopie = copy_bpel(firsttrans->bpel_code);
							copy_links(kopie);
							tmpptr = start->bpel_code->branches->bpel_code;
							if(tmpptr != NULL)
							{
								while(tmpptr->next != NULL)
								{
									tmpptr = tmpptr->next;
								}
							 	tmpptr->next = kopie;
							}else
							{
								start->bpel_code->branches->bpel_code = kopie;
							}

							//add place bpel code of the last place of the circle
							placeptr = return_placeptr(firsttrans->consumes->name);

							if(placeptr == NULL)
							{
								cout << "Error 1 in function 'reoccrepl_circlemultexits': Place '" << firsttrans->consumes->name << "' vanished. This should not happen...\n";
								exit(EXIT_FAILURE);
							}

							if(placeptr->bpel_code != NULL)
							{
								kopie = copy_bpel(placeptr->bpel_code);
								copy_links(kopie);
								tmpptr = start->bpel_code->branches->bpel_code;
								if(tmpptr != NULL)
								{
									while(tmpptr->next != NULL)
									{
										tmpptr = tmpptr->next;
									}
								 	tmpptr->next = kopie;
								}else
								{
									start->bpel_code->branches->bpel_code = kopie;
								}
							}


							//copy bpel code of the remaining circle
							tmplist = trans_of_circle;
							new_bpel = NULL;
							while(tmplist != NULL)
							{
								//first the transition
								if(tmplist->transitionptr->bpel_code == NULL)
								{
									tmplist->transitionptr->add_bpel(OPAQUE, tmplist->transitionptr->name);
								}
								kopie = copy_bpel(tmplist->transitionptr->bpel_code);
								copy_links(kopie);
								tmpptr = kopie;

								while(tmpptr->next != NULL)
								{
									tmpptr = tmpptr->next;
								}
								tmpptr->next = new_bpel;
								new_bpel = kopie;
								//next the place
								if(tmplist->next != NULL)
								{
									placeptr = return_placeptr(tmplist->next->transitionptr->consumes->name);
									if(placeptr == NULL)
									{
										cout << "Error 2 in function 'reoccrepl_circlemultexits': Place '" << tmplist->next->transitionptr->consumes->name << "' vanished. This should not happen...\n";
										exit(EXIT_FAILURE);
									}
									if(placeptr->bpel_code != NULL)
									{
										kopie = copy_bpel(placeptr->bpel_code);
										copy_links(kopie);
										tmpptr = kopie;

										while(tmpptr->next != NULL)
										{
											tmpptr = tmpptr->next;
										}
										tmpptr->next = new_bpel;
										new_bpel = kopie;
									}
								}
								
								tmplist = tmplist->next;
							}

							//compose bpel codes
							tmpptr = start->bpel_code->branches->bpel_code;
							if(tmpptr != NULL)
							{
								while(tmpptr->next != NULL)
								{
									tmpptr = tmpptr->next;
								}
							 	tmpptr->next = new_bpel;
							}else
							{
								start->bpel_code->branches->bpel_code = new_bpel;
							}
//BPEL Code des Startplatzes ist fertig

//alle Transitionen im Nachbereich der Ausgänge mit BPEL Code Kopien beschriften
//Und gleichzeitig Vorbereich der Transitionen im Nachbereich der Ausgänge = startplatz
//Startplatz als Ausgang wird ignoriert, da ist kein zusätzlicher BPEL Code nötig

//Vorletzten Platz einzeln betrachten
							realplace = return_placeptr(firsttrans->consumes->name);
							if(realplace == NULL)
							{
								cout << "Error 2 in function 'reoccrepl_circlemultexits': Place '" << tmplist->next->transitionptr->consumes->name << "' vanished. This should not happen...\n";
								exit(EXIT_FAILURE);
							}
							transptr = transitions;
							while(transptr != NULL)
							{
								if(transptr->name != firsttrans->name)
								{
									test1 = 0;
									con = transptr->consumes;
									while(con != NULL && test1 == 0)
									{
										if(con->name == realplace->name)
										{
											test1 = 1;
											//Transition transptr befindet sich im Nachbereich eines Ausgangsplatzes
											//Platz aus dem Zyklus aus dem Vorbereich der Transition durch Startplatz ersetzen
											con->name = start->name;
											
											//BPEL Code des Zyklus bis zu dieser Transition vor den vorhandenen BPEL Code der Transition kopieren
											
											if(transptr->bpel_code == NULL)
											{
												transptr->add_bpel(OPAQUE, transptr->name);
											}
											
											//Zuerst der vorletzte Platz des Zyklus											
											if(realplace->bpel_code != NULL)
											{
												kopie = copy_bpel(realplace->bpel_code);
												copy_links(kopie);
												tmpptr = transptr->bpel_code;
												if(tmpptr != NULL)
												{
													while(tmpptr->next != NULL)
													{
														tmpptr = tmpptr->next;
													}
												 	tmpptr->next = kopie;
												}else
												{
													transptr->bpel_code = kopie;
												}
											}
											
											//Dann alle anderen Transitionen und Plätze
											tmplist = trans_of_circle;
											new_bpel = NULL;
											while(tmplist != NULL)
											{
												//first the transition
												if(tmplist->transitionptr->bpel_code == NULL)
												{
													tmplist->transitionptr->add_bpel(OPAQUE, tmplist->transitionptr->name);
												}
												kopie = copy_bpel(tmplist->transitionptr->bpel_code);
												copy_links(kopie);
												tmpptr = kopie;
												while(tmpptr->next != NULL)
												{
													tmpptr = tmpptr->next;
												}
												tmpptr->next = new_bpel;
												new_bpel = kopie;
												//next the place
												if(tmplist->next != NULL)
												{
													placeptr = return_placeptr(tmplist->next->transitionptr->consumes->name);
													if(placeptr->bpel_code != NULL)
													{
														kopie = copy_bpel(placeptr->bpel_code);
														copy_links(kopie);
														tmpptr = kopie;

														while(tmpptr->next != NULL)
														{
															tmpptr = tmpptr->next;
														}
														tmpptr->next = new_bpel;
														new_bpel = kopie;
													}
												}
												
												tmplist = tmplist->next;
											}
											tmpptr = transptr->bpel_code;
											if(tmpptr != NULL)
											{
												while(tmpptr->next != NULL)
												{
													tmpptr = tmpptr->next;
												}
											 	tmpptr->next = new_bpel;
											}else
											{
												transptr->bpel_code = new_bpel;
											}
										}
										con = con->next;
									}
								}
								transptr = transptr->next;
							}
							
							
//alle anderen Plätze bis zum zweiten

							tmplist = trans_of_circle;
							while(tmplist != NULL)
							{
								if(tmplist->next != NULL)
								{
									realplace = return_placeptr(tmplist->next->transitionptr->consumes->name);
									if(realplace == NULL)
									{
										cout << "Error 3 in function 'reoccrepl_circlemultexits': Place '" << tmplist->next->transitionptr->consumes->name << "' vanished. This should not happen...\n";
										exit(EXIT_FAILURE);
									}
									transptr = transitions;
									while(transptr != NULL)
									{
										if(transptr->name != firsttrans->name)
										{
											test1 = 0;
											con = transptr->consumes;
											while(con != NULL && test1 == 0)
											{
												if(con->name == realplace->name)
												{
													test1 = 1;
													//Transition transptr befindet sich im Nachbereich eines Ausgangsplatzes
													//Platz aus dem Zyklus aus dem Vorbereich der Transition durch Startplatz ersetzen
													con->name = start->name;
													
													//BPEL Code des Zyklus bis zu dieser Transition vor den vorhandenen BPEL Code der Transition kopieren
													
													if(transptr->bpel_code == NULL)
													{
														transptr->add_bpel(OPAQUE, transptr->name);
													}

													//Zuerst den aktuellen Platz
													if(realplace->bpel_code != NULL)
													{
														kopie = copy_bpel(realplace->bpel_code);
														copy_links(kopie);
														tmpptr = transptr->bpel_code;
														if(tmpptr != NULL)
														{
															while(tmpptr->next != NULL)
															{
																tmpptr = tmpptr->next;
															}
														 	tmpptr->next = kopie;
														}else
														{
															transptr->bpel_code = kopie;
														}
													}

													//Alle Transitionen und Plätze ab dem aktuellen Platz
													tlptr = trans_of_circle;
													new_bpel = NULL;
													test2 = 0;
													while(tlptr != NULL)
													{
														if(test2 == 0 && tlptr->next != NULL && tlptr->transitionptr->produces->name == realplace->name)
														{
															test2 = 1;	//ab hier gehts los
														}
														if(test2 == 1)
														{
															//first the transition
															if(tlptr->transitionptr->bpel_code == NULL)
															{
																tlptr->transitionptr->add_bpel(OPAQUE, tlptr->transitionptr->name);
															}
															kopie = copy_bpel(tlptr->transitionptr->bpel_code);
															copy_links(kopie);
															tmpptr = kopie;
															while(tmpptr->next != NULL)
															{
																tmpptr = tmpptr->next;
															}
															tmpptr->next = new_bpel;
															new_bpel = kopie;
															//next the place
															if(tlptr->next != NULL)
															{
																placeptr = return_placeptr(tlptr->next->transitionptr->consumes->name);
																if(placeptr == NULL)
																{
																	cout << "Error 4 in function 'reoccrepl_circlemultexits': Place '" << tlptr->next->transitionptr->consumes->name << "' vanished. This should not happen...\n";
																	exit(EXIT_FAILURE);
																}
																if(placeptr->bpel_code != NULL)
																{
																	kopie = copy_bpel(placeptr->bpel_code);
																	copy_links(kopie);
																	tmpptr = kopie;
			
																	while(tmpptr->next != NULL)
																	{
																		tmpptr = tmpptr->next;
																	}
																	tmpptr->next = new_bpel;
																	new_bpel = kopie;
																}
															}
														}
														
														tlptr = tlptr->next;
													}
													tmpptr = transptr->bpel_code;
													if(tmpptr != NULL)
													{
														while(tmpptr->next != NULL)
														{
															tmpptr = tmpptr->next;
														}
													 	tmpptr->next = new_bpel;
													}else
													{
														transptr->bpel_code = new_bpel;
													}
												}
												con = con->next;
											}
										}
										transptr = transptr->next;
									}


								}
								
								tmplist = tmplist->next;
							}



//Plätze und Transitionen des Zyklus löschen
							//delete firsttrans
							deltrans = transitions;
							prev = NULL;
							while(deltrans != firsttrans)
							{
								prev = deltrans;
								deltrans = deltrans->next;
							}
							if(deltrans == NULL)
							{
								cout << "Error 2 in function 'reoccrepl_circlemultexits': Transition '" << firsttrans->name << "' vanished. This should not happen...\n";
								exit(EXIT_FAILURE);
							}
							if(prev == NULL)
							{
								transitions = transitions->next;
							}else
							{
								prev->next = deltrans->next;
							}
							firsttrans->del_lists();
							delete(firsttrans);

							tmplist = trans_of_circle;
							while(tmplist != NULL)
							{
								//erst den Platz im Nachbereich löschen
								placeptr = places;
								prevplace = NULL;
								while(placeptr != NULL && placeptr->name != tmplist->transitionptr->produces->name)
								{
									prevplace = placeptr;
									placeptr = placeptr->next;
								}
								if(placeptr == NULL)
								{
									cout << "Error 3 in function 'reoccrepl_circlemultexits': Place '" << tmplist->transitionptr->produces->name << "' vanished. This should not happen...\n";
									exit(EXIT_FAILURE);
								}
								if(prevplace == NULL)
								{
									places = places->next;
								}else
								{
									prevplace->next = placeptr->next;
								}
//An dieser Stelle kann noch der BPEL Code am Platz gelöscht werden
								delete(placeptr);
								
								//dann die Transition löschen
								deltrans = transitions;
								prev = NULL;
								while(deltrans != tmplist->transitionptr)
								{
									prev = deltrans;
									deltrans = deltrans->next;
								}
								if(deltrans == NULL)
								{
									cout << "Error 4 in function 'reoccrepl_circlemultexits': Transition '" << tmplist->transitionptr->name << "' vanished. This should not happen...\n";
									exit(EXIT_FAILURE);
								}
								if(prev == NULL)
								{
									transitions = transitions->next;
								}else
								{
									prev->next = deltrans->next;
								}
								tmplist->transitionptr->del_lists();
								delete(tmplist->transitionptr);
								
								tmplist = tmplist->next;
							}
							
							
							//restart
							start = NULL;
							return_value++;
						}

					}else
					{
						//kreis gefunden
						//aber einfache Doppelkante, sollte nach Doppelkantenregel gar nicht passieren
						cout << "Warning: Small circle found that should not have been found!" << endl;
					}
				}

				//delete trans_of_circle
				while(trans_of_circle != NULL)
				{
					delptr = trans_of_circle;
					trans_of_circle = trans_of_circle->next;
					delete(delptr);
				}

				translistptr = translistptr->next;
			}
		}

		//delete list_of_trans
		while(list_of_trans != NULL)
		{
			delptr = list_of_trans;
			list_of_trans = list_of_trans->next;
			delete(delptr);
		}

		if(start == NULL)
		{
			start = places;
		}else
		{
			start = start->next;
		}
	}


#ifdef VERBOSE
	cout << "  " << return_value << " circle(s) found and reduced.\n";
#endif
#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " circle(s) found and reduced.\n";
	}
#endif
	return return_value;
}





int owfn::find_circles(string startplatzname, string aktuellerplatz)
{
	transition *transptr;
	transitionlist *translistptr;
	transitionlist *delptr;
	transitionlist *checklistptr;
	place *placeptr;


	//translistptr = Liste mit allen Transitionen im Nachbereich von aktuellerplatz, die nur einen Platz im Nachbereich haben
	translistptr = NULL;
	transptr = transitions;
	while(transptr != NULL)
	{
		if(transptr->consumes != NULL && transptr->consumes->next == NULL && transptr->consumes->name == aktuellerplatz && transptr->produces != NULL && transptr->produces->next == NULL)
		{
			translistptr = new transitionlist(transptr, translistptr);
		}
		transptr = transptr->next;
	}

	while(translistptr)
	{
		if(translistptr->transitionptr->produces->name == startplatzname)
		{
			trans_in_B = new transitionlist(translistptr->transitionptr, trans_in_B);
			placeptr = return_placeptr(aktuellerplatz);
			if(placeptr == NULL)
			{
				cout << "Error 1 in function 'find_circles': Place '" << aktuellerplatz << "' vanished. This should not happen...\n";
				exit(EXIT_FAILURE);
			}
			places_in_B = new placelist(placeptr, places_in_B);


			//delete old translist
			while(translistptr != NULL)
			{
				delptr = translistptr;
				translistptr = translistptr->next;
				delete(delptr);
			}
			return 1;
		}else
		{
			//Wenn translistptr->transitionptr->produces->name noch nicht gesehen wurde, dann anschauen
			checklistptr = checked_trans;
			while(checklistptr != NULL && checklistptr->transitionptr != translistptr->transitionptr)
			{
				checklistptr = checklistptr->next;
			}
			if(checklistptr == NULL)
			{
				checked_trans = new transitionlist(translistptr->transitionptr, checked_trans);
				
				//if(Kreisende gefunden)
				if(find_circles(startplatzname, translistptr->transitionptr->produces->name) == 1)
				{
					//Ein Kreisende wird immer weiter hochgereicht und alle Plätze und Transitionen auf dem Rückweg gespeichert
					trans_in_B = new transitionlist(translistptr->transitionptr, trans_in_B);
					placeptr = return_placeptr(aktuellerplatz);
					if(placeptr == NULL)
					{
						cout << "Error 2 in function 'find_circles': Place '" << aktuellerplatz << "' vanished. This should not happen...\n";
						exit(EXIT_FAILURE);
					}
					places_in_B = new placelist(placeptr, places_in_B);

					//delete old translist
					while(translistptr != NULL)
					{
						delptr = translistptr;
						translistptr = translistptr->next;
						delete(delptr);
					}
					return 1;
				}
			}
		}
		translistptr = translistptr->next;
	}

//delete old translist
	while(translistptr != NULL)
	{
		delptr = translistptr;
		translistptr = translistptr->next;
		delete(delptr);
	}

	return 0;	//kein Kreisende gefunden
}

int owfn::reoccrepl_circlemultins()
{
#ifdef VERBOSE
	cout << " looking for circles with multiple entrances...\n";
#endif

	place *start;
	place *placeptr;
	place *p0;
	place *p1 = NULL;
	place *con;
	transition *transptr;
//	transitionlist *trans_in_B;	//globaly defined
//	transitionlist *checked_trans;
//	placelist *places_in_B;
	placelist *placelistptr;
	placelist *plptr;
	transitionlist *translistptr;
	transitionlist *tlptr;
	string ex_name;
	string in_name;
	string p0_name;
//	bpel *new_code;
	bpel *tmp_code;
	bpel *tmp;
	bpel *var;

	int return_value = 0;
	int entr;
	int stop;
	int found;
	
	start = places;
	while(start != NULL)
	{
		trans_in_B = NULL;
		places_in_B = NULL;
		checked_trans = NULL;
		
		if(find_circles(start->name, start->name) == 1)
		{
			//test if the circle has multiple entrances
			//Falls einer der Plätze auf dem Zyklus 2 eingehende Kanten hat (eine aus dem Zyklus und eine zwangsläufig von außen)
			entr = 0;
			found = 0;
			placelistptr = places_in_B;
			while(placelistptr != NULL)
			{
				stop = 0;	//check every place only once
				transptr = transitions;
				while(transptr != NULL && stop <= 1)
				{
					placeptr = transptr->produces;
					while(placeptr != NULL && placeptr->name != placelistptr->placeptr->name)
					{
						placeptr = placeptr->next;
					}
					if(placeptr != NULL)
					{
						stop++;
					}
					transptr = transptr->next;
				}
				
				if(stop > 1)
				{
					if(!found)
					{
						//einen Eingangsplatz als Start für die Abwicklung des Zyklus festlegen
						p1 = placelistptr->placeptr;
						found = 1;
					}
					placelistptr->ausgang = 1;
					entr++;
				}
				placelistptr = placelistptr->next;
			}

			if(entr > 1)
			{
#ifdef DEBUG
				cout << "   Circle with multiple entrances at " << start->name << "\n";
#endif
				return_value++;

				//transform circle

//1. p0 einfügen
//2. Nachbereich von tn: - p1 + p0
//3. Alle Transitionen im Nachbereich von Ausgängen einmal kopieren
//4. Vorbereich der Transitionen: -Ausgangsplatz + p0
//5. BPEL Code für die kopierten Transitionen erzeugen
//6. BPEL Code für p0 erzeugen




//1.
				p0_name = start->name + "_circle_start";
				add_place(p0_name);
				p0 = places;

				p0->bpel_code = copy_bpel(p1->bpel_code);
				copy_links(p1->bpel_code);
				p0->add_bpel(WHILE, p0->name);
				p0->bpel_code->add_branch();
				p0->bpel_code->branches->bpel_code = NULL;


//Transition suchen, die von p1 konsumiert
				tlptr = trans_in_B;
				while(tlptr != NULL && tlptr->transitionptr->consumes->name != p1->name)
				{
					tlptr = tlptr->next;
				}
				if(tlptr == NULL)
				{
					cout << "Error 3 in function 'reoccrepl_circlemultins': Place '" << p1->name << "' vanished. This should not happen...\n";
					exit(EXIT_FAILURE);
				}
//6.
				//Ab p1 den BPEL Code aller Transitionen und Plätze kopieren, bis hin zum Platz selbst
				while(tlptr != NULL)
				{
					//Kopie
					if(tlptr->transitionptr->bpel_code == NULL)
					{
						tlptr->transitionptr->add_bpel(OPAQUE, tlptr->transitionptr->name);
					}
				
					tmp = p0->bpel_code->branches->bpel_code;
					p0->bpel_code->branches->bpel_code = copy_bpel(tlptr->transitionptr->bpel_code);
					copy_links(p0->bpel_code->branches->bpel_code);
					var = p0->bpel_code->branches->bpel_code;
					if(var != NULL)
					{
						while(var->next != NULL)
						{
							var = var->next;
						}
						var->next = tmp;
					}else
					{
						p0->bpel_code->branches->bpel_code = tmp;
					}
					
					plptr = places_in_B;
					while(plptr != NULL && plptr->placeptr->name != tlptr->transitionptr->produces->name)
					{
						plptr = plptr->next;
					}
					
					if(plptr == NULL)
					{
						cout << "Error 4 in function 'reoccrepl_circlemultins': Place '" << tlptr->transitionptr->produces->name << "' vanished. This should not happen...\n";
						exit(EXIT_FAILURE);
					}
					
					tmp = p0->bpel_code->branches->bpel_code;
					p0->bpel_code->branches->bpel_code = copy_bpel(plptr->placeptr->bpel_code);
					copy_links(p0->bpel_code->branches->bpel_code);
					var = p0->bpel_code->branches->bpel_code;
					if(var != NULL)
					{
						while(var->next != NULL)
						{
							var = var->next;
						}
						var->next = tmp;
					}else
					{
						p0->bpel_code->branches->bpel_code = tmp;
					}
					
					tlptr = tlptr->next;
				}
				//falls das Ende der Liste erreicht ist, vorne bis maximal p0 weiter machen
				if(tlptr == NULL)
				{
					tlptr = trans_in_B;
					while(tlptr != NULL && tlptr->transitionptr->consumes->name != p1->name)
					{
						//Kopie
						if(tlptr->transitionptr->bpel_code == NULL)
						{
							tlptr->transitionptr->add_bpel(OPAQUE, tlptr->transitionptr->name);
						}
					
						tmp = p0->bpel_code->branches->bpel_code;
						p0->bpel_code->branches->bpel_code = copy_bpel(tlptr->transitionptr->bpel_code);
						copy_links(p0->bpel_code->branches->bpel_code);
						var = p0->bpel_code->branches->bpel_code;
						if(var != NULL)
						{
							while(var->next != NULL)
							{
								var = var->next;
							}
							var->next = tmp;
						}else
						{
							p0->bpel_code->branches->bpel_code = tmp;
						}
						
						tlptr = tlptr->next;
						
						plptr = places_in_B;
						while(plptr != NULL && plptr->placeptr->name != tlptr->transitionptr->produces->name)
						{
							plptr = plptr->next;
						}
						
						if(plptr == NULL)
						{
							cout << "Error 4 in function 'reoccrepl_circlemultins': Place '" << tlptr->transitionptr->produces->name << "' vanished. This should not happen...\n";
							exit(EXIT_FAILURE);
						}
						
						tmp = p0->bpel_code->branches->bpel_code;
						p0->bpel_code->branches->bpel_code = copy_bpel(plptr->placeptr->bpel_code);
						copy_links(p0->bpel_code->branches->bpel_code);
						var = p0->bpel_code->branches->bpel_code;
						if(var != NULL)
						{
							while(var->next != NULL)
							{
								var = var->next;
							}
							var->next = tmp;
						}else
						{
							p0->bpel_code->branches->bpel_code = tmp;
						}
					}
				}





//2.
//tn finden:
				translistptr = trans_in_B;
				while(translistptr != NULL && translistptr->transitionptr->produces != NULL && translistptr->transitionptr->produces->name != p1->name)
				{
					translistptr = translistptr->next;
				}
				if(translistptr == NULL)
				{
					cout << "Error 1 in function 'reoccrepl_circlemultins': Transition vanished. This should not happen...\n";
					exit(EXIT_FAILURE);
				}

				translistptr->transitionptr->produces->name = p0_name;
//3.
//Ausgänge:
				placelistptr = places_in_B;
				while(placelistptr != NULL)
				{
					if(placelistptr->ausgang == 1)
					{
						transptr = transitions;
						while(transptr != NULL)
						{
							placeptr = transptr->consumes;
							while(placeptr != NULL && placeptr->name != placelistptr->placeptr->name)
							{
								placeptr = placeptr->next;
							}
							if(placeptr != NULL)
							{
								//test, ob transptr auf dem Zyklus ist
								translistptr = trans_in_B;
								while(translistptr != NULL && translistptr->transitionptr != transptr)
								{
									translistptr = translistptr->next;
								}
								//wenn nicht, kopieren
								if(translistptr == NULL)
								{
									//Kopie erzeugen (Name: name + "_C1"
									copy_transition_once(transptr, 1);
									//Kopie in den Nachbereich von p0 setzen
									con = transitions->consumes;
									while(con != NULL && con->name != placelistptr->placeptr->name)
									{
										con = con->next;
									}
									if(con == NULL)
									{
										cout << "Error 2 in function 'reoccrepl_circlemultins': Place '" << placelistptr->placeptr->name << "' vanished. This should not happen...\n";
										exit(EXIT_FAILURE);
									}
//4.
									con->name = p0_name;
//5.
									//Wenn p1 Ausgang ist, braucht der BPEL Code nicht geändert zu werden
									if(placelistptr->placeptr->name != p1->name)
									{
										//Transition suchen, die von p1 konsumiert
										tlptr = trans_in_B;
										while(tlptr != NULL && tlptr->transitionptr->consumes->name != p1->name)
										{
											tlptr = tlptr->next;
										}
										if(tlptr == NULL)
										{
											cout << "Error 3 in function 'reoccrepl_circlemultins': Place '" << p1->name << "' vanished. This should not happen...\n";
											exit(EXIT_FAILURE);
										}
										//Ab p1 den BPEL Code aller Transitionen und Plätze kopieren, bis hin zum Platz selbst
										while(tlptr != NULL && tlptr->transitionptr->consumes->name != placelistptr->placeptr->name)
										{
											//Kopie
											if(tlptr->transitionptr->bpel_code == NULL)
											{
												tlptr->transitionptr->add_bpel(OPAQUE, tlptr->transitionptr->name);
											}
											
											tmp_code = copy_bpel(tlptr->transitionptr->bpel_code);
											copy_links(tmp_code);
											transitions->append_bpel(tmp_code);

											plptr = places_in_B;
											while(plptr != NULL && plptr->placeptr->name !=tlptr->transitionptr->produces->name)
											{
												plptr = plptr->next;
											}
											
											if(plptr == NULL)
											{
												cout << "Error 4 in function 'reoccrepl_circlemultins': Place '" << tlptr->transitionptr->produces->name << "' vanished. This should not happen...\n";
												exit(EXIT_FAILURE);
											}
											
											tmp_code = copy_bpel(plptr->placeptr->bpel_code);
											copy_links(tmp_code);
											transitions->append_bpel(tmp_code);

											tlptr = tlptr->next;
										}
										//falls das Ende der Liste erreicht ist, vorne bis maximal p1 weiter machen
										if(tlptr == NULL)
										{
											tlptr = trans_in_B;
											while(tlptr != NULL && tlptr->transitionptr->consumes->name != p1->name && tlptr->transitionptr->consumes->name != placelistptr->placeptr->name)
											{
												//Kopie
												if(tlptr->transitionptr->bpel_code == NULL)
												{
													tlptr->transitionptr->add_bpel(OPAQUE, tlptr->transitionptr->name);
												}
												
												tmp_code = copy_bpel(tlptr->transitionptr->bpel_code);
												copy_links(tmp_code);
												transitions->append_bpel(tmp_code);
												
												plptr = places_in_B;
												while(plptr != NULL && plptr->placeptr->name != tlptr->transitionptr->produces->name)
												{
													plptr = plptr->next;
												}
												if(plptr == NULL)
												{
													cout << "Error 4 in function 'reoccrepl_circlemultins': Place '" << tlptr->transitionptr->produces->name << "' vanished. This should not happen...\n";
													exit(EXIT_FAILURE);
												}
												tmp_code = copy_bpel(plptr->placeptr->bpel_code);
												copy_links(tmp_code);
												transitions->append_bpel(tmp_code);
												
												tlptr = tlptr->next;
											}
										}
									}
								}
							}
							transptr = transptr->next;
						}
					}
					
					placelistptr = placelistptr->next;
				}
			}
		}

		//globale listen löschen

		//delete old translist
		while(trans_in_B != NULL)
		{
			translistptr = trans_in_B;
			trans_in_B = trans_in_B->next;
			delete(translistptr);
		}

		while(checked_trans != NULL)
		{
			translistptr = checked_trans;
			checked_trans = checked_trans->next;
			delete(translistptr);
		}

		//delete old placelist
		while(places_in_B != NULL)
		{
			placelistptr = places_in_B;
			places_in_B = places_in_B->next;
			delete(placelistptr);
		}

		if(return_value > 0)
		{
			start = NULL;
		}else
		{
			start = start->next;
		}
	}
#ifdef VERBOSE
	cout << "  " << return_value << " circle(s) found and transformed.\n";
#endif
#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " circle(s) found and transformed.\n";
	}
#endif
	return return_value;
}


void owfn::reset_dfs()
{
	transition *transptr;
	place *placeptr;
	
	transptr = transitions;
	while(transptr != NULL)
	{
		transptr->dfs = -1;
		transptr->lowlink = -1;
		transptr = transptr->next;
	}

	placeptr = places;
	while(placeptr != NULL)
	{
		placeptr->dfs = -1;
		placeptr->lowlink = -1;
		placeptr = placeptr->next;
	}
}


void owfn::dfs(place *startplace, transition *starttrans)
{
	place *placeptr;
	place *realplace;
	stack *stackptr;
	transition *realtrans;
	int stop;

//Function with place
	if(startplace != NULL)
	{
		startplace->dfs = max_dfs;
		startplace->lowlink = max_dfs;
		max_dfs++;

		//push
		dfs_stack = new stack(startplace, NULL, dfs_stack);
		
		//find all transitions that consume from startplace
		realtrans = transitions;
		while(realtrans != NULL)
		{
			realplace = realtrans->consumes;
			while(realplace != NULL)
			{
				if(realplace->name == startplace->name)
				{
					if(realtrans->dfs == -1)
					{
						dfs(NULL, realtrans);
						if(realtrans->lowlink < startplace->lowlink)
						{
							startplace->lowlink = realtrans->lowlink;
						}
					}else
					{
						stop = 0;
						stackptr = dfs_stack;
						while(stackptr != NULL && !stop)
						{
							if(stackptr->transptr != NULL && stackptr->transptr == realtrans)
							{
								stop = 1;
								if(realtrans->dfs < startplace->lowlink)
								{
									startplace->lowlink = realtrans->lowlink;
								}
							}else
							{
								stackptr = stackptr->next;
							}
						}
					}
				}
				realplace = realplace->next;
			}
			
			realtrans = realtrans->next;
		}
		
		if(startplace->lowlink == startplace->dfs)
		{
			//pop
			stop = 0;
			while(!stop && dfs_stack != NULL)
			{
				if(dfs_stack->placeptr != NULL && dfs_stack->placeptr->name == startplace->name)
				{
					stop = 1;
				}

				stackptr = dfs_stack;
				dfs_stack = dfs_stack->next;
				delete(stackptr);
			}
		}

//Function with transition
	}else
	{
		if(starttrans != NULL)
		{
			starttrans->dfs = max_dfs;
			starttrans->lowlink = max_dfs;
			max_dfs++;
			
			//push
			dfs_stack = new stack(NULL, starttrans, dfs_stack);
			
			//find all places that starttrans produces on
			placeptr = starttrans->produces;
			while(placeptr != NULL)
			{
				//find the real place "placeptr"
				realplace = places;
				while(realplace != NULL && realplace->name != placeptr->name)
				{
					realplace = realplace->next;
				}
				
				if(realplace == NULL)
				{
					cout << "Error 1 in function 'dfs': Place '" << placeptr->name << "' vanished. This should not happen...\n";
					exit(EXIT_FAILURE);
				}

				if(realplace->dfs == -1)
				{
					dfs(realplace, NULL);
					
					if(realplace->lowlink < starttrans->lowlink)
					{
						starttrans->lowlink = realplace->lowlink;
					}
				}else
				{
					stop = 0;
					stackptr = dfs_stack;
					while(stackptr != NULL && !stop)
					{
						if(stackptr->placeptr != NULL && stackptr->placeptr->name == realplace->name)
						{
							stop = 1;
							if(realplace->dfs < starttrans->lowlink)
							{
								starttrans->lowlink = realplace->lowlink;
							}
						}else
						{
							stackptr = stackptr->next;
						}
					}
				}

				placeptr = placeptr->next;
			}
			
			if(starttrans->lowlink == starttrans->dfs)
			{
				//pop
				stop = 0;
				while(!stop && dfs_stack != NULL)
				{
					if(dfs_stack->transptr != NULL && dfs_stack->transptr == starttrans)
					{
						stop = 1;
					}

					stackptr = dfs_stack;
					dfs_stack = dfs_stack->next;
					delete(stackptr);
				}
			}
		}else
		{
			cout << " Warning: DF-Search was called for empty knot!\n";
		}
	}
}


void owfn::in_to_in(place *start)
{
	transitionlist *transptr;
	place *placeptr;
	place *realplace;
	place *prod;
	placelist *placelistptr;
	transitionlist *translistptr;

//no place
	if(start == NULL)
	{
		return;
	}

	transptr = trans_in_circle;
	while(transptr != NULL)
	{
		placeptr = transptr->transitionptr->consumes;
		while(placeptr != NULL)
		{
			if(placeptr->name == start->name)
			{
				translistptr = trans_in_B;
				while(translistptr != NULL && translistptr->transitionptr != transptr->transitionptr)
				{
					translistptr = translistptr->next;
				}
				
				if(translistptr == NULL)
				{
					trans_in_B = new transitionlist(transptr->transitionptr, trans_in_B);
					prod = transptr->transitionptr->produces;
					while(prod != NULL)
					{
						
					//allready in the list?
						placelistptr = places_in_B;
						while(placelistptr != NULL && placelistptr->placeptr->name != prod->name)
						{
							placelistptr = placelistptr->next;
						}
						if(placelistptr == NULL)
						{
					//in or out place?
							placelistptr = inoutplaces;
							while(placelistptr != NULL && placelistptr->placeptr->name != prod->name)
							{
								placelistptr = placelistptr->next;
							}
							realplace = return_placeptr(prod->name);
							if(realplace == NULL)
							{
								cout << "Error 1 in function 'in_to_in': Place '" << prod->name << "' vanished. This should not happen...\n";
								exit(EXIT_FAILURE);
							}
							if(placelistptr == NULL)
							{
								places_in_B = new placelist(realplace, places_in_B);
								in_to_in(realplace);
							}else
							{
								if(end_place == NULL)
								{
									end_place = realplace;
								}else
								{
									if(end_place->name != realplace->name)
									{
										cout << " Warning: Place name missmatch! Net structure should not allow this!\n";
									}
								}
							}
						}
						
						prod = prod->next;
					}
				}
			}
			placeptr = placeptr->next;
		}
		
		transptr = transptr->next;
	}
}

int owfn::reoccrepl_circlemultexecution()
{
#ifdef VERBOSE
	cout << " looking for circles with multiple marked places...\n";
#endif

	int return_value = 0;
	place *startplace;
	place *placeptr;
	transition *transptr;
	stack *dfs_stackptr;
	int counter;
	int i;
	int stop;
	owfn *newoWFN;
	transitionlist *translistptr;
	placelist *placelistptr;
	placelist *ps;
	transitionlist *ts;
	place *prevp;
	transition *prevt;
	place *con;


	max_dfs = 0;
	dfs_stack = NULL;
	reset_dfs();

	//find start place
	counter = 0;
	startplace = places;
	while(startplace != NULL  && counter != -1)
	{
		counter = 0;
		transptr = transitions;
		while(transptr != NULL && !counter)
		{
			placeptr = transptr->produces;
			while(placeptr != NULL && !counter)
			{
				if(placeptr->name == startplace->name)
				{
					counter++;
				}
				placeptr = placeptr->next;
			}
			transptr = transptr->next;
		}
		if(counter == 0)
		{
			counter = -1;
		}else
		{
			startplace = startplace->next;
		}
	}

	if(startplace != NULL)
	{
		dfs(startplace, NULL);
	}else
	{
		cout << "Error 1 in function 'reoccrepl_circlemultexecution': Start place vanished. This should not happen...\n";
		exit(EXIT_FAILURE);
	}

	while(dfs_stack != NULL)
	{
		dfs_stackptr = dfs_stack;
		dfs_stack = dfs_stack->next;
		delete(dfs_stackptr);
	}



	//net is annotated with lowlink values
	//search for nots with the same lowlink
	stop = 0;
	for(i = 0; i < max_dfs && !stop; i++)
	{
		counter = 0;
		
		transptr = transitions;
		while(transptr != NULL)
		{
			if(transptr->lowlink == i)
			{
				counter++;
			}
			transptr = transptr->next;
		}
		placeptr = places;
		while(placeptr != NULL)
		{
			if(placeptr->lowlink == i)
			{
				counter++;
			}
			placeptr = placeptr->next;
		}
		
//cout << "lowlink: " << i << ", knots: " << counter << endl;

		if(counter > 3)
		{
			//circle found, save it
			
			trans_in_circle = NULL;
			places_in_circle = NULL;
			
			transptr = transitions;
			while(transptr != NULL)
			{
				if(transptr->lowlink == i)
				{
					trans_in_circle = new transitionlist(transptr, trans_in_circle);
				}
				transptr = transptr->next;
			}
			placeptr = places;
			while(placeptr != NULL)
			{
				if(placeptr->lowlink == i)
				{
					places_in_circle = new placelist(placeptr, places_in_circle);
				}
				placeptr = placeptr->next;
			}

			//find entry or exit places

			inoutplaces = NULL;
			placelistptr = places_in_circle;
			while(placelistptr != NULL)
			{
				counter = 0;
				transptr = transitions;
				while(transptr != NULL && !counter)
				{
					if(transptr->lowlink != i)
					{
						placeptr = transptr->consumes;
						while(placeptr != NULL && !counter)
						{
							if(placeptr->name == placelistptr->placeptr->name)
							{
								inoutplaces = new placelist(placeptr, inoutplaces);
								counter = 1;
							}
							placeptr = placeptr->next;
						}
						placeptr = transptr->produces;
						while(placeptr != NULL && !counter)
						{
							if(placeptr->name == placelistptr->placeptr->name)
							{
								inoutplaces = new placelist(placeptr, inoutplaces);
								counter = 1;
							}
							placeptr = placeptr->next;
						}
					}
					transptr = transptr->next;
				}
				placelistptr = placelistptr->next;
			}



			if(inoutplaces != NULL)
			{
				//start at one entry and follow the net to the next
				placelistptr = inoutplaces;
				while(placelistptr != NULL && !stop)
				{
					end_place = NULL;
					places_in_B = NULL;
					trans_in_B = NULL;
					in_to_in(placelistptr->placeptr);
					
					//at this point there should be a list of transitions and places
					//all between placelistptr->placeptr and other in or out places
					//and because of the layout of allowed owfn that should be only one place


					if(places_in_B != NULL)
					{
						//if there are places between start and end there have to be transitions, too

#ifdef DEBUG
						cout << "   Circle at " << placelistptr->placeptr->name << "\n";
#endif
						
						//create new oWFN
						newoWFN = new owfn();
						
						//add new start and end place
						newoWFN->places = new place(placelistptr->placeptr->name + "_circle_start", newoWFN->places);
						newoWFN->places = new place(end_place->name + "_circle_end", newoWFN->places);
						
						//move all places and transitions from here to the new oWFN
						ps = places_in_B;
						while(ps != NULL)
						{
							prevp = NULL;
							placeptr = places;
							while(placeptr->name != ps->placeptr->name)
							{
								prevp = placeptr;
								placeptr = placeptr->next;
							}
							if(prevp == NULL)
							{
								places = places->next;
							}else
							{
								prevp->next = placeptr->next;
							}
							placeptr->next = newoWFN->places;
							newoWFN->places = placeptr;
							
							ps = ps->next;
						}

						ts = trans_in_B;
						while(ts != NULL)
						{
						
							//adjust transitions to new places
							con = ts->transitionptr->consumes;
							while(con != NULL)
							{
								if(con->name == placelistptr->placeptr->name)
								{
									con->name = placelistptr->placeptr->name + "_circle_start";
								}
								if(con->name == end_place->name)
								{
									con->name = end_place->name + "_circle_end";
								}
								con = con->next;
							}
							con = ts->transitionptr->produces;
							while(con != NULL)
							{
								if(con->name == placelistptr->placeptr->name)
								{
									con->name = placelistptr->placeptr->name + "_circle_start";
								}
								if(con->name == end_place->name)
								{
									con->name = end_place->name + "_circle_end";
								}
								con = con->next;
							}

							prevt = NULL;
							transptr = transitions;
							while(transptr != ts->transitionptr)
							{
								prevt = transptr;
								transptr = transptr->next;
							}
							if(prevt == NULL)
							{
								transitions = transitions->next;
							}else
							{
								prevt->next = transptr->next;
							}
							transptr->next = newoWFN->transitions;
							newoWFN->transitions = transptr;
							
							ts = ts->next;
						}

						//transform new oWFN
						newoWFN->transform();
						
						//add bpel code of new oWFN to old oWFN
						//remove places and transitions
						//create a new transition for the bpel code
						
						if((newoWFN->places != NULL && newoWFN->transitions != NULL) || (newoWFN->places != NULL && newoWFN->places->next != NULL) || (newoWFN->transitions != NULL && newoWFN->transitions->next != NULL))
						{
							cout << "Error: Could not reduce sub-net. The oWFN seems to violate this tools specifications." << endl << "No output was generated." << endl;
							exit(EXIT_FAILURE);
						}
						
						if(newoWFN->places != NULL)
						{
							transitions = new transition(newoWFN->places->name, transitions);
							transitions->bpel_code = newoWFN->places->bpel_code;
							delete(newoWFN->places);
							delete(newoWFN);
						}else
						{
							if(newoWFN->transitions != NULL)
							{
								transitions = new transition(newoWFN->transitions->name, transitions);
								transitions->bpel_code = newoWFN->transitions->bpel_code;
								delete(newoWFN->transitions);
								delete(newoWFN);
							}
						}
						
						stop = 1;
						return_value++;
					}

					while(trans_in_B != NULL)
					{
						translistptr = trans_in_B;
						trans_in_B = trans_in_B->next;
						delete(translistptr);
					}
					while(places_in_B != NULL)
					{
						placelistptr = places_in_B;
						places_in_B = places_in_B->next;
						delete(placelistptr);
					}
					placelistptr = placelistptr->next;
				}


			}	//else circle with no entry or exit, skip (and wonder)



			while(trans_in_circle != NULL)
			{
				translistptr = trans_in_circle;
				trans_in_circle = trans_in_circle->next;
				delete(translistptr);
			}
			while(places_in_circle != NULL)
			{
				placelistptr = places_in_circle;
				places_in_circle = places_in_circle->next;
				delete(placelistptr);
			}
			while(inoutplaces != NULL)
			{
				placelistptr = inoutplaces;
				inoutplaces = inoutplaces->next;
				delete(placelistptr);
			}
		}
	}

#ifdef VERBOSE
	cout << "  " << return_value << " circle(s) found and transformed.\n";
#endif
#ifdef MINVERBOSE
	if(return_value > 0)
	{
		cout << "  " << return_value << " circle(s) found and transformed.\n";
	}
#endif
	return return_value;
}



void owfn::transform()
{
	int repl_counter;
	repl_counter = 1;

//functions repeated while changes occur:
	while(repl_counter)
	{
		number_of_rounds++;
		repl_counter = 0;

//16.		//typische Entscheidungen von Extended Free-Choice Netzen auflösen
		if((repl_counter = reoccrepl_exfc_A()))
		{
			stat_array[1] += repl_counter;
			continue;
		}

//16.5		//mehrere Transitionen mit mehreren Plätzen im Nachbereich und Kanten zwischen allen
		if((repl_counter = reoccrepl_exfc_B()))
		{
			stat_array[2] += repl_counter;
			continue;
		}

//7.		//lineare Folgen mit Transition in der Mitte zusammenfassen
		if((repl_counter = reoccrepl_linplaces()))
		{
			stat_array[3] += repl_counter;
			continue;
		}
//8.		//lineare Folgen mit Platz in der Mitte zusammenfassen
		if((repl_counter = reoccrepl_lintrans()))
		{
			stat_array[4] += repl_counter;
			continue;
		}
//9.		//lineare Folgen mit Transition und offenen Ende zusammenfassen
		if((repl_counter = reoccrepl_endtrans()))
		{
			stat_array[5] += repl_counter;
			continue;
		}
//10.		//lineare Folgen mit Platz und offenen Ende zusammenfassen
		if((repl_counter = reoccrepl_endplaces()))
		{
			stat_array[6] += repl_counter;
			continue;
		}

//12.		//flows über einen Platz, erste Funktion für offene Enden
		if((repl_counter = reoccrepl_flowshortopenend()))
		{
			stat_array[7] += repl_counter;
			continue;
		}
		if((repl_counter = reoccrepl_flowshort()))
		{
			stat_array[8] += repl_counter;
			continue;
		}

//11.		//switches über eine Transition, zweite Funktion für offene Enden
		if((repl_counter = reoccrepl_switchshort()))
		{
			stat_array[9] += repl_counter;
			continue;
		}
		if((repl_counter = reoccrepl_switchshortopenend()))
		{
			stat_array[10] += repl_counter;
			continue;
		}

//		//while aus nur einer Doppelkante
		if((repl_counter = reoccrepl_circleshort()))
		{
			stat_array[11] += repl_counter;
			continue;
		}

//		//while aus Kreis mit mehreren Ausgängen
		if((repl_counter = reoccrepl_circlemultexits()))
		{
			stat_array[12] += repl_counter;
			continue;
		}

//11.5 möglichst spät, aber vor 18
//11.5
		if((repl_counter = reoccrepl_switch()))
		{
			stat_array[13] += repl_counter;
			continue;
		}

//18		//Kreis mit mehreren Eingängen umwandeln in Kreis mit einem
		if((repl_counter = reoccrepl_circlemultins()))
		{
			stat_array[14] += repl_counter;
			continue;
		}

//		//Flows aus Teilnetzen
		if((repl_counter = reoccrepl_bigflows()))
		{
			stat_array[15] += repl_counter;
			continue;
		}

//Sollten nach allen if Regeln stehen
//17.a)		//Teilnetze kopieren, wenn es keine Bögen von A nach B gibt
		if((repl_counter = reoccrepl_copy_A()))
		{
			stat_array[16] += repl_counter;
			continue;
		}

//17.b)		//Partielle Kopie der Transitionen im Nachbereich
		if((repl_counter = reoccrepl_copy_B()))
		{
			stat_array[17] += repl_counter;
			continue;
		}

//		//Kreis mit Unsicherheit zerlegen
		if((repl_counter = reoccrepl_circlemultexecution()))
		{
			stat_array[18] += repl_counter;
			continue;
		}

	}
}


void owfn::owfn_out()
{
	if(places != NULL && transitions != NULL)
	{
		cout << "PLACE" << endl << "INTERNAL" << endl;
		places->owfn_out();
	
		cout << "INPUT" << endl << ";" << endl << endl << "OUTPUT" << endl << ";" << endl << endl << "INITIALMARKING" << endl << places->name << " : 1; { zufaellig ausgewaehlter Platz }" << endl << endl << "FINALMARKING" << endl << places->name << " : 1; { zufaellig ausgewaehlter Platz }" << endl;
	  
		transitions->owfn_out();
	}
}


void owfn::owfn_to_file()
{
	place *placeptr;
	transition *transptr;
	
	bpel_file.open(outfile, ios::out);

	if(!bpel_file)
	{
		cout << "Error 1 in function 'owfn_to_file': BPEL file '" << outfile << "' could not be opened!" << endl;
		exit(EXIT_FAILURE);
	}

		bpel_file << "PLACE" << endl << "INTERNAL" << endl;

		placeptr = places;
		while(placeptr != NULL)
		{
			bpel_file << placeptr->name;
			if(placeptr->next != NULL)
			{
				bpel_file << ", " << endl << " { ";
				if(placeptr->bpel_code != NULL)
					placeptr->bpel_code->code(&bpel_file, NULL, NULL, 0);
				bpel_file << " }" << endl;
			}else
			{
				bpel_file << ";" << endl << " { ";
				if(placeptr->bpel_code != NULL)
					placeptr->bpel_code->code(&bpel_file, NULL, NULL, 0);
				bpel_file << " }" << endl << endl;
			}
			
			placeptr = placeptr->next;
		}
	
		bpel_file << "INPUT" << endl << ";" << endl << endl << "OUTPUT" << endl << ";" << endl << endl << "INITIALMARKING" << endl << places->name << " : 1; { zufaellig ausgewaehlter Platz }" << endl << endl << "FINALMARKING" << endl << places->name << " : 1; { zufaellig ausgewaehlter Platz }" << endl << endl;
	  
		transptr = transitions;
		while(transptr != NULL)
		{
			
			bpel_file << "TRANSITION " << transptr->name << endl;
			
			bpel_file << "CONSUME " << endl;
			placeptr = transptr->consumes;
			if(placeptr == NULL)
			{
				bpel_file << ";" << endl;
			}
			while(placeptr != NULL)
			{
				bpel_file << placeptr->name << " : 1";
				if(placeptr->next != NULL)
				{
					bpel_file << "," << endl;
				}else
				{
					bpel_file << ";" << endl;
				}
				
				placeptr = placeptr->next;
			}

			bpel_file << "PRODUCE " << endl;
			placeptr = transptr->produces;
			if(placeptr == NULL)
			{
				bpel_file << ";" << endl;
			}
			while(placeptr != NULL)
			{
				bpel_file << placeptr->name << " : 1";
				if(placeptr->next != NULL)
				{
					bpel_file << "," << endl;
				}else
				{
					bpel_file << ";" << endl;
				}
				
				placeptr = placeptr->next;
			}

			if(transptr->bpel_code != NULL)
			{
				bpel_file << "{ " << endl;
					transptr->bpel_code->code(&bpel_file, NULL, NULL, 0);
				bpel_file << "}" << endl << endl;
			}

			transptr = transptr->next;
		}
	bpel_file.close();
}

















int owfn::check_subnet(string start, int is_trans, string trans_start_name, string trans_end_name)
{
	//Tests:
	//1. keine von außen eingehenden oder nach außen gehenden Kanten
	//2. zyklenfrei
	//3. sicher, fehlt noch, LoLa lässt sich nicht kompilieren

	ofstream lola_file;
	ifstream lola_res;
	char *lola_name = "lola_safety.tmp";
	char *lola_result = "lola_safety_result.tmp";
	char keyword[9];
    assert(args_info.lola_arg != NULL);
	string tmp = string(args_info.lola_arg); //("owfn2bpel_lola/owfn2bpel_lola");
	string execution;
	string buffer;
	string::size_type loc;

	execution = tmp + " lola_safety.tmp 2> lola_safety_result.tmp > /dev/null";
//	execution = tmp + " lola_safety.tmp 2> lola_safety_result.tmp";

#ifdef UNIX
	execution = tmp + " lola_safety.tmp 2> lola_safety_result.tmp > /dev/null";
#endif

#ifdef WIN
	execution = tmp + " lola_safety.tmp 2> lola_safety_result.tmp > NUL";
#endif
	place *con;
	place *placeptr;
	placelist *plptr;
	transitionlist *tlptr;
	transition *transptr;
	transitionlist *deltrans;
	placelist *delplace;
	
	int stop;
	int lola;
	stop = 0;

	//1. Test
	plptr = places_in_B;
	while(plptr != NULL && !stop)
	{
		transptr = transitions;
		while(transptr != NULL && !stop)
		{
			//falls der Start keine Transition ist, dann auf jeden Fall testen
			//Ansonsten alle außer das Original der Starttransition testen
			//Dafür ist eine Ersatztransition in die Liste aufgenommen und Kanten von dort stören nicht
			if(!is_trans || (transptr->name != trans_start_name && transptr->name != trans_end_name))
			{
				con = transptr->consumes;
				while(con != NULL && !stop)
				{
					if(con->name == plptr->placeptr->name)
					{
						tlptr = trans_in_B;
						while(tlptr != NULL && tlptr->transitionptr->name != transptr->name)
						{
							tlptr = tlptr->next;
						}
						if(tlptr == NULL)
						{
							stop = 1;
						}
					}
					con = con->next;
				}
	
				con = transptr->produces;
				while(con != NULL && !stop)
				{
					if(con->name == plptr->placeptr->name)
					{
						tlptr = trans_in_B;
						while(tlptr != NULL && tlptr->transitionptr->name != transptr->name)
						{
							tlptr = tlptr->next;
						}
						
						if(tlptr == NULL)
						{
							stop = 1;
						}
					}
					con = con->next;
				}
			}
			transptr = transptr->next;
		}

		plptr = plptr->next;
	}
	if(stop)
	{
		return 0;
	}

	//Test 1.5
	tlptr = trans_in_B;
	while(tlptr != NULL && !stop)
	{
		con = tlptr->transitionptr->consumes;
		while(con != NULL && !stop)
		{
			plptr = places_in_B;
			while(plptr != NULL && plptr->placeptr->name != con->name)
			{
				plptr = plptr->next;
			}

			if(plptr == NULL)
			{
				stop = 1;
			}

			con = con->next;
		}

		con = tlptr->transitionptr->produces;
		while(con != NULL && !stop)
		{
			plptr = places_in_B;
			while(plptr != NULL && plptr->placeptr->name != con->name)
			{
				plptr = plptr->next;
			}

			if(plptr == NULL)
			{
				stop = 1;
			}

			con = con->next;
		}
		tlptr = tlptr->next;
	}

	if(stop)
	{
		return 0;
	}

	//2. Test
	//Test für jeden Knoten des Teilnetzes, ob man zu ihm zurückgelangen kann
	plptr = places_in_B;
	while(plptr != NULL && !stop)
	{
		seen_places = NULL;
		
		subnet_circle = 0;
		circle_subnet_p(plptr->placeptr, plptr->placeptr);
		stop = subnet_circle;
		plptr = plptr->next;
		
		while(seen_places != NULL)
		{
			delplace = seen_places;
			seen_places = seen_places->next;
			delete(delplace);
		}
	}
	tlptr = trans_in_B;
	while(tlptr != NULL && !stop)
	{
		seen_trans = NULL;
		
		subnet_circle = 0;
		circle_subnet_t(tlptr->transitionptr, tlptr->transitionptr);
		stop = subnet_circle;
		tlptr = tlptr->next;

		while(seen_trans != NULL)
		{
			deltrans = seen_trans;
			seen_trans = seen_trans->next;
			delete(deltrans);
		}
	}

	if(stop)
	{
		return 0;
	}

	//3. Test
	lola_file.open(lola_name, ios::out);
	if(!lola_file)
	{
		cout << "Error 1 in function 'check_subnet': Temporary file '" << lola_name << "' could not be opened!\n";
		exit(EXIT_FAILURE);
	}

	//Allgemeines
	lola_file << "PLACE" << endl;
	plptr = places_in_B;
	while(plptr != NULL)
	{
		lola_file << plptr->placeptr->name;
		if(plptr->next != NULL)
		{
			lola_file << ", ";
		}else
		{
			lola_file << ";" << endl << endl;
		}

		plptr = plptr->next;
	}
	
	lola_file << "MARKING " << start << ": 1;" << endl;

	tlptr = trans_in_B;
	while(tlptr != NULL)
	{
		lola_file << "TRANSITION " << tlptr->transitionptr->name << endl;

		lola_file << "CONSUME ";

		placeptr = tlptr->transitionptr->consumes;
		if(placeptr == NULL)
		{
			lola_file << ";" << endl;
		}
		while(placeptr != NULL)
		{
			lola_file << placeptr->name << " : 1";
			if(placeptr->next != NULL)
			{
				lola_file << ", ";
			}else
			{
				lola_file << ";" << endl;
			}

			placeptr = placeptr->next;
		}

		lola_file << "PRODUCE ";
		placeptr = tlptr->transitionptr->produces;
		if(placeptr == NULL)
		{
			lola_file << ";" << endl;
		}
		while(placeptr != NULL)
		{
			lola_file << placeptr->name << " : 1";
			if(placeptr->next != NULL)
			{
				lola_file << ", ";
			}else
			{
				lola_file << ";" << endl;
			}

			placeptr = placeptr->next;
		}

		tlptr = tlptr->next;
	}

	lola_file.close();


	//Aufruf LoLa
#ifdef VERBOSE
	cout << "   calling LoLA..." << endl;
#endif
	lola = system(execution.c_str());

	lola_res.open(lola_result, ios::in);
	if(!lola_res)
	{
		cout << "Error 2 in function 'check_subnet': Temporary file '" << lola_result << "' could not be opened!\n";
		exit(EXIT_FAILURE);
	}
	strcpy(keyword, "exceeded");

	lola = 1;
	while(lola_res >> buffer && lola)
	{
		loc = buffer.find(keyword, 0);
		if(loc != string::npos)
		{
			lola = 0;	//Netz nicht sicher
		}
	}

	lola_res.close();

	if(remove(lola_name) != 0)
	{
		cout << "Warning in function 'check_subnet': Temporary file '" << lola_name << "' could not be deleted!" << endl;
		//exit(EXIT_FAILURE);
	}
	if(remove(lola_result) != 0)
	{
		cout << "Warning in function 'check_subnet': Temporary file '" << lola_result << "' could not be deleted!" << endl;
		//exit(EXIT_FAILURE);
	}

	return lola;
}



bpel *owfn::transform_subnet(string start)
{
	//Alles unmarkierte mit empty/opaque markieren
	flow_empty();
	//Alle verbleibenden switches vorbereiten
	flow_switches();
	//Alle Kanten durch Links ersetzen
	flow_links();
	//Einen flow erzeugen, an den alles angehängt wird
	return flow_create_flow(start);
}


void owfn::circle_subnet_p(place *start, place *search)
{
	place *con;
	place *prod;
	transitionlist *tlptr;
	placelist *complist;

	tlptr = trans_in_B;
	while(tlptr != NULL)
	{
		con = tlptr->transitionptr->consumes;
		while(con != NULL)
		{
			if(con->name == start->name)
			{
				prod = tlptr->transitionptr->produces;
				while(prod != NULL)
				{
					if(prod->name == search->name)
					{
						subnet_circle = 1;
					}
					
					if(!subnet_circle)
					{
						complist = seen_places;
						while(complist != NULL && complist->placeptr->name != prod->name)
						{
							complist = complist->next;
						}
						if(complist == NULL)
						{
							seen_places = new placelist(start, seen_places);
							circle_subnet_p(prod, start);
						}
					}
					prod = prod->next;
				}
			}

			con = con->next;
		}
		tlptr = tlptr->next;
	}
}



void owfn::circle_subnet_t(transition *start, transition *search)
{
	place *con;
	place *prod;
	transitionlist *tlptr;
	transitionlist *complist;
	
	prod = start->produces;
	while(prod != NULL)
	{
		tlptr = trans_in_B;
		while(tlptr != NULL)
		{
			con = tlptr->transitionptr->consumes;
			while(con != NULL)
			{
				if(con->name == prod->name)
				{
					if(tlptr->transitionptr->name == search->name)
					{
						subnet_circle = 1;
					}
					
					if(!subnet_circle)
					{
						complist = seen_trans;
						while(complist != NULL && complist->transitionptr->name != tlptr->transitionptr->name)
						{
							complist = complist->next;
						}
						if(complist == NULL)
						{
							seen_trans = new transitionlist(tlptr->transitionptr, seen_trans);
							circle_subnet_t(tlptr->transitionptr, search);
						}
					}
				}
				con = con->next;
			}
			tlptr = tlptr->next;
		}
		prod = prod->next;
	}
}



void owfn::create_subnet_copy()
{
	transitionlist *tlptr;
	placelist *plptr;
	place *tmpp;
	transition *tmpt;

	trans_original = NULL;
	places_original = NULL;

	tlptr = trans_in_B;
	while(tlptr != NULL)
	{
		tmpt = tlptr->transitionptr;
		trans_original = new transitionlist(tmpt, trans_original);
		tlptr->transitionptr = new transition(tmpt->name);
		tlptr->transitionptr->bpel_code = copy_bpel(tmpt->bpel_code);
		copy_links(tlptr->transitionptr->bpel_code);
		
		tmpp = tmpt->produces;
		while(tmpp != NULL)
		{
			tlptr->transitionptr->add_produce(tmpp->name);
			tmpp = tmpp->next;
		}

		tmpp = tmpt->consumes;
		while(tmpp != NULL)
		{
			tlptr->transitionptr->add_consume(tmpp->name);
			tmpp = tmpp->next;
		}

		tlptr = tlptr->next;
	}
	plptr = places_in_B;
	while(plptr != NULL)
	{
		tmpp = plptr->placeptr;
		places_original = new placelist(tmpp, places_original);
		plptr->placeptr = new place(tmpp->name);
		plptr->placeptr->bpel_code = copy_bpel(tmpp->bpel_code);
		copy_links(plptr->placeptr->bpel_code);

		plptr = plptr->next;
	}
}


void owfn::delete_subnet_copy(int inc_bpel)
{
	transitionlist *tlptr;
	placelist *plptr;
	bpel *tmp;

	while(trans_in_B != NULL)
	{
		tlptr = trans_in_B;
		trans_in_B = trans_in_B->next;
		tlptr->transitionptr->del_lists();

		if(inc_bpel)
		{
			while(tlptr->transitionptr->bpel_code != NULL)
			{
				tmp = tlptr->transitionptr->bpel_code;
				tlptr->transitionptr->bpel_code = tlptr->transitionptr->bpel_code->next;
//enthält unbekannten fehler				tmp->delete_lists();
				delete(tmp);
			}
		}
		delete(tlptr->transitionptr);
		delete(tlptr);
	}
	while(places_in_B != NULL)
	{
		plptr = places_in_B;
		places_in_B = places_in_B->next;
		if(inc_bpel)
		{
			while(plptr->placeptr->bpel_code != NULL)
			{
				tmp = plptr->placeptr->bpel_code;
				plptr->placeptr->bpel_code = plptr->placeptr->bpel_code->next;
//				tmp->delete_lists();
				delete(tmp);
			}
		}
		delete(plptr->placeptr);
		delete(plptr);
	}
}


int owfn::replace_place(place *start, string name)
{
	string newname;
	transitionlist *tlptr;
	placelist *plptr;
	place *con;
	bpel *tmp;
	int found;
	
	newname = start->name + name;
	
	tlptr = trans_in_B;
	while(tlptr != NULL)
	{
		con = tlptr->transitionptr->consumes;
		while(con != NULL)
		{
			if(con->name == start->name)
			{
				con->name = newname;
			}
			con = con->next;
		}

		con = tlptr->transitionptr->produces;
		while(con != NULL)
		{
			if(con->name == start->name)
			{
				con->name = newname;
			}
			con = con->next;
		}
		tlptr = tlptr->next;
	}

	found = 0;
	plptr = places_in_B;
	while(plptr != NULL && !found)
	{
		if(plptr->placeptr->name == start->name)
		{
			found = 1;
			plptr->placeptr->name = newname;
			//Bpel Code bleibt leer, da dieser Platz als Original im oWFN den BPEL Code behält
			
			while(plptr->placeptr->bpel_code != NULL)
			{
				tmp = plptr->placeptr->bpel_code;
				plptr->placeptr->bpel_code = plptr->placeptr->bpel_code->next;
				delete(tmp);
			}
		}
		plptr = plptr->next;
	}
	
	return found;
}

int owfn::reoccrepl_bigflows()
{
#ifdef VERBOSE
	cout << " looking for subnets to be turned into flows...\n";
#endif
	int length;
	int max_length;
	int stop;
	place *placeptr;
	transition *transptr;
	placelist *all_p;
	placelist *tmp;
	transitionlist *all_t;
	transitionlist *tmp_t;
	transition *t;
	place *con;
	place *prod;
	
	length = 3;
	stop = 0;

	
	transptr = transitions;
	for(max_length = 0; transptr != NULL; max_length++)
	{
		transptr = transptr->next;
	}

	while(!stop && length <= max_length + 1)
	{
		placeptr = places;
		while(placeptr != NULL && !stop)
		{
			all_t = NULL;
			t = transitions;
			while(t != NULL)
			{
				con = t->consumes;
				while(con != NULL)
				{
					if(con->name == placeptr->name)
					{
						all_t = new transitionlist(t, all_t);
					}
					con = con->next;
				}
				t = t->next;
			}
			//alle Tests mit Plätzen als Start
			stop = f_place_end_a(placeptr, length, NULL, all_t);
			while(all_t != NULL)
			{
				tmp_t = all_t;
				all_t = all_t->next;
				delete(tmp_t);
			}
			
			placeptr = placeptr->next;
		}

		transptr = transitions;
		while(transptr != NULL && !stop)
		{
			all_p = NULL;
			prod = transptr->produces;
			while(prod != NULL)
			{
				all_p = new placelist(prod, all_p);
				prod = prod->next;
			}
			stop = f_trans_end_a(transptr, length, NULL, all_p);
			while(all_p != NULL)
			{
				tmp = all_p;
				all_p = all_p->next;
				delete(tmp);
			}

			transptr = transptr->next;
		}

		length++;
	}
#ifdef VERBOSE
	cout << "  " << stop << " subnet found and reduced.\n";
#endif
#ifdef MINVERBOSE
	if(stop > 0)
	{
		cout << "  " << stop << " subnet found and reduced.\n";
	}
#endif
	return stop;
}




void owfn::flow_empty()
{
	transitionlist *tlptr;
	placelist *plptr;
	
	tlptr = trans_in_B;
	while(tlptr != NULL)
	{
		if(tlptr->transitionptr->bpel_code == NULL)
		{
			tlptr->transitionptr->add_bpel(EMPTY, tlptr->transitionptr->name);
			tlptr->transitionptr->add_bpel(OPAQUE, tlptr->transitionptr->name);
			tlptr->transitionptr->add_bpel(EMPTY, tlptr->transitionptr->name);
		}else
		{
			tlptr->transitionptr->add_bpel(EMPTY, tlptr->transitionptr->name);
			tlptr->transitionptr->add_last_bpel(EMPTY, tlptr->transitionptr->name);
		}
		tlptr = tlptr->next;
	}

	plptr = places_in_B;
	while(plptr != NULL)
	{
		if(plptr->placeptr->bpel_code == NULL)
		{
			plptr->placeptr->add_bpel(EMPTY, plptr->placeptr->name);
		}else
		{
			plptr->placeptr->add_bpel(EMPTY, plptr->placeptr->name);
			plptr->placeptr->add_last_bpel(EMPTY, plptr->placeptr->name);
		}
		plptr = plptr->next;
	}
}


void owfn::flow_switches()
{
	placelist *plptr;
	place *con;
	place *con_prev;
	transitionlist *tlptr;
	string placename;
	string newplace;
	int found;
	
	plptr = places_in_B;
	while(plptr != NULL)
	{
		found = 0;
		tlptr = trans_in_B;
		while(tlptr != NULL)
		{
			con = tlptr->transitionptr->consumes;
			while(con != NULL)
			{
				if(con->name == plptr->placeptr->name)
				{
					found++;	//there needs to be more than one transition
				}
				con = con->next;
			}

			tlptr = tlptr->next;
		}

		if(found > 1)
		{
			//add  switch
			plptr->placeptr->add_bpel(SWITCH, plptr->placeptr->name);

			//add branches for every transition
			tlptr = trans_in_B;
			while(tlptr != NULL)
			{
				con = tlptr->transitionptr->consumes;
				con_prev = NULL;
				while(con != NULL)
				{
					if(con->name == plptr->placeptr->name)
					{
						plptr->placeptr->bpel_code->add_branch();
						plptr->placeptr->bpel_code->branches->bpel_code = new bpel(EMPTY, NULL, "empty_after_switch");
						//create link arc
						links::increment();
						plptr->placeptr->bpel_code->branches->bpel_code->add_source();
						tlptr->transitionptr->bpel_code->add_target(1);
						//remove old arc
						if(con_prev == NULL)
						{
							tlptr->transitionptr->consumes = tlptr->transitionptr->consumes->next;
						}else
						{
							con_prev->next = con->next;
						}
						delete(con);
						con = NULL;	//end of search, there can only be one arc
					}else
					{
						con_prev = con;
						con = con->next;
					}
				}
	
				tlptr = tlptr->next;
			}
		}
		plptr = plptr->next;
	}
}


void owfn::flow_links()
{
	transitionlist *tlptr;
	place *plist;
	placelist *plptr;
	bpel *tmp;
	
	tlptr = trans_in_B;
	while(tlptr != NULL)
	{
		plist = tlptr->transitionptr->consumes;
		while(plist != NULL)
		{
			plptr = places_in_B;
			while(plptr != NULL && plptr->placeptr->name != plist->name)
			{
				plptr = plptr->next;
			}
			if(plptr == NULL)
			{
				cout << "Error 1 in function 'flow_links': Place '" << plist->name << "' vanished. This should not happen...\n";
				exit(EXIT_FAILURE);
			}

			//turn arc from place to transition into a link
			links::increment();
			plptr->placeptr->bpel_code->add_source();
			tlptr->transitionptr->bpel_code->add_target(1);


			plist = plist->next;
		}
		
		plist = tlptr->transitionptr->produces;
		while(plist != NULL)
		{
			plptr = places_in_B;
			while(plptr != NULL && plptr->placeptr->name != plist->name)
			{
				plptr = plptr->next;
			}
			if(plptr == NULL)
			{
				cout << "Error 2 in function 'flow_links': Place '" << plist->name << "' vanished. This should not happen...\n";
				exit(EXIT_FAILURE);
			}

			//falls der Platz bereits Ziel von Links ist, an dieser Stelle das OR Flag setzen
			//test, ob die erste BPEL Aktivität am Ende der Liste target ist!
			if((tmp = plptr->placeptr->bpel_code->is_target()) != NULL)
			{
				tmp->link_is_or = 1;
			}

			//turn arc from transition to place into a link
			links::increment();
			plptr->placeptr->bpel_code->add_target(1);
			tlptr->transitionptr->bpel_code->add_source();

			plist = plist->next;
		}
		
		tlptr = tlptr->next;
	}
}


bpel *owfn::flow_create_flow(string start)
{
	transitionlist *tlptr;
	placelist *plptr;
	branch *newbranch;
	bpel *list;
	bpel *help;
	bpel *final_code;

	//create new flow
	final_code = new bpel(FLOW, NULL, "flow_subnet_" + start);

	//add branch for every transition
	tlptr = trans_in_B;
	while(tlptr != NULL)
	{
		final_code->add_branch();
		newbranch = final_code->branches;

		list = tlptr->transitionptr->bpel_code;

		if(list != NULL)
		{
			help = list;
			while(list->next != NULL)
			{
				list = list->next;
			}
			list->next = newbranch->bpel_code;
			newbranch->bpel_code = help; 
		}	//else nothing to do

		tlptr = tlptr->next;
	}

	//add branch for every place
	plptr = places_in_B;
	while(plptr != NULL)
	{
		final_code->add_branch();
		newbranch = final_code->branches;

		list = plptr->placeptr->bpel_code;

		if(list != NULL)
		{
			help = list;
			while(list->next != NULL)
			{
				list = list->next;
			}
			list->next = newbranch->bpel_code;
			newbranch->bpel_code = help; 
		}	//else nothing to do

		plptr = plptr->next;
	}

	return final_code;
}




void owfn::delete_subnet_original()
{
	transitionlist *tlptr;
	placelist *plptr;
	transition *prevt;
	transition *transptr;
	place *prevp;
	place *placeptr;
	bpel *tmp;

	while(trans_original != NULL)
	{
		transptr = transitions;
		prevt = NULL;
		while(transptr != NULL && transptr != trans_original->transitionptr)
		{
			prevt = transptr;
			transptr = transptr->next;
		}
		
		if(transptr == NULL)
		{
			cout << "Error 1 in function 'delete_subnet_original': Transition '" << trans_original->transitionptr->name << "' vanished. This should not happen...\n";
			exit(EXIT_FAILURE);
		}else
		{
			if(prevt == NULL)
			{
				transitions = transitions->next;
			}else
			{
				prevt->next = transptr->next;
			}
			transptr->del_lists();
			while(transptr->bpel_code != NULL)
			{
				tmp = transptr->bpel_code;
				transptr->bpel_code = transptr->bpel_code->next;
//				tmp->delete_lists();
				delete(tmp);
			}
			delete(transptr);
		}
		
		tlptr = trans_original;
		trans_original = trans_original->next;
		delete(tlptr);
	}

	while(places_original != NULL)
	{
		placeptr = places;
		prevp = NULL;
		while(placeptr != NULL && placeptr->name != places_original->placeptr->name)
		{
			prevp = placeptr;
			placeptr = placeptr->next;
		}
		
		if(placeptr == NULL)
		{
			cout << "Error 2 in function 'delete_subnet_original': Place '" << places_original->placeptr->name << "' vanished. This should not happen...\n";
			exit(EXIT_FAILURE);
		}else
		{
			if(prevp == NULL)
			{
				places = places->next;
			}else
			{
				prevp->next = placeptr->next;
			}

			while(placeptr->bpel_code != NULL)
			{
				tmp = placeptr->bpel_code;
				placeptr->bpel_code = placeptr->bpel_code->next;
//				tmp->delete_lists();
				delete(tmp);
			}
			delete(placeptr);
		}
		
		plptr = places_original;
		places_original = places_original->next;
		delete(plptr);
	}
}


void owfn::remove_place_from_original(string name)
{
	placelist *plptr;
	placelist *prev;
	
	plptr = places_original;
	prev = NULL;
	while(plptr != NULL && plptr->placeptr->name != name)
	{
		prev = plptr;
		plptr = plptr->next;
	}
	
	if(plptr == NULL)
	{
		cout << "Error 1 in function 'remove_place_from_original': Place '" << name << "' vanished. This should not happen...\n";
		exit(EXIT_FAILURE);
	}else
	{
		if(prev == NULL)
		{
			places_original = places_original->next;
		}else
		{
			prev->next = plptr->next;
		}
		delete(plptr);
	}
}


int owfn::remove_transition_from_original(string name)
{
	transitionlist *tlptr;
	transitionlist *prev;
	
	tlptr = trans_original;
	prev = NULL;
	while(tlptr != NULL && tlptr->transitionptr->name != name)
	{
		prev = tlptr;
		tlptr = tlptr->next;
	}
	
	if(tlptr == NULL)
	{
		return 0;
	}else
	{
		if(prev == NULL)
		{
			trans_original = trans_original->next;
		}else
		{
			prev->next = tlptr->next;
		}
		delete(tlptr);
		
		return 1;
	}
}





void owfn::replace_trans(transition *start, string name, string placename, int start_or_end)
{
	string newname;
	transitionlist *tlptr;
	place *con;
	bpel *tmp;
	place *prev;
	placelist *plptr;
	
	newname = start->name + name;

	tlptr = trans_in_B;
	while(tlptr != NULL)
	{
		con = tlptr->transitionptr->consumes;
		while(con != NULL)
		{
			if(con->name == start->name)
			{
				con->name = newname;
			}
			con = con->next;
		}

		con = tlptr->transitionptr->produces;
		while(con != NULL)
		{
			if(con->name == start->name)
			{
				con->name = newname;
			}
			con = con->next;
		}
		tlptr = tlptr->next;
	}

	tlptr = trans_in_B;
	while(tlptr != NULL)
	{
		if(tlptr->transitionptr->name == start->name)
		{
			tlptr->transitionptr->name = newname;
			//Bpel Code bleibt leer, da dieser Platz als Original im oWFN den BPEL Code behält
			
			while(tlptr->transitionptr->bpel_code != NULL)
			{
				tmp = tlptr->transitionptr->bpel_code;
				tlptr->transitionptr->bpel_code = tlptr->transitionptr->bpel_code->next;
				tmp->delete_lists();
				delete(tmp);
			}
			
			//empty einfügen, damit diese Transition im flow nicht als sie selbst auftaucht
			//und so vom Nutzer doppelt interpretiert werden könnte, da das Original ja bestehen bleibt
			tlptr->transitionptr->add_bpel(EMPTY, "flow_sync");

			//Alle Kanten zu und von Plätzen nicht in B entfernen
			prev = NULL;
			con = tlptr->transitionptr->produces;
			while(con != NULL)
			{
				plptr = places_in_B;
				while(plptr != NULL && plptr->placeptr->name != con->name)
				{
					plptr = plptr->next;
				}
				if(plptr == NULL)
				{
					if(prev == NULL)
					{
						tlptr->transitionptr->produces = tlptr->transitionptr->produces->next;
					}else
					{
						prev->next = con->next;
					}
					delete(con);
					con = tlptr->transitionptr->produces;
					prev = NULL;
				}else
				{
					prev = con;
					con = con->next;
				}
			}

			prev = NULL;
			con = tlptr->transitionptr->consumes;
			while(con != NULL)
			{
				plptr = places_in_B;
				while(plptr != NULL && plptr->placeptr->name != con->name)
				{
					plptr = plptr->next;
				}
				if(plptr == NULL)
				{
					if(prev == NULL)
					{
						tlptr->transitionptr->consumes = tlptr->transitionptr->consumes->next;
					}else
					{
						prev->next = con->next;
					}
					delete(con);
					con = tlptr->transitionptr->consumes;
					prev = NULL;
				}else
				{
					prev = con;
					con = con->next;
				}
			}

			if(start_or_end)
			{
				tlptr->transitionptr->add_consume(start->name + placename);
			}else
			{
				tlptr->transitionptr->add_produce(start->name + placename);
			}
		}
		tlptr = tlptr->next;
	}
}


int owfn::f_trans_end(transition *start, int max, placelist *selected)
{
	int i;
	placelist *plptr;
	placelist *newplaces;
	placelist *placelistptr;
	placelist *tmp;
	transitionlist *tlptr;
	transitionlist *newtrans;
	transitionlist *translistptr;
	transition *transptr;
	place *con;
	place *prev;
	place *realplace;
	place *placeptr;

	places_in_B = NULL;
	trans_in_B = NULL;
	newplaces = NULL;
	newtrans = NULL;
	//newtrans = new transitionlist(start, newtrans);
	trans_in_B = new transitionlist(start, trans_in_B);

	tmp = selected;
	while(tmp != NULL)
	{
		realplace = return_placeptr(tmp->placeptr->name);
		places_in_B = new placelist(realplace, places_in_B);
		newplaces = new placelist(realplace, newplaces);
		tmp = tmp->next;
	}

	for(i = 0; i <= max; i++)
	{
		plptr = newplaces;
		while(plptr != NULL)
		{
			transptr = transitions;
			while(transptr != NULL)
			{
				con = transptr->consumes;
				while(con != NULL)
				{
					if(con->name == plptr->placeptr->name)
					{
						//if not in list
						translistptr = trans_in_B;
						while(translistptr != NULL && translistptr->transitionptr->name != transptr->name)
						{
							translistptr = translistptr->next;
						}
						if(translistptr == NULL)
						{
							newtrans = new transitionlist(transptr, newtrans);
							trans_in_B = new transitionlist(transptr, trans_in_B);
						}
					}
					con = con->next;
				}
				transptr = transptr->next;
			}
			plptr = plptr->next;
		}

		while(newplaces != NULL)
		{
			plptr = newplaces;
			newplaces = newplaces->next;
			delete(plptr);
		}

		i++;
		if(i <= max)
		{
			tlptr = newtrans;
			while(tlptr != NULL)
			{
				con = tlptr->transitionptr->produces;
				while(con != NULL)
				{
					//if not in list
					placelistptr = places_in_B;
					while(placelistptr != NULL && placelistptr->placeptr->name != con->name)
					{
						placelistptr = placelistptr->next;
					}
					if(placelistptr == NULL)
					{
						realplace = return_placeptr(con->name);
						if(realplace == NULL)
						{
							cout << "Error 1 in function 'f_trans_end': Place '" << con->name << "' vanished. This should not happen...\n";
							exit(EXIT_FAILURE);
						}
						newplaces = new placelist(realplace, newplaces);
						places_in_B = new placelist(realplace, places_in_B);
					}
					con = con->next;
				}
				tlptr = tlptr->next;
			}
		}
		while(newtrans != NULL)
		{
			tlptr = newtrans;
			newtrans = newtrans->next;
			delete(tlptr);
		}
	}

	//Mindestens 2 Plätze
	if(places_in_B != NULL && places_in_B->next != NULL && trans_in_B != NULL && trans_in_B->next != NULL)
	{
		//Kopie des gesamten Teilnetzes erstellen
		create_subnet_copy();
		//Umbau Teilnetz zu Teilnetz mit Kopie des Startplatzes
		//zusätzlicher Platz und Transition ersetzen
		placeptr = new place(start->name + "_flow_subnet");
		places_in_B = new placelist(placeptr, places_in_B);
		replace_trans(start, "_flow_start", "_flow_subnet", 1);
		//starttransition aus Original Teilnetz entfernen, damit dieser nicht mitgelöscht wird
		if(!remove_transition_from_original(start->name))
		{
			cout << "Error 2 in function 'f_trans_end': Transition '" << start->name << "' vanished. This should not happen...\n";
			exit(EXIT_FAILURE);
		}

		if(check_subnet(start->name + "_flow_subnet", 1, start->name, ""))	//der Name des Platzes, der ganz oben steht, als Anfangsmarkierung für die LoLA Datei
		{
			places = new place(start->name + "_flow_subnet", places);

			//alle Kanten von start auf nun nicht mehr vorhandene Plätze in B entfernen
			prev = NULL;
			con = start->produces;
			while(con != NULL)
			{
				plptr = places_in_B;
				while(plptr != NULL && plptr->placeptr->name != con->name)
				{
					plptr = plptr->next;
				}
				if(plptr != NULL)
				{
					if(prev == NULL)
					{
						start->produces = start->produces->next;
					}else
					{
						prev->next = con->next;
					}
					delete(con);
					con = start->produces;
					prev = NULL;
				}else
				{
					prev = con;
					con = con->next;
				}
			}
			//alle Kanten von nun nicht mehr vorhandenen Plätzen in B auf start entfernen
			prev = NULL;
			con = start->consumes;
			while(con != NULL)
			{
				plptr = places_in_B;
				while(plptr != NULL && plptr->placeptr->name != con->name)
				{
					plptr = plptr->next;
				}
				if(plptr != NULL)
				{
					if(prev == NULL)
					{
						start->consumes = start->consumes->next;
					}else
					{
						prev->next = con->next;
					}
					delete(con);
					con = start->consumes;
					prev = NULL;
				}else
				{
					prev = con;
					con = con->next;
				}
			}

			//neue Kante von start auf Ersatzplatz
			start->add_produce(start->name + "_flow_subnet");
			places->bpel_code = transform_subnet(start->name);

			//0 um BPEL Code nicht mit zu löschen, der wird im Netz erhalten
			delete_subnet_copy(0);
			delete_subnet_original();

			start->name = start->name + "_f";

			while(places_in_B != NULL)
			{
				plptr = places_in_B;
				places_in_B = places_in_B->next;
				delete(plptr);
			}

			while(trans_in_B != NULL)
			{
				tlptr = trans_in_B;
				trans_in_B = trans_in_B->next;
				delete(tlptr);
			}
#ifdef VERBOSE
	cout << "    - subnet transition to open end found -\n";
#endif
			return 1;
		}else
		{
			delete_subnet_copy(1);
			
			while(places_in_B != NULL)
			{
				plptr = places_in_B;
				places_in_B = places_in_B->next;
				delete(plptr);
			}

			while(trans_in_B != NULL)
			{
				tlptr = trans_in_B;
				trans_in_B = trans_in_B->next;
				delete(tlptr);
			}
			return 0;
		}
	}else
	{
		while(places_in_B != NULL)
		{
			plptr = places_in_B;
			places_in_B = places_in_B->next;
			delete(plptr);
		}

		while(trans_in_B != NULL)
		{
			tlptr = trans_in_B;
			trans_in_B = trans_in_B->next;
			delete(tlptr);
		}
		
		return 0;
	}
}

int owfn::f_trans_end_a(transition *start, int max, placelist *fixed, placelist *all)
{
	placelist *plptr;
	placelist *tmpplptr;
	placelist *tmp;
	placelist *rest;
	placelist *neu;
	place *placeptr;
	transition *transptr;
	
	rest = NULL;
	
	if(fixed == NULL)
	{
		tmpplptr = all;
		while(tmpplptr != NULL)
		{
			rest = new placelist(tmpplptr->placeptr, rest);
			tmpplptr = tmpplptr->next;
		}
	}else
	{
		plptr = fixed;
		while(plptr->next != NULL)
		{
			plptr = plptr->next;
		}
		//plptr ist nun letztes Element der fixed Plätze
		tmpplptr = all;
		while(tmpplptr != NULL && tmpplptr->placeptr->name != plptr->placeptr->name)
		{
			tmpplptr = tmpplptr->next;
		}
		if(tmpplptr == NULL)
		{
			cout << "Error 1 in function 'f_trans_end_a': Place '" << plptr->placeptr->name << "' vanished. This should not happen...\n";
			exit(EXIT_FAILURE);
		}
		//tmpplptr zeigt nun auf den gleichen Platznamen wie plptr
		//alles nach diesem letzten Element von fixed wird die Liste rest
		tmpplptr = tmpplptr->next;	//ein Element weiter
		while(tmpplptr != NULL)
		{
			rest = new placelist(tmpplptr->placeptr, rest);
			tmpplptr = tmpplptr->next;
		}
	}
	
	if(rest == NULL)
	{
		return 0;	//kein Treffer, nich mal ein weiterer Test
	}

	neu = NULL;
	plptr = rest;
	while(plptr != NULL)
	{
		//neu wird Kopie von fixed + das plptr Element,
		//das an das ende der liste muss, da die reihenfolge wichtig ist
		tmpplptr = fixed;
		while(tmpplptr != NULL)
		{
			if(neu == NULL)
			{
				neu = new placelist(tmpplptr->placeptr, NULL);
			}else
			{
				tmp = neu;
				while(tmp->next != NULL)
				{
					tmp = tmp->next;
				}
				tmp->next = new placelist(tmpplptr->placeptr, NULL);
			}
			tmpplptr = tmpplptr->next;
		}
		if(neu == NULL)
		{
			neu = new placelist(plptr->placeptr, NULL);
		}else
		{
			tmp = neu;
			while(tmp->next != NULL)
			{
				tmp = tmp->next;
			}
			tmp->next = new placelist(plptr->placeptr, NULL);
		}

		//hier testen, ob start + fixed Elemente im Nachbereich + max Tiefe einen Treffer ergeben
		if(f_trans_end(start, max, neu))
		{
			while(neu != NULL)
			{
				tmp = neu;
				neu = neu->next;
				delete(tmp);
			}

			return 1;
		}


		placeptr = places;
		while(placeptr != NULL)
		{
			if(f_trans_to_place(start, max, neu, placeptr))
			{
				while(neu != NULL)
				{
					tmp = neu;
					neu = neu->next;
					delete(tmp);
				}
	
				return 1;
			}
			
			placeptr = placeptr->next;
		}

		transptr = transitions;
		while(transptr != NULL)
		{
			if(f_trans_to_trans(start, max, neu, transptr))
			{
				while(neu != NULL)
				{
					tmp = neu;
					neu = neu->next;
					delete(tmp);
				}
	
				return 1;
			}
			
			transptr = transptr->next;
		}

		//sonst weitere Kombinationen testen:
		if(f_trans_end_a(start, max, neu, all))
		{
			while(neu != NULL)
			{
				tmp = neu;
				neu = neu->next;
				delete(tmp);
			}

			return 1;
		}

		while(neu != NULL)
		{
			tmp = neu;
			neu = neu->next;
			delete(tmp);
		}

		plptr = plptr->next;
	}
	
	//falls alle Tests fehlgeschlagen sind
	return 0;
}

int owfn::f_place_end_a(place *start, int max, transitionlist *fixed, transitionlist *all)
{
	transitionlist *tlptr;
	transitionlist *tl;
	transitionlist *rest;
	transitionlist *neu;
	transitionlist *tmp;
	place *placeptr;
	transition *transptr;
	
	rest = NULL;
	
	if(fixed == NULL)
	{
		tl = all;
		while(tl != NULL)
		{
			rest = new transitionlist(tl->transitionptr, rest);
			tl = tl->next;
		}
	}else
	{
		tlptr = fixed;
		while(tlptr->next != NULL)
		{
			tlptr = tlptr->next;
		}
		//tlptr ist nun letztes Element der fixed Plätze
		tl = all;
		while(tl != NULL && tl->transitionptr->name != tlptr->transitionptr->name)
		{
			tl = tl->next;
		}
		if(tl == NULL)
		{
			cout << "Error 1 in function 'f_place_end_a': Transition '" << tlptr->transitionptr->name << "' vanished. This should not happen...\n";
			exit(EXIT_FAILURE);
		}
		//tl zeigt nun auf die gleiche Transition wie tlptr
		//alles nach diesem letzten Element von fixed wird die Liste rest
		tl = tl->next;	//ein Element weiter
		while(tl != NULL)
		{
			rest = new transitionlist(tl->transitionptr, rest);
			tl = tl->next;
		}
	}
	
	if(rest == NULL)
	{
		return 0;	//kein Treffer, nich mal ein weiterer Test
	}

	neu = NULL;
	tlptr = rest;
	while(tlptr != NULL)
	{
		//neu wird Kopie von fixed + das tlptr Element,
		//das an das ende der liste muss, da die reihenfolge wichtig ist
		tl = fixed;
		while(tl != NULL)
		{
			if(neu == NULL)
			{
				neu = new transitionlist(tl->transitionptr, NULL);
			}else
			{
				tmp = neu;
				while(tmp->next != NULL)
				{
					tmp = tmp->next;
				}
				tmp->next = new transitionlist(tl->transitionptr, NULL);
			}
			tl = tl->next;
		}
		if(neu == NULL)
		{
			neu = new transitionlist(tlptr->transitionptr, NULL);
		}else
		{
			tmp = neu;
			while(tmp->next != NULL)
			{
				tmp = tmp->next;
			}
			tmp->next = new transitionlist(tlptr->transitionptr, NULL);
		}

		//hier testen, ob start + fixed Elemente im Nachbereich + max Tiefe einen Treffer ergeben
		if(f_place_end(start, max, neu))
		{
			while(neu != NULL)
			{
				tmp = neu;
				neu = neu->next;
				delete(tmp);
			}

			return 1;
		}


		placeptr = places;
		while(placeptr != NULL)
		{
			if(f_place_to_place(start, max, neu, placeptr))
			{
				while(neu != NULL)
				{
					tmp = neu;
					neu = neu->next;
					delete(tmp);
				}
	
				return 1;
			}
			
			placeptr = placeptr->next;
		}

		transptr = transitions;
		while(transptr != NULL)
		{
			if(f_place_to_trans(start, max, neu, transptr))
			{
				while(neu != NULL)
				{
					tmp = neu;
					neu = neu->next;
					delete(tmp);
				}
	
				return 1;
			}
			
			transptr = transptr->next;
		}

		if(f_place_circle(start, max, neu))
		{
			while(neu != NULL)
			{
				tmp = neu;
				neu = neu->next;
				delete(tmp);
			}
			
			return 1;
		}

		//sonst weitere Kombinationen testen:
		if(f_place_end_a(start, max, neu, all))
		{
			while(neu != NULL)
			{
				tmp = neu;
				neu = neu->next;
				delete(tmp);
			}

			return 1;
		}

		while(neu != NULL)
		{
			tmp = neu;
			neu = neu->next;
			delete(tmp);
		}

		tlptr = tlptr->next;
	}
	
	//falls alle Tests fehlgeschlagen sind
	return 0;
}


int owfn::f_place_end(place *start, int max, transitionlist *selected)
{
	//funktion erzeugt Teilnetz mit allen Knoten im Nachbereich von Start

	int i;
	placelist *plptr;
	placelist *newplaces;
	placelist *placelistptr;
	transitionlist *tlptr;
	transitionlist *newtrans;
	transitionlist *translistptr;
	transitionlist *tmp;
	transition *transptr;
	place *con;
	place *realplace;

	places_in_B = NULL;
	trans_in_B = NULL;
	newplaces = NULL;
	newtrans = NULL;
//	newplaces = new placelist(start, newplaces);
	places_in_B = new placelist(start, places_in_B);
	
	tmp = selected;
	while(tmp != NULL)
	{
		trans_in_B = new transitionlist(tmp->transitionptr, trans_in_B);
		newtrans = new transitionlist(tmp->transitionptr, newtrans);
		tmp = tmp->next;
	}

	for(i = 0; i <= max; i++)	//bedingung überwachen
	{
		tlptr = newtrans;
		while(tlptr != NULL)
		{
			con = tlptr->transitionptr->produces;
			while(con != NULL)
			{
				//if not in list
				placelistptr = places_in_B;
				while(placelistptr != NULL && placelistptr->placeptr->name != con->name)
				{
					placelistptr = placelistptr->next;
				}
				if(placelistptr == NULL)
				{
					realplace = return_placeptr(con->name);
					if(realplace == NULL)
					{
						cout << "Error 1 in function 'f_place_end': Place '" << con->name << "' vanished. This should not happen...\n";
						exit(EXIT_FAILURE);
					}
					newplaces = new placelist(realplace, newplaces);
					places_in_B = new placelist(realplace, places_in_B);
				}
				con = con->next;
			}
			tlptr = tlptr->next;
		}

		while(newtrans != NULL)
		{
			tlptr = newtrans;
			newtrans = newtrans->next;
			delete(tlptr);
		}

		i++;
		if(i <= max)
		{
			plptr = newplaces;
			while(plptr != NULL)
			{
				transptr = transitions;
				while(transptr != NULL)
				{
					con = transptr->consumes;
					while(con != NULL)
					{
						if(con->name == plptr->placeptr->name)
						{
							//if not in list
							translistptr = trans_in_B;
							while(translistptr != NULL && translistptr->transitionptr != transptr)
							{
								translistptr = translistptr->next;
							}
							if(translistptr == NULL)
							{
								newtrans = new transitionlist(transptr, newtrans);
								trans_in_B = new transitionlist(transptr, trans_in_B);
							}
						}
						con = con->next;
					}
					transptr = transptr->next;
				}
				plptr = plptr->next;
			}
		}
		while(newplaces != NULL)
		{
			plptr = newplaces;
			newplaces = newplaces->next;
			delete(plptr);
		}
	}

	//Mindestens 2 Plätze
	if(places_in_B != NULL && places_in_B->next != NULL && trans_in_B != NULL && trans_in_B->next != NULL)
	{
		//Kopie des gesamten Teilnetzes erstellen
		create_subnet_copy();
		//Umbau Teilnetz zu Teilnetz mit Kopie des Startplatzes
		if(!(replace_place(start, "_flow_start")))
		{
			cout << "Error 2 in function 'f_place_end': Place '" << start->name << "' vanished. This should not happen...\n";
			exit(EXIT_FAILURE);
		}
		//startplatz aus Original Teilnetz entfernen, damit dieser nicht mitgelöscht wird
		remove_place_from_original(start->name);

		if(check_subnet(start->name + "_flow_start", 0, "", ""))
		{
			transitions = new transition(start->name + "_flow_subnet", transitions);
			transitions->add_consume(start->name);
			transitions->bpel_code = transform_subnet(start->name);
			
			rename_place(start->name, "_f");

			//0 um BPEL Code nicht mit zu löschen, der wird im Netz erhalten
			delete_subnet_copy(0);
			delete_subnet_original();

			while(places_in_B != NULL)
			{
				plptr = places_in_B;
				places_in_B = places_in_B->next;
				delete(plptr);
			}

			while(trans_in_B != NULL)
			{
				tlptr = trans_in_B;
				trans_in_B = trans_in_B->next;
				delete(tlptr);
			}
			
#ifdef VERBOSE
	cout << "    - subnet place to open end found -\n";
#endif
			return 1;
		}else
		{
			delete_subnet_copy(1);

			while(places_in_B != NULL)
			{
				plptr = places_in_B;
				places_in_B = places_in_B->next;
				delete(plptr);
			}

			while(trans_in_B != NULL)
			{
				tlptr = trans_in_B;
				trans_in_B = trans_in_B->next;
				delete(tlptr);
			}
			return 0;
		}
	}else
	{
		while(places_in_B != NULL)
		{
			plptr = places_in_B;
			places_in_B = places_in_B->next;
			delete(plptr);
		}

		while(trans_in_B != NULL)
		{
			tlptr = trans_in_B;
			trans_in_B = trans_in_B->next;
			delete(tlptr);
		}

		return 0;
	}
}


int owfn::f_place_to_place(place *start, int max, transitionlist *selected, place *ende)
{
	//funktion erzeugt Teilnetz mit allen Knoten im Nachbereich von Start bis Ende

	int i;
	int counter;
	placelist *plptr;
	placelist *newplaces;
	placelist *placelistptr;
	transitionlist *tlptr;
	transitionlist *newtrans;
	transitionlist *translistptr;
	transitionlist *tmp;
	transition *transptr;
	place *con;
	place *realplace;

	places_in_B = NULL;
	trans_in_B = NULL;
	newplaces = NULL;
	newtrans = NULL;
//	newplaces = new placelist(start, newplaces);
	places_in_B = new placelist(start, places_in_B);
	
	tmp = selected;
	while(tmp != NULL)
	{
		trans_in_B = new transitionlist(tmp->transitionptr, trans_in_B);
		newtrans = new transitionlist(tmp->transitionptr, newtrans);
		tmp = tmp->next;
	}

	for(i = 0; i <= max; i++)	//bedingung überwachen
	{
		tlptr = newtrans;
		while(tlptr != NULL)
		{
			con = tlptr->transitionptr->produces;
			if(con == NULL)
			{
				//Eine Transition hat keine Nachfolgeplätze, nicht erlaubt
				while(newtrans != NULL)
				{
					tlptr = newtrans;
					newtrans = newtrans->next;
					delete(tlptr);
				}
				
				while(newplaces != NULL)
				{
					plptr = newplaces;
					newplaces = newplaces->next;
					delete(plptr);
				}
				
				while(places_in_B != NULL)
				{
					plptr = places_in_B;
					places_in_B = places_in_B->next;
					delete(plptr);
				}
	
				while(trans_in_B != NULL)
				{
					tlptr = trans_in_B;
					trans_in_B = trans_in_B->next;
					delete(tlptr);
				}
				return 0;
			}
			while(con != NULL)
			{
				//if not in list
				placelistptr = places_in_B;
				while(placelistptr != NULL && placelistptr->placeptr->name != con->name)
				{
					placelistptr = placelistptr->next;
				}
				if(placelistptr == NULL)
				{
					realplace = return_placeptr(con->name);
					if(realplace == NULL)
					{
						cout << "Error 1 in function 'f_place_to_place': Place '" << con->name << "' vanished. This should not happen...\n";
						exit(EXIT_FAILURE);
					}
					newplaces = new placelist(realplace, newplaces);
					places_in_B = new placelist(realplace, places_in_B);
				}
				con = con->next;
			}
			tlptr = tlptr->next;
		}

		while(newtrans != NULL)
		{
			tlptr = newtrans;
			newtrans = newtrans->next;
			delete(tlptr);
		}

		i++;
		if(i <= max)
		{
			plptr = newplaces;
			while(plptr != NULL)
			{
				if(plptr->placeptr->name != ende->name)	//keine Transitionen nach Ende betrachten
				{
					counter = 0;
					transptr = transitions;
					while(transptr != NULL)
					{
						con = transptr->consumes;
						while(con != NULL)
						{
							if(con->name == plptr->placeptr->name)
							{
								counter++;
								//if not in list
								translistptr = trans_in_B;
								while(translistptr != NULL && translistptr->transitionptr != transptr)
								{
									translistptr = translistptr->next;
								}
								if(translistptr == NULL)
								{
									newtrans = new transitionlist(transptr, newtrans);
									trans_in_B = new transitionlist(transptr, trans_in_B);
								}
							}
							con = con->next;
						}
						transptr = transptr->next;
					}
					if(counter == 0)
					{
						//Eine Platz hat keine Nachfolgetransitionen, nicht erlaubt
						while(newtrans != NULL)
						{
							tlptr = newtrans;
							newtrans = newtrans->next;
							delete(tlptr);
						}
						
						while(newplaces != NULL)
						{
							plptr = newplaces;
							newplaces = newplaces->next;
							delete(plptr);
						}
						
						while(places_in_B != NULL)
						{
							plptr = places_in_B;
							places_in_B = places_in_B->next;
							delete(plptr);
						}
			
						while(trans_in_B != NULL)
						{
							tlptr = trans_in_B;
							trans_in_B = trans_in_B->next;
							delete(tlptr);
						}
						
						return 0;
					}
				}
				plptr = plptr->next;
			}
		}
		while(newplaces != NULL)
		{
			plptr = newplaces;
			newplaces = newplaces->next;
			delete(plptr);
		}
	}

	//Mindestens 2 Plätze und 2 Transitionen
	if(places_in_B != NULL && places_in_B->next != NULL && trans_in_B != NULL && trans_in_B->next != NULL)
	{
		//Kopie des gesamten Teilnetzes erstellen
		create_subnet_copy();
		//Umbau Teilnetz zu Teilnetz mit Kopie des Start- und Endeplatzes
		if(!(replace_place(start, "_flow_start")))
		{
			cout << "Error 2 in function 'f_place_to_place': Place '" << start->name << "' vanished. This should not happen...\n";
			exit(EXIT_FAILURE);
		}
		if(!(replace_place(ende, "_flow_ende")))
		{
			//kein Weg hat den Endplatz gefunden
			delete_subnet_copy(1);
			
			while(places_in_B != NULL)
			{
				plptr = places_in_B;
				places_in_B = places_in_B->next;
				delete(plptr);
			}

			while(trans_in_B != NULL)
			{
				tlptr = trans_in_B;
				trans_in_B = trans_in_B->next;
				delete(tlptr);
			}
			
			return 0;
		}
		//Start- und Endeplatz aus Original Teilnetz entfernen, damit dieser nicht mitgelöscht wird
		remove_place_from_original(start->name);
		remove_place_from_original(ende->name);

		if(check_subnet(start->name + "_flow_start", 0, "", ""))
		{
			transitions = new transition(start->name + "_flow_subnet", transitions);
			transitions->add_consume(start->name);
			transitions->add_produce(ende->name);
			transitions->bpel_code = transform_subnet(start->name);
			
			rename_place(start->name, "_f");

			//0 um BPEL Code nicht mit zu löschen, der wird im Netz erhalten
			delete_subnet_copy(0);
			delete_subnet_original();

			while(places_in_B != NULL)
			{
				plptr = places_in_B;
				places_in_B = places_in_B->next;
				delete(plptr);
			}

			while(trans_in_B != NULL)
			{
				tlptr = trans_in_B;
				trans_in_B = trans_in_B->next;
				delete(tlptr);
			}

#ifdef VERBOSE
	cout << "    - subnet place to place found -\n";
#endif
			return 1;
		}else
		{
			delete_subnet_copy(1);

			while(places_in_B != NULL)
			{
				plptr = places_in_B;
				places_in_B = places_in_B->next;
				delete(plptr);
			}

			while(trans_in_B != NULL)
			{
				tlptr = trans_in_B;
				trans_in_B = trans_in_B->next;
				delete(tlptr);
			}
			return 0;
		}
	}else
	{
		while(places_in_B != NULL)
		{
			plptr = places_in_B;
			places_in_B = places_in_B->next;
			delete(plptr);
		}

		while(trans_in_B != NULL)
		{
			tlptr = trans_in_B;
			trans_in_B = trans_in_B->next;
			delete(tlptr);
		}

		return 0;
	}
}


int owfn::f_place_circle(place *start, int max, transitionlist *selected)
{
	//funktion erzeugt Teilnetz mit allen Knoten im Nachbereich von Start bis Ende

	int i;
	int counter;
	placelist *plptr;
	placelist *newplaces;
	placelist *placelistptr;
	transitionlist *tlptr;
	transitionlist *newtrans;
	transitionlist *translistptr;
	transitionlist *tmp;
	transition *transptr;
	place *con;
	place *realplace;

	places_in_B = NULL;
	trans_in_B = NULL;
	newplaces = NULL;
	newtrans = NULL;
//	newplaces = new placelist(start, newplaces);
	places_in_B = new placelist(start, places_in_B);
	
	tmp = selected;
	while(tmp != NULL)
	{
		trans_in_B = new transitionlist(tmp->transitionptr, trans_in_B);
		newtrans = new transitionlist(tmp->transitionptr, newtrans);
		tmp = tmp->next;
	}

	for(i = 0; i <= max; i++)	//bedingung überwachen
	{
		tlptr = newtrans;
		while(tlptr != NULL)
		{
			con = tlptr->transitionptr->produces;
			if(con == NULL)
			{
				//Eine Transition hat keine Nachfolgeplätze, nicht erlaubt
				while(newtrans != NULL)
				{
					tlptr = newtrans;
					newtrans = newtrans->next;
					delete(tlptr);
				}
				
				while(newplaces != NULL)
				{
					plptr = newplaces;
					newplaces = newplaces->next;
					delete(plptr);
				}
				
				while(places_in_B != NULL)
				{
					plptr = places_in_B;
					places_in_B = places_in_B->next;
					delete(plptr);
				}
	
				while(trans_in_B != NULL)
				{
					tlptr = trans_in_B;
					trans_in_B = trans_in_B->next;
					delete(tlptr);
				}
				return 0;
			}
			while(con != NULL)
			{
				//if not in list
				placelistptr = places_in_B;
				while(placelistptr != NULL && placelistptr->placeptr->name != con->name)
				{
					placelistptr = placelistptr->next;
				}
				if(placelistptr == NULL)
				{
					realplace = return_placeptr(con->name);
					if(realplace == NULL)
					{
						cout << "Error 1 in function 'f_place_circle': Place '" << con->name << "' vanished. This should not happen...\n";
						exit(EXIT_FAILURE);
					}
					newplaces = new placelist(realplace, newplaces);
					places_in_B = new placelist(realplace, places_in_B);
				}
				con = con->next;
			}
			tlptr = tlptr->next;
		}

		while(newtrans != NULL)
		{
			tlptr = newtrans;
			newtrans = newtrans->next;
			delete(tlptr);
		}

		i++;
		if(i <= max)
		{
			plptr = newplaces;
			while(plptr != NULL)
			{
				if(plptr->placeptr->name != start->name)	//keine Transitionen nach Ende betrachten
				{
					counter = 0;
					transptr = transitions;
					while(transptr != NULL)
					{
						con = transptr->consumes;
						while(con != NULL)
						{
							if(con->name == plptr->placeptr->name)
							{
								counter++;
								//if not in list
								translistptr = trans_in_B;
								while(translistptr != NULL && translistptr->transitionptr != transptr)
								{
									translistptr = translistptr->next;
								}
								if(translistptr == NULL)
								{
									newtrans = new transitionlist(transptr, newtrans);
									trans_in_B = new transitionlist(transptr, trans_in_B);
								}
							}
							con = con->next;
						}
						transptr = transptr->next;
					}
					if(counter == 0)
					{
						//Eine Platz hat keine Nachfolgetransitionen, nicht erlaubt
						while(newtrans != NULL)
						{
							tlptr = newtrans;
							newtrans = newtrans->next;
							delete(tlptr);
						}
						
						while(newplaces != NULL)
						{
							plptr = newplaces;
							newplaces = newplaces->next;
							delete(plptr);
						}
						
						while(places_in_B != NULL)
						{
							plptr = places_in_B;
							places_in_B = places_in_B->next;
							delete(plptr);
						}
			
						while(trans_in_B != NULL)
						{
							tlptr = trans_in_B;
							trans_in_B = trans_in_B->next;
							delete(tlptr);
						}
						return 0;
					}
				}
				plptr = plptr->next;
			}
		}
		while(newplaces != NULL)
		{
			plptr = newplaces;
			newplaces = newplaces->next;
			delete(plptr);
		}
	}

	//Mindestens 2 Plätze
	if(places_in_B != NULL && places_in_B->next != NULL && trans_in_B != NULL && trans_in_B->next != NULL)
	{
		//Kopie des gesamten Teilnetzes erstellen
		create_subnet_copy();
		places_in_B = new placelist(new place(start->name + "_flow_ende"), places_in_B);
		//Umbau Teilnetz zu Teilnetz mit Kopie des Start- und Endeplatzes
		//start ist nun zweimal in der Platzmenge enthalten (falls wirklich ein Zyklus gefunden wurde)
		if(!(replace_place_circle(start, "_flow_start", "_flow_ende")))
		{
			cout << "Error 2 in function 'f_place_circle': Place '" << start->name << "' vanished. This should not happen...\n";
			exit(EXIT_FAILURE);
		}
	
		//Start- und Endeplatz aus Original Teilnetz entfernen, damit dieser nicht mitgelöscht wird, wenn das ganze Teilnetz ersetzt wird
		//Original Teilnetz ist eine Liste, die markiert, welche Knoten des oWFN gerade betrachtet werden
		//und nach der Transformation der Kopie (places_in_B und trans_in_B) gelöscht werden müssen
		remove_place_from_original(start->name);

		if(check_subnet(start->name + "_flow_start", 0, "", ""))
		{
			transitions = new transition(start->name + "_flow_subnet", transitions);
			transitions->add_consume(start->name);
			transitions->add_produce(start->name);
			transitions->bpel_code = transform_subnet(start->name);

			rename_place(start->name, "_f");

			//0 um BPEL Code nicht mit zu löschen, der wird im Netz erhalten
			delete_subnet_copy(0);
			delete_subnet_original();

			while(places_in_B != NULL)
			{
				plptr = places_in_B;
				places_in_B = places_in_B->next;
				delete(plptr);
			}

			while(trans_in_B != NULL)
			{
				tlptr = trans_in_B;
				trans_in_B = trans_in_B->next;
				delete(tlptr);
			}

#ifdef VERBOSE
	cout << "    - subnet circle found -\n";
#endif
			return 1;
		}else
		{
			delete_subnet_copy(1);

			while(places_in_B != NULL)
			{
				plptr = places_in_B;
				places_in_B = places_in_B->next;
				delete(plptr);
			}

			while(trans_in_B != NULL)
			{
				tlptr = trans_in_B;
				trans_in_B = trans_in_B->next;
				delete(tlptr);
			}

			return 0;
		}
	}else
	{
		while(places_in_B != NULL)
		{
			plptr = places_in_B;
			places_in_B = places_in_B->next;
			delete(plptr);
		}

		while(trans_in_B != NULL)
		{
			tlptr = trans_in_B;
			trans_in_B = trans_in_B->next;
			delete(tlptr);
		}

		return 0;
	}
}



int owfn::replace_place_circle(place *start, string startname, string endname)
{
	string newstart;
	string newende;
	transitionlist *tlptr;
	placelist *plptr;
	place *con;
	bpel *tmp;
	int found;
	
	newstart = start->name + startname;
	newende = start->name + endname;
	
	tlptr = trans_in_B;
	while(tlptr != NULL)
	{
		con = tlptr->transitionptr->consumes;
		while(con != NULL)
		{
			if(con->name == start->name)
			{
				con->name = newstart;
			}
			con = con->next;
		}

		con = tlptr->transitionptr->produces;
		while(con != NULL)
		{
			if(con->name == start->name)
			{
				con->name = newende;
			}
			con = con->next;
		}
		tlptr = tlptr->next;
	}

	found = 0;
	plptr = places_in_B;
	while(plptr != NULL && !found)
	{
		if(plptr->placeptr->name == start->name)
		{
			found = 1;
			plptr->placeptr->name = newstart;

			//Bpel Code bleibt leer, da dieser Platz als Original im oWFN den BPEL Code behält
			
			while(plptr->placeptr->bpel_code != NULL)
			{
				tmp = plptr->placeptr->bpel_code;
				plptr->placeptr->bpel_code = plptr->placeptr->bpel_code->next;
				delete(tmp);
			}
		}
		plptr = plptr->next;
	}

	return found;
}


int owfn::f_place_to_trans(place *start, int max, transitionlist *selected, transition *ende)
{
	//funktion erzeugt Teilnetz mit allen Knoten im Nachbereich von Start bis Ende

	int i;
	int counter;
	placelist *plptr;
	placelist *newplaces;
	placelist *placelistptr;
	transitionlist *tlptr;
	transitionlist *newtrans;
	transitionlist *translistptr;
	transitionlist *tmp;
	transition *transptr;
	place *con;
	place *prev;
	place *realplace;

	places_in_B = NULL;
	trans_in_B = NULL;
	newplaces = NULL;
	newtrans = NULL;
//	newplaces = new placelist(start, newplaces);
	places_in_B = new placelist(start, places_in_B);
	
	tmp = selected;
	while(tmp != NULL)
	{
		trans_in_B = new transitionlist(tmp->transitionptr, trans_in_B);
		newtrans = new transitionlist(tmp->transitionptr, newtrans);
		tmp = tmp->next;
	}

	for(i = 0; i <= max; i++)	//bedingung überwachen
	{
		tlptr = newtrans;
		while(tlptr != NULL)
		{
			if(tlptr->transitionptr->name != ende->name)	//keinen Platz nach Ende betrachten
			{
				con = tlptr->transitionptr->produces;
				if(con == NULL)
				{
					//Eine Transition hat keine Nachfolgeplätze, nicht erlaubt
					while(newtrans != NULL)
					{
						tlptr = newtrans;
						newtrans = newtrans->next;
						delete(tlptr);
					}
					
					while(newplaces != NULL)
					{
						plptr = newplaces;
						newplaces = newplaces->next;
						delete(plptr);
					}
					
					while(places_in_B != NULL)
					{
						plptr = places_in_B;
						places_in_B = places_in_B->next;
						delete(plptr);
					}
		
					while(trans_in_B != NULL)
					{
						tlptr = trans_in_B;
						trans_in_B = trans_in_B->next;
						delete(tlptr);
					}
					return 0;
				}
				while(con != NULL)
				{
					//if not in list
					placelistptr = places_in_B;
					while(placelistptr != NULL && placelistptr->placeptr->name != con->name)
					{
						placelistptr = placelistptr->next;
					}
					if(placelistptr == NULL)
					{
						realplace = return_placeptr(con->name);
						if(realplace == NULL)
						{
							cout << "Error 1 in function 'f_place_to_trans': Place '" << con->name << "' vanished. This should not happen...\n";
							exit(EXIT_FAILURE);
						}
						newplaces = new placelist(realplace, newplaces);
						places_in_B = new placelist(realplace, places_in_B);
					}
					con = con->next;
				}
			}
			tlptr = tlptr->next;
		}

		while(newtrans != NULL)
		{
			tlptr = newtrans;
			newtrans = newtrans->next;
			delete(tlptr);
		}

		i++;
		if(i <= max)
		{
			plptr = newplaces;
			while(plptr != NULL)
			{
				counter = 0;
				transptr = transitions;
				while(transptr != NULL)
				{
					con = transptr->consumes;
					while(con != NULL)
					{
						if(con->name == plptr->placeptr->name)
						{
							counter++;
							//if not in list
							translistptr = trans_in_B;
							while(translistptr != NULL && translistptr->transitionptr != transptr)
							{
								translistptr = translistptr->next;
							}
							if(translistptr == NULL)
							{
								newtrans = new transitionlist(transptr, newtrans);
								trans_in_B = new transitionlist(transptr, trans_in_B);
							}
						}
						con = con->next;
					}
					transptr = transptr->next;
				}
				if(counter == 0)
				{
					//Eine Platz hat keine Nachfolgetransitionen, nicht erlaubt
					while(newtrans != NULL)
					{
						tlptr = newtrans;
						newtrans = newtrans->next;
						delete(tlptr);
					}
					
					while(newplaces != NULL)
					{
						plptr = newplaces;
						newplaces = newplaces->next;
						delete(plptr);
					}
					
					while(places_in_B != NULL)
					{
						plptr = places_in_B;
						places_in_B = places_in_B->next;
						delete(plptr);
					}
		
					while(trans_in_B != NULL)
					{
						tlptr = trans_in_B;
						trans_in_B = trans_in_B->next;
						delete(tlptr);
					}
					return 0;
				}
				plptr = plptr->next;
			}
		}
		while(newplaces != NULL)
		{
			plptr = newplaces;
			newplaces = newplaces->next;
			delete(plptr);
		}
	}

	//Mindestens 2 Plätze und 2 Transitionen
	if(places_in_B != NULL && places_in_B->next != NULL && trans_in_B != NULL && trans_in_B->next != NULL && (places_in_B->next->next != NULL || trans_in_B->next->next != NULL) )
	{
		//Kopie des gesamten Teilnetzes erstellen
		create_subnet_copy();
		//Umbau Teilnetz zu Teilnetz mit Kopie des Start- und Endeplatzes
		if(!(replace_place(start, "_flow_start")))
		{
			cout << "Error 2 in function 'f_place_to_trans': Place '" << start->name << "' vanished. This should not happen...\n";
			exit(EXIT_FAILURE);
		}
		tlptr = trans_in_B;
		while(tlptr != NULL && tlptr->transitionptr->name != ende->name)
		{
			tlptr = tlptr->next;
		}
		if(tlptr == NULL)
		{
			//kein Weg hat die Endtransition gefunden
			delete_subnet_copy(1);
			
			while(places_in_B != NULL)
			{
				plptr = places_in_B;
				places_in_B = places_in_B->next;
				delete(plptr);
			}

			while(trans_in_B != NULL)
			{
				tlptr = trans_in_B;
				trans_in_B = trans_in_B->next;
				delete(tlptr);
			}
			return 0;
		}
		replace_trans(ende, "_flow_end", "_flow_end", 0);
		places_in_B = new placelist(new place(ende->name + "_flow_end"), places_in_B);
		
		//Startplatz und Endetransition aus Original Teilnetz entfernen, damit diese nicht mitgelöscht werden
		remove_place_from_original(start->name);
		if(!remove_transition_from_original(ende->name))
		{
			return 0;
		}

		if(check_subnet(start->name + "_flow_start", 0, "", ""))
		{
			transitions = new transition(start->name + "_flow_subnet", transitions);
			transitions->add_consume(start->name);
			transitions->add_produce(start->name + "_flow_subnet_end");
			transitions->bpel_code = transform_subnet(start->name);
			places = new place(start->name + "_flow_subnet_end", places);


			//es müssen alle Kanten entfernt werden, die aus B auf ende gezeigt haben
			prev = NULL;
			con = ende->consumes;
			while(con != NULL)
			{
				plptr = places_in_B;
				while(plptr != NULL && plptr->placeptr->name != con->name)
				{
					plptr = plptr->next;
				}
				if(plptr != NULL)
				{
					if(prev == NULL)
					{
						ende->consumes = ende->consumes->next;
					}else
					{
						prev->next = con->next;
					}
					delete(con);
					con = ende->consumes;
					prev = NULL;
				}else
				{
					prev = con;
					con = con->next;
				}
			}
			//stattdessen eine neue:
			ende->add_consume(start->name + "_flow_subnet_end");

			rename_place(start->name, "_f");

			//0 um BPEL Code nicht mit zu löschen, der wird im Netz erhalten
			delete_subnet_copy(0);
			delete_subnet_original();

			while(places_in_B != NULL)
			{
				plptr = places_in_B;
				places_in_B = places_in_B->next;
				delete(plptr);
			}

			while(trans_in_B != NULL)
			{
				tlptr = trans_in_B;
				trans_in_B = trans_in_B->next;
				delete(tlptr);
			}

#ifdef VERBOSE
	cout << "    - subnet place to trans found -\n";
#endif

			return 1;
		}else
		{
			delete_subnet_copy(1);

			while(places_in_B != NULL)
			{
				plptr = places_in_B;
				places_in_B = places_in_B->next;
				delete(plptr);
			}

			while(trans_in_B != NULL)
			{
				tlptr = trans_in_B;
				trans_in_B = trans_in_B->next;
				delete(tlptr);
			}
			return 0;
		}
	}else
	{
		while(places_in_B != NULL)
		{
			plptr = places_in_B;
			places_in_B = places_in_B->next;
			delete(plptr);
		}

		while(trans_in_B != NULL)
		{
			tlptr = trans_in_B;
			trans_in_B = trans_in_B->next;
			delete(tlptr);
		}

		return 0;
	}
}


int owfn::f_trans_to_place(transition *start, int max, placelist *selected, place *ende)
{
	//funktion erzeugt Teilnetz mit allen Knoten im Nachbereich von Start bis Ende

	int i;
	int counter;
	placelist *plptr;
	placelist *newplaces;
	placelist *placelistptr;
	transitionlist *tlptr;
	transitionlist *newtrans;
	transitionlist *translistptr;
	placelist *tmp;
	transition *transptr;
	place *con;
	place *prev;
	place *realplace;

	places_in_B = NULL;
	trans_in_B = NULL;
	newplaces = NULL;
	newtrans = NULL;
	trans_in_B = new transitionlist(start, trans_in_B);

	tmp = selected;
	while(tmp != NULL)
	{
		realplace = return_placeptr(tmp->placeptr->name);
		places_in_B = new placelist(realplace, places_in_B);
		newplaces = new placelist(realplace, newplaces);
		tmp = tmp->next;
	}

	for(i = 0; i <= max; i++)	//bedingung überwachen
	{
		plptr = newplaces;
		while(plptr != NULL)
		{
			if(plptr->placeptr->name != ende->name)	//keine Transitionen nach Ende betrachten
			{
				counter = 0;
				transptr = transitions;
				while(transptr != NULL)
				{
					con = transptr->consumes;
					while(con != NULL)
					{
						if(con->name == plptr->placeptr->name)
						{
							counter++;
							//if not in list
							translistptr = trans_in_B;
							while(translistptr != NULL && translistptr->transitionptr != transptr)
							{
								translistptr = translistptr->next;
							}
							if(translistptr == NULL)
							{
								newtrans = new transitionlist(transptr, newtrans);
								trans_in_B = new transitionlist(transptr, trans_in_B);
							}
						}
						con = con->next;
					}
					transptr = transptr->next;
				}
				if(counter == 0)
				{
					//Eine Platz hat keine Nachfolgetransitionen, nicht erlaubt
					while(newtrans != NULL)
					{
						tlptr = newtrans;
						newtrans = newtrans->next;
						delete(tlptr);
					}
					
					while(newplaces != NULL)
					{
						plptr = newplaces;
						newplaces = newplaces->next;
						delete(plptr);
					}
					
					while(places_in_B != NULL)
					{
						plptr = places_in_B;
						places_in_B = places_in_B->next;
						delete(plptr);
					}
		
					while(trans_in_B != NULL)
					{
						tlptr = trans_in_B;
						trans_in_B = trans_in_B->next;
						delete(tlptr);
					}
					
					return 0;
				}
			}
			plptr = plptr->next;
		}

		while(newplaces != NULL)
		{
			plptr = newplaces;
			newplaces = newplaces->next;
			delete(plptr);
		}

		i++;
		if(i <= max)
		{
			tlptr = newtrans;
			while(tlptr != NULL)
			{
				con = tlptr->transitionptr->produces;
				if(con == NULL)
				{
					//Eine Transition hat keine Nachfolgeplätze, nicht erlaubt
					while(newtrans != NULL)
					{
						tlptr = newtrans;
						newtrans = newtrans->next;
						delete(tlptr);
					}
					
					while(newplaces != NULL)
					{
						plptr = newplaces;
						newplaces = newplaces->next;
						delete(plptr);
					}
					
					while(places_in_B != NULL)
					{
						plptr = places_in_B;
						places_in_B = places_in_B->next;
						delete(plptr);
					}
		
					while(trans_in_B != NULL)
					{
						tlptr = trans_in_B;
						trans_in_B = trans_in_B->next;
						delete(tlptr);
					}
					return 0;
				}
				while(con != NULL)
				{
					//if not in list
					placelistptr = places_in_B;
					while(placelistptr != NULL && placelistptr->placeptr->name != con->name)
					{
						placelistptr = placelistptr->next;
					}
					if(placelistptr == NULL)
					{
						realplace = return_placeptr(con->name);
						if(realplace == NULL)
						{
							cout << "Error 1 in function 'f_trans_to_place': Place '" << con->name << "' vanished. This should not happen...\n";
							exit(EXIT_FAILURE);
						}
						newplaces = new placelist(realplace, newplaces);
						places_in_B = new placelist(realplace, places_in_B);
					}
					con = con->next;
				}
				tlptr = tlptr->next;
			}
		}

		while(newtrans != NULL)
		{
			tlptr = newtrans;
			newtrans = newtrans->next;
			delete(tlptr);
		}
	}

	//Mindestens 2 Plätze und 2 Transitionen
	if(places_in_B != NULL && places_in_B->next != NULL && trans_in_B != NULL && trans_in_B->next != NULL && (trans_in_B->next->next != NULL || places_in_B->next->next != NULL) )
	{
		//Kopie des gesamten Teilnetzes erstellen
		create_subnet_copy();

		//zusätzlicher Platz und Transition ersetzen
		places_in_B = new placelist(new place(start->name + "_flow_subnet"), places_in_B);
		replace_trans(start, "_flow_start", "_flow_subnet", 1);
		//starttransition aus Original Teilnetz entfernen, damit dieser nicht mitgelöscht wird
		if(!remove_transition_from_original(start->name))
		{
			cout << "Error 2 in function 'f_trans_to_place': Transition '" << start->name << "' vanished. This should not happen...\n";
			exit(EXIT_FAILURE);
		}

		if(!(replace_place(ende, "_flow_ende")))
		{
			//kein Weg hat den Endplatz gefunden
			delete_subnet_copy(1);
			
			while(places_in_B != NULL)
			{
				plptr = places_in_B;
				places_in_B = places_in_B->next;
				delete(plptr);
			}

			while(trans_in_B != NULL)
			{
				tlptr = trans_in_B;
				trans_in_B = trans_in_B->next;
				delete(tlptr);
			}
			
			return 0;
		}
		//Start- und Endeplatz aus Original Teilnetz entfernen, damit dieser nicht mitgelöscht wird
		remove_place_from_original(ende->name);

		if(check_subnet(start->name + "_flow_subnet", 1, start->name, ""))	//der Name des Platzes, der ganz oben steht, als Anfangsmarkierung für die LoLA Datei
		{
			places = new place(start->name + "_flow_subnet", places);
			
			//alle Kanten von start auf nun nicht mehr vorhandene Plätze in B entfernen
			prev = NULL;
			con = start->produces;
			while(con != NULL)
			{
				plptr = places_in_B;
				while(plptr != NULL && plptr->placeptr->name != con->name)
				{
					plptr = plptr->next;
				}
				if(plptr != NULL)
				{
					if(prev == NULL)
					{
						start->produces = start->produces->next;
					}else
					{
						prev->next = con->next;
					}
					delete(con);
					con = start->produces;
					prev = NULL;
				}else
				{
					prev = con;
					con = con->next;
				}
			}
			//alle Kanten von nun nicht mehr vorhandenen Plätzen in B auf start entfernen
			prev = NULL;
			con = start->consumes;
			while(con != NULL)
			{
				plptr = places_in_B;
				while(plptr != NULL && plptr->placeptr->name != con->name)
				{
					plptr = plptr->next;
				}
				if(plptr != NULL)
				{
					if(prev == NULL)
					{
						start->consumes = start->consumes->next;
					}else
					{
						prev->next = con->next;
					}
					delete(con);
					con = start->consumes;
					prev = NULL;
				}else
				{
					prev = con;
					con = con->next;
				}
			}
			
			start->add_produce(start->name + "_flow_subnet");
			places->bpel_code = transform_subnet(start->name);
			
			transitions = new transition(start->name + "_flow_subnet", transitions);
			transitions->add_consume(start->name + "_flow_subnet");
			transitions->add_produce(ende->name);
			transitions->add_bpel(EMPTY, start->name + "_flow_subnet");

			start->name = start->name + "_f";

			//0 um BPEL Code nicht mit zu löschen, der wird im Netz erhalten
			delete_subnet_copy(0);
			delete_subnet_original();

			while(places_in_B != NULL)
			{
				plptr = places_in_B;
				places_in_B = places_in_B->next;
				delete(plptr);
			}

			while(trans_in_B != NULL)
			{
				tlptr = trans_in_B;
				trans_in_B = trans_in_B->next;
				delete(tlptr);
			}

#ifdef VERBOSE
	cout << "    - subnet transition to place found -\n";
#endif
			return 1;
		}else
		{
			delete_subnet_copy(1);

			while(places_in_B != NULL)
			{
				plptr = places_in_B;
				places_in_B = places_in_B->next;
				delete(plptr);
			}

			while(trans_in_B != NULL)
			{
				tlptr = trans_in_B;
				trans_in_B = trans_in_B->next;
				delete(tlptr);
			}
			return 0;
		}
	}else
	{
		while(places_in_B != NULL)
		{
			plptr = places_in_B;
			places_in_B = places_in_B->next;
			delete(plptr);
		}

		while(trans_in_B != NULL)
		{
			tlptr = trans_in_B;
			trans_in_B = trans_in_B->next;
			delete(tlptr);
		}

		return 0;
	}
}

int owfn::f_trans_to_trans(transition *start, int max, placelist *selected, transition *ende)
{
	//funktion erzeugt Teilnetz mit allen Knoten im Nachbereich von Start bis Ende

	int i;
	int counter;
	placelist *plptr;
	placelist *newplaces;
	placelist *placelistptr;
	transitionlist *tlptr;
	transitionlist *newtrans;
	transitionlist *translistptr;
	placelist *tmp;
	transition *transptr;
	place *con;
	place *prev;
	place *realplace;

	places_in_B = NULL;
	trans_in_B = NULL;
	newplaces = NULL;
	newtrans = NULL;
	trans_in_B = new transitionlist(start, trans_in_B);

	tmp = selected;
	while(tmp != NULL)
	{
		realplace = return_placeptr(tmp->placeptr->name);
		places_in_B = new placelist(realplace, places_in_B);
		newplaces = new placelist(realplace, newplaces);
		tmp = tmp->next;
	}

	for(i = 0; i <= max; i++)	//bedingung überwachen
	{
		plptr = newplaces;
		while(plptr != NULL)
		{
			counter = 0;
			transptr = transitions;
			while(transptr != NULL)
			{
				con = transptr->consumes;
				while(con != NULL)
				{
					if(con->name == plptr->placeptr->name)
					{
						counter++;
						//if not in list
						translistptr = trans_in_B;
						while(translistptr != NULL && translistptr->transitionptr != transptr)
						{
							translistptr = translistptr->next;
						}
						if(translistptr == NULL)
						{
							newtrans = new transitionlist(transptr, newtrans);
							trans_in_B = new transitionlist(transptr, trans_in_B);
						}
					}
					con = con->next;
				}
				transptr = transptr->next;
			}
			if(counter == 0)
			{
				//Eine Platz hat keine Nachfolgetransitionen, nicht erlaubt
				while(newtrans != NULL)
				{
					tlptr = newtrans;
					newtrans = newtrans->next;
					delete(tlptr);
				}
				
				while(newplaces != NULL)
				{
					plptr = newplaces;
					newplaces = newplaces->next;
					delete(plptr);
				}
				
				while(places_in_B != NULL)
				{
					plptr = places_in_B;
					places_in_B = places_in_B->next;
					delete(plptr);
				}
	
				while(trans_in_B != NULL)
				{
					tlptr = trans_in_B;
					trans_in_B = trans_in_B->next;
					delete(tlptr);
				}
				return 0;
			}
			plptr = plptr->next;
		}

		while(newplaces != NULL)
		{
			plptr = newplaces;
			newplaces = newplaces->next;
			delete(plptr);
		}

		i++;
		if(i <= max)
		{
			tlptr = newtrans;
			while(tlptr != NULL)
			{
				if(tlptr->transitionptr->name != ende->name)	//keinen Platz nach Ende betrachten
				{
					con = tlptr->transitionptr->produces;
					if(con == NULL)
					{
						//Eine Transition hat keine Nachfolgeplätze, nicht erlaubt
						while(newtrans != NULL)
						{
							tlptr = newtrans;
							newtrans = newtrans->next;
							delete(tlptr);
						}
						
						while(newplaces != NULL)
						{
							plptr = newplaces;
							newplaces = newplaces->next;
							delete(plptr);
						}
						
						while(places_in_B != NULL)
						{
							plptr = places_in_B;
							places_in_B = places_in_B->next;
							delete(plptr);
						}
			
						while(trans_in_B != NULL)
						{
							tlptr = trans_in_B;
							trans_in_B = trans_in_B->next;
							delete(tlptr);
						}
						return 0;
					}
					while(con != NULL)
					{
						//if not in list
						placelistptr = places_in_B;
						while(placelistptr != NULL && placelistptr->placeptr->name != con->name)
						{
							placelistptr = placelistptr->next;
						}
						if(placelistptr == NULL)
						{
							realplace = return_placeptr(con->name);
							if(realplace == NULL)
							{
								cout << "Error 1 in function 'f_trans_to_trans': Place '" << con->name << "' vanished. This should not happen...\n";
								exit(EXIT_FAILURE);
							}
							newplaces = new placelist(realplace, newplaces);
							places_in_B = new placelist(realplace, places_in_B);
						}
						con = con->next;
					}
				}
				tlptr = tlptr->next;
			}
		}

		while(newtrans != NULL)
		{
			tlptr = newtrans;
			newtrans = newtrans->next;
			delete(tlptr);
		}
	}

	//Mindestens 2 Plätze und 2 Transitionen
	if(places_in_B != NULL && places_in_B->next != NULL && trans_in_B != NULL && trans_in_B->next != NULL)
	{
		//Kopie des gesamten Teilnetzes erstellen
		create_subnet_copy();

		tlptr = trans_in_B;
		while(tlptr != NULL && tlptr->transitionptr->name != ende->name)
		{
			tlptr = tlptr->next;
		}
		if(tlptr == NULL)
		{
			//kein Weg hat die Endtransition gefunden
			delete_subnet_copy(1);
			
			while(places_in_B != NULL)
			{
				plptr = places_in_B;
				places_in_B = places_in_B->next;
				delete(plptr);
			}

			while(trans_in_B != NULL)
			{
				tlptr = trans_in_B;
				trans_in_B = trans_in_B->next;
				delete(tlptr);
			}
			return 0;
		}
		replace_trans(ende, "_flow_end", "_flow_end", 0);
		places_in_B = new placelist(new place(ende->name + "_flow_end"), places_in_B);

		//zusätzlicher Platz und Transition ersetzen
		places_in_B = new placelist(new place(start->name + "_flow_subnet"), places_in_B);
		replace_trans(start, "_flow_start", "_flow_subnet", 1);

		//starttransition aus Original Teilnetz entfernen, damit dieser nicht mitgelöscht wird
		if(!remove_transition_from_original(start->name))
		{
			cout << "Error 2 in function 'f_trans_to_trans': Transition '" << start->name << "' vanished. This should not happen...\n";
			exit(EXIT_FAILURE);
		}
		if(!remove_transition_from_original(ende->name))
		{
			return 0;
		}

		if(check_subnet(start->name + "_flow_subnet", 1, start->name, ende->name))	//der Name des Platzes, der ganz oben steht, als Anfangsmarkierung für die LoLA Datei
		{
			//alle Kanten von start auf nun nicht mehr vorhandene Plätze in B entfernen
			prev = NULL;
			con = start->produces;
			while(con != NULL)
			{
				plptr = places_in_B;
				while(plptr != NULL && plptr->placeptr->name != con->name)
				{
					plptr = plptr->next;
				}
				if(plptr != NULL)
				{
					if(prev == NULL)
					{
						start->produces = start->produces->next;
					}else
					{
						prev->next = con->next;
					}
					delete(con);
					con = start->produces;
					prev = NULL;
				}else
				{
					prev = con;
					con = con->next;
				}
			}
			//alle Kanten von nun nicht mehr vorhandenen Plätzen in B auf start entfernen
			prev = NULL;
			con = start->consumes;
			while(con != NULL)
			{
				plptr = places_in_B;
				while(plptr != NULL && plptr->placeptr->name != con->name)
				{
					plptr = plptr->next;
				}
				if(plptr != NULL)
				{
					if(prev == NULL)
					{
						start->consumes = start->consumes->next;
					}else
					{
						prev->next = con->next;
					}
					delete(con);
					con = start->consumes;
					prev = NULL;
				}else
				{
					prev = con;
					con = con->next;
				}
			}

			//alle Kanten von nun nicht mehr vorhandenen Plätzen in B auf ende entfernen
			prev = NULL;
			con = ende->consumes;
			while(con != NULL)
			{
				plptr = places_in_B;
				while(plptr != NULL && plptr->placeptr->name != con->name)
				{
					plptr = plptr->next;
				}
				if(plptr != NULL)
				{
					if(prev == NULL)
					{
						ende->consumes = ende->consumes->next;
					}else
					{
						prev->next = con->next;
					}
					delete(con);
					con = ende->consumes;
					prev = NULL;
				}else
				{
					prev = con;
					con = con->next;
				}
			}

			places = new place(start->name + "_flow_subnet", places);
			start->add_produce(start->name + "_flow_subnet");
			ende->add_consume(start->name + "_flow_subnet");
			places->bpel_code = transform_subnet(start->name);
			
			start->name = start->name + "_f";

			//0 um BPEL Code nicht mit zu löschen, der wird im Netz erhalten
			delete_subnet_copy(0);
			delete_subnet_original();

			while(places_in_B != NULL)
			{
				plptr = places_in_B;
				places_in_B = places_in_B->next;
				delete(plptr);
			}

			while(trans_in_B != NULL)
			{
				tlptr = trans_in_B;
				trans_in_B = trans_in_B->next;
				delete(tlptr);
			}

#ifdef VERBOSE
	cout << "    - subnet transition to transition found -\n";
#endif
			return 1;
		}else
		{
			delete_subnet_copy(1);

			while(places_in_B != NULL)
			{
				plptr = places_in_B;
				places_in_B = places_in_B->next;
				delete(plptr);
			}

			while(trans_in_B != NULL)
			{
				tlptr = trans_in_B;
				trans_in_B = trans_in_B->next;
				delete(tlptr);
			}
			return 0;
		}
	}else
	{
		while(places_in_B != NULL)
		{
			plptr = places_in_B;
			places_in_B = places_in_B->next;
			delete(plptr);
		}

		while(trans_in_B != NULL)
		{
			tlptr = trans_in_B;
			trans_in_B = trans_in_B->next;
			delete(tlptr);
		}

		return 0;
	}
}


void owfn::rename_place(string placename, string addition)
{
	place *placeptr;
	transition *transptr;
	
	placeptr = places;
	while(placeptr != NULL && placeptr->name != placename)
	{
		placeptr = placeptr->next;
	}
	
	if(placeptr != NULL)
	{
		placeptr->name = placename + addition;
		transptr = transitions;
		while(transptr != NULL)
		{
			placeptr = transptr->consumes;
			while(placeptr != NULL)
			{
				if(placeptr->name == placename)
				{
					placeptr->name = placename + addition;
				}
				placeptr = placeptr->next;
			}
			
			placeptr = transptr->produces;
			while(placeptr != NULL)
			{
				if(placeptr->name == placename)
				{
					placeptr->name = placename + addition;
				}
				placeptr = placeptr->next;
			}
			
			transptr = transptr->next;
		}
	}else
	{
		cout << "Error 1 in function 'rename_place': Place '" << placename << "' vanished. This should not happen...\n";
		exit(EXIT_FAILURE);
	}
}

