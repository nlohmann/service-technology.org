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
#include "owfn.h"

using namespace std;

class Adapter {
	
	private:
		/// VARIABLES
        /* none */


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

		/// expand given adapter rewriter by the rules for the interface of given service
		void expandRewriterWithInterface(PNapi::PetriNet *, PNapi::PetriNet *);

		/// read given rules file and expand given adapter rewriter
		void expandRewriterWithHiddenRules(PNapi::PetriNet *);
		void expandRewriterWithObservableRules(PNapi::PetriNet *);
		void expandRewriterWithControllableRules(PNapi::PetriNet *);
		void expandRewriterWithTotalRules(PNapi::PetriNet *, int);

        /// adapter reduction methods
        void reduceRewriter(PNapi::PetriNet *, int);

	public:
		
		/// basic constructor
		Adapter();
		
		/// basic destructor
		~Adapter();
		
		/// generate adapter
		void generate();
};




// defines an adapter rule as pair of two string lists (channel lists)
typedef std::pair< std::list< std::string >, std::list< std::string > > adapterRule;

typedef enum {
    ADAPT_HIDDEN,
    ADAPT_OBSERVABLE,
    ADAPT_CONTROLLABLE,
    ADAPT_TOTAL
} ruleType;

extern std::list< adapterRule > hiddenRules;
extern std::list< adapterRule > observableRules;
extern std::list< adapterRule > controllableRules;
extern std::list< adapterRule > totalRules;
extern std::map< std::string, ruleType > ruleTypePerChannel;
extern std::map< std::string, unsigned int > consumeRulesPerChannel;
//extern std::map< std::string, unsigned int > produceRulesPerChannel;


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
