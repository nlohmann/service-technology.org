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
 * \file    frontend-parser-wsdl.cc
 *
 * \brief   WSDL 1.1 parser
 *
 *          WSDL is parsed as defined in
 *          http://schemas.xmlsoap.org/wsdl/2003-02-11.xsd
 *
 *          Partner Link Types are parsed according to the "Schema for OASIS
 *          Business Process Execution Language (WS-BPEL) 2.0 - Schema for
 *          Partner Link Type" (http://docs.oasis-open.org/wsbpel/2.0/plnktype)
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2007/04/29
 *
 * \date    \$Date: 2007/06/28 07:38:16 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using GNU Bison reading file
 *          frontend-parser-chor.yy.
 *          See http://www.gnu.org/software/bison/bison.html for details
 *
 * \version \$Revision: 1.10 $
 *
 * \ingroup frontend
 */
%}
// options for the bison flex interplay 
//
// Bison generates a list of all used tokens in file "frontend-parser.h" (for
// Flex).
%token_table
%defines
%yacc
/******************************************************************************
 * Terminal symbols (tokens).
 *****************************************************************************/
%token APOSTROPHE EQUAL GREATER GREATEROREQUAL K_AND K_ASSIGN K_BRANCHES K_CASE
%token K_CATCH K_CATCHALL K_COMPENSATE K_COMPENSATESCOPE K_COMPENSATIONHANDLER
%token K_COMPLETIONCONDITION K_COPY K_CORRELATIONS
%token K_CORRELATIONSET K_CORRELATIONSETS K_ELSE K_ELSEIF K_EMPTY
%token K_EVENTHANDLERS K_EXIT K_EXTENSION K_EXTENSIONACTIVITY
%token K_EXTENSIONASSIGNOPERATION K_EXTENSIONS K_FAULTHANDLERS
%token K_FINALCOUNTERVALUE K_FLOW K_FOR K_FOREACH K_FROM K_FROMPART K_FROMPARTS
%token K_GETLINKSTATUS K_IF K_IMPORT K_INVOKE K_JOINCONDITION K_LINK K_LINKS
%token K_LITERAL K_MESSAGEEXCHANGE K_MESSAGEEXCHANGES K_ONALARM K_ONEVENT
%token K_ONMESSAGE K_OPAQUEACTIVITY K_OPAQUEFROM K_OR K_OTHERWISE K_PARTNER
%token K_PARTNERLINK K_PARTNERLINKS K_PARTNERS K_PICK K_QUERY K_RECEIVE
%token K_REPEATEVERY K_REPEATUNTIL K_REPLY K_RETHROW K_SCOPE K_SEQUENCE
%token K_SOURCES K_STARTCOUNTERVALUE K_SWITCH K_TARGETS
%token K_TERMINATE K_TERMINATIONHANDLER K_THROW K_TO K_TOPART K_TOPARTS
%token K_TRANSITIONCONDITION K_UNTIL K_VALIDATE K_VARIABLE K_VARIABLES K_WAIT
%token K_WHILE LBRACKET LESS LESSOREQUAL NOTEQUAL RBRACKET X_CLOSE X_EQUALS
%token X_NEXT X_OPEN X_SLASH
%token K_TOPOLOGY K_PARTICIPANTTYPES K_PARTICIPANTTYPE K_PARTICIPANTS K_PARTICIPANT K_PARTICIPANTSET K_MESSAGELINKS K_MESSAGELINK
%token K_TYPES K_PORTTYPE K_FAULT K_OPERATION K_DEFINITIONS  K_MESSAGE K_PART K_BINDING K_PORT
%token K_PARTNERLINKTYPE K_ROLE K_SCHEMA K_PROPERTY K_PROPERTYALIAS
%token K_PROCESSMODEL K_PROCESSES K_TASKS K_SERVICES K_REPOSITORIES
%token K_PROCESS K_TASK K_SERVICE K_REPOSITORY K_OUTPUT K_INPUT
%token K_DESCRIPTION K_OUTPUTCRITERION K_INPUTCRITERION K_OUTPUTS K_INPUTS
%token K_PRECONDITION K_POSTCONDITION K_FLOWCONTENT K_STARTNODE K_STOPNODE K_ENDNODE K_ENTRYPOINT
%token K_CONDITION K_OUTPUTBRANCH K_INPUTBRANCH K_DECISION K_MERGE K_JOIN K_FORK
%token K_NOTIFICATIONFILTER K_NOTIFICATIONBROADCASTER K_NOTIFICATIONRECEIVER K_OBSERVER K_OBSERVATIONEXPRESSION
%token K_TIMER K_MAP K_LOOP K_FORLOOP K_LOOPCONDITION K_OPERATIONALDATA 
%token K_CALLTOPROCESS K_CALLTOTASK K_CALLTOSERVICE K_ADDITIONALINPUT K_ADDITIONALOUTPUT
%token K_INPUTREPOSITORYVALUE K_OUTPUTREPOSITORYVALUE K_INPUTCONSTANTVALUE K_GLOBALREPOSITORY
%token K_LOCALREPOSITORY K_LITERALVALUE K_PUBLICINSTANCE K_CONNECTION K_ANNOTATION
%token K_SOURCE K_TARGET K_ANNOTATIONTEXT K_ANNOTATEDNODE K_CLASSIFIER K_CLASSIFIERVALUE
%token K_NAME K_VALUE K_CONSTANTVALUE K_REPOSITORYVALUE K_METAINFORMATION
%token K_CONSTRAINT K_CORRELATION K_PREDICATE K_RELATEDINPUTCRITERIA
%token K_RESOURCES K_RESOURCEREQUIREMENT K_RESOURCEQUANTITY
%token NUMBER
%token X_NAME
%token VARIABLENAME
%token X_STRING
// OR and AND are left-associative.
%left K_OR
%left K_AND
// The start symbol of the grammar.
%start tProcessModel
/******************************************************************************
 * Bison options.
 *****************************************************************************/
