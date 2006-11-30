%{
#include "ast-config.h"
#include "helpers.h"
#include "ast-tools.h"
#include "ast-details.h"
#include "options.h"

extern PetriNet PN;
extern map<unsigned int, ASTE*> ASTEmap;
extern string currentScope;

extern set<string> ASTE_inputChannels;
extern set<string> ASTE_outputChannels;
extern set<string> linkNames;
extern set<string> variableNames;
%}





/******************************************************************************
  PROCESS
******************************************************************************/

void PProcess(impl_tProcess* abstract_phylum, uview current_view)
{
  impl_tProcess_Process* phylum = (impl_tProcess_Process*) abstract_phylum;
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";


  switch(current_view)
  {
    case(petrinet_enum):
      {
	currentScope = prefix;

	// Create the interface places.
	// The used lists are filled during parsing mapping each combination of
	// partnerLink, portType and operation to one input (resp. output) place.
	for (set<string>::iterator in = ASTE_inputChannels.begin(); in != ASTE_inputChannels.end(); in++)
	  PN.newPlace("in." + *in, IN);
	for (set<string>::iterator out = ASTE_outputChannels.begin(); out != ASTE_outputChannels.end(); out++)
	  PN.newPlace("out." + *out, OUT);


  	// Create push-places for the children of the process.
    	for (set<unsigned int>::iterator child = ASTEmap[phylum->id]->enclosedScopes.begin(); child != ASTEmap[phylum->id]->enclosedScopes.end(); child++)
      	{
  	  PN.newPlace(toString(*child) + ".push");
  	  PN.newPlace(toString(*child) + ".!push");
    	}

  
    	// Create the places for the links.
      	// The used list is filled during parsing and contains all links of the
	// whole process -- also links defined in a deeper hierarchy.
	for (set<string>::iterator link = linkNames.begin(); link != linkNames.end(); link++)
	{
	  PN.newPlace("link." + *link);
	  PN.newPlace("!link." + *link);
	}

      
	if (!parameters[P_NOVARIABLES])
	{
	  // Create variable places and initially mark them.
	  for (set<string>::iterator variable = variableNames.begin(); variable != variableNames.end(); variable++)
	  {
	    Place *v = PN.newPlace("variable." + *variable);
	    v->mark();
	  }
  
	  // Create a place for the global clock and mark it.
	  Place *clock = PN.newPlace(prefix + "clock");
	  clock->mark();
	}

	// Create the initial place of the process pattern and mark it.
	Place *p1 = PN.newPlace(prefix + "initial");
	p1->mark();
      
	// Create the internal places of the process pattern.
	Place *p3  = PN.newPlace(prefix + "scopeCompensated");
	Place *p5  = PN.newPlace(prefix + "upperFH");
	Place *p6  = PN.newPlace(prefix + "upperTerminate");
	Place *p7  = PN.newPlace(prefix + "Active");
	Place *p8  = PN.newPlace(prefix + "!Completed");
	Place *p9  = PN.newPlace(prefix + "Completed");
	Place *p10 = PN.newPlace(prefix + "!Compensated");
	Place *p11 = PN.newPlace(prefix + "!Active");
	Place *p12 = PN.newPlace(prefix + "Compensated");
	Place *p13 = PN.newPlace(prefix + "!Ended");
	Place *p14 = PN.newPlace(prefix + "Ended");
	Place *p15 = PN.newPlace(prefix + "!Faulted");
	Place *p16 = PN.newPlace(prefix + "Faulted");
	Place *p17 = PN.newPlace(prefix + "!Terminated");
	Place *p18 = PN.newPlace(prefix + "Terminated");
	Place *p19 = PN.newPlace(prefix + "p19");
	Place *p20 = PN.newPlace(prefix + "p20");
	Place *p21 = PN.newPlace(prefix + "p21");
	Place *p22 = PN.newPlace(prefix + "p22");
	Place *p33 = PN.newPlace(prefix + "p33");
	Place *p34 = PN.newPlace(prefix + "p34");
	Place *p35 = PN.newPlace(prefix + "p35");
	Place *p36 = PN.newPlace(prefix + "p36");
	Place *p37 = PN.newPlace(prefix + "fault");
	Place *p38 = PN.newPlace(prefix + "faultSave");
	Place *p39 = PN.newPlace(prefix + "final");
	Place *p40 = PN.newPlace(prefix + "rethrow");
	Place *p41 = PN.newPlace(prefix + "p41");
	Place *p42 = PN.newPlace(prefix + "ch_out");


	// State places of the process modeling the new fault model: The places
	// guarantee that the occurrence of more than one fault inside the fault
	// or compensation handler is prevented. The places are used as guards
	// by the transitions signalling faults.
	Place *fh_faulted = PN.newPlace(prefix + "FHFaulted");
	Place *fh_not_faulted = PN.newPlace(prefix + "!FHFaulted");
	Place *ch_faulted = PN.newPlace(prefix + "CHFaulted");
	Place *ch_not_faulted = PN.newPlace(prefix + "!CHFaulted");


	// The transitions of the process pattern.
	Transition *t2 = PN.newTransition(prefix + "t2");
	PN.newArc(p1, t2);
	PN.newArc(t2, p7);
	PN.newArc(t2, p8);
	PN.newArc(t2, p10);
	PN.newArc(t2, p13);
	PN.newArc(t2, p15);
	PN.newArc(t2, p17);
	PN.newArc(t2, p19);
	PN.newArc(t2, fh_not_faulted);

	Transition *t4 = PN.newTransition(prefix + "t4");
	PN.newArc(p7, t4);
	PN.newArc(p8, t4);
	PN.newArc(t4, p9);
	PN.newArc(t4, p11);
	PN.newArc(p33, t4);
	PN.newArc(t4, p39);


	// If the process has an event handler several places and transitions
	// have to be added to embed it. If the process lacks an event handler
	// some places can be merged.
	if (ASTEmap[phylum->id]->hasEH)
	{
	  Place *p23 = PN.newPlace(prefix + "p23");
	  Place *p24 = PN.newPlace(prefix + "p24");
	  Place *p25 = PN.newPlace(prefix + "p25");
	  Place *p26 = PN.newPlace(prefix + "p26");
	  Place *p27 = PN.newPlace(prefix + "p27");

	  PN.newArc(t2, p23);
	  PN.newArc(p25, t4);

	  Transition *t1 = PN.newTransition(prefix + "t1");
	  PN.newArc(p27, t1);
	  PN.newArc(t1, p34);

	  Transition *t3 = PN.newTransition(prefix + "t3");
	  PN.newArc(p20, t3);
	  PN.newArc(t3, p33);
	  PN.newArc(t3, p24);

	  Transition *t5 = PN.newTransition(prefix + "t5");
	  PN.newArc(p22, t5);
	  PN.newArc(t5, p26);

	  Transition *t6 = PN.newTransition(prefix + "t6");
	  PN.newArc(t6, p26);
	  PN.newArc(p21, t6);
	  PN.newArc(p33, t6);
	}
	else
	{
	  PN.mergePlaces(p20, p33);
	  PN.mergePlaces(p22, p34);
	}


	// unparse the stop pattern
	phylum->tStop_1->unparse(pseudoPrinter, current_view);

	// unparse the fault handlers
	phylum->tFaultHandlers_1->unparse(pseudoPrinter, current_view);

	// unparse the compensation handler
	phylum->tCompensationHandler_1->unparse(pseudoPrinter, current_view);

	// unparse the event handler
	phylum->tEventHandlers_1->unparse(pseudoPrinter, current_view);

	// unparse the embedded activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);


    	// After the patterns of the inner activity of the process are generated
      	// the push places of the child scopes are available and have to be
	// connected with the transition t2 of the process.
	for (set<unsigned int>::iterator child = ASTEmap[phylum->id]->enclosedScopes.begin(); child != ASTEmap[phylum->id]->enclosedScopes.end(); child++)
	  PN.newArc(t2, PN.findPlace( toString(*child) + ".!push" ));


	// Initialize the link places of the process.
	for (set<string>::iterator link = linkNames.begin(); link != linkNames.end(); link++)
	  PN.newArc(t2, PN.findPlace("!link." + *link));


	// Embed the fault handler of the process.
	PN.mergePlaces("1.internal.fault",     "1.internal.faultHandler.fault");
	PN.mergePlaces("1.internal.faultSave", "1.internal.faultHandler.faultSave");
	PN.mergePlaces("1.internal.rethrow",   "1.internal.faultHandler.rethrow");
	PN.mergePlaces("1.internal.!Ended",    "1.internal.faultHandler.!Ended");
	PN.mergePlaces("1.internal.Ended",     "1.internal.faultHandler.Ended");
	PN.mergePlaces("1.internal.p41",       "1.internal.faultHandler.ch_fh");
	PN.mergePlaces("1.internal.ch_out",    "1.internal.faultHandler.ch_out");
	PN.mergePlaces("1.internal.final",     "1.internal.faultHandler.final");


	// Embed the compensation handler of the process.
	PN.mergePlaces("1.internal.Completed",        "1.internal.compensationHandler.Completed");
	PN.mergePlaces("1.internal.!Completed",       "1.internal.compensationHandler.!Completed");
	PN.mergePlaces("1.internal.Compensated",      "1.internal.compensationHandler.Compensated");
	PN.mergePlaces("1.internal.p41",              "1.internal.compensationHandler.fh");
	PN.mergePlaces("1.internal.scopeCompensated", "1.internal.compensationHandler.scopeCompensated");
	PN.mergePlaces("1.internal.p34",              "1.internal.compensationHandler.clean");
	PN.mergePlaces("1.internal.p35",              "1.internal.compensationHandler.cleaned");
	PN.mergePlaces("1.internal.ch_out",           "1.internal.compensationHandler.ch_out");


	// If present, embed the event handler of the process.
	if (ASTEmap[phylum->id]->hasEH)
	{
	  PN.mergePlaces("1.internal.p23",      "1.internal.eventHandler.initial");
	  PN.mergePlaces("1.internal.p24",      "1.internal.eventHandler.finish");
	  PN.mergePlaces("1.internal.p25",      "1.internal.eventHandler.final");
	  PN.mergePlaces("1.internal.p26",      "1.internal.eventHandler.stop");
	  PN.mergePlaces("1.internal.p27",      "1.internal.eventHandler.stopped");
	}


	// Embed the stop-pattern of the process.
	PN.mergePlaces("1.internal.Faulted",        "1.internal.stop.Faulted");
	PN.mergePlaces("1.internal.upperFH",        "1.internal.stop.fault_in");
	PN.mergePlaces("1.internal.fault",          "1.internal.stop.fault");
	PN.mergePlaces("1.internal.faultSave",      "1.internal.stop.faultSave");
	PN.mergePlaces("1.internal.Active",         "1.internal.stop.Active");
	PN.mergePlaces("1.internal.!Active",        "1.internal.stop.!Active");
	PN.mergePlaces("1.internal.final",          "1.internal.stop.final");
	PN.mergePlaces("1.internal.upperTerminate", "1.internal.stop.terminate");
	PN.mergePlaces("1.internal.!Faulted",       "1.internal.stop.!Faulted");
	PN.mergePlaces("1.internal.rethrow",        "1.internal.stop.rethrow");
	PN.mergePlaces("1.internal.Ended",          "1.internal.stop.Ended");
	PN.mergePlaces("1.internal.Compensated",    "1.internal.stop.Compensated");
	PN.mergePlaces("1.internal.!Ended",         "1.internal.stop.!Ended");
	PN.mergePlaces("1.internal.p21",            "1.internal.stop.stop");
	PN.mergePlaces("1.internal.p34",            "1.internal.stop.stopped");
	PN.mergePlaces("1.internal.p35",            "1.internal.stop.cleanCH");
	PN.mergePlaces("1.internal.p36",            "1.internal.stop.ch_cleaned");
	PN.mergePlaces("1.internal.FHFaulted",      "1.internal.stop.FHFaulted");
	PN.mergePlaces("1.internal.!FHFaulted",     "1.internal.stop.!FHFaulted");
	PN.mergePlaces("1.internal.CHFaulted",      "1.internal.stop.CHFaulted");
	PN.mergePlaces("1.internal.!CHFaulted",     "1.internal.stop.!CHFaulted");


	// Embed the inner activity of the process.
	PN.mergePlaces(PN.findPlace("1.internal.p19"), PN.findPlace(phylum->activity_1->id, ".initial"));
	PN.mergePlaces(PN.findPlace("1.internal.p20"), PN.findPlace(phylum->activity_1->id, ".final"));
	PN.mergePlaces(PN.findPlace("1.internal.p21"), PN.findPlace(phylum->activity_1->id, ".stop"));
	PN.mergePlaces(PN.findPlace("1.internal.p22"), PN.findPlace(phylum->activity_1->id, ".stopped"));

	break;
      }

    case(petrinetnew_enum):
      {
	currentScope = prefix;

	// Create the interface places.
	// The used lists are filled during parsing mapping each combination of
	// partnerLink, portType and operation to one input (resp. output) place.
	for (set<string>::iterator in = ASTE_inputChannels.begin(); in != ASTE_inputChannels.end(); in++)
	  PN.newPlace("in." + *in, IN);
	for (set<string>::iterator out = ASTE_outputChannels.begin(); out != ASTE_outputChannels.end(); out++)
	  PN.newPlace("out." + *out, OUT);


	// Create the places for the links.
	// The used list is filled during parsing and contains all links of the
	// whole process -- also links defined in a deeper hierarchy.
	for (set<string>::iterator link = linkNames.begin(); link != linkNames.end(); link++)
	{
	  PN.newPlace("link." + *link);
	  PN.newPlace("!link." + *link);
	}


	Place *p1  = PN.newPlace(prefix + "initial"); p1->mark();
	Place *p2  = PN.newPlace(prefix + "final");
	Place *p3  = PN.newPlace(prefix + "fault_in");
	Place *p4  = PN.newPlace(prefix + "exit");
	Place *p5  = PN.newPlace(prefix + "Active");
	Place *p6  = PN.newPlace(prefix + "!Active"); p6->mark();
	Place *p7  = PN.newPlace(prefix + "Exiting");
	Place *p8  = PN.newPlace(prefix + "!Exiting"); p8->mark();
	Place *p9  = PN.newPlace(prefix + "inner_stop");
	Place *p10 = PN.newPlace(prefix + "inner_stopped");
	Place *p11 = PN.newPlace(prefix + "activity_initial");
	Place *p12 = PN.newPlace(prefix + "activity_final");
	Place *p13 = PN.newPlace(prefix + "fh_fault");
	Place *p14 = PN.newPlace(prefix + "ch_fault_in");

	Transition *t1 = PN.newTransition(prefix + "initialize");
	PN.newArc(p1, t1);
	PN.newArc(t1, p5);
	PN.newArc(p6, t1);
	PN.newArc(t1, p11);

	Transition *t2 = PN.newTransition(prefix + "finalize");
	PN.newArc(t2, p2);
	PN.newArc(p5, t2);
	PN.newArc(t2, p6);
	PN.newArc(p12, t2);

	Transition *t3 = PN.newTransition(prefix + "ft");
	PN.newArc(p3, t3);
	PN.newArc(p5, t3);
	PN.newArc(t3, p6);
	PN.newArc(t3, p9);
	PN.newArc(t3, p13);

	Transition *t4 = PN.newTransition(prefix + "ignore_fault");
	PN.newArc(p3, t4);
	PN.newArc(p6, t4, READ);

	Transition *t5 = PN.newTransition(prefix + "exit_process");
	PN.newArc(p4, t5);
	PN.newArc(p5, t5);
	PN.newArc(t5, p6);
	PN.newArc(t5, p7);
	PN.newArc(p8, t5);
	PN.newArc(t5, p9);

	Transition *t6 = PN.newTransition(prefix + "exit_fh");
	PN.newArc(p4, t6);
	PN.newArc(p6, t6, READ);
	PN.newArc(t6, p7);
	PN.newArc(p8, t6);
	PN.newArc(t6, p9);

	Transition *t7 = PN.newTransition(prefix + "ignore_exit");
	PN.newArc(p4, t7);
	PN.newArc(p7, t7, READ);

	Transition *t8 = PN.newTransition(prefix + "process_exited");
	PN.newArc(t8, p2);
	PN.newArc(p7, t8, READ);
	PN.newArc(p10, t8);

	Transition *t9 = PN.newTransition(prefix + "forced_exit");
	PN.newArc(t9, p4);
	PN.newArc(p6, t9, READ);
	PN.newArc(p14, t9);

	Transition *t10 = PN.newTransition(prefix + "handle_ch_fault");
	PN.newArc(t10, p3);
	PN.newArc(p5, t10, READ);
	PN.newArc(p14, t10);


	// unparse the embedded activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);

	// unparse the fault handlers
	phylum->tFaultHandlers_1->unparse(pseudoPrinter, current_view);


  	// embed fault handler
    	PN.mergePlaces(PN.findPlace(prefix + "fh_fault"),      PN.findPlace(prefix + "faultHandler.fault"));
      	PN.mergePlaces(PN.findPlace(prefix + "inner_stopped"), PN.findPlace(prefix + "faultHandler.initial"));
	PN.mergePlaces(PN.findPlace(prefix + "final"),         PN.findPlace(prefix + "faultHandler.final"));
	PN.mergePlaces(PN.findPlace(prefix + "inner_stop"),    PN.findPlace(prefix + "faultHandler.stop"));
	PN.mergePlaces(PN.findPlace(prefix + "final"),         PN.findPlace(prefix + "faultHandler.stopped"));

	// embed inner activity
	PN.mergePlaces(PN.findPlace(prefix + "activity_initial"), PN.findPlace(phylum->activity_1->id, ".initial"));
	PN.mergePlaces(PN.findPlace(prefix + "activity_final"),   PN.findPlace(phylum->activity_1->id, ".final"));
	PN.mergePlaces(PN.findPlace(prefix + "inner_stop"),       PN.findPlace(phylum->activity_1->id, ".stop"));
	PN.mergePlaces(PN.findPlace(prefix + "inner_stopped"),    PN.findPlace(phylum->activity_1->id, ".stopped"));      

	break;
      }

    case(petrinetsmall_enum):
      {
	// Create the interface places.
	// The used lists are filled during parsing mapping each combination of
	// partnerLink, portType and operation to one input (resp. output) place.
	for (set<string>::iterator in = ASTE_inputChannels.begin(); in != ASTE_inputChannels.end(); in++)
	  PN.newPlace("in." + *in, IN);
	for (set<string>::iterator out = ASTE_outputChannels.begin(); out != ASTE_outputChannels.end(); out++)
	  PN.newPlace("out." + *out, OUT);

	// Create the places for the links.
	// The used list is filled during parsing and contains all links of the
	// whole process -- also links defined in a deeper hierarchy.
	for (set<string>::iterator link = linkNames.begin(); link != linkNames.end(); link++)
	{
	  PN.newPlace("link." + *link);
	  PN.newPlace("!link." + *link);
	}

	// Create the initial place of the process pattern and mark it.
	Place *p1 = PN.newPlace(prefix + "initial");
	p1->mark();
      
	// Create the internal places of the process pattern.
	Place *p2 = PN.newPlace(prefix + "final");


	// unparse the embedded activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);


	PN.mergePlaces(PN.findPlace("1.internal.initial"), PN.findPlace(phylum->activity_1->id, ".initial"));
	PN.mergePlaces(PN.findPlace("1.internal.final"), PN.findPlace(phylum->activity_1->id, ".final"));

	break;
      }

    default: { /* no pattern in this case */ }
  }


  footer(phylum->id);
}




