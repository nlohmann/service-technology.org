/*****************************************************************************\
 Rachel -- Reparing Service Choreographies

 Copyright (c) 2008, 2009 Niels Lohmann

 Rachel is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Rachel is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Rachel.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#ifndef __ACTION_H
#define __ACTION_H

#include "types.h"


/// an edit action
class Action {
    private:
        /// the type of the action
        action_type type;

    public:
        /// set the type of the action (INSERT, DELETE, ...)
        void setType(const action_type t);

        /// get the type of the action
        action_type getType() const;

        /// value of the action
        Value value;

        Node stateA;
        Node stateB;
        Label label_old;
        Label label_new;

        /// return a string representation of the action
        std::string toString() const;

        // constructor
        Action(action_type myType, Value myValue,
               Node myNodeA = 0, Node myNodeB = 0);
};


/// a script of edit actions
class ActionScript {
    public:
        /// the vector of edit actions
        std::vector<Action> script;

        /// the aggregated value of the script
        Value value;

        /// add an action to the script
        void add(const Action& a);

        /// return a string representation of the script
        std::string toString() const;

        /// constructor
        ActionScript();

        /// constructor to construct a singleton script
        ActionScript(const Action& a);
};


#endif