// We know what we are doing. Abort if any shift/reduce or reduce/reduce errors
// arise.
%expect 0
// All "yy"-prefixes are replaced with "frontend_wsdl_".
%name-prefix="frontend_"
%{
// Options for Bison (1): Enable debug mode for verbose messages during
// parsing. The messages can be enabled by using command-line parameter
// "-d bison".
#define YYDEBUG 1
#define YYERROR_VERBOSE 1
// Options for Bison (2): To avoid the message "parser stack overflow". or
// "memory exhausted". These values are just guesses. Increase them if
// necessary.
#define YYMAXDEPTH 1000000
#define YYINITDEPTH 10000
#include <cassert>
#include <map>
#include <set>
#include<iostream>
#include "helpers.h"
#include "debug.h"
#include "globals.h"
#include "block.h"
#include "yystype.h"
#include <string>
using std::cerr;
using std::endl;
/******************************************************************************
 * External variables
 *****************************************************************************/
extern char *frontend_text;	// from flex: the current token
extern int frontend_lex();	// from flex: the lexer funtion
extern int frontend_lineno;	// from flex: the current line number
/******************************************************************************
 * Global variables
 *****************************************************************************/
Block* currentBlock = NULL;
Block* currentStructure = NULL;
BlockConnection* currentConnection;
set<std::string, std::string>tempAttributes;
string currentInput;
string currentOutput;
string currentInputBranch;
string currentOutputBranch;
string currentInputCriterion; 
string currentOutputCriterion; 
string currentAdditionalInput;
string currentAdditionalOutput;
bool relatedInputCriteria = false;
int attributeMode = 0;
  // 0 = dont read any attributes
  // 1 = read BlockAttributes 
  // 2 = Input
  // 3 = Output
  // 4 = Connection name
  // 5 = Connection Source
  // 6 = Connection Target
  // 7 = InputCriterion
  // 8 = OutputCriterion
  // 9 = AdditionalInput
  // 10 = AdditionalOutput
  // 11 = InputBranch
  // 12 = OutputBranch
  // 13 = Role
%}
%%
tProcessModel:
    {
      // initialisation (for multiple input files, i.e. `consistency' mode)
      frontend_lineno = 1;
    }
  X_OPEN K_PROCESSMODEL arbitraryAttributes X_NEXT
  tProcessModelContent_list
  X_SLASH K_PROCESSMODEL X_CLOSE
;
tProcessModelContent_list:
  /* empty */
| tProcessModelContent X_NEXT tProcessModelContent_list
;
tProcessModelContent:
  tProcesses 
| tTasks 
| tServices 
;
tProcesses:
  K_PROCESSES arbitraryAttributes X_NEXT
  tProcess_list
  X_SLASH K_PROCESSES
;
tProcess_list:
  /* empty */
| tProcess X_NEXT tProcess_list
;
tTasks:
  K_TASKS arbitraryAttributes X_NEXT
  tTask_list
  X_SLASH K_TASKS
;
tTask_list:
  /* empty */
| tTask X_NEXT tTask_list
;
tServices:
  K_SERVICES arbitraryAttributes X_NEXT
  tService_list
  X_SLASH K_SERVICES
;
tService_list:
  /* empty */
| tService X_NEXT tService_list
;
/******************************************************************************
 * PROCESS
 *****************************************************************************/
tProcess:
    { 
        currentBlock = new Block(PROCESS, true, false);
        if (currentStructure != NULL)
        {
            currentBlock->parent = currentStructure;
        }
        currentStructure = currentBlock;
    }
    K_PROCESS 
    {
        attributeMode = 1;
    }
    arbitraryAttributes 
    {
        attributeMode = 0;
    }
    X_NEXT
    tProcessContent_list
    X_SLASH K_PROCESS
    {
        // do not filter in the parser
        /*
        currentBlock->filter();
        if (!currentBlock->filtered)
        */ 
        {
            globals::processes.insert(currentBlock);
            if (currentBlock->parent != NULL)
            {
                currentBlock->parent->filtered = true;
            }
        }
        
        if (currentBlock->parent != NULL)
        {
            currentStructure = currentBlock->parent;
            currentBlock = currentBlock->parent;
        }
        else
        {
            currentStructure = NULL;
            currentBlock = NULL;
        }
    }
;
tProcessContent_list:
  /* empty */
| tProcessContent X_NEXT tProcessContent_list
;
tProcessContent:
  tInputs 
| tOutputs 
| tPrecondition 
| tPostcondition
/*tOrganizations */
/*tOperationalData */ 
| tFlowContent 
/*  tExtendedAttributes*/
| tClassifier 
;
tTask:
    { 
        currentBlock = new Block(TASK, false, false);
        if (currentStructure != NULL)
        {
            currentBlock->parent = currentStructure;
        }
    }
    K_TASK
    {
        attributeMode = 1;
    }
    arbitraryAttributes 
    {
        attributeMode = 0;
    }
    X_NEXT
    tTaskContent_list
    X_SLASH K_TASK
    { 
        globals::tasks.insert(currentBlock);
        if (currentBlock->parent != NULL)
        {
            currentBlock = currentBlock->parent;
        }
        else
        {
            currentBlock = NULL;
        }
    }
;
tTaskContent_list:
  /*empty*/
| tTaskContent X_NEXT tTaskContent_list
;
tTaskContent:
  tDescription
| tInputs
| tOutputs
| tPrecondition
| tPostcondition
/*tOperationalData */ 
/*  tExtendedAttributes*/
| tClassifier
| { attributeMode = 13; } tResources { attributeMode = 0; }
;
tService:
    { 
        currentBlock = new Block(SERVICE, false, false);
        if (currentStructure != NULL)
        {
            currentBlock->parent = currentStructure;
        }
    }
    K_SERVICE 
    {
        attributeMode = 1;
    }
    arbitraryAttributes 
    {
        attributeMode = 0;
    }
    X_NEXT
    tServiceContent_list
    X_SLASH K_SERVICE
    { 
        globals::services.insert(currentBlock);
        if (currentBlock->parent != NULL)
        {
            currentBlock = currentBlock->parent;
        }
        else
        {
            currentBlock = NULL;
        }
    }
;
tServiceContent_list:
  /*empty*/
| tServiceContent X_NEXT tServiceContent_list
;
tServiceContent:
  tDescription
| tInputs
| tOutputs
| tPrecondition
| tPostcondition
/*tOperationalData */ 
/*  tExtendedAttributes*/
| tClassifier
| { attributeMode = 13; } tResources { attributeMode = 0; }
;
tInputs:
    K_INPUTS arbitraryAttributes X_NEXT
    { 
        attributeMode = 2;
    }
    tInput_list 
    { 
        attributeMode = 0;
    }
    tInputCriterion_list
    X_SLASH K_INPUTS