/******************************************************************************
  SCOPE
******************************************************************************/

void PScope(impl_tScope* abstract_phylum, uview current_view)
{
  impl_tScope_Scope* phylum = (impl_tScope_Scope*) abstract_phylum;
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";


  switch(current_view)
  {
    case(petrinet_enum):
      {
	currentScope = prefix;

	// Create push-places for the children of the scope.
	for (set<unsigned int>::iterator child = ASTEmap[phylum->id]->enclosedScopes.begin();
	    child != ASTEmap[phylum->id]->enclosedScopes.end(); child++)
	{
	  Place *push_child    = PN.newPlace(toString(*child) + ".push");
	  Place *notPush_child = PN.newPlace(toString(*child) + ".!push");
	}

	Place *p2  = PN.newPlace(prefix + "initial");
	Place *p3  = PN.newPlace(prefix + "ch_in");
	Place *p4  = PN.newPlace(prefix + "scopeCompensated");
	Place *p6  = PN.newPlace(prefix + "p6");
	Place *p7  = PN.newPlace(prefix + "upperTerminate");
	Place *p8  = PN.newPlace(prefix + "!Terminated");
	Place *p9  = PN.newPlace(prefix + "Terminated");
	Place *p10 = PN.newPlace(prefix + "Active");
	Place *p11 = PN.newPlace(prefix + "!Completed");
	Place *p12 = PN.newPlace(prefix + "Completed");
	Place *p13 = PN.newPlace(prefix + "!Compensated");
	Place *p14 = PN.newPlace(prefix + "!Active");
	Place *p15 = PN.newPlace(prefix + "Compensated");
	Place *p16 = PN.newPlace(prefix + "!Ended");
	Place *p17 = PN.newPlace(prefix + "Ended");
	Place *p18 = PN.newPlace(prefix + "!Faulted");
	Place *p19 = PN.newPlace(prefix + "Faulted");
       	Place *p20 = PN.newPlace(prefix + "stop");
	Place *p21 = PN.newPlace(prefix + "p21");
  	Place *p22 = PN.newPlace(prefix + "p22");
    	Place *p23 = PN.newPlace(prefix + "p23");
      	Place *p24 = PN.newPlace(prefix + "p24");
	Place *p35 = PN.newPlace(prefix + "p35");
	Place *p36 = PN.newPlace(prefix + "p36");
	Place *p37 = PN.newPlace(prefix + "p37");
	Place *p38 = PN.newPlace(prefix + "p38");
	Place *p39 = PN.newPlace(prefix + "p39");
	Place *p40 = PN.newPlace(prefix + "p40");
       	Place *p41 = PN.newPlace(prefix + "p41");
	Place *p42 = PN.newPlace(prefix + "p42");
      	Place *p43 = PN.newPlace(prefix + "p43");
	Place *p44 = PN.newPlace(prefix + "p44");
	Place *p45 = PN.newPlace(prefix + "stopped");
	Place *p46 = PN.newPlace(prefix + "p46");
	Place *p47 = PN.newPlace(prefix + "p47");
	Place *p48 = PN.newPlace(prefix + "p48");
	Place *p49 = PN.newPlace(prefix + "p49");
	Place *p50 = PN.newPlace(prefix + "p50");
	Place *p51 = PN.newPlace(prefix + "final");
	Place *p52 = PN.newPlace(prefix + "p52");
	Place *p53 = PN.newPlace(prefix + "p53");
	Place *p54 = PN.newPlace(prefix + "p54");
	Place *p56 = PN.newPlace(prefix + "compensated");


	// State places of the scope modeling the new fault model: The places
	// guarantee that the occurrence of more than one fault inside the fault
	// or compensation handler is prevented. The places are used as guards
	// by the transitions signalling faults.
	Place *fh_faulted = PN.newPlace(prefix + "FHFaulted");
	Place *fh_not_faulted = PN.newPlace(prefix + "!FHFaulted");
	Place *ch_faulted = PN.newPlace(prefix + "CHFaulted");
	Place *ch_not_faulted = PN.newPlace(prefix + "!CHFaulted");


	// The transitions of the process pattern.
	Transition *t2 = PN.newTransition(prefix + "t2");
	PN.newArc(p2, t2);
	PN.newArc(t2, p10);
	PN.newArc(t2, p11);
	PN.newArc(t2, p13);
	PN.newArc(t2, p16);
	PN.newArc(t2, p18);
	PN.newArc(t2, p21);
	PN.newArc(t2, fh_not_faulted);

	Transition *t4 = PN.newTransition(prefix + "t4");
	PN.newArc(p10, t4);
	PN.newArc(p11, t4);
	PN.newArc(t4, p12);
	PN.newArc(t4, p14);
	PN.newArc(PN.findPlace(toString(phylum->id) + ".!push"), t4);
	PN.newArc(t4, PN.findPlace(toString(phylum->id) + ".push"));
	PN.newArc(p35, t4);
	PN.newArc(t4, p51);

	Transition *t7 = PN.newTransition(prefix + "t7");
	PN.newArc(p8, t7, READ);
	PN.newArc(p10, t7);
	PN.newArc(t7, p14);
	PN.newArc(p20, t7);
	PN.newArc(t7, p36);

	Transition *t8 = PN.newTransition(prefix + "t8");
	PN.newArc(p9, t8, READ);
	PN.newArc(p10, t8);
	PN.newArc(t8, p14);
	PN.newArc(p20, t8);
	PN.newArc(t8, p37);

	Transition *t16 = PN.newTransition(prefix + "t16");
	PN.newArc(p53, t16);


	// Transitions for the negative control flow.
	Transition *t9  = stop(p2, "initial", prefix);
	Transition *t10 = stop(p17, "Ended", prefix);
	PN.newArc(t10, p17); // p17 is only read
	Transition *t11 = stop(p51, "final", prefix);


	// If the process has an event handler several places and transitions
	// have to be added to embed it. If the process lacks an event handler
	// some places can be merged.
	if (ASTEmap[phylum->id]->hasEH)
	{
	  Place *p25 = PN.newPlace(prefix + "p25");
	  Place *p26 = PN.newPlace(prefix + "p26");
	  Place *p27 = PN.newPlace(prefix + "p27");
	  Place *p28 = PN.newPlace(prefix + "p28");
	  Place *p29 = PN.newPlace(prefix + "p29");

	  PN.newArc(t2, p25);
	  PN.newArc(p27, t4);

	  Transition *t1 = PN.newTransition(prefix + "t1");
	  PN.newArc(p29, t1);
	  PN.newArc(t1, p38);

	  Transition *t3 = PN.newTransition(prefix + "t3");
	  PN.newArc(p22, t3);
	  PN.newArc(t3, p26);
	  PN.newArc(t3, p35);

	  Transition *t5 = PN.newTransition(prefix + "t5");
	  PN.newArc(p24, t5);
	  PN.newArc(t5, p28);

	  Transition *t6 = PN.newTransition(prefix + "t6");
	  PN.newArc(p23, t6);
	  PN.newArc(t6, p28);
	  PN.newArc(p35, t6);
	}
	else
	{
	  PN.mergePlaces(p22, p35);
	  PN.mergePlaces(p24, p38);
	}

	// New transitions to pass fault to parent.
	Transition *t12 = PN.newTransition(prefix + "passFaultUp");
	PN.newArc(p48, t12);
	PN.newArc(PN.findPlace(toString(ASTEmap[phylum->id]->parentScopeId) + ".internal.Active"), t12);
	PN.newArc(t12, PN.findPlace(toString(ASTEmap[phylum->id]->parentScopeId) + ".internal.!Active")); //newer fault management
	PN.newArc(t12, PN.findPlace(toString(ASTEmap[phylum->id]->parentScopeId) + ".internal.stop.fault_in"));

	Transition *t13 = PN.newTransition(prefix + "ignoreFault");
	PN.newArc(p48, t13);
	PN.newArc(PN.findPlace(toString(ASTEmap[phylum->id]->parentScopeId) + ".internal.!Active"), t13, READ);

	// Merge places with parent scope.
	PN.mergePlaces(PN.findPlace(toString(ASTEmap[phylum->id]->parentScopeId) +
	      ".internal.compensationHandler.compScope." + toString(phylum->id)), p3); // was "....internal.compScope."
	PN.mergePlaces(PN.findPlace(toString(ASTEmap[phylum->id]->parentScopeId) + ".internal.scopeCompensated"), p56);
	PN.mergePlaces(PN.findPlace(toString(ASTEmap[phylum->id]->parentScopeId) + ".internal.upperTerminate"), p46);


	// unparse the stop pattern
	phylum->tStop_1->unparse(pseudoPrinter, current_view);

	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the fault handlers
	phylum->tFaultHandlers_1->unparse(pseudoPrinter, current_view);

	// unparse the compensation handler
	phylum->tCompensationHandler_1->unparse(pseudoPrinter, current_view);

	// unparse the event handler
	phylum->tEventHandlers_1->unparse(pseudoPrinter, current_view);

	// unparse the embedded activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);

	// unparse the correlation sets
	phylum->tCorrelationSet_list_1->unparse(pseudoPrinter, current_view);


	// After the patterns of the inner activity of the process are generated
	// the push places of the child scopes are available and have to be
	// connected with the transition t2 of the process.
	for (set<unsigned int>::iterator child = ASTEmap[phylum->id]->enclosedScopes.begin(); child != ASTEmap[phylum->id]->enclosedScopes.end(); child++)
	  PN.newArc(t2, PN.findPlace(toString(*child) + ".!push"));

	string prefix2 = toString(phylum->id) + ".internal.";


    	// Embed the fault handler of the scope.
      	PN.mergePlaces(prefix + "p43",     prefix2 + "faultHandler.fault");
	PN.mergePlaces(prefix + "p44",     prefix2 + "faultHandler.faultSave");
	PN.mergePlaces(prefix + "p47",     prefix2 + "faultHandler.rethrow");
	PN.mergePlaces(prefix + "p41",     prefix2 + "faultHandler.ft");
	PN.mergePlaces(prefix + "p42",     prefix2 + "faultHandler.ftSave");
	PN.mergePlaces(prefix + "!Ended",  prefix2 + "faultHandler.!Ended");
	PN.mergePlaces(prefix + "Ended",   prefix2 + "faultHandler.Ended");
	PN.mergePlaces(prefix + "p49",     prefix2 + "faultHandler.ch_fh");
	PN.mergePlaces(prefix + "p50",     prefix2 + "faultHandler.ch_out");
	PN.mergePlaces(prefix + "final",   prefix2 + "faultHandler.out");	// only for user-defined FH
	PN.mergePlaces(prefix + "p52",     prefix2 + "faultHandler.trueOut");
	PN.mergePlaces(prefix + "p53",     prefix2 + "faultHandler.sourceFalse");
	PN.mergePlaces(prefix + "p54",     prefix2 + "faultHandler.falseOut");
	PN.mergePlaces(prefix + "p48",     prefix2 + "faultHandler.upperFH");
	PN.mergePlaces(prefix + "stopped", prefix2 + "faultHandler.stopped");


	// Embed the compensation handler of the scope.
	PN.mergePlaces(prefix + "ch_in",            prefix2 + "compensationHandler.ch_in");
	PN.mergePlaces(prefix + "Completed",        prefix2 + "compensationHandler.Completed");
	PN.mergePlaces(prefix + "!Completed",       prefix2 + "compensationHandler.!Completed");
	PN.mergePlaces(prefix + "Compensated",      prefix2 + "compensationHandler.Compensated");
	PN.mergePlaces(prefix + "!Compensated",     prefix2 + "compensationHandler.!Compensated");
	PN.mergePlaces(prefix + "p49",              prefix2 + "compensationHandler.fh");
	PN.mergePlaces(prefix + "scopeCompensated", prefix2 + "compensationHandler.scopeCompensated");
	PN.mergePlaces(prefix + "p39",              prefix2 + "compensationHandler.clean");
	PN.mergePlaces(prefix + "p40",              prefix2 + "compensationHandler.cleaned");
	PN.mergePlaces(prefix + "p50",              prefix2 + "compensationHandler.ch_out");
	PN.mergePlaces(prefix + "compensated",      prefix2 + "compensationHandler.compensated");


	// If present, embed the event handler of the scope.
	if (ASTEmap[phylum->id]->hasEH)
	{
	  PN.mergePlaces(prefix + "p25", prefix2 + "eventHandler.initial");
	  PN.mergePlaces(prefix + "p26", prefix2 + "eventHandler.finish");
	  PN.mergePlaces(prefix + "p27", prefix2 + "eventHandler.final");
	  PN.mergePlaces(prefix + "p28", prefix2 + "eventHandler.stop");
	  PN.mergePlaces(prefix + "p29", prefix2 + "eventHandler.stopped");
	}


	// Embed the stop-pattern of the scope.
	PN.mergePlaces(prefix + "p36",            prefix + "stop.ft_in");
	PN.mergePlaces(prefix + "Faulted",        prefix + "stop.Faulted");
	PN.mergePlaces(prefix + "p6",             prefix + "stop.fault_in");
	PN.mergePlaces(prefix + "Active",         prefix + "stop.Active");
	PN.mergePlaces(prefix + "p37",            prefix + "stop.terminate_up");
	PN.mergePlaces(prefix + "!Active",        prefix + "stop.!Active");
	PN.mergePlaces(prefix + "upperTerminate", prefix + "stop.terminate");
	PN.mergePlaces(prefix + "!Faulted",       prefix + "stop.!Faulted");
	PN.mergePlaces(prefix + "Ended",          prefix + "stop.Ended");
	PN.mergePlaces(prefix + "Compensated",    prefix + "stop.Compensated");
	PN.mergePlaces(prefix + "!Ended",         prefix + "stop.!Ended");
	PN.mergePlaces(prefix + "p23",            prefix + "stop.stop");
	PN.mergePlaces(prefix + "p38",            prefix + "stop.stopped");
	PN.mergePlaces(prefix + "p39",            prefix + "stop.cleanCH");
	PN.mergePlaces(prefix + "p40",            prefix + "stop.ch_cleaned");
	PN.mergePlaces(prefix + "p41",            prefix + "stop.ft");
	PN.mergePlaces(prefix + "p42",            prefix + "stop.ftSave");
	PN.mergePlaces(prefix + "p43",            prefix + "stop.fault");
	PN.mergePlaces(prefix + "p44",            prefix + "stop.faultSave");
	PN.mergePlaces(prefix + "stopped",        prefix + "stop.stopped_s");
	PN.mergePlaces(prefix + "p46",            prefix + "stop.upperTerminate");
	PN.mergePlaces(prefix + "p47",            prefix + "stop.rethrow");
	PN.mergePlaces(prefix + "p48",            prefix + "stop.upperFH");

	PN.mergePlaces(prefix + "FHFaulted",      prefix + "stop.FHFaulted");
	PN.mergePlaces(prefix + "!FHFaulted",     prefix + "stop.!FHFaulted");
	PN.mergePlaces(prefix + "CHFaulted",      prefix + "stop.CHFaulted");
	PN.mergePlaces(prefix + "!CHFaulted",     prefix + "stop.!CHFaulted");


	// Embed the inner activity of the scope.
	PN.mergePlaces(PN.findPlace(prefix + "p21"), PN.findPlace(phylum->activity_1->id, ".initial"));
	PN.mergePlaces(PN.findPlace(prefix + "p22"), PN.findPlace(phylum->activity_1->id, ".final"));
	PN.mergePlaces(PN.findPlace(prefix + "p23"), PN.findPlace(phylum->activity_1->id, ".stop"));
	PN.mergePlaces(PN.findPlace(prefix + "p24"), PN.findPlace(phylum->activity_1->id, ".stopped"));


	// Merge (!)Terminated places
	PN.mergePlaces(PN.findPlace("1.internal.!Terminated"), p8);
	PN.mergePlaces(PN.findPlace("1.internal.Terminated"), p9);

	break;
      }

    case(petrinetnew_enum):
      {
	currentScope = prefix;

	Place *p1  = PN.newPlace(prefix + "initial");
	Place *p2  = PN.newPlace(prefix + "final");
	Place *p3  = PN.newPlace(prefix + "fault_in");
	Place *p4  = PN.newPlace(prefix + "compensate");
	Place *p5  = PN.newPlace(prefix + "compensated");
	Place *p6  = PN.newPlace(prefix + "stop");
	Place *p7  = PN.newPlace(prefix + "stopped");
	Place *p8  = PN.newPlace(prefix + "fault_up");
	Place *p9  = PN.newPlace(prefix + "Active");
	Place *p10 = PN.newPlace(prefix + "!Active"); p10->mark();
	Place *p11 = PN.newPlace(prefix + "Successful");
	Place *p12 = PN.newPlace(prefix + "!Successful"); p12->mark();
	Place *p13 = PN.newPlace(prefix + "inner_stopped");
	Place *p14 = PN.newPlace(prefix + "activity_initial");
	Place *p15 = PN.newPlace(prefix + "activity_final");
	Place *p16 = PN.newPlace(prefix + "fh_fault");
	Place *p17 = PN.newPlace(prefix + "ch_initial");
	Place *p18 = PN.newPlace(prefix + "th_initial");
	Place *p19 = PN.newPlace(prefix + "ch_fault_in");
	Place *p20 = PN.newPlace(prefix + "ch_fault_up");
	Place *p21 = PN.newPlace(prefix + "p21"); p21->mark();
	Place *p22 = PN.newPlace(prefix + "p22");
	Place *p23 = PN.newPlace(prefix + "p23");
	Place *p24 = PN.newPlace(prefix + "ch_stop");
	Place *p25 = PN.newPlace(prefix + "ch_stopped");

	Transition *t1 = PN.newTransition(prefix + "initialize");
	PN.newArc(p1, t1);
	PN.newArc(t1, p9);
	PN.newArc(p10, t1);
	PN.newArc(t1, p14);

	Transition *t2 = PN.newTransition(prefix + "finalize");
	PN.newArc(t2, p2);
	PN.newArc(p9, t2);
	PN.newArc(t2, p10);
	PN.newArc(t2, p11);
	PN.newArc(p12, t2);
	PN.newArc(p15, t2);

	Transition *t3 = PN.newTransition(prefix + "ft");
	PN.newArc(p3, t3);
	PN.newArc(t3, p6);
	PN.newArc(p9, t3);
	PN.newArc(t3, p10);
	PN.newArc(t3, p16);

	Transition *t4 = PN.newTransition(prefix + "ignore_fault");
	PN.newArc(p3, t4);
	PN.newArc(p10, t4, READ);

	Transition *t5 = PN.newTransition(prefix + "start_ch");
	PN.newArc(p4, t5);
	PN.newArc(p11, t5);
	PN.newArc(t5, p12);
	PN.newArc(t5, p17);

	Transition *t6 = PN.newTransition(prefix + "skip_ch");
	PN.newArc(p4, t6);
	PN.newArc(t6, p5);
	PN.newArc(p12, t6, READ);

	Transition *t7 = PN.newTransition(prefix + "start_th");
	PN.newArc(p9, t7);
	PN.newArc(t7, p10);
	PN.newArc(p13, t7);
	PN.newArc(t7, p18);
	PN.newArc(PN.findPlace("1.internal.!Exiting"), t7, READ);

	Transition *t8 = PN.newTransition(prefix + "skip_th");
	PN.newArc(t8, p7);
	PN.newArc(p13, t8);
	PN.newArc(PN.findPlace("1.internal.Exiting"), t8, READ);

	Transition *t9 = PN.newTransition(prefix + "rethrow_ch_fault");
	PN.newArc(p10, t9, READ);
	PN.newArc(p19, t9);
	PN.newArc(t9, p20);
	PN.newArc(p21, t9);
	PN.newArc(t9, p22);

	Transition *t10 = PN.newTransition(prefix + "handle_ch_fault");
	PN.newArc(t10, p3);
	PN.newArc(p9, t10, READ);
	PN.newArc(p19, t10);
	PN.newArc(p21, t10);
	PN.newArc(t10, p22);

	Transition *t11 = PN.newTransition(prefix + "ignore_ch_fault");
	PN.newArc(p19, t11);
	PN.newArc(p22, t11, READ);

	Transition *t12 = stop(p1, "initial", prefix);

	// now comes everything we need for compensation in loops
	assert(ASTEmap[phylum->id] != NULL);
	if (ASTEmap[phylum->id]->ancestorLoops().size() != 0 && parameters[P_LOOPCOUNT])
	{
	  // rename existing places
	  PN.renamePlace(prefix + "final", prefix + "final1");
	  PN.renamePlace(prefix + "compensate", prefix + "compensate1");
	  PN.renamePlace(prefix + "compensated", prefix + "compensated1");

	  // add new places (the interface places of the wrapper)
	  Place *p21 = PN.newPlace(prefix + "final");
	  Place *p22 = PN.newPlace(prefix + "compensate");
	  Place *p23 = PN.newPlace(prefix + "compensated");

	  list<unsigned int> ancestor_loops = ASTEmap[phylum->id]->ancestorLoops();
	  cerr << ancestor_loops.size() << " ancestor_loops" << endl;
	  if (ancestor_loops.size() > 1)
	  {
	    vector<unsigned int> loop_bounds;
	    vector<unsigned int> loop_identifiers;
	    unsigned int i = 0;
	    for (list<unsigned int>::iterator loop = ancestor_loops.begin(); loop != ancestor_loops.end(); loop++)
	    {
	      assert(ASTEmap[*loop] != NULL);

	      if (loop == ancestor_loops.begin())
		continue;
  
	      loop_bounds.push_back(ASTEmap[*loop]->max_loops);
	      loop_identifiers.push_back(*loop);
	    }

	    process_loop_bounds(loop_bounds, loop_identifiers, prefix, ASTEmap[*ancestor_loops.begin()]->max_loops); 
	  }
	  else
	  {
	    PN.newPlace(prefix + "c");
	    PN.newPlace(prefix + "!c");
	  }
	}

	// don't stop from "final1" -- that's why this line is down here
	Transition *t13 = stop(PN.findPlace(prefix + "final"), "final", prefix);


	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the embedded activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);

	// unparse the fault handlers
	phylum->tFaultHandlers_1->unparse(pseudoPrinter, current_view);

	// unparse the compensation handler
	phylum->tCompensationHandler_1->unparse(pseudoPrinter, current_view);

	// unparse the termination handler
	phylum->tTerminationHandler_1->unparse(pseudoPrinter, current_view);


	// embed fault handler
	PN.mergePlaces(PN.findPlace(prefix + "inner_stopped"), PN.findPlace(prefix + "faultHandler.initial"));
	PN.mergePlaces(PN.findPlace(prefix + "fh_fault"),      PN.findPlace(prefix + "faultHandler.fault"));
	PN.mergePlaces(PN.findPlace(prefix + "final"),         PN.findPlace(prefix + "faultHandler.final"));
	PN.mergePlaces(PN.findPlace(prefix + "stop"),          PN.findPlace(prefix + "faultHandler.stop"));
	PN.mergePlaces(PN.findPlace(prefix + "stopped"),       PN.findPlace(prefix + "faultHandler.stopped"));

	impl_tCompensationHandler *d = phylum->tCompensationHandler_1;

	// embed compensation handler
	with (d)
	{
	  d = userDefinedCompensationHandler(ch_activity, *):
	  {
	    PN.mergePlaces(PN.findPlace(prefix + "ch_initial"),  PN.findPlace(ch_activity->id, ".initial"));
	    PN.mergePlaces(PN.findPlace(prefix + "ch_stop"),        PN.findPlace(ch_activity->id, ".stop"));
	    PN.mergePlaces(PN.findPlace(prefix + "ch_stopped"),     PN.findPlace(ch_activity->id, ".stopped"));

	    if (ASTEmap[phylum->id]->ancestorLoops().size() != 0 && parameters[P_LOOPCOUNT])
	      PN.mergePlaces(PN.findPlace(prefix + "compensated1"), PN.findPlace(ch_activity->id, ".final"));
	    else
	      PN.mergePlaces(PN.findPlace(prefix + "compensated"), PN.findPlace(ch_activity->id, ".final"));
	  }
	  default: { assert(false); }
	}

	// embed termination handler
	PN.mergePlaces(PN.findPlace(prefix + "th_initial"), PN.findPlace(prefix + "terminationHandler.initial"));
	PN.mergePlaces(PN.findPlace(prefix + "stopped"),    PN.findPlace(prefix + "terminationHandler.final"));
	PN.mergePlaces(PN.findPlace(prefix + "stop"),       PN.findPlace(prefix + "terminationHandler.stop"));
	PN.mergePlaces(PN.findPlace(prefix + "stopped"),    PN.findPlace(prefix + "terminationHandler.final"));

	// embed inner activity
	PN.mergePlaces(PN.findPlace(prefix + "activity_initial"), PN.findPlace(phylum->activity_1->id, ".initial"));
	PN.mergePlaces(PN.findPlace(prefix + "activity_final"),   PN.findPlace(phylum->activity_1->id, ".final"));
	PN.mergePlaces(PN.findPlace(prefix + "stop"),             PN.findPlace(phylum->activity_1->id, ".stop"));
	PN.mergePlaces(PN.findPlace(prefix + "inner_stopped"),    PN.findPlace(phylum->activity_1->id, ".stopped"));

      	// merge `ch_fault_up' place with `ch_fault_in' place of the parent scope
    	PN.mergePlaces(PN.findPlace(prefix + "ch_fault_up"), PN.findPlace(ASTEmap[phylum->id]->parentScopeId, ".internal.ch_fault_in"));
  
	break;
      }

    case(petrinetsmall_enum):
      {
        cerr << "the handlers of the <scope>-activity will not be modelled!" << endl;

	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");


	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the embedded activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);

  
	// Embed the inner activity.
	PN.mergePlaces(phylum->id, ".internal.running", phylum->activity_1->id, ".initial");
	PN.mergePlaces(phylum->id, ".internal.done", phylum->activity_1->id, ".final");	

	break;
      }

    default: { /* no pattern in this case */ }
  }


  footer(phylum->id);
}





