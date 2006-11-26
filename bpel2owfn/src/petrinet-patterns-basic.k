%{
#include "ast-config.h"
#include "helpers.h"
#include "ast-tools.h"
#include "ast-details.h"
#include "options.h"

extern PetriNet *TheNet;
extern map<unsigned int, ASTE*> ASTEmap;
%}




/******************************************************************************
  EMPTY
******************************************************************************/

void PEmpty(const impl_tEmpty* abstract_phylum, uview current_view)
{
  impl_tEmpty_Empty* phylum = (impl_tEmpty_Empty*) abstract_phylum;
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";  


  // places
  Place *p1 = TheNet->newPlace(prefix + "initial");
  Place *p2 = TheNet->newPlace(prefix + "final");


  // transitions
  Transition *t1 = TheNet->newTransition(prefix + "empty");
  TheNet->newArc(p1, t1);
  TheNet->newArc(t1, p2);


  // places and transitions for the negative control flow
  if (current_view != petrinetsmall_enum)
  {
    Place *p3 = TheNet->newPlace(prefix + "stop");
    Place *p4 = TheNet->newPlace(prefix + "stopped");
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

void PAsynchronousInvoke(const impl_tInvoke* abstract_phylum, uview current_view)
{
  impl_tInvoke_AsynchronousInvoke* phylum = (impl_tInvoke_AsynchronousInvoke*) abstract_phylum;
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";  

  Place *p1 = TheNet->newPlace(prefix + "initial");
  Place *p2 = TheNet->newPlace(prefix + "running");
  Place *p3 = TheNet->newPlace(prefix + "final");
  Place *p4 = TheNet->newPlace(prefix + "p4");
  Place *p5 = TheNet->newPlace(prefix + "stop");
  Place *p6 = TheNet->newPlace(prefix + "stopped");

  Transition *t1 = TheNet->newTransition(prefix + "t1");
  TheNet->newArc(p1, t1); 
  TheNet->newArc(t1, p2);
  if (!parameters[P_NOVARIABLES])
    TheNet->newArc(TheNet->findPlace("variable." + ASTEmap[phylum->id]->inputVariableName), t1, READ);

  Transition *t2 = TheNet->newTransition(prefix + "t2");
  TheNet->newArc(p2, t2);
  TheNet->newArc(t2, p3);
  TheNet->newArc(t2, TheNet->findPlace("out." + ASTEmap[phylum->id]->channelName));
  t2->type = OUT;

  
  // Transitions for the negative control flow.
  if (parameters[P_NOSTANDARDFAULTS] == false)
    Transition *t3 = throwFault(p2, p4, "running", prefix, mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);

  Transition *t4 = stop(p1, "initial", prefix);
  Transition *t5 = stop(p2, "running", prefix);
  Transition *t6 = stop(p4, "p4", prefix);
  Transition *t7 = stop(p3, "final", prefix);


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);
}





void PSynchronousInvoke(const impl_tInvoke* abstract_phylum, uview current_view)
{
  impl_tInvoke_SynchronousInvoke* phylum = (impl_tInvoke_SynchronousInvoke*) abstract_phylum;
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";  


  Place *p1 = TheNet->newPlace(prefix + "initial");
  Place *p2 = TheNet->newPlace(prefix + "running1");
  Place *p3 = TheNet->newPlace(prefix + "p3");
  Place *p4 = TheNet->newPlace(prefix + "running2");
  Place *p5 = TheNet->newPlace(prefix + "final");
  Place *p6 = TheNet->newPlace(prefix + "p6");
  Place *p7 = TheNet->newPlace(prefix + "stop");
  Place *p8 = TheNet->newPlace(prefix + "stopped");

  Transition *t1 = TheNet->newTransition(prefix + "t1");
  TheNet->newArc(p1, t1);
  TheNet->newArc(t1, p2);
  if (!parameters[P_NOVARIABLES])
    TheNet->newArc(TheNet->findPlace("variable." + ASTEmap[phylum->id]->inputVariableName), t1, READ);

  Transition *t2 = TheNet->newTransition(prefix + "t2");
  TheNet->newArc(t2, TheNet->findPlace("out." + ASTEmap[phylum->id]->channelName));
  TheNet->newArc(p2, t2);
  TheNet->newArc(t2, p3);
  t2->type = OUT;

  Transition *t3 = TheNet->newTransition(prefix + "t3");
  TheNet->newArc(TheNet->findPlace("in." + ASTEmap[phylum->id]->channelName), t3);
  TheNet->newArc(p3, t3);
  TheNet->newArc(t3, p4);
  t3->type = IN;

  Transition *t4 = TheNet->newTransition(prefix + "t4");
  TheNet->newArc(p4, t4);
  TheNet->newArc(t4, p5);
  if (!parameters[P_NOVARIABLES])
  {
    TheNet->newArc(TheNet->findPlace("variable." + ASTEmap[phylum->id]->outputVariableName), t4);
    TheNet->newArc(t4, TheNet->findPlace("variable." + ASTEmap[phylum->id]->outputVariableName));
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


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);
}





/******************************************************************************
  RECEIVE
******************************************************************************/

void PReceive(const impl_tReceive* abstract_phylum, uview current_view)
{
  impl_tReceive_Receive* phylum = (impl_tReceive_Receive*) abstract_phylum;
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";  


  Place *p1 = TheNet->newPlace(prefix + "initial");
  Place *p2 = TheNet->newPlace(prefix + "running");
  Place *p3 = TheNet->newPlace(prefix + "final");
  Place *p4 = TheNet->newPlace(prefix + "p4");
  Place *p5 = TheNet->newPlace(prefix + "stop");
  Place *p6 = TheNet->newPlace(prefix + "stopped");

  Transition *t1 = TheNet->newTransition(prefix + "t1");
  TheNet->newArc(TheNet->findPlace("in." + ASTEmap[phylum->id]->channelName), t1);
  TheNet->newArc(p1, t1);
  TheNet->newArc(t1, p2);
  t1->type = IN;

  Transition *t2 = TheNet->newTransition(prefix + "t2");
  TheNet->newArc(p2, t2);
  TheNet->newArc(t2, p3);
  if (!parameters[P_NOVARIABLES])
  {
    TheNet->newArc(TheNet->findPlace("variable." + ASTEmap[phylum->id]->variableName), t2);
    TheNet->newArc(t2, TheNet->findPlace("variable." + ASTEmap[phylum->id]->variableName));
  }


  // Transitions for the negative control flow.
  if (parameters[P_NOSTANDARDFAULTS] == false)
    Transition *t3 = throwFault(p2, p4, "running", prefix, mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);

  Transition *t4 = stop(p1, "initial", prefix);
  Transition *t5 = stop(p2, "running", prefix);
  Transition *t6 = stop(p4, "p4", prefix);
  Transition *t7 = stop(p3, "final", prefix);  


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);      
}




