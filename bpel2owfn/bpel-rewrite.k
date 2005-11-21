%{
/*!
 * \file bpel-rewrite.cc
 *
 * \brief Rewrite rules for BPEL (implementation)
 *
 * This file defines and implements certain rewrite rules to realize the
 * implicit presence of e.g. scopes or other empty otherwise brances.
 * 
 * \author  
 *          - Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          
 * \date    2005-11-10
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \note    This file was created using Kimwitu++ version 2.3.8 (C) 1998-2003
 *          Humboldt-University of Berlin reading file bpel-rewrite.k. See
 *          http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version
 *          - 2005-11-10 (nlohmann) Added doxygen comments.
 * 
 * \todo
 *          - there are certainly more implicit transformations neccessary
 *          - add copy-rules for <invoke>-cases
 *
 * \bug
 *          - XML-output works, but Petri net output crashes
 * 
 */
%}


%{	
/*!
 * \file bpel-rewrite.h
 * \brief Rewrite rules for BPEL (interface)
 *
 * See \ref bpel-rewrite.cc for more information.
 */
%}


%{
/*!
 * \class kc::implicit_class
 * \brief Rewrite rules: implicit transformations
 */
%}
%rview implicit;





%{ KC_REWRITE_HEADER
#include "bpel-attributes.h"
  
tSwitch cp_attr(tSwitch from, tSwitch to);
tEmpty implicitEmpty(tEmpty old);
%}





%{ KC_REWRITE
tSwitch cp_attr(tSwitch from, tSwitch to)
{
  to->name = from->name;
  to->joinCondition = from->joinCondition;
  to->suppressJoinFailure = from->suppressJoinFailure;
  to->id = from->id;
  return to;
}

tEmpty implicitEmpty(tEmpty old)
{
  extern attributeManager att;
  old->id = att.nextId();
  old->name = mkcasestring("implicit_otherwise");
  return old;
}
%}



/******************************************************************************
  INVOKE SHORTCUT
******************************************************************************/

/*
  Finally, an activity can be associated with another activity that acts as its
  compensation action. This compensation handler can be invoked either
  explicitly or by the default compensation handler of the enclosing scope (see
  Scopes and Compensation Handlers).
  
  Semantically, the specification of local fault and/or compensation handlers
  is equivalent to the presence of an implicit scope immediately enclosing the
  activity and providing those handlers. The name of such an implicit scope is
  always the same as the name of the activity it encloses.
*/



/* All <invoke>-elements with a fault or compensation handler have to be
 * embedded into a <scope>. <invoke>-elements are stored inside the phylum
 * Invoke(a,b,c,d,e). There are three cases to be detected:
 */

/* case 1: there is a Fault Handler in the <invoke> */

/* case 1a: there is a <catch>-element */

activityInvoke(
    Invoke(
      a, //standardElements
      b, //tCorrelation_list
      c = ConstCatch_list(*,*), //tCatch_list
      d, //tCatchAll_list
      e //tCompensationHandler_opt
      )
    )

->

< implicit: activityScope(
    Scope(
      a, //standardElements of <invoke>
      NiltVariable_list(),
      ConstFaultHandlers_opt( // create a FaultHandler
	FaultHandlers(
	  c, //tCatch_list of <invoke>
	  d  //tCatchAll_list of <invoke>
	  ),
	NiltFaultHandlers_opt()
	),
      e, //tCompensationHandler_opt of <invoke>
      NiltEventHandlers_opt(),
      activityInvoke(
	Invoke2(
	  StandardElements(
	    NiltTarget_list(),
	    NiltSource_list()
	    ),
	  b //tCorrelation_list of <invoke>
	  )
	)
      )
    )	
>;



/* case 1b: there is a <catchAll>-element */

activityInvoke(
    Invoke(
      a, //standardElements
      b, //tCorrelation_list
      c, //tCatch_list
      d = ConstCatchAll_list(*,*), //tCatchAll_list
      e //tCompensationHandler_opt
      )
    )

->

< implicit: activityScope(
    Scope(
      a, //standardElements of <invoke>
      NiltVariable_list(),
      ConstFaultHandlers_opt( // create a FaultHandler
	FaultHandlers(
	  c, //tCatch_list of <invoke>
	  d  //tCatchAll_list of <invoke>
	  ),
	NiltFaultHandlers_opt()
	),
      e, //tCompensationHandler_opt of <invoke>
      NiltEventHandlers_opt(),
      activityInvoke(
	Invoke2(
	  StandardElements(
	    NiltTarget_list(),
	    NiltSource_list()
	    ),
	  b //tCorrelation_list of <invoke>
	  )
	)
      )
    )	
>;




/* case 2: there is a Compensation Handler in the <invoke> */

activityInvoke(
    Invoke(
      a, //standardElements
      b, //tCorrelation_list
      c, //tCatch_list
      d, //tCatchAll_list
      e = ConstCompensationHandler_opt(*,*) //tCompensationHandler_opt
      )
    )

->

< implicit: activityScope(
    Scope(
      a, //standardElements of <invoke>
      NiltVariable_list(),
      ConstFaultHandlers_opt( // create a FaultHandler
	FaultHandlers(
	  c, //tCatch_list of <invoke>
	  d  //tCatchAll_list of <invoke>
	  ),
	NiltFaultHandlers_opt()
	),
      e, //tCompensationHandler_opt of <invoke>
      NiltEventHandlers_opt(),
      activityInvoke(
	Invoke2(
	  StandardElements(
	    NiltTarget_list(),
	    NiltSource_list()
	    ),
	  b //tCorrelation_list of <invoke>
	  )
	)
      )
    )	
>;




/******************************************************************************
  EMPTY OTHERWISE BRANCH
******************************************************************************/

/*
  If the otherwise branch is not explicitly specified, then an otherwise branch
   with an empty activity is deemed to be present.
*/

origin=Switch(
    a, // standardElements
    b, // tCase_list
    NiltOtherwise_list() // tOtherwise_list
    )

->

< implicit: cp_attr(origin, Switch(
    a,
    b,
    ConstOtherwise_list( // create a tOtherwise_list
      Otherwise(
	activityEmpty(
	  implicitEmpty(Empty(
	    StandardElements(
	      NiltTarget_list(),
	      NiltSource_list()
	      )
	    )
	  ))
	),
      NiltOtherwise_list()
      )
    ))
>;