/******************************************************************************
  PROCESS
******************************************************************************/

void PStop(impl_tStop* abstract_phylum, uview current_view)
{
  switch(current_view)
  {
    case(petrinet_enum):
      {
	impl_tStop *phylum = abstract_phylum;

	with (phylum)
	{
	  phylum = StopInProcess():
	  {
	    string prefix = "1.internal.stop.";
	    trace(TRACE_DEBUG, "[PNU]" + inString() + "<stopPatternInsideProcess>\n");

      	    Place *p1  = PN.newPlace(prefix + "Faulted");
	    Place *p2  = PN.newPlace(prefix + "p2");
	    Place *p3  = PN.newPlace(prefix + "p3");
	    Place *p4  = PN.newPlace(prefix + "p4");
	    Place *p5  = PN.newPlace(prefix + "fault_in");
	    Place *p6  = PN.newPlace(prefix + "p6");
	    Place *p7  = PN.newPlace(prefix + "fault");
	    Place *p8  = PN.newPlace(prefix + "faultSave");
	    Place *p9  = PN.newPlace(prefix + "Active");
	    Place *p10 = PN.newPlace(prefix + "!Active");
	    Place *p11 = PN.newPlace(prefix + "p11");
	    Place *p12 = PN.newPlace(prefix + "final");
	    Place *p13 = PN.newPlace(prefix + "terminate");
	    Place *p14 = PN.newPlace(prefix + "!Faulted");
	    Place *p15 = PN.newPlace(prefix + "rethrow");
	    Place *p16 = PN.newPlace(prefix + "Ended");
	    Place *p17 = PN.newPlace(prefix + "Compensated");
	    Place *p19 = PN.newPlace(prefix + "p19");
	    Place *p20 = PN.newPlace(prefix + "!Ended");
	    Place *p21 = PN.newPlace(prefix + "stop");
	    Place *p22 = PN.newPlace(prefix + "stopped");
	    Place *p23 = PN.newPlace(prefix + "cleanCH");
	    Place *p24 = PN.newPlace(prefix + "ch_cleaned");
	    Place *fh_faulted = PN.newPlace(prefix + "FHFaulted");
	    Place *fh_not_faulted = PN.newPlace(prefix + "!FHFaulted");
	    Place *fh_fault_in = PN.newPlace(prefix + "fh_fault_in");  // for faults from the FH
	    Place *ch_faulted = PN.newPlace(prefix + "CHFaulted");
	    Place *ch_not_faulted = PN.newPlace(prefix + "!CHFaulted");
	    Place *ch_fault_in = PN.newPlace(prefix + "ch_fault_in");  // for faults from the CH

	    Transition *t1 = PN.newTransition(prefix + "t1");
	    PN.newArc(p1, t1, READ);
	    PN.newArc(fh_fault_in, t1);
	    PN.newArc(t1, p15);

	    Transition *t2 = PN.newTransition(prefix + "t2");
	    PN.newArc(p2, t2);
	    PN.newArc(t2, p3);
	    PN.newArc(t2, p21);

	    Transition *t3 = PN.newTransition(prefix + "t3");
	    PN.newArc(p3, t3);
	    PN.newArc(p22, t3);
	    PN.newArc(t3, p4);

	    Transition *t4 =  PN.newTransition(prefix + "t4");
	    PN.newArc(p5, t4);
	    PN.newArc(t4, p2);
	    PN.newArc(t4, p6);

	    Transition *t5 = PN.newTransition(prefix + "t5");
	    PN.newArc(p4, t5);
	    PN.newArc(p6, t5);
	    PN.newArc(p14, t5);
	    PN.newArc(t5, p1);
	    PN.newArc(t5, p7);
	    PN.newArc(t5, p8);

	    Transition *t6 = PN.newTransition(prefix + "t6");
	    PN.newArc(p9, t6);
	    PN.newArc(p13, t6);
	    PN.newArc(t6, p2);
	    PN.newArc(t6, p10);
	    PN.newArc(t6, p11);

	    Transition *t7 = PN.newTransition(prefix + "t7");
	    PN.newArc(p4, t7);
	    PN.newArc(p11, t7);
	    PN.newArc(p20, t7);
	    PN.newArc(t7, p12);
	    PN.newArc(t7, p16);

	    Transition *t8 = PN.newTransition(prefix + "t8");
	    PN.newArc(p10, t8, READ);
	    PN.newArc(p13, t8);

	    Transition *t10 = PN.newTransition(prefix + "t10");
	    PN.newArc(ch_fault_in, t10);
	    PN.newArc(p17, t10, READ);
	    PN.newArc(p20, t10);
	    PN.newArc(t10, p19);
	    PN.newArc(t10, p23);

	    Transition *t12 = PN.newTransition(prefix + "t12");
	    PN.newArc(p19, t12);
	    PN.newArc(p24, t12);
	    PN.newArc(t12, p16);
	    PN.newArc(t12, p12);

	    trace(TRACE_DEBUG, "[PNU]" + inString() + "</stopPatternInsideProcess>\n");
	  }

	  phylum = StopInScope():
	  {
      	    string prefix = currentScope + "stop.";
	    trace(TRACE_DEBUG, "[PNU]" + inString() + "<stopPatternInsideScope scope=\"" + currentScope + "\">\n");

	    Place *p1 =  PN.newPlace(prefix + "ft_in");
	    Place *p2 =  PN.newPlace(prefix + "p2");
	    Place *p3 =  PN.newPlace(prefix + "ftSave");
	    Place *p4 =  PN.newPlace(prefix + "ft");
	    Place *p5 =  PN.newPlace(prefix + "Faulted");
	    Place *p6 =  PN.newPlace(prefix + "p6");
	    Place *p7 =  PN.newPlace(prefix + "p7");
	    Place *p8 =  PN.newPlace(prefix + "p8");
	    Place *p9 =  PN.newPlace(prefix + "!Faulted");
	    Place *p10 = PN.newPlace(prefix + "fault_in");
	    Place *p11 = PN.newPlace(prefix + "p11");
	    Place *p12 = PN.newPlace(prefix + "fault");
	    Place *p13 = PN.newPlace(prefix + "faultSave");
	    Place *p14 = PN.newPlace(prefix + "Active");
	    Place *p15 = PN.newPlace(prefix + "terminate_up");
	    Place *p16 = PN.newPlace(prefix + "p16");
	    Place *p17 = PN.newPlace(prefix + "stopped_s");
	    Place *p18 = PN.newPlace(prefix + "!Active");
	    Place *p19 = PN.newPlace(prefix + "p19");
	    Place *p20 = PN.newPlace(prefix + "upperTerminate");
	    Place *p21 = PN.newPlace(prefix + "terminate");
	    Place *p22 = PN.newPlace(prefix + "Ended");
	    Place *p23 = PN.newPlace(prefix + "rethrow");
	    Place *p24 = PN.newPlace(prefix + "Compensated");
	    Place *p25 = PN.newPlace(prefix + "p25");
	    Place *p26 = PN.newPlace(prefix + "p26");
	    Place *p27 = PN.newPlace(prefix + "upperFH");
	    Place *p28 = PN.newPlace(prefix + "!Ended");
	    Place *p29 = PN.newPlace(prefix + "stop");
	    Place *p30 = PN.newPlace(prefix + "stopped");
	    Place *p31 = PN.newPlace(prefix + "cleanCH");
	    Place *p32 = PN.newPlace(prefix + "ch_cleaned");
	    Place *fh_faulted = PN.newPlace(prefix + "FHFaulted");
	    Place *fh_not_faulted = PN.newPlace(prefix + "!FHFaulted");
	    Place *fh_fault_in = PN.newPlace(prefix + "fh_fault_in"); // for faults from the FH
	    Place *ch_faulted = PN.newPlace(prefix + "CHFaulted");
	    Place *ch_not_faulted = PN.newPlace(prefix + "!CHFaulted");
	    Place *ch_fault_in = PN.newPlace(prefix + "ch_fault_in"); // for faults from the CH

	    Transition *t1 = PN.newTransition(prefix + "t1");
	    PN.newArc(p1, t1);
	    PN.newArc(t1, p2);
	    PN.newArc(t1, p6);

	    Transition *t2 = PN.newTransition(prefix + "t2");
	    PN.newArc(p2, t2);
	    PN.newArc(t2, p3);
	    PN.newArc(t2, p4);
	    PN.newArc(t2, p5);
	    PN.newArc(p8, t2);
	    PN.newArc(p9, t2);

	    Transition *t3 = PN.newTransition(prefix + "t3");
	    PN.newArc(p5, t3, READ);
	    PN.newArc(fh_fault_in, t3);
	    PN.newArc(t3, p23);

	    Transition *t4 = PN.newTransition(prefix + "t4");
	    PN.newArc(p6, t4);
	    PN.newArc(t4, p7);
	    PN.newArc(t4, p29);

	    Transition *t5 = PN.newTransition(prefix + "t5");
	    PN.newArc(p7, t5);
	    PN.newArc(t5, p8);
	    PN.newArc(p30, t5);

	    Transition *t6 = PN.newTransition(prefix + "t6");
	    PN.newArc(t6, p6);
	    PN.newArc(p10, t6);
	    PN.newArc(t6, p11);

	    Transition *t7 = PN.newTransition(prefix + "t7");
	    PN.newArc(t7, p5);
	    PN.newArc(p8, t7);
	    PN.newArc(p9, t7);
	    PN.newArc(p11, t7);
	    PN.newArc(t7, p12);
	    PN.newArc(t7, p13);

	    Transition *t8 = PN.newTransition(prefix + "t8");
	    PN.newArc(t8, p6);
	    PN.newArc(p15, t8);
	    PN.newArc(t8, p16);

	    Transition *t9 = PN.newTransition(prefix + "t9");
	    PN.newArc(p8, t9);
	    PN.newArc(p16, t9);
	    PN.newArc(t9, p17);
	    PN.newArc(t9, p22);
	    PN.newArc(p28, t9);

	    Transition *t10 = PN.newTransition(prefix + "t10");
	    PN.newArc(t10, p6);
	    PN.newArc(p14, t10);
	    PN.newArc(t10, p19);
	    PN.newArc(t10, p18);
	    PN.newArc(p21, t10);

	    Transition *t11 = PN.newTransition(prefix + "t11");
	    PN.newArc(p8, t11);
	    PN.newArc(p19, t11);
      	    PN.newArc(t11, p20);
	    PN.newArc(t11, p22);
	    PN.newArc(p28, t11);

	    Transition *t12 = PN.newTransition(prefix + "t12");
	    PN.newArc(p18, t12, READ);
	    PN.newArc(p21, t12);
	    PN.newArc(t12, p20);

	    Transition *t14 = PN.newTransition(prefix + "faultInCH");
	    PN.newArc(ch_fault_in, t14);
	    PN.newArc(p24, t14, READ);
	    PN.newArc(t14, p26);
	    PN.newArc(p28, t14);
	    PN.newArc(t14, p31);

	    Transition *t16 = PN.newTransition(prefix + "t16");
	    PN.newArc(t16, p22);
	    PN.newArc(p26, t16);
	    PN.newArc(t16, p27);
	    PN.newArc(p32, t16);

	    trace(TRACE_DEBUG, "[PNU]" + inString() + "</stopPatternInsideScope scope=\"" + currentScope + "\">\n");
	  }
	}

	break;
      }

    default: { /* no pattern in this case */ }
  }
}
