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
#include "cmdline.h"

using namespace std;

int *stat_array;
int number_of_rounds;

//initialising number of links
int links::link_counter = 0;


/// shows statistics (triggered by flag "--stats")
void showStats(int interface[])
{
    cout << endl << "--------------------------------------------" << endl;
    cout <<         "Interface Transformationen:";
    cout << endl << "--------------------------------------------" << endl << endl;
    cout <<         "Mehrere invoke:                       " << interface[1] << endl;
    cout <<         "Mehrere receive:                      " << interface[2] << endl;
    cout <<         "Receive und invoke zusammen:          " << interface[3] << endl;
    cout <<         "Einzelne receive:                     " << interface[4] << endl;
    cout <<         "Einzelne invoke:                      " << interface[5] << endl;
    cout << endl << "--------------------------------------------" << endl;
    cout <<         number_of_rounds << " Runden ausgefuehrt:";
    cout << endl << "--------------------------------------------" << endl << endl;
    cout <<         "Vollverknuepfungen Plaetze oben:      " << stat_array[1] << endl;
    cout <<         "Vollverknuepfungen Transitionen oben: " << stat_array[2] << endl;
    cout <<         "Sequenzen P-T-P:                      " << stat_array[3] << endl;
    cout <<         "Sequenzen T-P-T:                      " << stat_array[4] << endl;
    cout <<         "Sequenzen P-T:                        " << stat_array[5] << endl;
    cout <<         "Sequenzen T-P:                        " << stat_array[6] << endl;
    cout <<         "Flow T-P-T:                           " << stat_array[7] << endl;
    cout <<         "Flow T-P:                             " << stat_array[8] << endl;
    cout <<         "If P-T-P:                             " << stat_array[9] << endl;
    cout <<         "If P-T:                               " << stat_array[10] << endl;
    cout <<         "While eine Doppelkante:               " << stat_array[11] << endl;
    cout <<         "While Zyklen mit mehreren Ausgaengen: " << stat_array[12] << endl;
    cout <<         "If allgemein:                         " << stat_array[13] << endl;
    cout <<         "Zyklen mit mehreren Eingaengen:       " << stat_array[14] << endl;
    cout <<         "Flow aus Teilnetz:                    " << stat_array[15] << endl;
    cout <<         "Kopien von Teilnetzen:                " << stat_array[16] << endl;
    cout <<         "Kopien eines Platzes:                 " << stat_array[17] << endl;
    cout <<         "Zyklen ausgliedern:                   " << stat_array[18] << endl;
    cout << endl << "--------------------------------------------" << endl;    
}




