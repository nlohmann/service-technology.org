%{
#include "ast-config.h"
#include "helpers.h"
#include "ast-tools.h"
#include "ast-details.h"
#include "options.h"

extern PetriNet PN;
extern map<unsigned int, ASTE*> ASTEmap;
extern string currentScope;
%}




/******************************************************************************
  EMPTY
******************************************************************************/

void PEmpty(impl_tEmpty* abstract_phylum, uview current_view)
{
  impl_tEmpty_Empty* phylum = static_cast<impl_tEmpty_Empty*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";  


  // places
  Place *p1 = PN.newPlace(prefix + "initial");
  Place *p2 = PN.newPlace(prefix + "final");


  // transitions
  Transition *t1 = PN.newTransition(prefix + "empty");
  PN.newArc(p1, t1);
  PN.newArc(t1, p2);


  // places and transitions for the negative control flow
  if (current_view != petrinetsmall_enum)
  {
    Place *p3 = PN.newPlace(prefix + "stop");
    Place *p4 = PN.newPlace(prefix + "stopped");
    Transition *t2 = stop(p1, "initial", prefix);
    Transition *t3 = stop(p2, "final", prefix);
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);
}





/******************************************************************************
  INVOKE
******************************************************************************/

void PAsynchronousInvoke(impl_tInvoke* abstract_phylum, uview current_view)
{
  impl_tInvoke_AsynchronousInvoke* phylum = static_cast<impl_tInvoke_AsynchronousInvoke*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";

  switch(current_view)
  {
    case(petrinet_enum):
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "running");
	Place *p3 = PN.newPlace(prefix + "final");
	Place *p4 = PN.newPlace(prefix + "p4");
	Place *p5 = PN.newPlace(prefix + "stop");
	Place *p6 = PN.newPlace(prefix + "stopped");

	Transition *t1 = PN.newTransition(prefix + "t1");
	PN.newArc(p1, t1); 
	PN.newArc(t1, p2);
	if (!parameters[P_NOVARIABLES])
	  PN.newArc(PN.findPlace("variable." + ASTEmap[phylum->id]->inputVariableName), t1, READ);

	Transition *t2 = PN.newTransition(prefix + "t2");
	PN.newArc(p2, t2);
	PN.newArc(t2, p3);
	PN.newArc(t2, PN.findPlace("out." + ASTEmap[phylum->id]->channelName));

	// Transitions for the negative control flow.
	if (parameters[P_NOSTANDARDFAULTS] == false)
	  Transition *t3 = throwFault(p2, p4, "running", prefix, mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);

	Transition *t4 = stop(p1, "initial", prefix);
	Transition *t5 = stop(p2, "running", prefix);
	Transition *t6 = stop(p4, "p4", prefix);
	Transition *t7 = stop(p3, "final", prefix);

	break;
      }

    case(petrinetsmall_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");

	Transition *t1 = PN.newTransition(prefix + "invoke");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);
	PN.newArc(t1, PN.findPlace("out." + ASTEmap[phylum->id]->channelName));
	
	break;
      }

    default: { /* no pattern in this case */ }      
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);
}





