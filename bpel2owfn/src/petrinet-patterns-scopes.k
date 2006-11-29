%{
#include "ast-config.h"
#include "helpers.h"
#include "ast-tools.h"
#include "ast-details.h"
#include "options.h"

extern PetriNet *TheNet;
extern map<unsigned int, ASTE*> ASTEmap;
extern string currentScope;

extern set<string> ASTE_inputChannels;
extern set<string> ASTE_outputChannels;
extern set<string> linkNames;
%}





/******************************************************************************
  PROCESS
******************************************************************************/

void PProcess(const impl_tProcess* abstract_phylum, uview current_view)
{
  impl_tProcess_Process* phylum = (impl_tProcess_Process*) abstract_phylum;
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";


  switch(current_view)
  {
    case(petrinetsmall_enum):
      {
	// Create the interface places.
	// The used lists are filled during parsing mapping each combination of
	// partnerLink, portType and operation to one input (resp. output) place.
	for (set<string>::iterator in = ASTE_inputChannels.begin(); in != ASTE_inputChannels.end(); in++)
	  TheNet->newPlace("in." + *in, IN);
	for (set<string>::iterator out = ASTE_outputChannels.begin(); out != ASTE_outputChannels.end(); out++)
	  TheNet->newPlace("out." + *out, OUT);

	// Create the places for the links.
	// The used list is filled during parsing and contains all links of the
	// whole process -- also links defined in a deeper hierarchy.
	for (set<string>::iterator link = linkNames.begin(); link != linkNames.end(); link++)
	{
	  TheNet->newPlace("link." + *link);
	  TheNet->newPlace("!link." + *link);
	}

	// Create the initial place of the process pattern and mark it.
	Place *p1 = TheNet->newPlace(prefix + "initial");
	p1->mark();
      
	// Create the internal places of the process pattern.
	Place *p2 = TheNet->newPlace(prefix + "final");


	// unparse the embedded activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);


	TheNet->mergePlaces(TheNet->findPlace("1.internal.initial"), TheNet->findPlace(phylum->activity_1->id, ".initial"));
	TheNet->mergePlaces(TheNet->findPlace("1.internal.final"), TheNet->findPlace(phylum->activity_1->id, ".final"));

	break;
      }

    default: { /* no pattern in this case */ }
  }


  footer(phylum->id);
}




/******************************************************************************
  SCOPE
******************************************************************************/

void PScope(const impl_tScope* abstract_phylum, uview current_view)
{
  impl_tScope_Scope* phylum = (impl_tScope_Scope*) abstract_phylum;
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";


  switch(current_view)
  {
    case(petrinetsmall_enum):
      {
        cerr << "the handlers of the <scope>-activity will not be modelled!" << endl;

	Place *p1 = TheNet->newPlace(prefix + "initial");
	Place *p2 = TheNet->newPlace(prefix + "final");


	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the embedded activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);

  
	// Embed the inner activity.
	TheNet->mergePlaces(phylum->id, ".internal.running", phylum->activity_1->id, ".initial");
	TheNet->mergePlaces(phylum->id, ".internal.done", phylum->activity_1->id, ".final");	

	break;
      }

    default: { /* no pattern in this case */ }
  }


  footer(phylum->id);
}
