/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds, Dennis Reinert      *
 *                                                                           *
 * This file is part of BPEL2oWFN.                                           *
 *                                                                           *
 * BPEL2oWFN is free software; you can redistribute it and/or modify it      *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT  *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with BPEL2oWFN; if not, write to the Free Software Foundation, Inc., 51   *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
\*****************************************************************************/

%{
/*!
 * \file    ast-to-petrinet-full1.cc
 *
 * \brief   Petri net unparser (implementation)
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nlohmann $
 *
 * \since   2005-11-01
 *
 * \date    \$Date: 2006/12/04 14:32:16 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          bpel-unparse-petri.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.215 $
 *
 */
%}





/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

/*
 * The user-defined fault handler inside a process. See Fig. 37 for details.
 */

This=userDefinedFaultHandler(a,b, _id) provided (ASTEmap[_id->value]->inProcess) ->
  [petrinet:
    {
      string prefix = "1.internal.faultHandler.";
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<userDefinedFaultHandlerInsideProcess>\n");

      Place *p3 =  PN.newPlace(prefix + "final");
      Place *p4 =  PN.newPlace(prefix + "fault");
      Place *p7 =  PN.newPlace(prefix + "faultSave");
      Place *p8 =  PN.newPlace(prefix + "p8");
      Place *p9 =  PN.newPlace(prefix + "p9");
      Place *p11 = PN.newPlace(prefix + "p11");
      Place *p12 = PN.newPlace(prefix + "p12");
      Place *p13 = PN.newPlace(prefix + "rethrow");
      Place *p14 = PN.newPlace(prefix + "p14");
      Place *p15 = PN.newPlace(prefix + "Ended");
      Place *p16 = PN.newPlace(prefix + "!Ended");
      Place *p17 = PN.newPlace(prefix + "ch_fh");
      Place *p18 = PN.newPlace(prefix + "ch_out");

      Transition *t7 = PN.newTransition(prefix + "t7");
      PN.newArc(p7, t7);
      PN.newArc(t7, p11);
      PN.newArc(p13, t7);
      PN.newArc(t7, p14);

      Transition *t8 = PN.newTransition(prefix + "t8");
      PN.newArc(t8, p3);
      PN.newArc(p12, t8);
      PN.newArc(p14, t8);
      PN.newArc(t8, p15);
      PN.newArc(p16, t8);

      Transition *t9 = PN.newTransition(prefix + "t9");
      PN.newArc(p13, t9);
      PN.newArc(p15, t9, READ);


      // The rethrow part is only necessary if there is no <catchAll> element.
      if (!ASTEmap[_id->value]->hasCatchAll)
      {
	Transition *t5 = PN.newTransition(prefix + "t5");
	PN.newArc(p4, t5);
	PN.newArc(p7, t5, READ);
	PN.newArc(t5, p8);

	Transition *t6 = PN.newTransition(prefix + "t6");
	PN.newArc(t6, p3);
	PN.newArc(p7, t6);
	PN.newArc(p9, t6);
	PN.newArc(t6, p15);
	PN.newArc(p16, t6);


	// The <compensate />-element.
	string newPrefix = "1.internal.faultHandler.compensate.";

	Place *compensate_p1 = PN.newPlace(newPrefix + "initial");
	Place *compensate_p2 = PN.newPlace(newPrefix + "running");
	Place *compensate_p3 = PN.newPlace(newPrefix + "final");
	Place *compensate_p4 = PN.newPlace(newPrefix + "stop");
	Place *compensate_p5 = PN.newPlace(newPrefix + "ch_fh");
	Place *compensate_p6 = PN.newPlace(newPrefix + "ch_out");
	Place *compensate_p7 = PN.newPlace(newPrefix + "stopped");

	Transition *compensate_t1 = PN.newTransition(newPrefix + "t1");
	PN.newArc(compensate_p1, compensate_t1);
	PN.newArc(compensate_t1, compensate_p2);
	PN.newArc(compensate_t1, compensate_p5);

	Transition *compensate_t2 = PN.newTransition(newPrefix + "t2");
	PN.newArc(compensate_p2, compensate_t2);
	PN.newArc(compensate_t2, compensate_p3);
	PN.newArc(compensate_p6, compensate_t2);

	Transition *compensate_t3 = PN.newTransition(newPrefix + "t3");
	PN.newArc(compensate_p1, compensate_t3);
	PN.newArc(compensate_p4, compensate_t3);
	PN.newArc(compensate_t3, compensate_p7);

	Transition *compensate_t4 = PN.newTransition(newPrefix + "t4");
	PN.newArc(compensate_p2, compensate_t4);
	PN.newArc(compensate_p4, compensate_t4);
	PN.newArc(compensate_t4, compensate_p7);

	Transition *compensate_t5 = PN.newTransition(newPrefix + "t5");
	PN.newArc(compensate_p2, compensate_t5);
	PN.newArc(compensate_p3, compensate_t5);
	PN.newArc(compensate_t5, compensate_p7);


	// Embed the <compensate />-element.
	PN.mergePlaces(p8,  compensate_p1);
	PN.mergePlaces(p9,  compensate_p3);
	PN.mergePlaces(p11, compensate_p4);
	PN.mergePlaces(p17, compensate_p5);
	PN.mergePlaces(p18, compensate_p6);
	PN.mergePlaces(p12, compensate_p7);
      }
    }
    a // catch branches
    b // catchAll branch
    {
      foreach (catchBranch=Catch(innerActivity); tCatch_list a)
      {
	string faultToCatch = ASTEmap[catchBranch->id]->attributes["faultName"];
	Transition *t1 = PN.newTransition(prefix + "t1.internal." + faultToCatch);
	Transition *t2 = PN.newTransition(prefix + "t2." + faultToCatch);

	// Embed innerActivity_1 to innerActivity_n-1.
	PN.newArc(t1, PN.findPlace(innerActivity->id, ".initial"));
	PN.newArc(PN.findPlace(innerActivity->id, ".final"), t2);
	PN.mergePlaces(PN.findPlace("1.internal.faultHandler.p11"), PN.findPlace(innerActivity->id, ".stop"));
	PN.mergePlaces(PN.findPlace("1.internal.faultHandler.p12"), PN.findPlace(innerActivity->id, ".stopped"));

	PN.newArc(p4, t1);
	PN.newArc(p7, t1, READ);
	PN.newArc(t2, p3);
	PN.newArc(p7, t2);
      }

      with(b) {
	CatchAll(innerActivity, _id):
	{
	  Transition *t3 = PN.newTransition(prefix + "t3.catchAll");
	  Transition *t4 = PN.newTransition(prefix + "t4.catchAll");

	  // Embed innerActivity_n.
	  PN.newArc(t3, PN.findPlace(innerActivity->id, ".initial"));
	  PN.newArc(PN.findPlace(innerActivity->id, ".final"), t4);
	  PN.mergePlaces(PN.findPlace("1.internal.faultHandler.p11"), PN.findPlace(innerActivity->id, ".stop"));
	  PN.mergePlaces(PN.findPlace("1.internal.faultHandler.p12"), PN.findPlace(innerActivity->id, ".stopped"));

	  PN.newArc(p4, t3);
	  PN.newArc(p7, t3, READ);
	  PN.newArc(t4, p3);
	  PN.newArc(p7, t4);
	}
	default: { /* this should never happen */ }
      }

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</userDefinedFaultHandlerInsideProcess>\n");
    }
  ]