void PSynchronousInvoke(impl_tInvoke* abstract_phylum, uview current_view)
{
  impl_tInvoke_SynchronousInvoke* phylum = static_cast<impl_tInvoke_SynchronousInvoke*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";


  switch(current_view)
  {
    case(petrinet_enum):
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "running1");
	Place *p3 = PN.newPlace(prefix + "p3");
	Place *p4 = PN.newPlace(prefix + "running2");
	Place *p5 = PN.newPlace(prefix + "final");
	Place *p6 = PN.newPlace(prefix + "p6");
	Place *p7 = PN.newPlace(prefix + "stop");
	Place *p8 = PN.newPlace(prefix + "stopped");

	Transition *t1 = PN.newTransition(prefix + "t1");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);
	if (!parameters[P_NOVARIABLES])
	  PN.newArc(PN.findPlace("variable." + ASTEmap[phylum->id]->inputVariableName), t1, READ);

	Transition *t2 = PN.newTransition(prefix + "t2");
	PN.newArc(t2, PN.findPlace("out." + ASTEmap[phylum->id]->channelName));
	PN.newArc(p2, t2);
	PN.newArc(t2, p3);

	Transition *t3 = PN.newTransition(prefix + "t3");
	PN.newArc(PN.findPlace("in." + ASTEmap[phylum->id]->channelName), t3);
	PN.newArc(p3, t3);
	PN.newArc(t3, p4);

	Transition *t4 = PN.newTransition(prefix + "t4");
	PN.newArc(p4, t4);
	PN.newArc(t4, p5);
	if (!parameters[P_NOVARIABLES])
	{
	  PN.newArc(PN.findPlace("variable." + ASTEmap[phylum->id]->outputVariableName), t4);
	  PN.newArc(t4, PN.findPlace("variable." + ASTEmap[phylum->id]->outputVariableName));
	}

	// Transitions for the negative control flow.
	if (parameters[P_NOSTANDARDFAULTS] == false)
	{
	  Transition *t5 = throwFault(p4, p6, "running2", prefix, mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);
	  Transition *t6 = throwFault(p2, p6, "running1", prefix, mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);
	}
	
	Transition *t7  = stop(p1, "initial", prefix);
	Transition *t8  = stop(p2, "running1", prefix);
	Transition *t9  = stop(p3, "p3", prefix);
	Transition *t10 = stop(p4, "running2", prefix);
	Transition *t11 = stop(p6, "p6", prefix);
	Transition *t12 = stop(p5, "final", prefix);
	
	break;
      }

    case(petrinetsmall_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "running");
	Place *p3 = PN.newPlace(prefix + "final");

	Transition *t1 = PN.newTransition(prefix + "invokeO");
	PN.newArc(t1, PN.findPlace("out." + ASTEmap[phylum->id]->channelName));
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	Transition *t2 = PN.newTransition(prefix + "invokeI");
	PN.newArc(PN.findPlace("in." + ASTEmap[phylum->id]->channelName), t2);
	PN.newArc(p2, t2);
	PN.newArc(t2, p3);

	break;
      }

    default: { /* no pattern in this case */ }
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);
}





/******************************************************************************
  RECEIVE
******************************************************************************/

void PReceive(impl_tReceive* abstract_phylum, uview current_view)
{
  impl_tReceive_Receive* phylum = static_cast<impl_tReceive_Receive*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";  


  switch(current_view)
  {
    case(petrinet_enum):
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "running");
	Place *p3 = PN.newPlace(prefix + "final");
	Place *p4 = PN.newPlace(prefix + "p4");
	Place *p5 = PN.newPlace(prefix + "stop");
	Place *p6 = PN.newPlace(prefix + "stopped");

	Transition *t1 = PN.newTransition(prefix + "t1");
	PN.newArc(PN.findPlace("in." + ASTEmap[phylum->id]->channelName), t1);
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	Transition *t2 = PN.newTransition(prefix + "t2");
	PN.newArc(p2, t2);
	PN.newArc(t2, p3);
	if (!parameters[P_NOVARIABLES])
	{
	  PN.newArc(PN.findPlace("variable." + ASTEmap[phylum->id]->variableName), t2);
	  PN.newArc(t2, PN.findPlace("variable." + ASTEmap[phylum->id]->variableName));
	}

	// Transitions for the negative control flow.
	if (parameters[P_NOSTANDARDFAULTS] == false)
	  Transition *t3 = throwFault(p2, p4, "running", prefix, mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);

	Transition *t4 = stop(p1, "initial", prefix);
	Transition *t5 = stop(p2, "running", prefix);
	Transition *t6 = stop(p4, "p4", prefix);
	Transition *t7 = stop(p3, "final", prefix);

	break;
      }

    case(petrinetsmall_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");

	Transition *t1 = PN.newTransition(prefix + "receive");
	PN.newArc(PN.findPlace("in." + ASTEmap[phylum->id]->channelName), t1);
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	break;
      }

    default: { /* no pattern in this case */ }      
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);      
}




