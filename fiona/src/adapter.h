/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    adapter.h
 *
 * \brief   all functionality for adapter generation
 *
 * \author  responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 */

#ifndef ADAPTER_H_
#define ADAPTER_H_

#include <list>
#include <map>
#include <string>
#include "pnapi.h"

using namespace std;

class Adapter {
	
	private:
		/// VARIABLES
		/// list of internal places of the rewriter
		list< string > internalPlaces;


		/// METHODS
		/// creates output stream
		ostream * openOutput(string name);

		/// closes output stream
		void closeOutput(ostream * file);

		/// output given petri net as oWFN
		void outputPNasOWFN(PNapi::PetriNet *, string);

		/// output given petri net as PNG
		void outputPNasPNG(PNapi::PetriNet *, string);


		/// read given services
		PNapi::PetriNet * readService(string, string &);

		/// read computed adapter control
		PNapi::PetriNet * readControl(string);

		/// expand given adapter rewriter by the rules for the interface of given service
		void expandRules(PNapi::PetriNet *, PNapi::PetriNet *, std::string);

		/// read given rules file and expand given adapter rewriter
		void readRules(PNapi::PetriNet *);

	public:
		
		/// basic constructor
		Adapter();
		
		/// basic destructor
		~Adapter();
		
		/// generate adapter
		void generateAdapter();
};




// defines an adapter rule as pair of two string lists (channel lists)
typedef std::pair< std::list< std::string >, std::list< std::string > > adapterRule;

extern std::list< adapterRule > adapter_rules;
extern std::map< std::string, unsigned int > rulesPerChannel;


extern int adapt_rules_yylineno;
extern int adapt_rules_yydebug;
extern int adapt_rules_yy_flex_debug;
extern FILE* adapt_rules_yyin;
extern int adapt_rules_yyerror();
extern int adapt_rules_yyparse();

#ifdef YY_FLEX_HAS_YYLEX_DESTROY
extern int adapt_rules_yylex_destroy();
#endif

#endif /*ADAPTER_H_*/
