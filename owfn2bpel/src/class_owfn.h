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

#include <fstream>
#include <string>

#include "classes.h"

using std::ifstream;
using std::ofstream;
using std::string;

class owfn {
    public:
        //vars
        ifstream owfn_file;     //file containing the oWFN
        Transition* transitions;    //List of all transitions
        Place* places;          //List of all places
        Place* inputs;          //List of all input places
        Place* outputs;         //List of all output places
        Place* initials;        //List of all initial places
        Place* finals;          //List of all final places (temporary, TODO, keine mengen)
        BPEL* final_code;       //Link to the final code object
        char* file;
        char* outfile;
        ofstream bpel_file;

    private:

        TransitionList* trans_in_B;     //used to create lists of transitions
        PlaceList* places_in_B;         //and places that get copied
        TransitionList* checked_trans;      //mark list of visited transitions
        int max_dfs;
        Stack* dfs_stack;
        TransitionList* trans_in_circle;
        PlaceList* places_in_circle;
        PlaceList* inoutplaces;
        Place* end_place;

        TransitionList* trans_original;
        PlaceList* places_original;
        int subnet_circle;

        int global_link_id;

        TransitionList* seen_trans;
        PlaceList* seen_places;

    public:

        //constructor
        owfn(char* infile = NULL, char* outfile = NULL) :
            owfn_file(),
            transitions(NULL),
            places(NULL),
            inputs(NULL),
            outputs(NULL),
            initials(NULL),
            finals(NULL),
            final_code(NULL),
            file(infile),
            outfile(outfile),
            bpel_file(),
            trans_in_B(NULL),
            places_in_B(NULL),
            checked_trans(NULL),
            max_dfs(0),
            dfs_stack(NULL),
            trans_in_circle(NULL),
            places_in_circle(NULL),
            inoutplaces(NULL),
            end_place(NULL),
            trans_original(NULL),
            places_original(NULL),
            subnet_circle(0),
            global_link_id(0),
            seen_trans(NULL),
            seen_places(NULL) {
            //      this->file = infile;
            //      this->outfile = outfile;
            //      transitions = NULL;
            //      places = NULL;
            //      inputs = NULL;
            //      outputs = NULL;
            //      initials = NULL;
            //      finals = NULL;
            //      final_code = NULL;
            //      places_in_B = NULL;
            //      trans_in_B = NULL;
            //      checked_trans = NULL;
            //      max_dfs = 0;
            //      dfs_stack = NULL;
            //      trans_in_circle = NULL;
            //      places_in_circle = NULL;
            //      inoutplaces = NULL;
            //      end_place = NULL;
        }
        /*
                owfn() :
                    owfn_file(),
                    transitions(NULL),
                    places(NULL),
                    inputs(NULL),
                    outputs(NULL),
                    initials(NULL),
                    finals(NULL),
                    final_code(NULL),
                    file(),
                    outfile(),
                    bpel_file(),
                    trans_in_B(NULL),
                    places_in_B(NULL),
                    checked_trans(NULL),
                    max_dfs(0),
                    dfs_stack(NULL),
                    trans_in_circle(NULL),
                    places_in_circle(NULL),
                    inoutplaces(NULL),
                    end_place(NULL),
                    trans_original(NULL),
                    places_original(NULL),
                    subnet_circle(0),
                    global_link_id(0),
                    seen_trans(NULL),
                    seen_places(NULL)
                {
                    //      transitions = NULL;
                    //      places = NULL;
                    //      inputs = NULL;
                    //      outputs = NULL;
                    //      initials = NULL;
                    //      finals = NULL;
                    //      final_code = NULL;
                    //      places_in_B = NULL;
                    //      trans_in_B = NULL;
                    //      checked_trans = NULL;
                    //      max_dfs = 0;
                    //      dfs_stack = NULL;
                    //      trans_in_circle = NULL;
                    //      places_in_circle = NULL;
                    //      inoutplaces = NULL;
                    //      end_place = NULL;
                }
        */

        void owfn_to_file();            //Ausgabe des annotierten oWFN
        void owfn_out();
        void transform();           //Transformation starten

        //read owfn file
        void read_owfn();           //oWFN einlesen
        void write_bpel();

        //initial transformations
        void intrans_initialmarkings();     //Zusätzliche Transformationen für mehrere anfangs markierte Plätze
        void intrans_oneinitial();

        //initial bpel replacements
        int inrepl_inout();         //Transitionen mit mehr als einem Platz aus I im Vor- und Nachbereich mit flow(invoke, receive, ...) beschriften
        int inrepl_ins();           //Transitionen mit nur receives
        int inrepl_outs();          //Transitionen mit nur invokes
        int inrepl_inputs();            //Transitionen mit einem Platz aus Input im Vorbereich mit receive beschriften
        int inrepl_outputs();           //Transitionen mit einem Platz aus Output im Nachbereich mit invoke beschriften