;
tInput_list:
  /* empty */
| tInput X_NEXT tInput_list
;
tInput:
  K_INPUT
  arbitraryAttributes 
  X_NEXT tInputContent_list
  X_SLASH K_INPUT
| K_INPUT
  arbitraryAttributes
  X_SLASH
;
tInputContent_list:
  /*empty*/
| tInputContent X_NEXT tInputContent_list
;
tInputContent:
  tConstantValue
| tRepositoryValue
| tMetaInformation
;
tInputCriterion_list:
    /* empty */
|   
    {
        attributeMode = 7;
    }
    tInputCriterion X_NEXT tInputCriterion_list
;
tInputCriterion:
  K_INPUTCRITERION arbitraryAttributes X_NEXT
    {
        attributeMode = 0;
    }
  tInputCriterionContent_list
  X_SLASH K_INPUTCRITERION
    {
        currentInputCriterion = "";
    }
| K_INPUTCRITERION arbitraryAttributes
  X_SLASH
    {
        attributeMode = 0;
        currentInputCriterion = "";
    }
;
tInputCriterionContent_list:
  /* empty */
| tInputCriterionContent X_NEXT tInputCriterionContent_list
;
tInputCriterionContent:
    {
        attributeMode = 2;
    }
    tInput
    {
        attributeMode = 0;
    }
|   tConstraint
|   tIBMCorrelation
;
tOutputs:
    { 
        attributeMode = 3;
    }
    K_OUTPUTS arbitraryAttributes X_NEXT
    tOutput_list 
    { 
        attributeMode = 0;
    }
    tOutputCriterion_list
    X_SLASH K_OUTPUTS
;
tOutput_list:
  /* empty */
| tOutput X_NEXT tOutput_list
;
tOutput:
  K_OUTPUT arbitraryAttributes X_NEXT
  tOutputContent_list
  X_SLASH K_OUTPUT
| K_OUTPUT arbitraryAttributes X_SLASH
;
tOutputContent_list:
  /*empty*/
| tOutputContent X_NEXT tOutputContent_list
;
tOutputContent:
  tOutputSecondRepositoryValue
| tMetaInformation
;
tOutputCriterion_list:
    /* empty */
| 
    {
        attributeMode = 8;
    }
    tOutputCriterion X_NEXT tOutputCriterion_list
;
tOutputCriterion:
  K_OUTPUTCRITERION arbitraryAttributes X_NEXT
    {
        attributeMode = 0;
    }
  tOutputCriterionContent_list
  X_SLASH K_OUTPUTCRITERION
    {
        currentOutputCriterion = "";
    }
| K_OUTPUTCRITERION arbitraryAttributes
  X_SLASH
    {
        attributeMode = 0;
        currentOutputCriterion = "";
    }
;
tOutputCriterionContent_list:
  /* empty */
| tOutputCriterionContent X_NEXT tOutputCriterionContent_list
;
tOutputCriterionContent:
    {
        attributeMode = 3;
    }
  tOutput  
    {
        attributeMode = 0;
    }
| tRelatedInputCriteria 
;
tRelatedInputCriteria:
    {
        relatedInputCriteria = true;
    }
    K_RELATEDINPUTCRITERIA arbitraryAttributes X_NEXT
    tInputCriterion_list
    X_SLASH K_RELATEDINPUTCRITERIA
    {
        relatedInputCriteria = false;
    }
;
tPrecondition:
  K_PRECONDITION arbitraryAttributes X_NEXT
  tDescription
  X_NEXT X_SLASH K_PRECONDITION
| K_PRECONDITION arbitraryAttributes X_NEXT
  X_SLASH K_PRECONDITION
;
tPostcondition:
  K_POSTCONDITION arbitraryAttributes X_NEXT
  tDescription 
  X_NEXT X_SLASH K_POSTCONDITION
| K_POSTCONDITION arbitraryAttributes X_NEXT
  X_SLASH K_POSTCONDITION
;
tFlowContent:
  K_FLOWCONTENT arbitraryAttributes X_NEXT
  tFlowContentContent_list
  X_SLASH K_FLOWCONTENT
| K_FLOWCONTENT X_SLASH /* empty flow content */
;
tFlowContentContent_list:
  /* empty */
| tFlowContentContent X_NEXT tFlowContentContent_list
;
tFlowContentContent:
    {
        attributeMode = 1;
        currentBlock = new Block(STARTNODE, false, true);
    }
    tStartNode
|   
    {
        attributeMode = 1;
        currentBlock = new Block(STOPNODE, false, true);
    }
    tStopNode 
|   
    {
        attributeMode = 1;
        currentBlock = new Block(ENDNODE, false, true);
    }
    tEndNode  
|   
{
    attributeMode = 1;
    currentBlock = new Block(TASK, false, false);
}
    tFlowContentTask  
|   
    {
        attributeMode = 1;
        currentBlock = new Block(DECISION, false, false);
    }
    tDecision  
|   
    {
        attributeMode = 1;
        currentBlock = new Block(MERGE, false, false);
    }
    tMerge 
|   
    {
        attributeMode = 1;
        currentBlock = new Block(FORK, false, false);
    }
    tFork 
|   
    {
        attributeMode = 1;
        currentBlock = new Block(JOIN, false, false);
    }
    tJoin 
|   
    {
        attributeMode = 1;
        currentBlock = new Block(NOTIFICATIONBROADCASTER, false, false);
    }
    tNotificationBroadcaster 
|   
    {
        attributeMode = 1;
        currentBlock = new Block(NOTIFICATIONRECEIVER, false, false);
    }
    tNotificationReceiver 
|   
    {
        attributeMode = 1;
        currentBlock = new Block(OBSERVER, false, false);
    }
    tObserver  
|   
    {
        attributeMode = 1;
        currentBlock = new Block(TIMER, false, false);
    }
    tTimer  
|   
    {
        attributeMode = 1;
        currentBlock = new Block(MAP, false, false);
    }
    tMap  
|   
    {
        attributeMode = 1;
        currentBlock = new Block(LOOP, false, false);
    }
    tLoop  