/******************************************************************************
  REPLY
******************************************************************************/

void PReply(impl_tReply* abstract_phylum, uview current_view)
{
  impl_tReply_Reply* phylum = static_cast<impl_tReply_Reply*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";  


  switch(current_view)
  {
    case(petrinet_enum):
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "running");
	Place *p3 = PN.newPlace(prefix + "final");
	Place *p4 = PN.newPlace(prefix + "p4");
	Place *p5 = PN.newPlace(prefix + "stop");
	Place *p6 = PN.newPlace(prefix + "stopped");

	Transition *t1 = PN.newTransition(prefix + "t1");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);
	if (!parameters[P_NOVARIABLES])
	  PN.newArc(PN.findPlace("variable." + ASTEmap[phylum->id]->variableName), t1, READ);

	Transition *t2 = PN.newTransition(prefix + "t2");
	PN.newArc(t2, PN.findPlace("out." + ASTEmap[phylum->id]->channelName));
	PN.newArc(p2, t2);
	PN.newArc(t2, p3);

	// Transitions for the negative control flow.
	if (parameters[P_NOSTANDARDFAULTS] == false)
	  Transition *t3 = throwFault(p2, p4, "running", prefix, mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);

	Transition *t3 = stop(p1, "initial", prefix);
	Transition *t5 = stop(p2, "running", prefix);
	Transition *t6 = stop(p4, "p4", prefix);
	Transition *t7 = stop(p3, "final", prefix);

	break;
      }

    case(petrinetsmall_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");

	Transition *t1 = PN.newTransition(prefix + "reply");
	PN.newArc(t1, PN.findPlace("out." + ASTEmap[phylum->id]->channelName));
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	break;
      }

    default: { /* no pattern in this case */ }      
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);
}





/******************************************************************************
  ASSIGN
******************************************************************************/

void PAssign(impl_tAssign* abstract_phylum, uview current_view)
{
  impl_tAssign_Assign* phylum = static_cast<impl_tAssign_Assign*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";


  switch (current_view)
  {
    case(petrinet_enum):
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p3 = PN.newPlace(prefix + "final");
	Place *p5 = PN.newPlace(prefix + "stop");
	Place *p6 = PN.newPlace(prefix + "stopped");
	
	Transition *t4 = stop(p1, "initial", prefix);
	Transition *t7 = stop(p3, "final", prefix);

	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	int copyCount = 1;
	Place *lastPlace = PN.findPlace(prefix + "initial");

	foreach (copyBlock=Copy(from,to); tCopy_list phylum->tCopy_list_1)
	{
	  string newPrefix = prefix + "copy." + toString(copyCount) + ".";

	  if (ASTEmap[from->id]->variableName != "")
	  {
	    Place *p2 = PN.newPlace(newPrefix + "running");
	    Place *p4 = PN.newPlace(newPrefix + "p4");
	    Place *p8 = PN.newPlace(newPrefix + "final");
      
	    Transition *t1 = PN.newTransition(newPrefix + "t1");
	    PN.newArc(lastPlace, t1);
	    PN.newArc(t1, p2);
            if (!parameters[P_NOVARIABLES])
	      PN.newArc(PN.findPlace("variable." + ASTEmap[from->id]->variableName), t1, READ);

	    Transition *t2 = PN.newTransition(newPrefix + "t2");
	    PN.newArc(p2, t2);
	    PN.newArc(t2, p8);
	    if (!parameters[P_NOVARIABLES])
	    {
	      PN.newArc(PN.findPlace("variable." + ASTEmap[to->id]->variableName), t2);
	      PN.newArc(t2, PN.findPlace("variable." + ASTEmap[to->id]->variableName));
	    }

	    // Transitions for the negative control flow.
	    if (parameters[P_NOSTANDARDFAULTS] == false)
	      Transition *t3 = throwFault(p2, p4, "running", newPrefix, mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);
	    
	    Transition *t6 = stop(p4, "p4at" + toString(copyCount), prefix);
	    
	    lastPlace = p8;
	  }
	  else
	  {
	    Place *p4 = PN.newPlace(newPrefix + "p4");
	    Place *p8 = PN.newPlace(newPrefix + "final");
	    
	    Transition *t2 = PN.newTransition(newPrefix + "t2");
	    PN.newArc(lastPlace, t2);
	    PN.newArc(t2, p8);
	    if (!parameters[P_NOVARIABLES])
	    {
	      PN.newArc(PN.findPlace("variable." + ASTEmap[to->id]->variableName), t2);
	      PN.newArc(t2, PN.findPlace("variable." + ASTEmap[to->id]->variableName));
	    }

	    // Transitions for the negative control flow.
	    if (parameters[P_NOSTANDARDFAULTS] == false)
	      Transition *t3 = throwFault(lastPlace, p4, "initial", newPrefix, mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);

	    Transition *t6 = stop(p4, "p4at" + toString(copyCount), prefix);

	    lastPlace = p8;
	  }

	  copyCount++;
	}

	PN.mergePlaces(PN.findPlace(prefix + "final"), lastPlace);
	break;
      }

    case (petrinetsmall_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");

	Transition *t1 = PN.newTransition(prefix + "assign");

	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);
      
	break;
      }

    default: { /* no pattern in this case */ }
  }
   
  footer(phylum->id);
}





