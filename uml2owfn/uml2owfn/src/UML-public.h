/*****************************************************************************\
  UML2oWFN -- Translating UML2 Activity Diagrams to Petri nets
  Copyright (C) 2008  Dirk Fahland <dirk.fahland@service-technolog.org>,

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*
 * UML-public.h
 *
 *  Created on: 01.12.2008
 *      Author: Dirk Fahland
 *
 *  declarations of classes and functions for use outside of internal-representation.cc
 */


#ifndef UMLPUBLIC_H_
#define UMLPUBLIC_H_

class FlowContentElement;
class FlowContentConnection;
class Pin;
class PinCombination;
class InputCriterion;
class OutputCriterion;
class FlowContentNode;
class Task;
class Process;
class SimpleTask;
class Decision;
class Fork;
class Join;
class Merge;
class AtomicCFN;

class UmlProcessStatistics;


// flags to set the characteristics of the process (for filtering)
#define UML_STANDARD 0
#define UML_OVERLAPPING_PINS 1
#define UML_PIN_MULTIPLICITIES 2
#define UML_EMPTY_PROCESS 4

#endif /* UMLPUBLIC_H_ */
