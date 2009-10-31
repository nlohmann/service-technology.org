/*****************************************************************************\
  GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models

  Copyright (C) 2009        Niels Lohmann
  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
                            Christian Gierds

  GNU BPEL2oWFN is free software: you can redistribute it and/or modify it
  under the terms of the GNU Affero General Public License as published by the
  Free Software Foundation, either version 3 of the License, or (at your
  option) any later version.

  GNU BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
  details.

  You should have received a copy of the GNU Affero General Public License
  along with GNU BPEL2oWFN. If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

%{
/*!
 * \file    ast-to-petrinet.cc
 *
 * \brief   Petri net unparser (implementation)
 *
 *          This unparser traverses the abstract syntax tree and calls the
 *          functions for the Petri net patterns.
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2005-11-01
 *
 * \date    \$Date: 2007/06/28 07:38:15 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          ast-to-petrinet.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.15 $
 */
%}





/******************************************************************************
  BASIC ACTIVITIES

  These patterns are implemented in file "petrinet-patterns-basic.k".
******************************************************************************/

Empty -> [petrinetnew petrinetsmall:
  { PEmpty($0, kc_current_view_base); }
];

OpaqueActivity -> [petrinetnew petrinetsmall:
  { POpaqueActivity($0, kc_current_view_base); }
];

AsynchronousInvoke -> [petrinetnew petrinetsmall:
  { PAsynchronousInvoke($0, kc_current_view_base); }
];

SynchronousInvoke -> [petrinetnew petrinetsmall:
  { PSynchronousInvoke($0, kc_current_view_base); }
];

Receive -> [petrinetnew petrinetsmall:
  { PReceive($0, kc_current_view_base); }
];

Reply -> [petrinetnew petrinetsmall:
  { PReply($0, kc_current_view_base); }
];

Assign -> [petrinetnew petrinetsmall:
  { PAssign($0, kc_current_view_base); }
];

Validate -> [petrinetnew petrinetsmall:
  { PValidate($0, kc_current_view_base); }
];

Wait -> [petrinetnew petrinetsmall:
  { PWait($0, kc_current_view_base); }
];

Throw -> [petrinetnew petrinetsmall:
  { PThrow($0, kc_current_view_base); }
];

Rethrow -> [petrinetnew:
  { PRethrow($0, kc_current_view_base); }
];

Exit -> [petrinetnew petrinetsmall:
  { PExit($0, kc_current_view_base); }
];

Compensate -> [petrinetnew petrinetsmall:
  { PCompensate($0, kc_current_view_base); }
];

CompensateScope -> [petrinetnew petrinetsmall:
  { PCompensateScope($0, kc_current_view_base); }
];





/******************************************************************************
  STRUCTURED ACTIVITIES

  These patterns are implemented in file "petrinet-patterns-structured.k".
******************************************************************************/

While -> [petrinetnew petrinetsmall:
  { PWhile($0, kc_current_view_base); }
];

RepeatUntil -> [petrinetnew petrinetsmall:
  { PRepeatUntil($0, kc_current_view_base); }
];

Sequence -> [petrinetnew petrinetsmall:
  { PSequence($0, kc_current_view_base); }
];

Flow -> [petrinetnew petrinetsmall:
  { PFlow($0, kc_current_view_base); }
];

Pick -> [petrinetnew petrinetsmall:
  { PPick($0, kc_current_view_base); }
];

If -> [petrinetnew petrinetsmall:
  { PIf($0, kc_current_view_base); }
];

ForEach -> [petrinetnew petrinetsmall:
  { PForEach($0, kc_current_view_base); }
];





/******************************************************************************
  SCOPE / PROCESS

  These patterns are implemented in file "petrinet-patterns-scopes.k".
******************************************************************************/

Process -> [petrinetnew petrinetsmall:
  { PProcess($0, kc_current_view_base);

    // clear global mappings
    globals::ASTE_scopeNames.clear(); }
];

Scope -> [petrinetnew petrinetsmall:
  { PScope($0, kc_current_view_base); }
];





/******************************************************************************
  HANDLERS

  These patterns are implemented in file "petrinet-patterns-handlers.k".
******************************************************************************/

FaultHandlers -> [petrinetnew:
  { PFaultHandlers($0, kc_current_view_base); }
];

TerminationHandler -> [petrinetnew:
  { PTerminationHandler($0, kc_current_view_base); }
];

CompensationHandler -> [petrinetnew:
  { PCompensationHandler($0, kc_current_view_base); }
];

EventHandlers -> [petrinetnew petrinetsmall:
  { PEventHandlers($0, kc_current_view_base); }
];





/******************************************************************************
  STANDARD ELEMENTS / LINKS

  These patterns are implemented in file "petrinet-patterns-links.k".
******************************************************************************/

StandardElements(NiltTarget_list(), NiltSource_list(), jc) -> [petrinetnew petrinetsmall:
  { P_LinkWrapper0($0, kc_current_view_base); }
];

StandardElements(NiltTarget_list(), a, jc) -> [petrinetnew petrinetsmall:
  { P_LinkWrapper1($0, kc_current_view_base); }
];

This=StandardElements(a, NiltSource_list(), jc) provided (This->suppressJF) -> [petrinetnew petrinetsmall:
  { P_LinkWrapper2($0, kc_current_view_base); }
];

This=StandardElements(a, NiltSource_list(), jc) provided (!This->suppressJF) -> [petrinetnew petrinetsmall:
  { P_LinkWrapper3($0, kc_current_view_base); }
];

This=StandardElements(a,b, jc) provided (This->suppressJF) -> [petrinetnew petrinetsmall:
  { P_LinkWrapper4($0, kc_current_view_base); }
];

This=StandardElements(a,b, jc) provided (!This->suppressJF) -> [petrinetnew petrinetsmall:
  { P_LinkWrapper5($0, kc_current_view_base); }
];