/******************************************************************************
  VALIDATE
******************************************************************************/

void PValidate(impl_tValidate* abstract_phylum, uview current_view)
{
  impl_tValidate_Validate* phylum = static_cast<impl_tValidate_Validate*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";


  
  switch (current_view)
  {
    case(petrinet_enum):
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "running");
	Place *p3 = PN.newPlace(prefix + "final");
	Place *p4 = PN.newPlace(prefix + "p4");
	Place *p5 = PN.newPlace(prefix + "stop");
	Place *p6 = PN.newPlace(prefix + "stopped");

	Transition *t1 = PN.newTransition(prefix + "t1");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	Transition *t2 = PN.newTransition(prefix + "t2");
	PN.newArc(p2, t2);
	PN.newArc(t2, p3);

	// Transitions for the negative control flow.
	if (parameters[P_NOSTANDARDFAULTS] == false)
	  Transition *t3 = throwFault(p2, p4, "running", prefix, mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);

	Transition *t4 = stop(p1, "initial", prefix);
	Transition *t5 = stop(p2, "running", prefix);
	Transition *t6 = stop(p4, "p4", prefix);
	Transition *t7 = stop(p3, "final", prefix);

	break;
      }

    case(petrinetsmall_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");

	Transition *t1 = PN.newTransition(prefix + "validate");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	break;
      }
    
    default: { /* no pattern in this case */ }
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);
}





/******************************************************************************
  WAIT
******************************************************************************/

void PWaitUntil(impl_tWait* abstract_phylum, uview current_view)
{
  impl_tWait_WaitUntil* phylum = static_cast<impl_tWait_WaitUntil*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";


  switch (current_view)
  {
    case(petrinet_enum):
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
	Place *p3 = PN.newPlace(prefix + "stop");
	Place *p4 = PN.newPlace(prefix + "stopped");

	Transition *t1 = PN.newTransition(prefix + "waitUntil");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);
        if (!parameters[P_NOVARIABLES])
          PN.newArc(PN.findPlace("1.internal.clock"), t1, READ);

	// Transitions for the negative control flow.
	Transition *t2 = stop(p1, "initial", prefix);
	Transition *t3 = stop(p2, "final", prefix);

	break;
      }

    case (petrinetsmall_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");

	Transition *t1 = PN.newTransition(prefix + "waitUntil");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	break;
      }

    default: { /* no pattern in this case */ }
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);      
}


