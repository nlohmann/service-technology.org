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


#ifndef _CLASS_OWFN_H_
#define _CLASS_OWFN_H_

class owfn
{
	public:
//vars
	ifstream owfn_file;		//file containing the oWFN
	transition *transitions;	//List of all transitions
	place *places;			//List of all places
	place *inputs;			//List of all input places
	place *outputs;			//List of all output places
	place *initials;		//List of all initial places
	place *finals;			//List of all final places (temporary, TODO, keine mengen)
	bpel *final_code;		//Link to the final code object
	char *file;
	char *outfile;
	ofstream bpel_file;


//constructor

	owfn(char *infile, char *outfile)
	{
		this->file = infile;
		this->outfile = outfile;
		transitions = NULL;
		places = NULL;
		inputs = NULL;
		outputs = NULL;
		initials = NULL;
		finals = NULL;
		final_code = NULL;
		places_in_B = NULL;
		trans_in_B = NULL;
		checked_trans = NULL;
		max_dfs = 0;
		dfs_stack = NULL;
		trans_in_circle = NULL;
		places_in_circle = NULL;
		inoutplaces = NULL;
		end_place = NULL;
	}

	owfn()
	{
		transitions = NULL;
		places = NULL;
		inputs = NULL;
		outputs = NULL;
		initials = NULL;
		finals = NULL;
		final_code = NULL;
		places_in_B = NULL;
		trans_in_B = NULL;
		checked_trans = NULL;
		max_dfs = 0;
		dfs_stack = NULL;
		trans_in_circle = NULL;
		places_in_circle = NULL;
		inoutplaces = NULL;
		end_place = NULL;
	}


	void owfn_to_file();			//Ausgabe des annotierten oWFN
	void owfn_out();
	void transform();			//Transformation starten

//read owfn file
	void read_owfn();			//oWFN einlesen
	void write_bpel();

//initial transformations
	void intrans_initialmarkings();		//Zusätzliche Transformationen für mehrere anfangs markierte Plätze
	void intrans_oneinitial();

//initial bpel replacements
	int inrepl_inout();			//Transitionen mit mehr als einem Platz aus I im Vor- und Nachbereich mit flow(invoke, receive, ...) beschriften
	int inrepl_ins();			//Transitionen mit nur receives
	int inrepl_outs();			//Transitionen mit nur invokes
	int inrepl_inputs();			//Transitionen mit einem Platz aus Input im Vorbereich mit receive beschriften
	int inrepl_outputs();			//Transitionen mit einem Platz aus Output im Nachbereich mit invoke beschriften

//reoccuring bpel replacements
//Sequenzen
	int reoccrepl_linplaces();
	int reoccrepl_lintrans();
	int reoccrepl_endplaces();
	int reoccrepl_endtrans();
//If Aktivitäten
	int reoccrepl_switch();			//Normale Transformation
	int reoccrepl_switchshort();		//Zusätzliche Transformation, schneller, für If samt Ende
	int reoccrepl_switchshortopenend();	//Spezialfall alle Nachbereiche leer
//Flow Aktivitäten fester Größe
	int reoccrepl_flowshortopenend();
	int reoccrepl_flowshort();
//Zyklen
	int reoccrepl_circleshort();		//Zusätzliche Transformation, schneller, für eine Doppelkante
	int reoccrepl_circlemultexits();	//Zyklus mit mehreren Ausgängen
	int reoccrepl_circlemultins();		//Zyklus mit mehreren Eingängen und Ausgängen
	int reoccrepl_circlemultexecution();	//Unsicherheit auf Zyklen
//Strukturelle Umformungen
	int reoccrepl_exfc_A();
	int reoccrepl_exfc_B();
//Kopien von Netzteilen
	int reoccrepl_copy_A();
	int reoccrepl_copy_B();
//Transformation von Teilnetzen zu flows
	int reoccrepl_bigflows();
//tests
	int somethingleft();			//Test, ob das Netz nur noch aus einem Knoten besteht
//final transformations
	void final_empty();
	void final_create_flow();
	void final_links();
	void final_switches();
	void final_create_final(char *name);
	void finish_sequence(branch *start);	//Sequence Aktivitäten einfügen
	void finish_links(bpel *start);		//Links löschen
	void finish_if(bpel *start);		//Ifs zusammenfassen
	void finish_while(bpel *start);		//Whiles zusammenfassen
	void finish_pick(bpel *start);		//Ifs in picks umformen
	void finish_flow(bpel *start);		//Flows zusammenfassen
	void finish_empties(bpel *start);	//Empties löschen
	void finish_more_empties(bpel *start);
	void finish_even_more_empties(bpel *start);
	void finish_opaques(bpel *start);	//OpaqueActivitys löschen
	void finish_more_opaques(bpel *start);
	void finish_even_more_opaques(bpel *start);