        //reoccuring bpel replacements
        //Sequenzen
        int reoccrepl_linplaces();
        int reoccrepl_lintrans();
        int reoccrepl_endplaces();
        int reoccrepl_endtrans();
        //If Aktivitäten
        int reoccrepl_switch();         //Normale Transformation
        int reoccrepl_switchshort();        //Zusätzliche Transformation, schneller, für If samt Ende
        int reoccrepl_switchshortopenend(); //Spezialfall alle Nachbereiche leer
        //Flow Aktivitäten fester Größe
        int reoccrepl_flowshortopenend();
        int reoccrepl_flowshort();
        //Zyklen
        int reoccrepl_circleshort();        //Zusätzliche Transformation, schneller, für eine Doppelkante
        int reoccrepl_circlemultexits();    //Zyklus mit mehreren Ausgängen
        int reoccrepl_circlemultins();      //Zyklus mit mehreren Eingängen und Ausgängen
        int reoccrepl_circlemultexecution();    //Unsicherheit auf Zyklen
        //Strukturelle Umformungen
        int reoccrepl_exfc_A();
        int reoccrepl_exfc_B();
        //Kopien von Netzteilen
        int reoccrepl_copy_A();
        int reoccrepl_copy_B();
        //Transformation von Teilnetzen zu flows
        int reoccrepl_bigflows();
        //tests
        int somethingleft();            //Test, ob das Netz nur noch aus einem Knoten besteht
        //final transformations
        void final_empty();
        void final_create_flow();
        void final_links();
        void final_switches();
        void final_create_final(char* name);
        void finish_sequence(Branch* start);    //Sequence Aktivitäten einfügen
        void finish_links(BPEL* start);     //Links löschen
        void finish_if(BPEL* start);        //Ifs zusammenfassen
        void finish_while(BPEL* start);     //Whiles zusammenfassen
        void finish_pick(BPEL* start);      //Ifs in picks umformen
        void finish_flow(BPEL* start);      //Flows zusammenfassen
        void finish_empties(BPEL* start);   //Empties löschen
        void finish_more_empties(BPEL* start);
        void finish_even_more_empties(BPEL* start);
        void finish_opaques(BPEL* start);   //OpaqueActivitys löschen
        void finish_more_opaques(BPEL* start);
        void finish_even_more_opaques(BPEL* start);

    private:
        void create_B(Place* start);
        int find_circles(string startplatzname, string aktuellerplatz);

        void add_place(string str);
        void add_initial(string str);
        void add_final(string str);
        void add_input(string str);
        void add_output(string str);
        void add_consume(string str);
        void add_produce(string str);

        int find_key(char* keyword);

        string remove_whitespace(string str);

        string remove_delims(string str);

        void store(void (owfn::*add_f)(string));

        void store_transition();

        BPEL* copy_bpel(BPEL* bpel_code);
        void copy_place(Place* place_to_copy, int n);
        void copy_transition(Transition* transition_to_copy, int n);
        void copy_transition_once(Transition* transition_to_copy, int n);
        void rename_copy(Place* place, int n);

        void reset_dfs();
        void dfs(Place* startplace, Transition* starttrans);
        void in_to_in(Place* start);
        Place* return_placeptr(string placename);





        int f_place_end(Place* start, int max, TransitionList* selected);       //Teste alle Teilnetze beginnend bei start mit offenen Enden
        int f_place_all(Place* start, int max);     //Teste alle Teilnetze beginnend bei start mit einem Knoten als Ende
        int f_trans_end(Transition* start, int max, PlaceList* selected);   //Teste alle Teilnetze beginnend bei start mit offenen Enden
        int f_trans_all(Transition* start, int max);    //Teste alle Teilnetze beginnend bei start mit einem Knoten als Ende

        int check_subnet(string start, int is_trans, string trans_start_name, string trans_end_name);   //überprüfe, ob Teilnetz brauchbar für Transformation, Test auf Sicherheit mit einer Marke auf start
        void circle_subnet_p(Place* start, Place* search);          //Test auf Kreise beginnend bei start
        void circle_subnet_t(Transition* start, Transition* search);        //Test auf Kreise beginnend bei start
        int replace_place(Place* start, string name);       //Start- oder Endplatz eines Teilnetzes durch Kopie ersetzen
        int replace_place_circle(Place* start, string startname, string endname);       //Start- und Endplatz eines Teilnetzes durch Kopie ersetzen
        void replace_trans(Transition* start, string name, string placename, int start_or_end);
        BPEL* transform_subnet(string start);               //Teilnetz in flow umwandeln
        void create_subnet_copy();              //Teilnetz als Kopie anlegen
        void delete_subnet_copy(int inc_bpel);              //Teilnetz vollständig entfernen
        void flow_empty();
        void flow_switches();
        void flow_links();
        BPEL* flow_create_flow(string start);
        void delete_subnet_original();
        void remove_place_from_original(string name);
        int remove_transition_from_original(string name);

        void copy_links(BPEL* bpel_code);
        void search_links(BPEL* bpel_code, int start);

        int f_trans_end_a(Transition* start, int max, PlaceList* fixed, PlaceList* all);
        int f_place_end_a(Place* start, int max, TransitionList* fixed, TransitionList* all);
        int f_place_to_place(Place* start, int max, TransitionList* selected, Place* ende);
        int f_place_to_trans(Place* start, int max, TransitionList* selected, Transition* ende);
        int f_place_circle(Place* start, int max, TransitionList* selected);
        int f_trans_to_place(Transition* start, int max, PlaceList* selected, Place* ende);
        int f_trans_to_trans(Transition* start, int max, PlaceList* selected, Transition* ende);

        void rename_place(string placename, string addition);

}; //end of class owfn

#endif