void PWaitFor(impl_tWait* abstract_phylum, uview current_view)
{
  impl_tWait_WaitFor* phylum = static_cast<impl_tWait_WaitFor*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";


  switch (current_view)
  {
    case(petrinet_enum):
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "TimeStamp");
	Place *p3 = PN.newPlace(prefix + "final");
	Place *p4 = PN.newPlace(prefix + "stop");
	Place *p5 = PN.newPlace(prefix + "stopped");

	Transition *t1 = PN.newTransition(prefix + "readTime");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);
        if (!parameters[P_NOVARIABLES])
	  PN.newArc(PN.findPlace("1.internal.clock"), t1, READ);

	Transition *t2 = PN.newTransition(prefix + "waitFor");
	PN.newArc(p2, t2);
	PN.newArc(t2, p3);
	PN.newArc(PN.findPlace("1.internal.clock"), t2, READ);

	// Transitions for the negative control flow.
	Transition *t3 = stop(p1, "initial", prefix);
	Transition *t4 = stop(p2, "TimeStamp", prefix);
	Transition *t5 = stop(p3, "final", prefix);
      }

    case (petrinetsmall_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");

	Transition *t1 = PN.newTransition(prefix + "waitFor");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	break;
      }

    default: { /* no pattern in this case */ }
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);      
}





/******************************************************************************
  THROW
******************************************************************************/

void PThrow(impl_tThrow* abstract_phylum, uview current_view)
{
  impl_tThrow_Throw* phylum = static_cast<impl_tThrow_Throw*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";


  switch(current_view)
  {
    case(petrinet_enum):
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "faulted");
	Place *p3 = PN.newPlace(prefix + "stop");
	Place *p4 = PN.newPlace(prefix + "stopped");

	// Transitions for the negative control flow.
	Transition *t1 = throwFault(p1, p2, "initial", prefix, mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);
	Transition *t2 = stop(p1, "initial", prefix);
	Transition *t3 = stop(p2, "faulted", prefix);

	break;
      }

    case(petrinetsmall_enum):
      {
	cerr << "the <throw> activity cannot be modelled!" << endl;

	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
	
	Transition *t1 = PN.newTransition(prefix + "empty");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	break;
      }

    default: { /* no pattern in this case */ }
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);      
}





/******************************************************************************
  RETHROW
******************************************************************************/

void PRethrow(impl_tRethrow* abstract_phylum, uview current_view)
{
  impl_tRethrow_Rethrow* phylum = static_cast<impl_tRethrow_Rethrow*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";


  Place *p1 = PN.newPlace(prefix + "initial");
  
  assert(ASTEmap[phylum->id] != NULL);
  Place *p2 = PN.newPlace(prefix + "faulted");
  Place *p3 = PN.newPlace(prefix + "stop");
  Place *p4 = PN.newPlace(prefix + "stopped");

  // Transitions for the negative control flow.
  Transition *t1 = throwFault(p1, p2, "initial", prefix, mkinteger(phylum->id), 4);
  PN.newArc(PN.findPlace(toString(ASTEmap[phylum->id]->parentScopeId) + ".internal.faultHandler.running"), t1, READ);

  Transition *t2 = stop(p1, "initial", prefix);
  Transition *t3 = stop(p2, "faulted", prefix);


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);
}





/******************************************************************************
  TERMINATE
******************************************************************************/

