%rview implicit;



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

Switch(
    a, // standardElements
    b, // tCase_list
    NiltOtherwise_list() // tOtherwise_list
    )

->

< implicit: Switch(
    a,
    b,
    ConstOtherwise_list( // create a tOtherwise_list
      Otherwise(
	activityEmpty(
	  Empty(
	    StandardElements(
	      NiltTarget_list(),
	      NiltSource_list()
	      )
	    )
	  )
	),
      NiltOtherwise_list()
      )
    )
>;