;






/*
 * The implicit fault handler inside a process. See Fig. 35 for details.
 */

This=implicitFaultHandler(_id) provided (ASTEmap[_id->value]->inProcess) ->
  [petrinet:
    {
      string prefix = "1.internal.faultHandler.";
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<implicitFaultHandlerInsideProcess>\n");

      Place *p1 =  PN.newPlace(prefix + "fault");
      Place *p2 =  PN.newPlace(prefix + "p2");
      Place *p3 =  PN.newPlace(prefix + "p3");
      Place *p4 =  PN.newPlace(prefix + "ch_fh");
      Place *p5 =  PN.newPlace(prefix + "ch_out");
      Place *p6 =  PN.newPlace(prefix + "p6");
      Place *p7 =  PN.newPlace(prefix + "faultSave");
      Place *p8 =  PN.newPlace(prefix + "p8");
      Place *p9 =  PN.newPlace(prefix + "final");
      Place *p10 = PN.newPlace(prefix + "rethrow");
      Place *p11 = PN.newPlace(prefix + "!Ended");
      Place *p12 = PN.newPlace(prefix + "Ended");

      Transition *t1 = PN.newTransition(prefix + "t1");
      PN.newArc(p2, t1);
      PN.newArc(p7, t1);
      PN.newArc(t1, p9);
      PN.newArc(p11, t1);
      PN.newArc(t1, p12);

      Transition *t2 = PN.newTransition(prefix + "t2");
      PN.newArc(t2, p3);
      PN.newArc(p7, t2);
      PN.newArc(t2, p8);
      PN.newArc(p10, t2);

      Transition *t3 = PN.newTransition(prefix + "t3");
      PN.newArc(p6, t3);
      PN.newArc(t3, p9);
      PN.newArc(p8, t3);
      PN.newArc(p11, t3);
      PN.newArc(t3, p12);

      Transition *t4 = PN.newTransition(prefix + "t4");
      PN.newArc(p10, t4);
      PN.newArc(p12, t4, READ);


      // The <compensate />-element.
      prefix += "compensate.";

      Place *compensate_p1 = PN.newPlace(prefix + "initial");
      Place *compensate_p2 = PN.newPlace(prefix + "running");
      Place *compensate_p3 = PN.newPlace(prefix + "final");
      Place *compensate_p4 = PN.newPlace(prefix + "stop");
      Place *compensate_p5 = PN.newPlace(prefix + "ch_fh");
      Place *compensate_p6 = PN.newPlace(prefix + "ch_out");
      Place *compensate_p7 = PN.newPlace(prefix + "stopped");

      Transition *compensate_t1 = PN.newTransition(prefix + "t1");
      PN.newArc(compensate_p1, compensate_t1);
      PN.newArc(compensate_t1, compensate_p2);
      PN.newArc(compensate_t1, compensate_p5);

      Transition *compensate_t2 = PN.newTransition(prefix + "t2");
      PN.newArc(compensate_p2, compensate_t2);
      PN.newArc(compensate_t2, compensate_p3);
      PN.newArc(compensate_p6, compensate_t2);

      Transition *compensate_t3 = PN.newTransition(prefix + "t3");
      PN.newArc(compensate_p1, compensate_t3);
      PN.newArc(compensate_p4, compensate_t3);
      PN.newArc(compensate_t3, compensate_p7);

      Transition *compensate_t4 = PN.newTransition(prefix + "t4");
      PN.newArc(compensate_p2, compensate_t4);
      PN.newArc(compensate_p4, compensate_t4);
      PN.newArc(compensate_t4, compensate_p7);

      Transition *compensate_t5 = PN.newTransition(prefix + "t5");
      PN.newArc(compensate_p2, compensate_t5);
      PN.newArc(compensate_p3, compensate_t5);
      PN.newArc(compensate_t5, compensate_p7);


      // Embed the <compensate />-element.
      PN.mergePlaces(p1, compensate_p1);
      PN.mergePlaces(p2, compensate_p3);
      PN.mergePlaces(p3, compensate_p4);
      PN.mergePlaces(p4, compensate_p5);
      PN.mergePlaces(p5, compensate_p6);
      PN.mergePlaces(p6, compensate_p7);

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</implicitFaultHandlerInsideProcess>\n");
    }
  ]
;


/*---------------------------------------------------------------------------*/


/*
 * The user-defined fault handler inside a scope. See Fig. 36 for details.
 */