|   
    {
        attributeMode = 1;
        currentBlock = new Block(FORLOOP, false, false);
    }
    tForLoop  
|   tProcess
|   tLocalRepository  
|   
    {
        attributeMode = 1;
        currentBlock = new Block(CALLTOPROCESS, false, false);
    }
    tCallToProcess  
|   
    {
        attributeMode = 1;
        currentBlock = new Block(CALLTOTASK, false, false);
    }
    tCallToTask 
|   
    {
        attributeMode = 1;
        currentBlock = new Block(CALLTOSERVICE, false, false);
    }
    tCallToService  
|
    {
        attributeMode = 4;
        currentConnection = new BlockConnection();
    }
    tConnection
|   tAnnotation 
;

tClassifier:
  K_CLASSIFIER arbitraryAttributes X_NEXT
  tDescription tClassifierValue_list
  X_SLASH K_CLASSIFIER
| K_CLASSIFIER arbitraryAttributes X_NEXT
  tClassifierValue_list
  X_SLASH K_CLASSIFIER
;

tClassifierValue_list:
  /*empty*/
| tClassifierValue X_NEXT tClassifierValue_list
;

tClassifierValue:
  K_CLASSIFIERVALUE arbitraryAttributes X_NEXT
  tDescription 
  X_NEXT X_SLASH K_CLASSIFIERVALUE
| K_CLASSIFIERVALUE arbitraryAttributes X_SLASH
| K_CLASSIFIERVALUE arbitraryAttributes X_NEXT
  X_SLASH K_CLASSIFIERVALUE
;


tStartNode:
    K_STARTNODE
    arbitraryAttributes X_NEXT
    {
        attributeMode = 0;
    }
    tEntryPoint_list
    X_SLASH K_STARTNODE
    {
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        // do not filter in the parser
		/*
		currentBlock->filter();
		if (currentBlock->filtered)
        {
            currentStructure->filtered = true;
        }
        */
        currentBlock = currentBlock->parent;
    }
|   K_STARTNODE arbitraryAttributes
    X_SLASH
    {
        attributeMode = 0;
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        // do not filter in the parser
        /*
        currentBlock->filter();
        if (currentBlock->filtered)
        {
            currentStructure->filtered = true;
        }
        */
        currentBlock = currentBlock->parent;
    }
;
tEntryPoint_list:
  /*empty*/
| tEntryPoint X_NEXT tEntryPoint_list
;
tEntryPoint:
  K_ENTRYPOINT arbitraryAttributes X_NEXT
  X_SLASH K_ENTRYPOINT
| K_ENTRYPOINT arbitraryAttributes
  X_SLASH
;
tStopNode:
  K_STOPNODE arbitraryAttributes X_NEXT X_SLASH K_STOPNODE
    {
        attributeMode = 0;
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        // do not filter in the parser
        /*
        currentBlock->filter();
        if (currentBlock->filtered)
        {
            currentStructure->filtered = true;
        }
        */
        currentBlock = currentBlock->parent;
    }
| K_STOPNODE arbitraryAttributes X_SLASH
    {
        attributeMode = 0;
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        // do not filter in the parser
        /*
        currentBlock->filter();
        if (currentBlock->filtered)
        {
            currentStructure->filtered = true;
        }
        */
        currentBlock = currentBlock->parent;
    }
;
tEndNode:
  K_ENDNODE arbitraryAttributes X_NEXT X_SLASH K_ENDNODE
    {
        attributeMode = 0;
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        // do not filter in the parser
        /*
        currentBlock->filter();
        if (currentBlock->filtered)
        {
            currentStructure->filtered = true;
        }
        */
        currentBlock = currentBlock->parent;
    }
| K_ENDNODE arbitraryAttributes X_SLASH 
    {
        attributeMode = 0;
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        // do not filter in the parser
        /*
        currentBlock->filter();
        if (currentBlock->filtered)
        {
            currentStructure->filtered = true;
        }
        */
        currentBlock = currentBlock->parent;
    }
;
tDecision:
    K_DECISION arbitraryAttributes X_NEXT
    {
        attributeMode = 0;
    }
    tDecisionContent_list
    X_SLASH K_DECISION
    {
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        // do not filter in the parser
        /*
        currentBlock->filter();
        if (currentBlock->filtered)
        {
            currentStructure->filtered = true;
        }
        */
        currentBlock = currentBlock->parent;
    }
;
tDecisionContent_list:
  /*empty*/
| tDecisionContent X_NEXT tDecisionContent_list
;
tDecisionContent:
  tInputBranch 
| tDecisionOutputBranch
;
tMerge:
    K_MERGE arbitraryAttributes X_NEXT
    {
        attributeMode = 0;
    }
    tMergeContent_list
    X_SLASH K_MERGE
    {
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        // do not filter in the parser
        /*
        currentBlock->filter();
        if (currentBlock->filtered)
        {
            currentStructure->filtered = true;
        }
        */
        currentBlock = currentBlock->parent;
    }
;
tMergeContent_list:
  /*empty*/
| tMergeContent X_NEXT tMergeContent_list
;
tMergeContent:
  tInputBranch
| tOutputBranch
;
tFork:
    K_FORK arbitraryAttributes X_NEXT
    {
        attributeMode = 0;
    }
    tForkContent_list
    X_SLASH K_FORK
    {
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        // do not filter in the parser
        /*
        currentBlock->filter();
        if (currentBlock->filtered)
        {
            currentStructure->filtered = true;
        }
        */
        currentBlock = currentBlock->parent;
    }
;
tForkContent_list:
  /*empty*/
| tForkContent X_NEXT tForkContent_list
;
tForkContent:
  tDescription
| tInputBranch
| tOutputBranch
;
tJoin:
    K_JOIN arbitraryAttributes X_NEXT
    {
        attributeMode = 0;
    }
    tJoinContent_list
    X_SLASH K_JOIN
    {
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        // do not filter in the parser
        /*
        currentBlock->filter();
        if (currentBlock->filtered)
        {
            currentStructure->filtered = true;
        }
        */
        currentBlock = currentBlock->parent;
    }
;
tJoinContent_list:
  /*empty*/
| tJoinContent X_NEXT tJoinContent_list
;
tJoinContent:
  tDescription