void PTerminate(impl_tTerminate* abstract_phylum, uview current_view)
{
  impl_tTerminate_Terminate* phylum = static_cast<impl_tTerminate_Terminate*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";

  
  switch(current_view)
  {
    case(petrinet_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
	Place *p3 = PN.newPlace(prefix + "stop");
	Place *p4 = PN.findPlace("1.internal.!Terminated");
	Place *p5 = PN.findPlace("1.internal.Terminated");
	Place *p6 = PN.findPlace(currentScope + "upperTerminate");
	Place *p7 = PN.newPlace(prefix + "stopped");

	Transition *t1 = PN.newTransition(prefix + "t1");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);
	PN.newArc(p5, t1, READ);

	Transition *t2 = PN.newTransition(prefix + "t2");
	PN.newArc(p1, t2);
	PN.newArc(t2, p2);
	PN.newArc(t2, p5);
	PN.newArc(p4, t2);
	PN.newArc(t2, p6);

	// Transitions for the negative control flow.
	Transition *t3 = stop(p1, "initial", prefix);
	Transition *t4 = stop(p2, "final", prefix);

	break;
      }

    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
	Place *p3 = PN.newPlace(prefix + "stop");
	Place *p4 = PN.newPlace(prefix + "stopped");

	Transition *t1 = PN.newTransition(prefix + "terminate");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);
	PN.newArc(t1, PN.findPlace("1.internal.exit"));

	// Transitions for the negative control flow.
	Transition *t2 = stop(p1, "initial", prefix);
	Transition *t3 = stop(p2, "final", prefix);

	break;
      }

    case(petrinetsmall_enum):
      {
	cerr << "the <terminate> activity cannot be modelled!" << endl;

	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
	
	Transition *t1 = PN.newTransition(prefix + "empty");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	break;
      }      

    default: { /* no pattern in this case */ }
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);
}





/******************************************************************************
  EXIT
******************************************************************************/

void PExit(impl_tExit* abstract_phylum, uview current_view)
{
  impl_tExit_Exit* phylum = static_cast<impl_tExit_Exit*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";

  
  switch(current_view)
  {
    case(petrinet_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
	Place *p3 = PN.newPlace(prefix + "stop");
	Place *p4 = PN.findPlace("1.internal.!Terminated");
	Place *p5 = PN.findPlace("1.internal.Terminated");
	Place *p6 = PN.findPlace(currentScope + "upperTerminate");
	Place *p7 = PN.newPlace(prefix + "stopped");

	Transition *t1 = PN.newTransition(prefix + "t1");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);
	PN.newArc(p5, t1, READ);

	Transition *t2 = PN.newTransition(prefix + "t2");
	PN.newArc(p1, t2);
	PN.newArc(t2, p2);
	PN.newArc(t2, p5);
	PN.newArc(p4, t2);
	PN.newArc(t2, p6);

	// Transitions for the negative control flow.
	Transition *t3 = stop(p1, "initial", prefix);
	Transition *t4 = stop(p2, "final", prefix);

	break;
      }

    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
	Place *p3 = PN.newPlace(prefix + "stop");
	Place *p4 = PN.newPlace(prefix + "stopped");

	Transition *t1 = PN.newTransition(prefix + "exit");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);
	PN.newArc(t1, PN.findPlace("1.internal.exit"));

	// Transitions for the negative control flow.
	Transition *t2 = stop(p1, "initial", prefix);
	Transition *t3 = stop(p2, "final", prefix);

	break;
      }

    case(petrinetsmall_enum):
      {
	cerr << "the <exit> activity cannot be modelled!" << endl;

	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
	
	Transition *t1 = PN.newTransition(prefix + "empty");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	break;
      }

    default: { /* no pattern in this case */ }
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);
}





/******************************************************************************
  COMPENSATE
******************************************************************************/

void PCompensate(impl_tCompensate* abstract_phylum, uview current_view)
{
  impl_tCompensate_Compensate* phylum = static_cast<impl_tCompensate_Compensate*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";

  
  switch(current_view)
  {
    case(petrinetsmall_enum):
      {
	cerr << "the <compensate> activity cannot be modelled!" << endl;

	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
	
	Transition *t1 = PN.newTransition(prefix + "empty");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	break;
      }

    default: { /* no pattern in this case */ }
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);
}





/******************************************************************************
  COMPENSATE SCOPE
******************************************************************************/

void PCompensateScope(impl_tCompensate* abstract_phylum, uview current_view)
{
  impl_tCompensate_CompensateScope* phylum = static_cast<impl_tCompensate_CompensateScope*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";

  
  switch(current_view)
  {
    case(petrinetsmall_enum):
      {
	cerr << "the <compensateScope> activity cannot be modelled!" << endl;

	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
	
	Transition *t1 = PN.newTransition(prefix + "empty");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	break;
      }

    default: { /* no pattern in this case */ }
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);
}
