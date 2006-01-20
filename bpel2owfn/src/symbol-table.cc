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
 * \file symbol-table.cc
 *
 * \brief
 *
 * \author  
 *          
 * \date
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit&auml;t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 */ 

#include "symbol-table.h"
#include "bpel-attributes.h"
#include "bpel-syntax.h"
#include "debug.h"

/********************************************
 * implementation of SymbolTable CLASS
 ********************************************/

/*!
 * constructor
 */
SymbolTable::SymbolTable()
{
  this->symTab.clear();
  this->entryId = 0;
}

/*!
 * increase the entryId of the SymbolTable 
 */
kc::integer SymbolTable::nextId()
{
  this->entryId++;
  return kc::mkinteger(this->entryId);
}

/*!
 * 
 */
void SymbolTable::insert()
{
	
}