This=userDefinedFaultHandler(a,b,_id) provided (!ASTEmap[_id->value]->inProcess) ->
  [petrinet:
    {
      string prefix = toString(ASTEmap[_id->value]->parentScopeId) + ".internal.faultHandler.";
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<userDefinedFaultHandlerInsideScope scope=\"" + currentScope + "\">\n");

      Place *p3 =  PN.newPlace(prefix + "out");
      Place *p4 =  PN.newPlace(prefix + "fault");
      Place *p7 =  PN.newPlace(prefix + "trueOut");
      Place *p8 =  PN.newPlace(prefix + "sourceFalse");
      Place *p9 =  PN.newPlace(prefix + "faultSave");
      Place *p10 = PN.newPlace(prefix + "p10");
      Place *p11 = PN.newPlace(prefix + "p11");
      Place *p12 = PN.newPlace(prefix + "falseOut");
      Place *p13 = PN.newPlace(prefix + "p13");
      Place *p14 = PN.newPlace(prefix + "p14");
      Place *p15 = PN.newPlace(prefix + "rethrow");
      Place *p16 = PN.newPlace(prefix + "p16");
      Place *p17 = PN.newPlace(prefix + "upperFH");
      Place *p18 = PN.newPlace(prefix + "Ended");
      Place *p19 = PN.newPlace(prefix + "ftSave");
      Place *p20 = PN.newPlace(prefix + "ft");
      Place *p21 = PN.newPlace(prefix + "p21");
      Place *p22 = PN.newPlace(prefix + "p22");
      Place *p23 = PN.newPlace(prefix + "stopped");
      Place *p24 = PN.newPlace(prefix + "!Ended");
      Place *p25 = PN.newPlace(prefix + "ch_fh");
      Place *p26 = PN.newPlace(prefix + "ch_out");

      Transition *t7 = PN.newTransition(prefix + "t7");
      PN.newArc(p9, t7);
      PN.newArc(t7, p13);
      PN.newArc(p15, t7);
      PN.newArc(t7, p16);

      Transition *t8 = PN.newTransition(prefix + "t8");
      PN.newArc(t8, p8);
      PN.newArc(t8, p12);
      PN.newArc(p14, t8);
      PN.newArc(p16, t8);
      PN.newArc(t8, p17);
      PN.newArc(t8, p18);
      PN.newArc(p24, t8);

      Transition *t9 = PN.newTransition(prefix + "t9");
      PN.newArc(p15, t9);
      PN.newArc(p18, t9, READ);

      Transition *t10 = PN.newTransition(prefix + "t10");
      PN.newArc(p15, t10);
      PN.newArc(p19, t10, READ);

      Transition *t11 = PN.newTransition(prefix + "t11");
      PN.newArc(t11, p7);
      PN.newArc(t11, p8);
      PN.newArc(p19, t11);
      PN.newArc(p22, t11);
      PN.newArc(t11, p23);

      Transition *t12 = PN.newTransition(prefix + "t12");
      PN.newArc(p20, t12);
      PN.newArc(t12, p21);


      // The rethrow part is only necessary if there is no <catchAll> element.
      if (!ASTEmap[_id->value]->hasCatchAll)
      {
  	Transition *t5 = PN.newTransition(prefix + "t5");
    	PN.newArc(p4, t5);
      	PN.newArc(p9, t5, READ);
	PN.newArc(t5, p10);

	Transition *t6 = PN.newTransition(prefix + "t6");
	PN.newArc(t6, p8);
	PN.newArc(p9, t6);
	PN.newArc(p11, t6);
	PN.newArc(t6, p12);
	PN.newArc(t6, p17);
	PN.newArc(t6, p18);
	PN.newArc(p24, t6);


	// The left <compensate />-element.
	string newPrefix = prefix + "compensate.left.";

	Place *compensate1_p1 = PN.newPlace(newPrefix + "initial");
	Place *compensate1_p2 = PN.newPlace(newPrefix + "running");
	Place *compensate1_p3 = PN.newPlace(newPrefix + "final");
	Place *compensate1_p4 = PN.newPlace(newPrefix + "stop");
	Place *compensate1_p5 = PN.newPlace(newPrefix + "ch_fh");
	Place *compensate1_p6 = PN.newPlace(newPrefix + "ch_out");
	Place *compensate1_p7 = PN.newPlace(newPrefix + "stopped");

	Transition *compensate1_t1 = PN.newTransition(newPrefix + "t1");
	PN.newArc(compensate1_p1, compensate1_t1);
	PN.newArc(compensate1_t1, compensate1_p2);
	PN.newArc(compensate1_t1, compensate1_p5);

	Transition *compensate1_t2 = PN.newTransition(newPrefix + "t2");
	PN.newArc(compensate1_p2, compensate1_t2);
	PN.newArc(compensate1_t2, compensate1_p3);
	PN.newArc(compensate1_p6, compensate1_t2);

	Transition *compensate1_t3 = PN.newTransition(newPrefix + "t3");
	PN.newArc(compensate1_p1, compensate1_t3);
	PN.newArc(compensate1_p4, compensate1_t3);
	PN.newArc(compensate1_t3, compensate1_p7);

	Transition *compensate1_t4 = PN.newTransition(newPrefix + "t4");
	PN.newArc(compensate1_p2, compensate1_t4);
	PN.newArc(compensate1_p4, compensate1_t4);
	PN.newArc(compensate1_t4, compensate1_p7);

	Transition *compensate1_t5 = PN.newTransition(newPrefix + "t5");
	PN.newArc(compensate1_p2, compensate1_t5);
	PN.newArc(compensate1_p3, compensate1_t5);
	PN.newArc(compensate1_t5, compensate1_p7);


	// Embed the left <compensate />-element.
	PN.mergePlaces(PN.findPlace(prefix + "p10"), compensate1_p1);
	PN.mergePlaces(PN.findPlace(prefix + "p11"), compensate1_p3);
	PN.mergePlaces(PN.findPlace(prefix + "p13"), compensate1_p4);
	PN.mergePlaces(PN.findPlace(prefix + "ch_fh"), compensate1_p5);
	PN.mergePlaces(PN.findPlace(prefix + "ch_out"), compensate1_p6);
	PN.mergePlaces(PN.findPlace(prefix + "p14"), compensate1_p7);
      }

      // The right <compensate />-element.
      string newPrefix = prefix + "compensate.right.";

      Place *compensate2_p1 = PN.newPlace(newPrefix + "initial");
      Place *compensate2_p2 = PN.newPlace(newPrefix + "running");
      Place *compensate2_p3 = PN.newPlace(newPrefix + "final");
      Place *compensate2_p4 = PN.newPlace(newPrefix + "stop");
      Place *compensate2_p5 = PN.newPlace(newPrefix + "ch_fh");
      Place *compensate2_p6 = PN.newPlace(newPrefix + "ch_out");
      Place *compensate2_p7 = PN.newPlace(newPrefix + "stopped");

      Transition *compensate2_t1 = PN.newTransition(newPrefix + "t1");
      PN.newArc(compensate2_p1, compensate2_t1);
      PN.newArc(compensate2_t1, compensate2_p2);
      PN.newArc(compensate2_t1, compensate2_p5);

      Transition *compensate2_t2 = PN.newTransition(newPrefix + "t2");
      PN.newArc(compensate2_p2, compensate2_t2);
      PN.newArc(compensate2_t2, compensate2_p3);
      PN.newArc(compensate2_p6, compensate2_t2);

      Transition *compensate2_t3 = PN.newTransition(newPrefix + "t3");
      PN.newArc(compensate2_p1, compensate2_t3);
      PN.newArc(compensate2_p4, compensate2_t3);
      PN.newArc(compensate2_t3, compensate2_p7);

      Transition *compensate2_t4 = PN.newTransition(newPrefix + "t4");
      PN.newArc(compensate2_p2, compensate2_t4);
      PN.newArc(compensate2_p4, compensate2_t4);
      PN.newArc(compensate2_t4, compensate2_p7);

      Transition *compensate2_t5 = PN.newTransition(newPrefix + "t5");
      PN.newArc(compensate2_p2, compensate2_t5);
      PN.newArc(compensate2_p3, compensate2_t5);
      PN.newArc(compensate2_t5, compensate2_p7);


      // Embed the right <compensate />-element.
      PN.mergePlaces(PN.findPlace(prefix + "p21"), compensate2_p1);
      PN.mergePlaces(PN.findPlace(prefix + "p22"), compensate2_p3);
      PN.mergePlaces(PN.findPlace(prefix + "ch_fh"), compensate2_p5);
      PN.mergePlaces(PN.findPlace(prefix + "ch_out"), compensate2_p6);
    }
    a // catch branches
    b // catchAll branch
    {
      foreach (catchBranch=Catch(innerActivity); tCatch_list a)
      {
	string faultToCatch = ASTEmap[catchBranch->id]->attributes["faultName"];

	Transition *t1 = PN.newTransition(prefix + "t1.internal." + faultToCatch);
	PN.newArc(t1, PN.findPlace(innerActivity->id, ".initial"));
	PN.newArc(p4, t1);
	PN.newArc(p9, t1, READ);

	Transition *t2 = PN.newTransition(prefix + "t2." + faultToCatch);
	PN.newArc(PN.findPlace(innerActivity->id, ".final"), t2);
	PN.newArc(t2, p3);
	PN.newArc(t2, p7);
	PN.newArc(t2, p8);
	PN.newArc(p9, t2);


	// Embed innerActivity_1 to innerActivity_1-n.
	PN.mergePlaces(PN.findPlace(prefix + "p13"), PN.findPlace(innerActivity->id, ".stop"));
	PN.mergePlaces(PN.findPlace(prefix + "p14"), PN.findPlace(innerActivity->id, ".stopped"));
      }

      with(b) {
	CatchAll(innerActivity, _id):
	{
	  Transition *t3 = PN.newTransition(prefix + "t3.catchAll");
	  PN.newArc(t3, PN.findPlace(innerActivity->id, ".initial"));
	  PN.newArc(p4, t3);
	  PN.newArc(p9, t3, READ);

	  Transition *t4 = PN.newTransition(prefix + "t4.catchAll");
	  PN.newArc(PN.findPlace(innerActivity->id, ".final"), t4);
	  PN.newArc(t4, p3);
	  PN.newArc(t4, p7);
	  PN.newArc(t4, p8);
	  PN.newArc(p9, t4);


	  // Embed innerActivity_n.
	  PN.mergePlaces(PN.findPlace(prefix + "p13"), PN.findPlace(innerActivity->id, ".stop"));
	  PN.mergePlaces(PN.findPlace(prefix + "p14"), PN.findPlace(innerActivity->id, ".stopped"));
	}
	default: { /* this should never happen */ }
      }

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</userDefinedFaultHandlerInsideScope scope=\"" + currentScope + "\">\n");
    }
  ]