	private:

	transitionlist *trans_in_B;		//used to create lists of transitions
	placelist *places_in_B;			//and places that get copied
	transitionlist *checked_trans;		//mark list of visited transitions
	int max_dfs;
	stack *dfs_stack;
	transitionlist *trans_in_circle;
	placelist *places_in_circle;
	placelist *inoutplaces;
	place *end_place;

	void create_B(place *start);
	int find_circles(string startplatzname, string aktuellerplatz);

	void add_place(string str);	
	void add_initial(string str);
	void add_final(string str);
	void add_input(string str);
	void add_output(string str);
	void add_consume(string str);
	void add_produce(string str);

	int find_key(char *keyword);

	string remove_whitespace(string str);

	string remove_delims(string str);

	void store(void (owfn::*add_f)(string));

	void store_transition();
	
	bpel *copy_bpel(bpel *bpel_code);
	void copy_place(place *place_to_copy, int n);
	void copy_transition(transition *transition_to_copy, int n);
	void copy_transition_once(transition *transition_to_copy, int n);
	void rename_copy(place *place, int n);
	
	void reset_dfs();
	void dfs(place *startplace, transition *starttrans);
	void in_to_in(place *start);
	place *return_placeptr(string placename);



	
	
	int f_place_end(place *start, int max, transitionlist *selected);		//Teste alle Teilnetze beginnend bei start mit offenen Enden
	int f_place_all(place *start, int max);		//Teste alle Teilnetze beginnend bei start mit einem Knoten als Ende
	int f_trans_end(transition *start, int max, placelist *selected);	//Teste alle Teilnetze beginnend bei start mit offenen Enden
	int f_trans_all(transition *start, int max);	//Teste alle Teilnetze beginnend bei start mit einem Knoten als Ende
	
	int check_subnet(string start, int is_trans, string trans_start_name, string trans_end_name);	//überprüfe, ob Teilnetz brauchbar für Transformation, Test auf Sicherheit mit einer Marke auf start
	void circle_subnet_p(place *start, place *search);			//Test auf Kreise beginnend bei start
	void circle_subnet_t(transition *start, transition *search);		//Test auf Kreise beginnend bei start
	int replace_place(place *start, string name);		//Start- oder Endplatz eines Teilnetzes durch Kopie ersetzen
	int replace_place_circle(place *start, string startname, string endname);		//Start- und Endplatz eines Teilnetzes durch Kopie ersetzen
	void replace_trans(transition *start, string name, string placename, int start_or_end);
	bpel *transform_subnet(string start);				//Teilnetz in flow umwandeln
	void create_subnet_copy();				//Teilnetz als Kopie anlegen
	void delete_subnet_copy(int inc_bpel);				//Teilnetz vollständig entfernen
	void flow_empty();
	void flow_switches();
	void flow_links();
	bpel *flow_create_flow(string start);
	void delete_subnet_original();
	void remove_place_from_original(string name);
	int remove_transition_from_original(string name);

	transitionlist *trans_original;
	placelist *places_original;
	int subnet_circle;
	
	void copy_links(bpel *bpel_code);
	void search_links(bpel *bpel_code, int start);
	int global_link_id;
	
	transitionlist *seen_trans;
	placelist *seen_places;
	
	int f_trans_end_a(transition *start, int max, placelist *fixed, placelist *all);
	int f_place_end_a(place *start, int max, transitionlist *fixed, transitionlist *all);
	int f_place_to_place(place *start, int max, transitionlist *selected, place *ende);
	int f_place_to_trans(place *start, int max, transitionlist *selected, transition *ende);
	int f_place_circle(place *start, int max, transitionlist *selected);
	int f_trans_to_place(transition *start, int max, placelist *selected, place *ende);
	int f_trans_to_trans(transition *start, int max, placelist *selected, transition *ende);
	
	void rename_place(string placename, string addition);
	
}; //end of class owfn

#endif
