/*****************************************************************************\
 Cosme -- Checking Simulation, Matching, and Equivalence

 Copyright (c) 2010 Andreas Lehmann

 Cosme is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Cosme is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Cosme.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#ifndef _LABEL_H
#define _LABEL_H

#include <config.h>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include "types.h"
#include "verbose.h"

using std::map;
using std::string;

class Label {
    public:

        Label();
        ~Label();

        label_id_t mapLabel(string label);
        inline label_index_t size() {
            return this->mCurLabelID + 1;
        }
        inline bool ID2Name(label_id_t id, string& label) {
            map<label_id_t, string>::iterator it;
            it = this->mID2Name.find(id);

            if (it == this->mID2Name.end()) {
                return false;
            }

            label = it->second;
            return true;
        }
        inline bool Name2ID(string label, label_id_t& id) {
            map<string, label_id_t>::iterator it;
            it = this->mName2ID.find(label);

            if (it == this->mName2ID.end()) {
                return false;
            }

            id = it->second;
            return true;
        }
        std::string toString() const;

    private:

        inline void nextLabelID() {
            this->mCurLabelID++;
        }

        label_id_t mCurLabelID;
        map<label_id_t, string> mID2Name;
        map<string, label_id_t> mName2ID;
};

#endif