;





/*
 * The implicit fault handler inside a scope. See Fig. 34 for details.
 */

This=implicitFaultHandler(_id) provided (!ASTEmap[_id->value]->inProcess) ->
  [petrinet:
    {
      string prefix = toString(ASTEmap[_id->value]->parentScopeId) + ".internal.faultHandler.";
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<implicitFaultHandlerInsideScope scope=\"" + currentScope + "\">\n");

      Place *p1 =  PN.newPlace(prefix + "ft");
      Place *p1a = PN.newPlace(prefix + "fault");
      Place *p2 =  PN.newPlace(prefix + "p2");
      Place *p3 =  PN.newPlace(prefix + "p3");
      Place *p4 =  PN.newPlace(prefix + "ch_fh");
      Place *p5 =  PN.newPlace(prefix + "ch_out");
      Place *p6 =  PN.newPlace(prefix + "p6");
      Place *p7 =  PN.newPlace(prefix + "faultSave");
      Place *p8 =  PN.newPlace(prefix + "ftSave");
      Place *p9 =  PN.newPlace(prefix + "upperFH");
      Place *p10 = PN.newPlace(prefix + "trueOut");
      Place *p11 = PN.newPlace(prefix + "sourceFalse");
      Place *p12 = PN.newPlace(prefix + "p12");
      Place *p13 = PN.newPlace(prefix + "rethrow");
      Place *p14 = PN.newPlace(prefix + "stopped");
      Place *p15 = PN.newPlace(prefix + "falseOut");
      Place *p16 = PN.newPlace(prefix + "!Ended");
      Place *p17 = PN.newPlace(prefix + "Ended");

      Transition *t1 = PN.newTransition(prefix + "t1");
      PN.newArc(p2, t1);
      PN.newArc(p7, t1);
      PN.newArc(t1, p9);
      PN.newArc(t1, p10);
      PN.newArc(t1, p11);
      PN.newArc(p16, t1);
      PN.newArc(t1, p17);

      Transition *t2 = PN.newTransition(prefix + "t2");
      PN.newArc(p2, t2);
      PN.newArc(p8, t2);
      PN.newArc(t2, p10);
      PN.newArc(t2, p11);
      PN.newArc(t2, p14);

      Transition *t3 = PN.newTransition(prefix + "t3");
      PN.newArc(t3, p3);
      PN.newArc(p7, t3);
      PN.newArc(t3, p12);
      PN.newArc(p13, t3);

      Transition *t4 = PN.newTransition(prefix + "t4");
      PN.newArc(p6, t4);
      PN.newArc(t4, p9);
      PN.newArc(t4, p11);
      PN.newArc(p12, t4);
      PN.newArc(t4, p15);
      PN.newArc(p16, t4);
      PN.newArc(t4, p17);

      Transition *t5 = PN.newTransition(prefix + "t5");
      PN.newArc(p8, t5, READ);
      PN.newArc(p13, t5);

      Transition *t6 = PN.newTransition(prefix + "t6");
      PN.newArc(p13, t6);
      PN.newArc(p17, t6, READ);


      // The <compensate />-element
      string compensate_prefix = prefix + "compensate.";

      Place *compensate_p1 = PN.newPlace(compensate_prefix + "initial");
      Place *compensate_p2 = PN.newPlace(compensate_prefix + "running");
      Place *compensate_p3 = PN.newPlace(compensate_prefix + "final");
      Place *compensate_p4 = PN.newPlace(compensate_prefix + "stop");
      Place *compensate_p5 = PN.newPlace(compensate_prefix + "ch_fh");
      Place *compensate_p6 = PN.newPlace(compensate_prefix + "ch_out");
      Place *compensate_p7 = PN.newPlace(compensate_prefix + "stopped");

      Transition *compensate_t1 = PN.newTransition(compensate_prefix + "t1");
      PN.newArc(compensate_p1, compensate_t1);
      PN.newArc(compensate_t1, compensate_p2);
      PN.newArc(compensate_t1, compensate_p5);

      Transition *compensate_t2 = PN.newTransition(compensate_prefix + "t2");
      PN.newArc(compensate_p2, compensate_t2);
      PN.newArc(compensate_t2, compensate_p3);
      PN.newArc(compensate_p6, compensate_t2);

      Transition *compensate_t3 = PN.newTransition(compensate_prefix + "t3");
      PN.newArc(compensate_p1, compensate_t3);
      PN.newArc(compensate_p4, compensate_t3);
      PN.newArc(compensate_t3, compensate_p7);

      Transition *compensate_t4 = PN.newTransition(compensate_prefix + "t4");
      PN.newArc(compensate_p2, compensate_t4);
      PN.newArc(compensate_p4, compensate_t4);
      PN.newArc(compensate_t4, compensate_p7);

      Transition *compensate_t5 = PN.newTransition(compensate_prefix + "t5");
      PN.newArc(compensate_p2, compensate_t5);
      PN.newArc(compensate_p3, compensate_t5);
      PN.newArc(compensate_t5, compensate_p7);


      // Embed the <compensate />-element
      PN.mergePlaces(p1, compensate_p1);
      PN.mergePlaces(p2, compensate_p3);
      PN.mergePlaces(p3, compensate_p4);
      PN.mergePlaces(p4, compensate_p5);
      PN.mergePlaces(p5, compensate_p6);
      PN.mergePlaces(p6, compensate_p7);

      PN.mergePlaces(prefix + "ft", prefix + "fault");

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</implicitFaultHandlerInsideScope scope=\"" + currentScope + "\">\n");
    }
  ]
;





CatchAll(innerActivity, _id) ->
  [petrinet: innerActivity]
;





/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

/*
 * Instead of creating patterns for a alarm event handler and a message event
 * handler we united both patterns to a single event handler. In order to do
 * so, we used the place and transition numberings as they are introduced in
 * Fig. 29. This situation leads to different numberings when embedding the
 * onAlarm-activities into the event handler. In this case, both names (the
 * name in Fig. 29 and in Fig. 30) are mentioned in a comment.
 */

