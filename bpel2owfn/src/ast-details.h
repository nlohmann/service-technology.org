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
\****************************************************************************/

/*!
 * \file ast-details.h
 *
 * \author  
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *          
 * \date
 *          - created: 2005/07/02
 *          - last changed: \$Date: 2006/10/24 13:47:31 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.22 $
 */





#ifndef AST_DETAILS_H
#define AST_DETAILS_H





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <map>
#include <string>
#include <list>
#include <set>

#include "parser.h"		// created by Bison
#include "ast-config.h"		// all you need from Kimwitu++

using namespace std;





/******************************************************************************
 * Data structures
 *****************************************************************************/

typedef enum
{
  POSITIVECF = 0,	///< positive control flow
  FHCF = 1,		///< control flow in fault handler
  CHCF = 2		///< control flow in compensation handler
} controlFlowType;





/******************************************************************************
 * Class definitions
 *****************************************************************************/

class ASTE
{
  private:
    /// the id of the AST extension
    unsigned int id;

    /// the type of the node, identified by the keywords as parsed by Flex
    int type;

  public:
    /// a pointer to the node in the abstract syntax tree
    kc::impl_abstract_phylum *node;

    /// the parsed attributes
    map<string, string> attributes;

    /// the identifiers of all (recursively) enclosed source links
    set<int> enclosedSourceLinks;

    /// the identifiers of all (direct) enclosed scopes (only relevant for <scope> and <process>)
    set<int> enclosedScopes;

    /// the identifier of the parent scope
    unsigned int parentScopeId;

    /// the identifier of the parent activity
    unsigned int parentActivityId;

    /// true if join failures are suppressed
    bool suppressJF;

    /// the kind of control flow the activity is embedded in
    controlFlowType controlFlow;

    /// true if activity is embedded in a while activity
    bool inWhile;

    /// constructor
    ASTE(int myid, int mytype);

    /// checks and returns attributes
    map<string, string> getAttributes();

    /// creates a communication channel
    string createChannel(bool synchronousCommunication = false);

    /// checks a variable and returns its name
    string checkVariable();
    /// checks a variable and returns its name
    string checkInputVariable();
    /// checks a variable and returns its name
    string checkOutputVariable();

    /// defines a variable
    void defineVariable();

    /// returns the name of an activity type
    string activityTypeName();

    string channelName;
    string variableName;
    string inputVariableName;
    string outputVariableName;
    string linkName;

    bool hasEH;
    bool hasCatchAll;
    bool inProcess;
};





#endif