| tInputBranch
| tOutputBranch
;
tNotificationBroadcaster:
    K_NOTIFICATIONBROADCASTER arbitraryAttributes X_NEXT
    {
        attributeMode = 0;
    }
    tTaskContent_list
    X_SLASH K_NOTIFICATIONBROADCASTER
    {
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        // do not filter in the parser
        /*
        currentBlock->filter();
        if (currentBlock->filtered)
        {
            currentStructure->filtered = true;
        }
        */
        currentBlock = currentBlock->parent;
    }
;
tNotificationReceiver:
    K_NOTIFICATIONRECEIVER arbitraryAttributes X_NEXT
    {
        attributeMode = 0;
    }
    tNotificationReceiverContent_list
    X_SLASH K_NOTIFICATIONRECEIVER
    {
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        // do not filter in the parser
        /*
        currentBlock->filter();
        if (currentBlock->filtered)
        {
            currentStructure->filtered = true;
        }
        */
        currentBlock = currentBlock->parent;
    }
;
tNotificationReceiverContent_list:
  /*empty*/
| tNotificationReceiverContent X_NEXT tNotificationReceiverContent_list
;
tNotificationReceiverContent:
  tNotificationFilter
| tDescription
| tInputs
| tOutputs
| tPrecondition
| tPostcondition
/*tOperationalData */ 
/*  tExtendedAttributes*/
| tClassifier
;
tObserver:
    K_OBSERVER arbitraryAttributes X_NEXT
    {
        attributeMode = 0;
    }
    tObserverContent_list
    X_SLASH K_OBSERVER
    {
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        // do not filter in the parser
        /*
        currentBlock->filter();
        if (currentBlock->filtered)
        {
            currentStructure->filtered = true;
        }
        */
        currentBlock = currentBlock->parent;
    }
;
tObserverContent_list:
  /*empty*/
| tObserverContent X_NEXT tObserverContent_list
;
tObserverContent:
  tObservationExpression
| tDescription
| tInputs
| tOutputs
| tPrecondition
| tPostcondition
/*tOperationalData */ 
/*  tExtendedAttributes*/
| tClassifier
;
tTimer:
    K_TIMER arbitraryAttributes X_NEXT
    {
        attributeMode = 0;
    }
    tTaskContent_list
    X_SLASH K_TIMER
    {
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        // do not filter in the parser
        /*
        currentBlock->filter();
        if (currentBlock->filtered)
        {
            currentStructure->filtered = true;
        }
        */
        currentBlock = currentBlock->parent;
    }
;
tMap:
    K_MAP arbitraryAttributes X_NEXT
    {
        attributeMode = 0;
    }
    tTaskContent_list
    X_SLASH K_MAP
    {
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        /*
        currentBlock->filter();
        if (currentBlock->filtered)
        {
            currentStructure->filtered = true;
        }
        */
        currentBlock = currentBlock->parent;
    }
;
tFlowContentTask:
    K_TASK arbitraryAttributes X_NEXT
    {
        attributeMode = 0;
    }
    tTaskContent_list
    X_SLASH K_TASK
    {
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        // do not filter in the parser
        /*
        currentBlock->filter();
        if (currentBlock->filtered)
        {
            currentStructure->filtered = true;
        }
        */
        currentBlock = currentBlock->parent;
    }
;
tLoop:  // BAM LOOP IST SEPERAT STRUCTURED
    K_LOOP arbitraryAttributes X_NEXT
    {
        attributeMode = 0;
    }
    tLoopContent_list
    X_SLASH K_LOOP
    {
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        // do not filter in the parser
        /*
        currentBlock->filter();
        if (currentBlock->filtered)
        {
            currentStructure->filtered = true;
        }
        */
        currentBlock = currentBlock->parent;
    }
;
tLoopContent_list:
  /*empty*/
| tLoopContent X_NEXT tLoopContent_list
;
tLoopContent:
  tLoopCondition
| tDescription
| tInputs
| tOutputs
| tPrecondition
| tPostcondition
| tFlowContent
;
tForLoop:
    K_FORLOOP arbitraryAttributes X_NEXT
    {
        attributeMode = 0;
    }
    tForLoopContent_list
    X_SLASH K_FORLOOP
    {
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        // do not filter in the parser
        /*
        currentBlock->filter();
        if (currentBlock->filtered)
        {
            currentStructure->filtered = true;
        }
        */
        currentBlock = currentBlock->parent;
    }
;
tForLoopContent_list:
  /*empty*/
| tForLoopContent X_NEXT tForLoopContent_list
;
tForLoopContent:
  tLoopCondition
| tDescription
| tInputs
| tOutputs
| tPrecondition
| tPostcondition
| tFlowContent
;

tLocalRepository:
  K_LOCALREPOSITORY arbitraryAttributes X_NEXT
  tDescription 
  X_NEXT X_SLASH K_LOCALREPOSITORY
| K_LOCALREPOSITORY arbitraryAttributes X_NEXT
  X_SLASH K_LOCALREPOSITORY
| K_LOCALREPOSITORY arbitraryAttributes X_SLASH
;

tCallToProcess:
  K_CALLTOPROCESS arbitraryAttributes X_NEXT
    {
        attributeMode = 0;
    }
  tInvocation_list
  X_SLASH K_CALLTOPROCESS
    {
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        currentBlock = currentBlock->parent;
    }
| K_CALLTOPROCESS arbitraryAttributes X_SLASH
    {
        attributeMode = 0;
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        currentBlock = currentBlock->parent;
    }
;

tCallToService:
  K_CALLTOSERVICE arbitraryAttributes X_NEXT
    {
        attributeMode = 0;
    }
  tInvocation_list
  X_SLASH K_CALLTOSERVICE
    {
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        currentBlock = currentBlock->parent;
    }
;

tCallToTask:
  K_CALLTOTASK arbitraryAttributes X_NEXT
    {
        attributeMode = 0;
    }
  tInvocation_list
  X_SLASH K_CALLTOTASK
    {
        currentBlock->parent = currentStructure;
        currentBlock->parent->children.insert(currentBlock);
        currentBlock = currentBlock->parent;
    }
;

tConnection:
    K_CONNECTION arbitraryAttributes X_NEXT
    {
        attributeMode = 0;
    }
    tDescription X_NEXT 
    {
        attributeMode = 5;
    }
    tIBMSource X_NEXT 
    {
        attributeMode = 6;
    }
    tIBMTarget X_NEXT
    X_SLASH K_CONNECTION
    {
        attributeMode = 0;
        currentStructure->addConnection(currentConnection);
    }
