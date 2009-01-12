/*****************************************************************************\
 Rachel -- Repairing Automata for Choreographies by Editing Labels
 
 Copyright (C) 2008, 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>
 
 Rachel is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Rachel is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Rachel (see file COPYING); if not, see http://www.gnu.org/licenses or write to
 the Free Software Foundation,Inc., 51 Franklin Street, Fifth
 Floor, Boston, MA 02110-1301  USA.
\*****************************************************************************/

#include "Action.h"


/**************************************
 * implementation of member functions *
 **************************************/

/****************
 * class Action *
 ****************/

/// return a string representation of the action
std::string Action::toString() const {
    switch (type) {
        case(INSERT):
            return "insert " + label_new;
 
        case(DELETE):
            return "delete " + label_old;
        
        case(MODIFY):
            if (label_old != label_new) {
                return "modify " + label_old + " to " + label_new;
            } else {
                return "keep " + label_old;
            }
            
        case(NONE):
        case(KEEP):
        default:
            return "none";
    }
}


// constructor
Action::Action(action_type myType, Value myValue, Node myNodeA, Node myNodeB) : 
    type(myType),
    value(myValue),
    stateA(myNodeA),
    stateB(myNodeB),
    label_old(""),
    label_new("")
{
}


void Action::setType(const action_type a) {
    type = a;
}


action_type Action::getType() const {
    if ( (type == MODIFY) && (label_new == label_old) )
        return KEEP;
    
    return type;
}


/**********************
 * class ActionScript *
 **********************/

/// add an action to the script
void ActionScript::add(const Action &a) {
    value += a.value;
    script.push_back(a);
}


/// return a string representation of the script
std::string ActionScript::toString() const {
    std::string result;
    
    for (size_t i = 0; i < script.size(); ++i) {
        if (i != 0)
            result += ", ";
        result += script[i].toString();
    }
    
    return result;
}


/// constructor
ActionScript::ActionScript() :
    script(), value(0)
{
}


/// constructor to construct a singleton script
ActionScript::ActionScript(const Action &a) :
    script(), value(0)
{
    add(a);
}