/******************************************************************************
  REPLY
******************************************************************************/

void PReply(const impl_tReply* abstract_phylum, uview current_view)
{
  impl_tReply_Reply* phylum = (impl_tReply_Reply*) abstract_phylum;
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";  

  Place *p1 = TheNet->newPlace(prefix + "initial");
  Place *p2 = TheNet->newPlace(prefix + "running");
  Place *p3 = TheNet->newPlace(prefix + "final");
  Place *p4 = TheNet->newPlace(prefix + "p4");
  Place *p5 = TheNet->newPlace(prefix + "stop");
  Place *p6 = TheNet->newPlace(prefix + "stopped");

  Transition *t1 = TheNet->newTransition(prefix + "t1");
  TheNet->newArc(p1, t1);
  TheNet->newArc(t1, p2);
  if (!parameters[P_NOVARIABLES])
    TheNet->newArc(TheNet->findPlace("variable." + ASTEmap[phylum->id]->variableName), t1, READ);

  Transition *t2 = TheNet->newTransition(prefix + "t2");
  TheNet->newArc(t2, TheNet->findPlace("out." + ASTEmap[phylum->id]->channelName));
  TheNet->newArc(p2, t2);
  TheNet->newArc(t2, p3);
  t2->type = OUT;

  // Transitions for the negative control flow.
  if (parameters[P_NOSTANDARDFAULTS] == false)
    Transition *t3 = throwFault(p2, p4, "running", prefix, mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);

  Transition *t3 = stop(p1, "initial", prefix);
  Transition *t5 = stop(p2, "running", prefix);
  Transition *t6 = stop(p4, "p4", prefix);
  Transition *t7 = stop(p3, "final", prefix);


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);
}





/******************************************************************************
  ASSIGN
******************************************************************************/

void PAssign(const impl_tAssign* abstract_phylum, uview current_view)
{
  impl_tAssign_Assign* phylum = (impl_tAssign_Assign*) abstract_phylum;
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";

  Place *p1 = TheNet->newPlace(prefix + "initial");
  Place *p3 = TheNet->newPlace(prefix + "final");
  Place *p5 = TheNet->newPlace(prefix + "stop");
  Place *p6 = TheNet->newPlace(prefix + "stopped");

  Transition *t4 = stop(p1, "initial", prefix);
  Transition *t7 = stop(p3, "final", prefix);


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  int copyCount = 1;
  Place *lastPlace = TheNet->findPlace(prefix + "initial");

}