|   K_CONNECTION arbitraryAttributes X_NEXT
    {
        attributeMode = 5;
    }
    tIBMSource X_NEXT 
    {
        attributeMode = 6;
    }
    tIBMTarget X_NEXT
    X_SLASH K_CONNECTION
    {
        attributeMode = 0;
        currentStructure->addConnection(currentConnection);
    }
;
tAnnotation:
  K_ANNOTATION arbitraryAttributes X_NEXT
  tAnnotatedNode_list
  X_SLASH K_ANNOTATION
| K_ANNOTATION arbitraryAttributes X_SLASH 
;
tInvocation_list:
  /*empty*/
| tInvocation X_NEXT tInvocation_list
;
tInvocation:
    {
        attributeMode = 9;
    }  
    tAdditionalInput 
|   
    {
        attributeMode = 10;
    }  
    tAdditionalOutput 
|   tInputRepositoryValue 
|   tInputConstantValue 
|   tOutputRepositoryValue 
;

tInputBranch:
    { 
        attributeMode = 11;
    }
    K_INPUTBRANCH arbitraryAttributes X_NEXT
    { 
        attributeMode = 2;
    }
    tInput_list
    { 
        attributeMode = 0;
    }
    X_SLASH K_INPUTBRANCH
    {
        currentInputBranch = "";
    }
;

tOutputBranch:
    { 
        attributeMode = 12;
    }
    K_OUTPUTBRANCH arbitraryAttributes X_NEXT
    { 
        attributeMode = 3;
    }
    tOutput_list
    { 
        attributeMode = 0;
    }
    X_SLASH K_OUTPUTBRANCH
    {
        currentOutputBranch = "";
    }
;
  
tDecisionOutputBranch:
    { 
        attributeMode = 12;
    }
    K_OUTPUTBRANCH arbitraryAttributes X_NEXT
    tDecisionOutputBranchContent_list
    X_SLASH K_OUTPUTBRANCH
    {
        currentOutputBranch = "";
    }    
;
tDecisionOutputBranchContent_list:
  /*empty*/
| tDecisionOutputBranchContent X_NEXT tDecisionOutputBranchContent_list
;
tDecisionOutputBranchContent:
    { 
        attributeMode = 3;
    }
  tOutput 
    { 
        attributeMode = 0;
    }
| tIBMCondition 
;
tAdditionalInput:
    K_ADDITIONALINPUT arbitraryAttributes X_NEXT
    { 
        attributeMode = 0;
    }
    tInputCriterion_list
    X_SLASH K_ADDITIONALINPUT
    {
        currentAdditionalInput = "";
    }    
;
tAdditionalOutput:
    K_ADDITIONALOUTPUT arbitraryAttributes X_NEXT
    { 
        attributeMode = 0;
    }
    tOutputCriterion_list
    X_SLASH K_ADDITIONALOUTPUT
    {
        currentAdditionalOutput = "";
    }    
;
tInputRepositoryValue:
  K_INPUTREPOSITORYVALUE arbitraryAttributes X_NEXT
  tGlobalRepositoryRef
  X_NEXT X_SLASH K_INPUTREPOSITORYVALUE
| K_INPUTREPOSITORYVALUE arbitraryAttributes X_NEXT
  tLocalRepositoryRef
  X_NEXT X_SLASH K_INPUTREPOSITORYVALUE
;
tRepositoryValue:
  K_REPOSITORYVALUE arbitraryAttributes X_NEXT
  tGlobalRepositoryRef
  X_NEXT X_SLASH K_REPOSITORYVALUE
| K_REPOSITORYVALUE arbitraryAttributes X_NEXT
  tLocalRepositoryRef
  X_NEXT X_SLASH K_REPOSITORYVALUE
;
tOutputRepositoryValue:
  K_OUTPUTREPOSITORYVALUE arbitraryAttributes X_NEXT
  tGlobalRepositoryRef
  X_NEXT X_SLASH K_OUTPUTREPOSITORYVALUE
| K_OUTPUTREPOSITORYVALUE arbitraryAttributes X_NEXT
  tLocalRepositoryRef
  X_NEXT X_SLASH K_OUTPUTREPOSITORYVALUE
;
tOutputSecondRepositoryValue:
  K_REPOSITORYVALUE arbitraryAttributes X_NEXT
  tGlobalRepositoryRef
  X_NEXT X_SLASH K_REPOSITORYVALUE
| K_REPOSITORYVALUE arbitraryAttributes X_NEXT
  tLocalRepositoryRef
  X_NEXT X_SLASH K_REPOSITORYVALUE
;
tGlobalRepositoryRef:
  K_GLOBALREPOSITORY arbitraryAttributes X_NEXT
  X_SLASH K_GLOBALREPOSITORY
  | K_GLOBALREPOSITORY arbitraryAttributes X_SLASH
;
tLocalRepositoryRef:
  K_LOCALREPOSITORY arbitraryAttributes X_NEXT
  X_SLASH K_LOCALREPOSITORY
| K_LOCALREPOSITORY arbitraryAttributes X_SLASH
;
tInputConstantValue:
  K_INPUTCONSTANTVALUE arbitraryAttributes X_NEXT
  tLiteralValue
  X_NEXT X_SLASH K_INPUTCONSTANTVALUE
| K_INPUTCONSTANTVALUE arbitraryAttributes X_NEXT
  tPublicInstance
  X_SLASH K_INPUTCONSTANTVALUE
;
tConstantValue:
  K_CONSTANTVALUE arbitraryAttributes X_NEXT
  tLiteralValue
  X_NEXT X_SLASH K_CONSTANTVALUE
| K_CONSTANTVALUE arbitraryAttributes X_NEXT
  tPublicInstance
  X_SLASH K_CONSTANTVALUE
;
tMetaInformation:
  K_METAINFORMATION arbitraryAttributes X_NEXT
  tName X_NEXT tValue
  X_NEXT X_SLASH K_METAINFORMATION
;
tPublicInstance:
  K_PUBLICINSTANCE arbitraryAttributes X_NEXT
  X_SLASH K_PUBLICINSTANCE