This=userDefinedEventHandler(a,b,_id) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<eventHandler scope=\"" + currentScope + "\">\n");

      string prefix = toString(ASTEmap[_id->value]->parentScopeId) + ".internal.eventHandler.";
      Place *p6  = PN.newPlace(prefix + "initial");
      Place *p8  = PN.newPlace(prefix + "final");
      Place *p13 = PN.newPlace(prefix + "finish");
      Place *p17 = PN.newPlace(prefix + "running");
      Place *p18 = PN.newPlace(prefix + "finishing");
      Place *p22 = PN.newPlace(prefix + "stop");
      Place *p23 = PN.newPlace(prefix + "stopped");

      Transition *t5 = PN.newTransition(prefix + "t5"); // t6 in MEH
      PN.newArc(p6, t5);
      PN.newArc(t5, p17);
      PN.newArc(PN.findPlace("1.internal.clock"), t5, READ);

      Transition *t7 = PN.newTransition(prefix + "allFinished");
      PN.newArc(t7, p8);
      PN.newArc(p18, t7);

      Transition *t11 = PN.newTransition(prefix + "t11"); // t13 in MEH
      PN.newArc(p13, t11); // finish
      PN.newArc(p17, t11); // running
      PN.newArc(t11, p18); // finishing

      Transition *t16 = PN.newTransition(prefix + "allStopped"); // t24 in MEH (allStopped)
      PN.newArc(t16, p23);

      Transition *t17 = PN.newTransition(prefix + "stop+finish"); // t18 in MEH (stop+finish)
      PN.newArc(p18, t17);
      PN.newArc(p22, t17);

      Transition *t18 = PN.newTransition(prefix + "t18");
      PN.newArc(p6, t18);
      PN.newArc(p22, t18);
      PN.newArc(t18, p23);

      Transition *t19 = PN.newTransition(prefix + "normalStop"); // t27 in MEH (normalStop)
      PN.newArc(p17, t19);
      PN.newArc(p22, t19);

      Transition *t20 = PN.newTransition(prefix + "t20");
      PN.newArc(p8, t20);
      PN.newArc(p22, t20);
      PN.newArc(t20, p23);
    }
    a // onMessage branches
    b // onAlarm branches
    {
      int onMessageCount = 1;
      foreach (ThisBranch = OnMessage(_corr, _from, innerActivity, _id2); tOnMessage_list a)
      {
	string message = toString(onMessageCount++);
	string newPrefix = prefix + "onMessage" + message + ".";

	Place *onMessage_p1  = PN.newPlace(newPrefix + "p1");
	Place *onMessage_p2  = PN.newPlace(newPrefix + "p2");
	Place *onMessage_p3  = PN.newPlace(newPrefix + "p3");
	Place *onMessage_p4  = PN.newPlace(newPrefix + "p4");
	Place *onMessage_p5  = PN.newPlace(newPrefix + "wait" + message );
	Place *onMessage_p6  = PN.newPlace(newPrefix + "p6");
	Place *onMessage_p7  = PN.newPlace(newPrefix + "finish" + message );
	Place *onMessage_p9  = PN.newPlace(newPrefix + "finish+stop" + message );
	Place *onMessage_p11 = PN.newPlace(newPrefix + "stop" + message );
	Place *onMessage_p12 = PN.newPlace(newPrefix + "p12");

	Transition *onMessage_t2 = PN.newTransition(newPrefix + "t2");
	PN.newArc(PN.findPlace("in." + ASTEmap[ThisBranch->id]->channelName), onMessage_t2);
	PN.newArc(onMessage_t2, onMessage_p2);
	PN.newArc(onMessage_p5, onMessage_t2);

	Transition *onMessage_t3 = PN.newTransition(newPrefix + "t3");
	PN.newArc(PN.findPlace("variable." + ASTEmap[ThisBranch->id]->variableName), onMessage_t3);
  	PN.newArc(onMessage_t3, PN.findPlace("variable." + ASTEmap[ThisBranch->id]->variableName));
	PN.newArc(onMessage_p2, onMessage_t3);
	PN.newArc(onMessage_t3, onMessage_p3);

	Transition *onMessage_t12 = PN.newTransition(newPrefix + "t12");
	PN.newArc(onMessage_p11, onMessage_t12);
	PN.newArc(onMessage_t12, onMessage_p12);

	Transition *onMessage_t4 = PN.newTransition(newPrefix + "t4");
	PN.newArc(onMessage_p4, onMessage_t4);
	PN.newArc(onMessage_t4, onMessage_p5); // loop
        PN.newArc(onMessage_p5, onMessage_t12);

	Transition *onMessage_t5 = PN.newTransition(newPrefix + "t5");
	PN.newArc(onMessage_p5, onMessage_t5);
	PN.newArc(onMessage_t5, onMessage_p6);
	PN.newArc(onMessage_p7, onMessage_t5);

	Transition *onMessage_t7 = PN.newTransition(newPrefix + "t7");
	PN.newArc(onMessage_p7, onMessage_t7);
	PN.newArc(onMessage_p9, onMessage_t7);
	PN.newArc(onMessage_t7, onMessage_p11);

	Transition *onMessage_t8 = PN.newTransition(newPrefix + "t8");
	PN.newArc(onMessage_p6, onMessage_t8);
	PN.newArc(onMessage_p9, onMessage_t8);
	PN.newArc(onMessage_t8, onMessage_p12);

	Transition *onMessage_t10 = PN.newTransition(newPrefix + "t10");
	PN.newArc(onMessage_p1, onMessage_t10);
	PN.newArc(onMessage_p11, onMessage_t10);
	PN.newArc(onMessage_t10, onMessage_p12);

	Transition *onMessage_t11 = PN.newTransition(newPrefix + "t11");
	PN.newArc(onMessage_p2, onMessage_t11);
	PN.newArc(onMessage_p11, onMessage_t11);
	PN.newArc(onMessage_t11, onMessage_p12);

	if (parameters[P_NOSTANDARDFAULTS] == false)
	  Transition *t1 = throwFault(onMessage_p2, onMessage_p1, toString(onMessageCount-1) + ".p2", prefix, _id2, 0); // the toString-part fixed a bug

	// arcs from the EH
	PN.newArc(t11, onMessage_p7); // t11 in AEH is t13 in MEH
	PN.newArc(t5,  onMessage_p5); // t5  in AEH is t6  in MEH
	PN.newArc(t17, onMessage_p9); // t17 in AEH is t18 in MEH (stop+finish)
	PN.newArc(t19, onMessage_p11); // t19 in AEH is t27 in MEH (normalStop)

	// arcs to the EH
	PN.newArc(onMessage_p6, t7); // "allFinished" in AEH is "finishAll" in MEH
	PN.newArc(onMessage_p12, t16);

	// embed the innerActivity
	PN.mergePlaces(onMessage_p3,  PN.findPlace(innerActivity->id, ".initial"));
	PN.mergePlaces(onMessage_p11, PN.findPlace(innerActivity->id, ".stop"));
	PN.mergePlaces(onMessage_p12, PN.findPlace(innerActivity->id, ".stopped"));
	PN.mergePlaces(onMessage_p4,  PN.findPlace(innerActivity->id, ".final"));
      }


      int onAlarmCount = 1;
      foreach (onAlarmBranch=OnAlarm(innerActivity); tOnAlarm_list b)
      {
	string newPrefix = prefix + "onAlarm" + toString(onAlarmCount) + ".";

	Place *onAlarm_p1  = PN.newPlace(newPrefix + "p1");
	Place *onAlarm_p2  = PN.newPlace(newPrefix + "p2");
	Place *onAlarm_p3  = PN.newPlace(newPrefix + "wait" + toString(onAlarmCount));
	Place *onAlarm_p4  = PN.newPlace(newPrefix + "p4");
	Place *onAlarm_p5  = PN.newPlace(newPrefix + "finish" + toString(onAlarmCount));
	Place *onAlarm_p7  = PN.newPlace(newPrefix + "stop+finish" + toString(onAlarmCount));
	Place *onAlarm_p9  = PN.newPlace(newPrefix + "stop" + toString(onAlarmCount));
	Place *onAlarm_p10 = PN.newPlace(newPrefix + "p10");

	Transition *onAlarm_t1 = PN.newTransition(newPrefix + "t1");
	PN.newArc(onAlarm_p3, onAlarm_t1);
	PN.newArc(onAlarm_t1, onAlarm_p1);
	PN.newArc(PN.findPlace("1.internal.clock"), onAlarm_t1, READ);

	Transition *onAlarm_t2 = PN.newTransition(newPrefix + "t2");
	PN.newArc(onAlarm_p2, onAlarm_t2);
	PN.newArc(onAlarm_t2, onAlarm_p4);
	PN.newArc(onAlarm_p5, onAlarm_t2);

	Transition *onAlarm_t3 = PN.newTransition(newPrefix + "t3");
	PN.newArc(onAlarm_p3, onAlarm_t3);
	PN.newArc(onAlarm_t3, onAlarm_p4);
	PN.newArc(onAlarm_p5, onAlarm_t3);

	Transition *onAlarm_t4 = PN.newTransition(newPrefix + "t4");
	PN.newArc(onAlarm_p5, onAlarm_t4);
	PN.newArc(onAlarm_p7, onAlarm_t4);
	PN.newArc(onAlarm_t4, onAlarm_p9);

	Transition *onAlarm_t6 = PN.newTransition(newPrefix + "t6");
	PN.newArc(onAlarm_p4, onAlarm_t6);
	PN.newArc(onAlarm_p7, onAlarm_t6);
	PN.newArc(onAlarm_t6, onAlarm_p10);

	Transition *onAlarm_t8 = PN.newTransition(newPrefix + "t8");
	PN.newArc(onAlarm_p3, onAlarm_t8);
	PN.newArc(onAlarm_p9, onAlarm_t8);
	PN.newArc(onAlarm_t8, onAlarm_p10);


	// Arcs from the EH.
	PN.newArc(t5,  onAlarm_p3);
	PN.newArc(t11, onAlarm_p5);
	PN.newArc(t17, onAlarm_p7);
	PN.newArc(t19, onAlarm_p9);


	// Arcs to the EH.
	PN.newArc(onAlarm_p4,  t7);
	PN.newArc(onAlarm_p10, t16);


	// Embed the innerActivity.
	PN.mergePlaces(onAlarm_p1,  PN.findPlace(innerActivity->id, ".initial"));
	PN.mergePlaces(onAlarm_p9,  PN.findPlace(innerActivity->id, ".stop"));
	PN.mergePlaces(onAlarm_p10, PN.findPlace(innerActivity->id, ".stopped"));
	PN.mergePlaces(onAlarm_p2,  PN.findPlace(innerActivity->id, ".final"));

	onAlarmCount++;
      }

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</eventHandler scope=\"" + currentScope + "\">\n");
    }
  ]
