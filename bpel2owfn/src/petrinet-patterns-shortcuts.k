/*
#include <string>

#include "petrinet-patterns-shortcuts.h"
#include "petrinet.h"

using std::string;
*/

/******************************************************************************
 * Functions for the Petri net unparser defined in bpel-unparse-petri.k
 *****************************************************************************/

/*
Transition *throwFault(Place *p1, Place *p2,
    string p1name, string prefix, kc::integer id,
    int negativeControlFlow, bool preventFurtherFaults = true);
Transition *stop(Place *p, string p_name, string prefix);
void dpeLinks(Transition* t, int id);
*/








/******************************************************************************
 * Functions for the Petri net unparsers
 *****************************************************************************/

/*!
 * \brief generates transition and places to throw a fault
 *
 * This functions generates a subnet to model the throwing of a fault. It
 * models faults in any control flow of the BPEL process as well as in any
 * patterns used.
 *
 * \param p1  the place in positive control flow from which the control flow
 *            enters the negative control flow
 *
 * \param p2  the place of the pattern on which a token shall be produced which
 *            can only be removed by stopping the pattern
 *
 * \param p1name  name of place p1 which is used to label the generated
 *                transitions (e.g. if p1 is called "running" generated
 *                transitions end with this name (e.g. "throwFault.running")
 *
 * \param prefix  prefix of the pattern to label generated places and
 *                transitions
 *
 * \param id  identifier of the caller activity
 *
 * \param negativeControlFlow  signals where the activity is located:
 *                             - 0: inside a scope or the process
 *                             - 1: inside a fault handler
 *                             - 2: inside a compensation handler
 *
 * \param preventFurtherFaults  controls what happens to further faults
 *                               - true: these faults are prevented (standard)
 *                               - false: these faults are suppressed
 *
 * \return a pointer to the (first) generated fault transition
 *
 * \ingroup creation
*/
Transition *throwFault(Place *p1, Place *p2,
    string p1name, string prefix, integer id,
    int negativeControlFlow, bool preventFurtherFaults = true)
{
  extern string currentScope;	// introduced in bpel-unparse.k
  extern PetriNet PN;		// introduced in main.c
  extern map<unsigned int, ASTE*> ASTEmap; // introduced in bpel-unparse-tools.h

  assert(p1 != NULL);
  assert(p2 != NULL);


  // no fault transitions in case of "communicationonly" parameter
  if (parameters[P_COMMUNICATIONONLY])
    return NULL;


  // if attribute "exitOnStandardFault" is set to "yes", the process should
  // terminate rather than handling the fault
  if (ASTEmap[ASTEmap[id->value]->parentScopeId]->attributes["exitOnStandardFault"] == "yes")
  {
    Transition *t1 = PN.newTransition(prefix + "exitOnStandardFault." + p1name);
    PN.newArc(p1, t1);
    PN.newArc(t1, p2);
    PN.newArc(t1, PN.findPlace("1.internal.exit"));

    return t1;
  }


  // fault handling for the `new' patterns
    switch (negativeControlFlow)
    {
      case(0): // activity in scope or process
	{
          unsigned int parentId = ASTEmap[id->value]->parentScopeId;

	  Transition *t1 = PN.newTransition(prefix + "throwFault." + p1name);
	  PN.newArc(p1, t1);
	  PN.newArc(t1, p2);
	  PN.newArc(t1, PN.findPlace(toString(parentId) + ".internal.fault_in"));

	  return t1;
	}

      case(1): // activity in fault handlers
      case(4): // <rethrow> activity
	{
	  // The parent scope is just the scope of the handler or the
	  // activity. Thus, we are interested in the parent's parent.
          unsigned int parentId = ASTEmap[ASTEmap[id->value]->parentScopeId]->parentScopeId;

	  Transition *t1;
	  if (negativeControlFlow == 4)
	    t1 = PN.newTransition(prefix + "rethrow");
	  else
	    t1 = PN.newTransition(prefix + "rethrowFault." + p1name);

	  PN.newArc(p1, t1);
	  PN.newArc(t1, p2);

	  if (ASTEmap[id->value]->parentScopeId == 1)
	    PN.newArc(t1, PN.findPlace(toString(parentId) + ".internal.exit"));
	  else
	    PN.newArc(t1, PN.findPlace(toString(parentId) + ".internal.fault_in"));

	  return t1;
	}

      case(2): // activity in compensation handler
	{
          unsigned int parentId = ASTEmap[id->value]->parentScopeId;

          // parent scope of a compensation handler shouldn't be the process :)
	  assert(parentId != 1);

	  Transition *t1 = PN.newTransition(prefix + "throwCHFault." + p1name);
	  PN.newArc(p1, t1);
	  PN.newArc(t1, p2);
	  PN.newArc(t1, PN.findPlace(toString(parentId) + ".internal.ch_fault_in"));

	  return t1;
	}

      case(3): // activity in termination handler
        {
          unsigned int parentId = ASTEmap[id->value]->parentScopeId;

	  Transition *t1 = PN.newTransition(prefix + "signalFault." + p1name);
	  PN.newArc(p1, t1);
	  PN.newArc(t1, p2);
	  PN.newArc(t1, PN.findPlace(toString(parentId) + ".internal.terminationHandler.inner_fault"));
        }
    }
    return NULL;
}





/*!
 * \brief generates a subnet to stop
 *
 * Generates a transition and places to stop the activity, i.e. a
 * transition moving a token on the "stop" place to "stopped".
 *
 * \param p  the place in control flow from which the token is move to
 *           "stop"
 *
 * \param p_name  name of place p which is used to label the generated
 *                transition (e.g. if p1 is called "running" generated
 *                transition end with this name (e.g. "stop.running")
 *
 * \param prefix  prefix of the pattern to label generated transition
 *
 * \return a pointer to the stop transition
 *
 * \ingroup creation
 */
Transition *stop(Place *p, string p_name, string prefix)
{
  extern PetriNet PN;	// introduced in main.c

  assert(p != NULL);

  // no stop transitions in case of "nano" parameter
  if (parameters[P_COMMUNICATIONONLY])
    return NULL;
  
  Transition *stopTransition = PN.newTransition(prefix + "stopped." + p_name);
  PN.newArc(PN.findPlace(prefix + "stop"), stopTransition);
  PN.newArc(stopTransition, PN.findPlace(prefix + "stopped"));
  PN.newArc(p, stopTransition);

  return stopTransition;
}







/*!
 * \brief add a subnet for dead-path elimination
 *
 * Creates arcs to set links on dead paths to false.
 *
 * \param t	transition that invokes DPE
 * \param id	the identifier of the AST node
 *
 * \ingroup creation
 */
void dpeLinks(Transition *t, int id)
{
  ENTER("[ASTT]");

  extern PetriNet PN;	// introduced in main.c
  extern map<unsigned int, ASTE*> ASTEmap; // introduced in bpel-unparse-tools.h

  assert(t != NULL);
  assert(ASTEmap[id] != NULL);

  for (set<unsigned int>::iterator linkID = ASTEmap[id]->enclosedSourceLinks.begin();
      linkID != ASTEmap[id]->enclosedSourceLinks.end();
      linkID++)
  {
    assert(ASTEmap[*linkID] != NULL);
    string linkName = ASTEmap[*linkID]->linkName;
    PN.newArc(t, PN.findPlace("!link." + linkName));
  }

  LEAVE("[ASTT]");
}