| K_PUBLICINSTANCE arbitraryAttributes X_SLASH 
;
tIBMSource:
    K_SOURCE arbitraryAttributes X_NEXT
    X_SLASH K_SOURCE
    {
        attributeMode = 0;
    }  
|   K_SOURCE arbitraryAttributes X_SLASH
    {
        attributeMode = 0;
    }
;
tIBMTarget:
    K_TARGET arbitraryAttributes X_NEXT
    X_SLASH K_TARGET
    {
        attributeMode = 0;
    }
|   K_TARGET arbitraryAttributes X_SLASH
    {
        attributeMode = 0;
    }
;
tAnnotatedNode_list:
  /*empty*/
| tAnnotatedNode X_NEXT tAnnotatedNode_list
;
tAnnotatedNode:
  K_ANNOTATEDNODE arbitraryAttributes X_NEXT
  X_SLASH K_ANNOTATEDNODE
| K_ANNOTATEDNODE arbitraryAttributes X_SLASH
;
tIBMCondition:
  K_CONDITION arbitraryAttributes X_NEXT
  tDescription 
  X_NEXT X_SLASH K_CONDITION
| K_CONDITION arbitraryAttributes X_NEXT
  X_SLASH K_CONDITION
| K_CONDITION arbitraryAttributes X_SLASH
;
tNotificationFilter:
  K_NOTIFICATIONFILTER arbitraryAttributes X_NEXT
  tDescription 
  X_NEXT X_SLASH K_NOTIFICATIONFILTER
| K_NOTIFICATIONFILTER arbitraryAttributes X_NEXT
  X_SLASH K_NOTIFICATIONFILTER
| K_NOTIFICATIONFILTER arbitraryAttributes X_SLASH
;
tObservationExpression:
  K_OBSERVATIONEXPRESSION arbitraryAttributes X_NEXT
  tDescription 
  X_NEXT X_SLASH K_OBSERVATIONEXPRESSION
| K_OBSERVATIONEXPRESSION arbitraryAttributes X_NEXT
  X_SLASH K_OBSERVATIONEXPRESSION
| K_OBSERVATIONEXPRESSION arbitraryAttributes X_SLASH
;
tLoopCondition:
  K_LOOPCONDITION arbitraryAttributes X_NEXT
  tDescription 
  X_NEXT X_SLASH K_LOOPCONDITION
| K_LOOPCONDITION arbitraryAttributes X_NEXT
  X_SLASH K_LOOPCONDITION
| K_LOOPCONDITION arbitraryAttributes X_SLASH
;
tConstraint:
  K_CONSTRAINT arbitraryAttributes X_NEXT
  tDescription 
  X_NEXT X_SLASH K_CONSTRAINT
| K_CONSTRAINT arbitraryAttributes X_NEXT
  X_SLASH K_CONSTRAINT
| K_CONSTRAINT arbitraryAttributes X_SLASH
;
tIBMCorrelation:
  K_CORRELATION arbitraryAttributes X_NEXT
  tPredicate 
  X_NEXT X_SLASH K_CORRELATION
| K_CORRELATION arbitraryAttributes X_NEXT
  X_SLASH K_CORRELATION
| K_CORRELATION arbitraryAttributes X_SLASH
;
tPredicate:
  K_PREDICATE arbitraryAttributes X_NEXT
  tDescription 
  X_NEXT X_SLASH K_PREDICATE
| K_PREDICATE arbitraryAttributes X_NEXT
  X_SLASH K_PREDICATE
| K_PREDICATE arbitraryAttributes X_SLASH
;


tResources:
  K_RESOURCES arbitraryAttributes X_NEXT
  tResource_list
  X_SLASH K_RESOURCES
| K_RESOURCES arbitraryAttributes X_SLASH
;

tResource_list:
  /* empty */
| tResourceRequirement X_NEXT tResource_list
;

tResourceRequirement:
  K_RESOURCEREQUIREMENT arbitraryAttributes X_NEXT
  tResourceQuantity
  X_NEXT X_SLASH K_RESOURCEREQUIREMENT
| K_RESOURCEREQUIREMENT arbitraryAttributes X_NEXT
  X_SLASH K_RESOURCEREQUIREMENT
| K_RESOURCEREQUIREMENT arbitraryAttributes X_SLASH
;

tResourceQuantity:
   K_RESOURCEQUANTITY arbitraryAttributes X_SLASH K_RESOURCEQUANTITY
|  K_RESOURCEQUANTITY arbitraryAttributes X_NEXT X_SLASH K_RESOURCEQUANTITY
|  K_RESOURCEQUANTITY arbitraryAttributes X_CLOSE NUMBER X_OPEN X_SLASH K_RESOURCEQUANTITY
;

/******************************************************************************
 * STRINGS
 *****************************************************************************/
tDescription:
   K_DESCRIPTION arbitraryAttributes X_SLASH K_DESCRIPTION
|  K_DESCRIPTION arbitraryAttributes X_NEXT X_SLASH K_DESCRIPTION
|  K_DESCRIPTION arbitraryAttributes X_CLOSE X_NAME X_OPEN X_SLASH K_DESCRIPTION
;
tValue:
   K_NAME arbitraryAttributes X_SLASH K_NAME
|  K_NAME arbitraryAttributes X_NEXT X_SLASH K_NAME
|  K_NAME arbitraryAttributes X_CLOSE X_NAME X_OPEN X_SLASH K_NAME
;
tName:
   K_VALUE arbitraryAttributes X_SLASH K_VALUE
|  K_VALUE arbitraryAttributes X_NEXT X_SLASH K_VALUE
|  K_VALUE arbitraryAttributes X_CLOSE X_NAME X_OPEN X_SLASH K_VALUE
;
tLiteralValue:
   K_LITERALVALUE arbitraryAttributes X_SLASH K_LITERALVALUE
|  K_LITERALVALUE arbitraryAttributes X_NEXT X_SLASH K_LITERALVALUE
|  K_LITERALVALUE arbitraryAttributes X_CLOSE X_NAME X_OPEN X_SLASH K_LITERALVALUE
;
/******************************************************************************
 * ATTRIBUTES
 *****************************************************************************/
arbitraryAttributes:
  /* empty */
