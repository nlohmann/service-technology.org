/*****************************************************************************\
 * Copyright 2006, 2007 Niels Lohmann                                        *
 *                                                                           *
 * This file is part of GNU BPEL2oWFN.                                       *
 *                                                                           *
 * GNU BPEL2oWFN is free software; you can redistribute it and/or modify it  *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * GNU BPEL2oWFN is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  *
 * Public License for more details.                                          *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with GNU BPEL2oWFN; see file COPYING. if not, write to the Free Software  *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. *
\*****************************************************************************/

%{
/*!
 * \file    ast-rewrite.cc
 *
 * \brief   AST rewrite rules (implementation)
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 * 
 * \since   2006/10/11
 *
 * \date    \$Date: 2007/03/04 13:40:41 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.24 $
 */
%}





/******************************************************************************
 * IMPLICIT REWRITE RULES
 *
 * These rewriting rules are used to "unfold" simplified phylums (e.g.
 * NoStandardElements() to an explicit StandardElements() element with empty
 * source and target list. Furthermore, the rules implement the implicit
 * transformations described in the WS-BPEL specification (e.g. standard
 * FTC-handlers or <invoke> activities with FC-handlers).
 *****************************************************************************/


/*
 * "Transform" standard <invoke>.
 */
activityInvoke(
  annotatedInvoke(
    my_standardElements,
    my_tCorrelation_list,
    NiltCatch_list(),
    NoCatchAll(),
    implicitCompensationHandler(a),
    my_tToPart_list,
    my_tFromPart_list,
    my_integer
  )
)
->
< invoke:
  activityInvoke(
    Invoke(
      my_standardElements,
      my_tCorrelation_list,
      my_tToPart_list,
      my_tFromPart_list,
      my_integer
    )
  )
>;


/*
 * Implicit transformation of the <invoke> activity with fault or compensation
 * handler (cf. Sect. 10.3 in WS-BPEL 2.0)
 */
activityInvoke(
  annotatedInvoke(
    my_standardElements,
    my_tCorrelation_list,
    my_tCatchList,
    my_tCatchAll,
    my_tCompensationHandler,
    my_tToPart_list,
    my_tFromPart_list,
    my_integer
  )
)
->
< invoke:
  activityScope(
    Scope(
      my_standardElements,
      NiltVariable_list(),
      userDefinedFaultHandler(
        my_tCatchList,
        my_tCatchAll,
        mkinteger(0)
      ),
      my_tCompensationHandler,
      standardTerminationHandler(
        mkinteger(0)
      ),
      implicitEventHandler(
        mkinteger(0)
      ),
      NiltCorrelationSet_list(),
      StopInScope(),
      activityInvoke(
        Invoke(
          NoStandardElements(),
          my_tCorrelation_list,
          my_tToPart_list,
          my_tFromPart_list,
          my_integer
        )
      ),
      NiltPartnerLink_list(),
      mkinteger(0)
    )
  )
>;


/*
 * A shortcut in the parser is unfolded to empty list of target and source
 * links.
 */
NoStandardElements()
->
< implicit:
  StandardElements(
    NiltTarget_list(),
    NiltSource_list(),
    standardJoinCondition()
  )
>;


/*
 * Implicit Otherwise Branch (cf. Sect. 12.2 in BPEL4WS 1.1)
 */
NoOtherwise()
->
< implicit:
  Otherwise(
    activityEmpty(
      Empty(
        NoStandardElements(),
        mkinteger(0)
      )
    ),
    mkinteger(0)
  )
>;


/*
 * Default Termination Handler (cf. Sect. 12.5.1 in WS-BPEL 2.0)
 */
standardTerminationHandler(_id)
->
< implicit:
  TerminationHandler(
    activityCompensate(
      Compensate(
        NoStandardElements(),
        mkinteger(0)
      )
    ),
    _id
  )
>;


/*
 * Default fault handler (cf. Sect. 12.5.1 in WS-BPEL 2.0)
 */
implicitFaultHandler(_id) ->
< implicit:
  userDefinedFaultHandler(
    NiltCatch_list(),
    CatchAll(
      activitySequence(
        Sequence(
          NoStandardElements(),
          Consactivity_list(
            activityCompensate(
              Compensate(
                NoStandardElements(),
                mkinteger(0)
              )
            ),
            Consactivity_list(
              activityRethrow(
                Rethrow(
                  NoStandardElements(),
                  mkinteger(0)
                )
              ),
              Nilactivity_list()
            )
          ),
          mkinteger(0)
        )
      ),
      mkinteger(0)
    ),
    _id
  )
>;


/*
 * Default compensation handler (cf. Sect. 12.5.1 in WS-BPEL 2.0)
 */

implicitCompensationHandler(_id) ->
< implicit:
  userDefinedCompensationHandler(
    activityCompensate(
      Compensate(
        NoStandardElements(),
        mkinteger(0)
      )
    ),
    _id
  )
>;      


/*
 * A list of <catch> branches is empty if it just encloses a NoCatch
 * element.
 *
ConstCatch_list(
  NoCatch(),
  NiltCatch_list()
)
->
<implicit:
  NiltCatch_list()
>;
*/


/*
 * A fault handler is implicit if it does not enclose any <catch> or <catchAll>
 * branches (only in newly generate <scope> around <invoke>)
 */
activityScope(
  Scope(
    my_standardElements,
    my_variables,
    userDefinedFaultHandler(
      NiltCatch_list(),
      NoCatchAll(),
      my_fh_integer
    ),
    my_compensationHandler,
    my_terminationHandler,
    my_eventHandler,
    my_correlationSets,
    my_stop,
    activityInvoke(myinvoke),
    my_partnerlinks,
    my_integer
  )
)
->
< implicit:
  activityScope(
    Scope(
      my_standardElements,
      my_variables,
      implicitFaultHandler(
        my_fh_integer
      ),
      my_compensationHandler,
      my_terminationHandler,
      my_eventHandler,
      my_correlationSets,
      my_stop,
      activityInvoke(myinvoke),
      my_partnerlinks,
      my_integer
    )
  )
>;





/******************************************************************************
 * RENAMING REWRITE RULES
 *
 * These rules rewrite some AST phyla to simplify the later Petri net
 * generation. In particular, the two <wait> activities (for vs. until) and
 * the two <invoke> activities (synchronous vs. asynchronous) are
 * distinguished.
 *****************************************************************************/

/*
 * Rewrite-rules to distinguish <wait> activities
 */
Wait(a,_id) provided (ASTEmap[_id->value]->attributes["for"] != "")
->
< newNames:
  WaitFor(a,_id)
>;

Wait(a,_id) provided (ASTEmap[_id->value]->attributes["until"] != "")
->
< newNames:
  WaitUntil(a,_id)
>;


/*
 * Rewrite-rules to distinguish <invoke> activities
 */
Invoke(a,b,c,d,_id) provided (ASTEmap[_id->value]->attributes["outputVariable"] != "")
->
< newNames:
  SynchronousInvoke(a,b,c,d,_id)
>;

Invoke(a,b,c,d,_id) provided (ASTEmap[_id->value]->attributes["outputVariable"] == "")
->
< newNames:
  AsynchronousInvoke(a,b,c,d,_id)
>;