;






/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/

/*
 * Implicit compensation handler as it is depicted in Fig. 42.
 */

This=implicitCompensationHandler(_id) provided (!ASTEmap[_id->value]->inProcess) ->
  [petrinet:
    {
      string prefix = toString(ASTEmap[_id->value]->parentScopeId) + ".internal.compensationHandler.";
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<implicitCompensationHandler scope=\"" + currentScope + "\">\n");

      Place *p2  = PN.newPlace(prefix + "ch_in");
      Place *p3  = PN.newPlace(prefix + "Compensated");
      Place *p4  = PN.newPlace(prefix + "!Completed");
      Place *p5  = PN.newPlace(prefix + "!Compensated");
      Place *p6  = PN.newPlace(prefix + "Completed");
      Place *p9  = PN.newPlace(prefix + "compensated");
      Place *p11 = PN.newPlace(prefix + "comp");
      Place *p13 = PN.newPlace(prefix + "cleaned");
      Place *p14 = PN.newPlace(prefix + "fh");
      Place *p15 = PN.newPlace(prefix + "ch_call");
      Place *p16 = PN.newPlace(prefix + "fh_call");
      Place *p17 = PN.newPlace(prefix + "ch_out");
      Place *p18 = PN.newPlace(prefix + "scopeCompensated");
      Place *p24 = PN.newPlace(prefix + "clean");

      // The following subnet is only needed if the standard faults are
      // switched on (i.e. not using the parameter "nostandardfaults").
      if (parameters[P_NOSTANDARDFAULTS] == false)
      {
	Place *p1  = PN.newPlace(prefix + "p1");

	Transition *t1 = throwFault(p2, p1, "ch_in", prefix, _id, 2);
	PN.newArc(p3, t1, READ);

	Transition *t3 = PN.newTransition(prefix + "t3");
	PN.newArc(p1, t3);
	PN.newArc(t3, p13);
	PN.newArc(p24, t3);
      }


      Transition *t2 = PN.newTransition(prefix + "t2");
      PN.newArc(p2, t2);
      PN.newArc(p4, t2, READ);
      PN.newArc(p5, t2, READ);
      PN.newArc(t2, p9);

      Transition *t4 = PN.newTransition(prefix + "t4");
      PN.newArc(p2, t4);
      PN.newArc(t4, p3);
      PN.newArc(p5, t4);
      PN.newArc(p6, t4, READ);
      PN.newArc(t4, p11);
      PN.newArc(t4, p15);

      Transition *t6 = PN.newTransition(prefix + "t6");
      PN.newArc(p14, t6);
      PN.newArc(t6, p11);
      PN.newArc(t6, p16);

      Transition *t7 = PN.newTransition(prefix + "t7");
      PN.newArc(t7, p9);
      PN.newArc(p11, t7);
      PN.newArc(p15, t7);

      Transition *t8 = PN.newTransition(prefix + "t8");
      PN.newArc(p11, t8);
      PN.newArc(p16, t8);
      PN.newArc(t8, p17);

      Transition *t9 = PN.newTransition(prefix + "t9");
      PN.newArc(t9, p11);
      PN.newArc(p16, t9, READ);
      PN.newArc(p18, t9);

      Transition *t10 = PN.newTransition(prefix + "t10");
      PN.newArc(t10, p11);
      PN.newArc(p15, t10, READ);
      PN.newArc(p18, t10);

      Transition *t11 = PN.newTransition(prefix + "t11");
      PN.newArc(t11, p13);
      PN.newArc(p15, t11);
      PN.newArc(p24, t11);

      Transition *t12 = PN.newTransition(prefix + "t12");
      PN.newArc(t12, p13);
      PN.newArc(p16, t12);
      PN.newArc(p24, t12);

      Transition *t15 = PN.newTransition(prefix + "t15");
      PN.newArc(p9, t15);
      PN.newArc(t15, p13);
      PN.newArc(p24, t15);

      Transition *t16 = PN.newTransition(prefix + "t16");
      PN.newArc(t16, p13);
      PN.newArc(p17, t16);
      PN.newArc(p24, t16);


      // Generate transitions t13 for the child scopes. This transition was
      // guarded in the original semantics and has to be unfolded for the
      // low-level representation. Therefore, each child gets its own place
      // "compScope".
      for (set<unsigned int>::iterator child = ASTEmap[This->id]->enclosedScopes.begin(); child != ASTEmap[This->id]->enclosedScopes.end(); child++)
      {
	Place *p19_new = PN.newPlace(prefix + "compScope." + toString(*child));
	Place *p20 = PN.findPlace(toString(*child) + ".!push");
	Place *p21 = PN.findPlace(toString(*child) + ".push");
	Transition *t13 = PN.newTransition(prefix + "t13." + toString(*child));
	PN.newArc(p20, t7);
	PN.newArc(p20, t8);
	PN.newArc(p11, t13);
	PN.newArc(t13, p19_new);
	PN.newArc(t13, p20);
	PN.newArc(p21, t13);
      }

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</implicitCompensationHandler scope=\"" + currentScope + "\">\n");
    }
  ]