|   X_NAME X_EQUALS X_STRING arbitraryAttributes
    {
        if(attributeMode == 1)
        {
            currentBlock->attributes[$1] = $3;
        }
        if(attributeMode == 2  && !(relatedInputCriteria))
        {
            if ($1 == "name")
            {
                currentBlock->addInput($3);
                currentInput = $3;
                if ( currentInputCriterion != "")
                {
                    currentBlock->inputCriterionSet[currentInputCriterion].insert($3);
                }
                else if ( currentInputBranch != "")
                {
                    currentBlock->inputBranchSet[currentInputBranch].insert($3);
                }
            }
            else if ($1 == "minimum")
            {
                if($3 != "1")
                {
                    //std::cerr << "Process \"" << currentStructure->attributes["name"] <<"\" Block \"" << currentBlock->attributes["name"] << " pinName " << currentInput << " ist riskant!\n";
                	currentBlock->wrongMultiplicity[currentInput] = true;
                }
            }
            else if ($1 == "maximum")
            {
                if($3 != "1")
                {
                    //std::cerr << "Process \"" << currentStructure->attributes["name"] <<"\" Block \"" << currentBlock->attributes["name"] << " pinName " << currentInput << " ist riskant!\n";
                	currentBlock->wrongMultiplicity[currentInput] = true;
                }
            }
        }
        if(attributeMode == 3)
        {
            if ($1 == "name")
            {
                currentBlock->addOutput($3);
                currentOutput = $3;
                if ( currentOutputCriterion != "")
                {
                	currentBlock->outputCriterionSet[currentOutputCriterion].insert($3);
                }
                else if ( currentOutputBranch != "")
                {
                	currentBlock->outputBranchSet[currentOutputBranch].insert($3);
                }
            }
            else if ($1 == "minimum")
            {
                if($3 != "1")
                {
                    //std::cerr << "Process \"" << currentStructure->attributes["name"] <<"\" Block \"" << currentBlock->attributes["name"] << " pinName " << currentOutput << " ist riskant!\n";
                	currentBlock->wrongMultiplicity[currentOutput] = true;
                }
            }
            else if ($1 == "maximum")
            {
                if($3 != "1")
                {
                    //std::cerr << "Process \"" << currentStructure->attributes["name"] <<"\" Block \"" << currentBlock->attributes["name"] << " pinName " << currentOutput << " ist riskant!\n";
                	currentBlock->wrongMultiplicity[currentOutput] = true;
                }
            }
        }
        if(attributeMode == 4)
        {
            if ($1 == "name")
            {
                currentConnection->name = $3;
            }
        }
        if(attributeMode == 5)
        {
            if ($1 == "contactPoint")
            {
                currentConnection->sourceContact = $3;
            }
            else if ($1 == "node")
            {
                currentConnection->source = $3;
            }
        }
        if(attributeMode == 6)
        {
            if ($1 == "contactPoint")
            {
                currentConnection->targetContact = $3;
            }
            else if ($1 == "node")
            {
                currentConnection->target = $3;
            }
        }
        if(attributeMode == 7  && !(relatedInputCriteria))
        {
            if ($1 == "name")
            {
                currentBlock->addInputCriterion($3);
                currentInputCriterion = $3;
                if ( currentAdditionalInput != "")
                {
                    currentBlock->additionalInputSet[currentAdditionalInput].insert(currentInputCriterion);
                }
            }
        } else if (attributeMode == 7 && relatedInputCriteria) {
            if ($1 == "name")
            {
                currentBlock->criterionMap[currentOutputCriterion].insert($3);
            }
        }
        
        if(attributeMode == 8)
        {
            if ($1 == "name")
            {
                currentBlock->addOutputCriterion($3);
                currentOutputCriterion = $3;
                if ( currentAdditionalOutput != "")
                {
                    currentBlock->additionalOutputSet[currentAdditionalOutput].insert(currentOutputCriterion);
                }
            }
        }
        if(attributeMode == 9)
        {
            if ($1 == "name")
            {
                currentBlock->addAdditionalInput($3);
                currentAdditionalInput = $3;
            }
            else if ($1 == "minimum")
            {
                if($3 != "1")
                {
                    //std::cerr << "Process \"" << currentStructure->attributes["name"] <<"\" Block \"" << currentBlock->attributes["name"] << " pinName " << currentAdditionalInput << " ist riskant!\n";
                	currentBlock->wrongMultiplicity[currentAdditionalInput] = true;
                }
            }
            else if ($1 == "maximum")
            {
                if($3 != "1")
                {
                    //std::cerr << "Process \"" << currentStructure->attributes["name"] <<"\" Block \"" << currentBlock->attributes["name"] << " pinName " << currentAdditionalInput << " ist riskant!\n";
                    currentBlock->wrongMultiplicity[currentAdditionalInput] = true;
                }
            }
        }
        if(attributeMode == 10)
        {
            if ($1 == "name")
            {
                currentBlock->addAdditionalOutput($3);
                currentAdditionalOutput = $3;
            }
            else if ($1 == "minimum")
            {
                if($3 != "1")
                {
                    //std::cerr << "Process \"" << currentStructure->attributes["name"] <<"\" Block \"" << currentBlock->attributes["name"] << " pinName " << currentAdditionalOutput << " ist riskant!\n";
                	currentBlock->wrongMultiplicity[currentAdditionalOutput] = true;
                }
            }
            else if ($1 == "maximum")
            {
                if($3 != "1")
                {
                    //std::cerr << "Process \"" << currentStructure->attributes["name"] <<"\" Block \"" << currentBlock->attributes["name"] << " pinName " << currentAdditionalOutput << " ist riskant!\n";
                	currentBlock->wrongMultiplicity[currentAdditionalOutput] = true;
                }
            }
        }
        if(attributeMode == 11)
        {
            if ($1 == "name")
            {
                currentBlock->addInputBranch($3);
                currentInputBranch = $3;
            }
        }
        if(attributeMode == 12)
        {
            if ($1 == "name")
            {
                currentBlock->addOutputBranch($3);
                currentOutputBranch = $3;
            }
        }
        if(attributeMode == 13)
        {
            if ($1 == "role")
            {
                currentBlock->addRole($3);
                // std::cerr << "Role \"" << $3  << "\" has been added to Block \""  << currentBlock->attributes["name"] << "\"!\n";
            }
        }
    }
;