int main(int argc, char *argv[])
{
    // variable holding the information from the command line
    gengetopt_args_info args_info;

    // let's call our cmdline parser
    if (cmdline_parser (argc, argv, &args_info) != 0)
        exit(1);    
        
        
    
    //create oWFN

    // organize output filename
    if (!args_info.output_given) {
        string output_filename = string(args_info.input_arg) + ".bpel";
        args_info.output_arg = (char*)calloc(output_filename.length()+1, sizeof(char));
        assert(args_info.output_arg != NULL);
        strcpy(args_info.output_arg, output_filename.c_str());
    }
    
    
	owfn oWFN(args_info.input_arg, args_info.output_arg);
    //parse oWFN
	oWFN.read_owfn();
    
    //initial net transformations
#ifdef VERBOSE
	cout << "\nstarting initial net transformations...\n";
#endif
    //2.	//Wenn es nur einen anfangs markierten Platz gibt, könnte dieser Anfang eines Kreises sein
	oWFN.intrans_oneinitial();
    //1.	//Mehrere anfangs markierte Plätze finden und auf flow vorbereiten
	oWFN.intrans_initialmarkings();
#ifdef VERBOSE
	cout << "initial net transformations finished.\n";
#endif
    
    
    //initial bpel replacements
#ifdef VERBOSE
	cout << "\nstarting initial bpel replacements...\n";
#endif
    
	int stat[19];
	int i;
    //	int stats;
	int interface[5];
    
	for(i = 0; i < 19; i++)
	{
		stat[i] = 0;
	}
    
	for(i = 0; i < 5; i++)
	{
		interface[i] = 0;
	}
    
	stat_array = stat;
    
    //	stats = 0;
    //#ifdef STATS
    //	stats = 1;
    //#endif
    
    //one time functions:
    //4a.	//Transitionen mit nur invokes
	interface[1] = oWFN.inrepl_outs();
    //4b.	//Transitionen mit nur receives
	interface[2] = oWFN.inrepl_ins();
	//Transitionen mit mehr als einem Platz aus I im Vor- und Nachbereich mit flow(invoke, receive, ...) beschriften
	interface[3] = oWFN.inrepl_inout();
    //5. und 6. müssen nach 4 stehen, da sie nicht überprüfen, ob mehrere Plätze ein oder ausgehen
    //6.	//Transitionen mit einem Platz aus Input im Vorbereich mit receive beschriften
	interface[4] = oWFN.inrepl_inputs();
    //5.	//Transitionen mit einem Platz aus Output im Nachbereich mit invoke beschriften
	interface[5] = oWFN.inrepl_outputs();
    //Alle Plätze aus I aus der Platzliste entfernen... überflüssig, da die Plätze aus Input und Output in getrennten Listen stehen
    
#ifdef VERBOSE
	cout << "initial bpel replacements finished.\n";
#endif
    
    /*
     //Um genauer den Verbleib von Plätzen testen zu können:
     place *placeptr = oWFN.places;
     while(placeptr != NULL)
     {
     placeptr->add_bpel(OPAQUE, placeptr->name);
     placeptr = placeptr->next;
     }
     */
    
    //initial bpel replacements
#ifdef VERBOSE
	cout << "\nstarting reoccuring bpel replacements..." << endl;
#endif
    
	number_of_rounds = 0;
	oWFN.transform();
    
#ifdef VERBOSE
	cout << "reoccuring bpel replacements finished." << endl;
#endif
    
    
    
    
	if(oWFN.somethingleft())
	{
#ifdef VERBOSE
		cout << "\nmore than one place left!\n\nstarting final transformation..." << endl;
#endif
        
		cout << endl << "oWFN konnte nicht reduziert werden. Daher Ausgabe des annotierten Netzes." << endl;
		oWFN.owfn_to_file();
        
#ifdef VERBOSE
		cout << endl << "final transformation finished." << endl;
#endif
        return 2;
	}
	
    
    
#ifdef VERBOSE
    cout << "\nonly one place or transition left." << endl;
#endif
    
    
#ifdef VERBOSE
    cout << "\ncreating process...\n";
#endif
    oWFN.final_create_final(args_info.input_arg);
#ifdef VERBOSE
    cout << "process created.\n";
#endif
    
    //oWFN.final_code->out();
    
#ifdef VERBOSE
    cout << "\nstarting code beautifying...\n";
#endif
    
#ifdef VERBOSE
    cout << " looking for multiple whiles...\n";
#endif
    
    oWFN.finish_while(oWFN.final_code);
    
#ifdef VERBOSE
    cout << "  whiles merged.\n";
#endif
    
    
#ifdef VERBOSE
    cout << " looking for direct links...\n";
#endif
    
    oWFN.finish_links(oWFN.final_code);
    
#ifdef VERBOSE
    cout << "  direct links removed.\n";
#endif
    
    
#ifdef VERBOSE
    cout << " looking for empties...\n";
#endif
    
    oWFN.finish_empties(oWFN.final_code);
    oWFN.finish_more_empties(oWFN.final_code);
    oWFN.finish_even_more_empties(oWFN.final_code);
    
#ifdef VERBOSE
    cout << "  empties removed.\n";
#endif
    
    
#ifdef VERBOSE
    cout << " looking for nested flows...\n";
#endif
    oWFN.finish_flow(oWFN.final_code);
    
#ifdef VERBOSE
    cout << "  nested flows merged.\n";
#endif
    
    
    //if-Korrektur muss vor pick-Korrektur stehen
#ifdef VERBOSE
    cout << " looking for nested ifs...\n";
#endif
    
    oWFN.finish_if(oWFN.final_code);
    
#ifdef VERBOSE
    cout << "  ifs merged.\n";
#endif
    
    
    //picks muss nach dem empties entfernen stehen
#ifdef VERBOSE
    cout << " looking for picks...\n";
#endif
    
    oWFN.finish_pick(oWFN.final_code);
    
#ifdef VERBOSE
    cout << "  picks added.\n";
#endif
    
    if (args_info.remove_opaque_given)
    {
        //muss nach pick-Korrektur stehen, da pick sonst für neue leere Zweige (leer weil pick receives entfernt) empties einfügt
        cout << " looking for opaqueActivities...\n";
        oWFN.finish_opaques(oWFN.final_code);
        oWFN.finish_more_opaques(oWFN.final_code);
        oWFN.finish_even_more_opaques(oWFN.final_code);
        cout << "  opaqueActivities removed.\n";
    }
    
    
#ifdef VERBOSE
    cout << " looking for sequences...\n";
#endif
    
    oWFN.finish_sequence(oWFN.final_code->branches);
    
#ifdef VERBOSE
    cout << "  sequences added.\n";
#endif
    
#ifdef VERBOSE
    cout << "code beautifying finished.\n\n";
#endif
    
    /*
     oWFN.final_code->out();
     
     cout << "\nPLACES:\n";
     if(oWFN.places != NULL)
     oWFN.places->out();
     
     cout << "\nTransitions:\n";
     if(oWFN.transitions != NULL)
     oWFN.transitions->out();
     */
    
    oWFN.write_bpel();
    
    //Optimierbar: Die neue Funktion add_last_bpel kann einige Stellen in den anderen Funktionen abkürzen
    
    
    // show stats if flag "--stats" is set
    if(args_info.stats_given)
        showStats(interface);
    
	
    
	cout << "Transformation from '" << args_info.input_arg << "' to '" << args_info.output_arg << "' complete." << endl;
    
	return 0;
}