;





/*
 * User-defined compensation handler as depicted in Fig. 42-45. Please note
 * that this pattern combines the four different compensation handler patterns.
 */

This=userDefinedCompensationHandler(innerActivity,_id) provided (!ASTEmap[_id->value]->inProcess) ->
  [petrinet:
    {
      string prefix = toString(ASTEmap[_id->value]->parentScopeId) + ".internal.compensationHandler.";
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<userDefinedCompensationHandler scope=\"" + currentScope + "\">\n");

      Place *p2  = PN.newPlace(prefix + "ch_in");
      Place *p3  = PN.newPlace(prefix + "Compensated");
      Place *p4  = PN.newPlace(prefix + "!Completed");
      Place *p5  = PN.newPlace(prefix + "!Compensated");
      Place *p6  = PN.newPlace(prefix + "Completed");
      Place *p7  = PN.newPlace(prefix + "p7");
      Place *p8  = PN.newPlace(prefix + "p8");
      Place *p9  = PN.newPlace(prefix + "compensated");
      Place *p11 = PN.newPlace(prefix + "comp");
      Place *p12 = PN.newPlace(prefix + "scopeCompensated");
      Place *p13 = PN.newPlace(prefix + "cleaned");
      Place *p14 = PN.newPlace(prefix + "fh");
      Place *p16 = PN.newPlace(prefix + "fh_call");
      Place *p17 = PN.newPlace(prefix + "ch_out");
      Place *p24 = PN.newPlace(prefix + "clean");


      // The following subnet is only needed if the standard faults are
      // switched on (i.e. not using the parameter "nostandardfaults").
      if (parameters[P_NOSTANDARDFAULTS] == false)
      {
	Place *p1  = PN.newPlace(prefix + "p1");

	Transition *t1 = throwFault(p2, p1, "ch_in", prefix, _id, 2);
	PN.newArc(p3, t1, READ);

	Transition *t3 = PN.newTransition(prefix + "t3");
	PN.newArc(p1, t3);
	PN.newArc(t3, p13);
	PN.newArc(p24, t3);
      }


      Transition *t2 = PN.newTransition(prefix + "t2");
      PN.newArc(p2, t2);
      PN.newArc(p4, t2, READ);
      PN.newArc(p5, t2, READ);
      PN.newArc(t2, p9);

      Transition *t4 = PN.newTransition(prefix + "t4");
      PN.newArc(p2, t4);
      PN.newArc(t4, p3);
      PN.newArc(p5, t4);
      PN.newArc(p6, t4, READ);
      PN.newArc(t4, p7);

      Transition *t5 = PN.newTransition(prefix + "t5");
      PN.newArc(p8, t5);
      PN.newArc(t5, p9);

      Transition *t6 = PN.newTransition(prefix + "t6");
      PN.newArc(t6, p11);
      PN.newArc(p14, t6);
      PN.newArc(t6, p16);

      Transition *t8 = PN.newTransition(prefix + "t8");
      PN.newArc(p11, t8);
      PN.newArc(p16, t8);
      PN.newArc(t8, p17);

      Transition *t9 = PN.newTransition(prefix + "t9");
      PN.newArc(t9, p11);
      PN.newArc(p12, t9);
      PN.newArc(p16, t9, READ);

      Transition *t12 = PN.newTransition(prefix + "t12");
      PN.newArc(t12, p13);
      PN.newArc(p16, t12);
      PN.newArc(p24, t12);

      Transition *t15 = PN.newTransition(prefix + "t15");
      PN.newArc(p9, t15);
      PN.newArc(t15, p13);
      PN.newArc(p24, t15);

      Transition *t16 = PN.newTransition(prefix + "t16");
      PN.newArc(t16, p13);
      PN.newArc(p17, t16);
      PN.newArc(p24, t16);


      // Above is the standard case as depicted in Fig. 45. Below are the other
      // cases as depicted in Fig. 43 and Fig. 44.
      if (This->hasCompensateWithoutScope)
      {
	// The Pattern of Fig. 43 (with <compensate />).
	Place *p10 = PN.newPlace(prefix + "p10");
	Place *p12 = PN.newPlace(prefix + "p12");
	Place *p15 = PN.newPlace(prefix + "ch_call");

	Transition *t7 = PN.newTransition(prefix + "t7");
	PN.newArc(p11, t7);
	PN.newArc(t7, p12);
	PN.newArc(p15, t7, READ);

        for (set<unsigned int>::iterator child = ASTEmap[This->id]->enclosedScopes.begin(); child != ASTEmap[This->id]->enclosedScopes.end(); child++)
	  PN.newArc(PN.findPlace(toString(*child) + ".!push"), t7);

	Transition *t10 = PN.newTransition(prefix + "t10");
	PN.newArc(t10, p11);
	PN.newArc(p12, t10);
	PN.newArc(p15, t10, READ);

	Transition *t11 = PN.newTransition(prefix + "t11");
	PN.newArc(t11, p10);
	PN.newArc(p15, t11);
	PN.newArc(p24, t11);
      }
      if (This->hasCompensateWithScope)
      {
	// Pattern of Fig. 44 (with <compensate scope="C"/>).
	Place *p10 = PN.newPlace(prefix + "p10");
	Place *p15 = PN.newPlace(prefix + "ch_call");

	Transition *t11 = PN.newTransition(prefix + "t11");
	PN.newArc(t4, p15);
	PN.newArc(p15, t5);
	PN.newArc(t11, p10);
	PN.newArc(p15, t11);
	PN.newArc(p24, t11);
      }


      // Generate transitions t13 for the child scopes. This transition was
      // guarded in the original semantics and has to be unfolded for the
      // low-level representation. Therefore, each child gets its own place
      // "compScope".
      for (set<unsigned int>::iterator child = ASTEmap[This->id]->enclosedScopes.begin(); child != ASTEmap[This->id]->enclosedScopes.end(); child++)
      {
	Place *p19_new = PN.newPlace(prefix + "compScope." + toString(*child));
	Place *p20 = PN.findPlace(toString(*child) + ".!push");
	Place *p21 = PN.findPlace(toString(*child) + ".push");
	Transition *t13 = PN.newTransition(prefix + "t13." + toString(*child));
	PN.newArc(p20, t8);
	PN.newArc(p11, t13);
	PN.newArc(t13, p19_new);
	PN.newArc(t13, p20);
	PN.newArc(p21, t13);
      }
    }
    innerActivity
    {
      // Embed inner activity.
      PN.newArc(t4, PN.findPlace(innerActivity->id, ".initial"));
      PN.newArc(PN.findPlace(innerActivity->id, ".final"), t5);
      PN.mergePlaces(p24, PN.findPlace(innerActivity->id, ".stop"));
      PN.mergePlaces(p13, PN.findPlace(innerActivity->id, ".stopped"));

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</userDefinedCompensationHandler scope=\"" + currentScope + "\">\n");
    }
  ]
;




/*
 * Compensation handler inside of a process. Since this compensation handler
 * will never be called from a surrounding scope, the whole "left" part of the
 * pattern depicted in Fig. 42 is not necessary.
 */
