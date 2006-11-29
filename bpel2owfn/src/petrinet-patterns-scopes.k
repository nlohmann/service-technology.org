%{
#include "ast-config.h"
#include "helpers.h"
#include "ast-tools.h"
#include "ast-details.h"
#include "options.h"

extern PetriNet *TheNet;
extern map<unsigned int, ASTE*> ASTEmap;
extern string currentScope;
%}





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
