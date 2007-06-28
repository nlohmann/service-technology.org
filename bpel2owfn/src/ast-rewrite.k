/*****************************************************************************\
  GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models

  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
			    Christian Gierds

  GNU BPEL2oWFN is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option) any
  later version.

  GNU BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  GNU BPEL2oWFN (see file COPYING); if not, see http://www.gnu.org/licenses
  or write to the Free Software Foundation,Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301  USA.
\*****************************************************************************/

%{
/*!
 * \file    ast-system-rk.cc
 *
 * \brief   AST rewrite rules (implementation)
 *
 *          After applying the rewrite rules "invoke" and "implict", the
 *          abstract syntax tree does not contain the following phyla:
 *           - activityInvoke
 *           - NoStandardElements
 *           - NoOtherwise
 *           - standardTerminationHandler
 *           - standardFaultHandlers
 *           - standardCompensationHandler
 *
 *          After applying these rules, the postprocessing is invoked.
 *
 *          Then, the rewrite rules "newNames" are applied, removing the
 *          following phylum:
 *           - Invoke
 *   
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 * 
 * \since   2006/10/11
 *
 * \date    \$Date: 2007/06/28 07:38:15 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file ast-rewrite.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.34 $
 */
%}





// the rewrite views (see definitions below)
%rview implicit;
%rview invoke;
%rview newNames;


// these lines are copied to file "ast-system-rk.cc"
%{ KC_REWRITE
#include "ast-details.h"
#include "globals.h"
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
  volatile_annotatedInvoke(
    my_standardElements,
    my_tCorrelation_list,
    NiltCatch_list(),
    NoCatchAll(),
    volatile_standardCompensationHandler(a),
    my_tToPart_list,
    my_tFromPart_list,
    my_integer
  )
)
->
< invoke:
  activityInvoke(
    volatile_Invoke(
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
  volatile_annotatedInvoke(
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
      NiltMessageExchange_list(),
      FaultHandlers(
        my_tCatchList,
        my_tCatchAll,
        mkinteger(0)
      ),
      my_tCompensationHandler,
      volatile_standardTerminationHandler(
        mkinteger(0)
      ),
      emptyEventHandlers(
        mkinteger(0)
      ),
      NiltCorrelationSet_list(),
      activityInvoke(
        volatile_Invoke(
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
 * Standard Termination Handler (cf. Sect. 12.5.1 in WS-BPEL 2.0)
 */
volatile_standardTerminationHandler(_id)
->
< implicit:
  TerminationHandler(
    activityCompensate(
      Compensate(
        NoStandardElements(),
        mkinteger(0)
      )
    ),
    mkinteger(0) // was: _id
  )
>;


/*
 * Standard fault handlers (cf. Sect. 12.5.1 in WS-BPEL 2.0)
 */
volatile_standardFaultHandlers(_id) ->
< implicit:
  FaultHandlers(
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
    mkinteger(0) // was: _id
  )
>;


/*
 * Standard Compensation Handler (cf. Sect. 12.5.1 in WS-BPEL 2.0)
 */
volatile_standardCompensationHandler(_id) ->
< implicit:
  CompensationHandler(
    activityCompensate(
      Compensate(
        NoStandardElements(),
        mkinteger(0)
      )
    ),
    mkinteger(0) // was: _id
  )
>;      


/*
 * A fault handler is implicit if it does not enclose any <catch> or <catchAll>
 * branches (only in newly generate <scope> around <invoke>)
 */
activityScope(
  Scope(
    my_standardElements,
    my_variables,
    my_messageExchanges,
    FaultHandlers(
      NiltCatch_list(),
      NoCatchAll(),
      my_fh_integer
    ),
    my_compensationHandler,
    my_terminationHandler,
    my_eventHandler,
    my_correlationSets,
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
      my_messageExchanges,
      volatile_standardFaultHandlers(
        my_fh_integer
      ),
      my_compensationHandler,
      my_terminationHandler,
      my_eventHandler,
      my_correlationSets,
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
 * generation. In particular, the two <invoke> activities (synchronous vs.
 * asynchronous) are distinguished.
 *
 * Note: If not variable is given, an asynchronous <invoke> is created.
 *****************************************************************************/

/*
 * Rewrite-rules to distinguish <invoke> activities
 */
volatile_Invoke(a,b,c,d,_id) provided (globals::ASTEmap[_id->value]->attributes["outputVariable"] != "")
->
< newNames:
  SynchronousInvoke(a,b,c,d,_id)
>;

volatile_Invoke(a,b,c,d,_id)
->
< newNames:
  AsynchronousInvoke(a,b,c,d,_id)
>;