This=implicitCompensationHandler(_id) provided (ASTEmap[_id->value]->inProcess) ->
  [petrinet:
    {
      string prefix = toString(ASTEmap[_id->value]->parentScopeId) + ".internal.compensationHandler.";
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<processCompensationHandler>\n");

      Place *p2  = PN.newPlace(prefix + "ch_in");
      Place *p3  = PN.newPlace(prefix + "Compensated");
      Place *p4  = PN.newPlace(prefix + "!Completed");
      Place *p5  = PN.newPlace(prefix + "!Compensated");
      Place *p6  = PN.newPlace(prefix + "Completed");
      Place *p9  = PN.newPlace(prefix + "compensated");
      Place *p11 = PN.newPlace(prefix + "comp");
      Place *p13 = PN.newPlace(prefix + "cleaned");
      Place *p14 = PN.newPlace(prefix + "fh");
      Place *p15 = PN.newPlace(prefix + "ch_call");
      Place *p16 = PN.newPlace(prefix + "fh_call");
      Place *p17 = PN.newPlace(prefix + "ch_out");
      Place *p18 = PN.newPlace(prefix + "scopeCompensated");
      Place *p24 = PN.newPlace(prefix + "clean");

      Transition *t6 = PN.newTransition(prefix + "t6");
      PN.newArc(p14, t6);
      PN.newArc(t6, p11);
      PN.newArc(t6, p16);

      Transition *t7 = PN.newTransition(prefix + "t7");
      PN.newArc(t7, p9);
      PN.newArc(p11, t7);
      PN.newArc(p15, t7);

      Transition *t8 = PN.newTransition(prefix + "t8");
      PN.newArc(p11, t8);
      PN.newArc(p16, t8);
      PN.newArc(t8, p17);

      Transition *t9 = PN.newTransition(prefix + "t9");
      PN.newArc(t9, p11);
      PN.newArc(p16, t9, READ);
      PN.newArc(p18, t9);

      Transition *t10 = PN.newTransition(prefix + "t10");
      PN.newArc(t10, p11);
      PN.newArc(p15, t10, READ);
      PN.newArc(p18, t10);

      Transition *t11 = PN.newTransition(prefix + "t11");
      PN.newArc(t11, p13);
      PN.newArc(p15, t11);
      PN.newArc(p24, t11);

      Transition *t12 = PN.newTransition(prefix + "t12");
      PN.newArc(t12, p13);
      PN.newArc(p16, t12);
      PN.newArc(p24, t12);

      Transition *t15 = PN.newTransition(prefix + "t15");
      PN.newArc(p9, t15);
      PN.newArc(t15, p13);
      PN.newArc(p24, t15);

      Transition *t16 = PN.newTransition(prefix + "t16");
      PN.newArc(t16, p13);
      PN.newArc(p17, t16);
      PN.newArc(p24, t16);


      // Generate transitions t13 for the child scopes. This transition was
      // guarded in the original semantics and has to be unfolded for the
      // low-level representation. Therefore, each child gets its own place
      // "compScope".
      for (set<unsigned int>::iterator child = ASTEmap[This->id]->enclosedScopes.begin(); child != ASTEmap[This->id]->enclosedScopes.end(); child++)
      {
	Place *p19_new = PN.newPlace(prefix + "compScope." + toString(*child));
	Place *p20 = PN.findPlace(toString(*child) + ".!push");
	Place *p21 = PN.findPlace(toString(*child) + ".push");
	Transition *t13 = PN.newTransition(prefix + "t13." + toString(*child));
	PN.newArc(p20, t7);
	PN.newArc(p20, t8);
	PN.newArc(p11, t13);
	PN.newArc(t13, p19_new);
	PN.newArc(t13, p20);
	PN.newArc(p21, t13);
      }

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</processCompensationHandler>\n");
    }
  ]
;



/* this pattern is just a copy from the pattern above */
This=userDefinedCompensationHandler(a,_id) provided (ASTEmap[_id->value]->inProcess) ->
  [petrinet:
    {
      string prefix = toString(ASTEmap[_id->value]->parentScopeId) + ".internal.compensationHandler.";
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<processCompensationHandler>\n");

      Place *p2  = PN.newPlace(prefix + "ch_in");
      Place *p3  = PN.newPlace(prefix + "Compensated");
      Place *p4  = PN.newPlace(prefix + "!Completed");
      Place *p5  = PN.newPlace(prefix + "!Compensated");
      Place *p6  = PN.newPlace(prefix + "Completed");
      Place *p9  = PN.newPlace(prefix + "compensated");
      Place *p11 = PN.newPlace(prefix + "comp");
      Place *p13 = PN.newPlace(prefix + "cleaned");
      Place *p14 = PN.newPlace(prefix + "fh");
      Place *p15 = PN.newPlace(prefix + "ch_call");
      Place *p16 = PN.newPlace(prefix + "fh_call");
      Place *p17 = PN.newPlace(prefix + "ch_out");
      Place *p18 = PN.newPlace(prefix + "scopeCompensated");
      Place *p24 = PN.newPlace(prefix + "clean");

      Transition *t6 = PN.newTransition(prefix + "t6");
      PN.newArc(p14, t6);
      PN.newArc(t6, p11);
      PN.newArc(t6, p16);

      Transition *t7 = PN.newTransition(prefix + "t7");
      PN.newArc(t7, p9);
      PN.newArc(p11, t7);
      PN.newArc(p15, t7);

      Transition *t8 = PN.newTransition(prefix + "t8");
      PN.newArc(p11, t8);
      PN.newArc(p16, t8);
      PN.newArc(t8, p17);

      Transition *t9 = PN.newTransition(prefix + "t9");
      PN.newArc(t9, p11);
      PN.newArc(p16, t9, READ);
      PN.newArc(p18, t9);

      Transition *t10 = PN.newTransition(prefix + "t10");
      PN.newArc(t10, p11);
      PN.newArc(p15, t10, READ);
      PN.newArc(p18, t10);

      Transition *t11 = PN.newTransition(prefix + "t11");
      PN.newArc(t11, p13);
      PN.newArc(p15, t11);
      PN.newArc(p24, t11);

      Transition *t12 = PN.newTransition(prefix + "t12");
      PN.newArc(t12, p13);
      PN.newArc(p16, t12);
      PN.newArc(p24, t12);

      Transition *t15 = PN.newTransition(prefix + "t15");
      PN.newArc(p9, t15);
      PN.newArc(t15, p13);
      PN.newArc(p24, t15);

      Transition *t16 = PN.newTransition(prefix + "t16");
      PN.newArc(t16, p13);
      PN.newArc(p17, t16);
      PN.newArc(p24, t16);


      // Generate transitions t13 for the child scopes. This transition was
      // guarded in the original semantics and has to be unfolded for the
      // low-level representation. Therefore, each child gets its own place
      // "compScope".
      for (set<unsigned int>::iterator child = ASTEmap[This->id]->enclosedScopes.begin(); child != ASTEmap[This->id]->enclosedScopes.end(); child++)
      {
	Place *p19_new = PN.newPlace(prefix + "compScope." + toString(*child));
	Place *p20 = PN.findPlace(toString(*child) + ".!push");
	Place *p21 = PN.findPlace(toString(*child) + ".push");
	Transition *t13 = PN.newTransition(prefix + "t13." + toString(*child));
	PN.newArc(p20, t7);
	PN.newArc(p20, t8);
	PN.newArc(p11, t13);
	PN.newArc(t13, p19_new);
	PN.newArc(t13, p20);
	PN.newArc(p21, t13);
      }

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</processCompensationHandler>\n");
    }
  ]
;
