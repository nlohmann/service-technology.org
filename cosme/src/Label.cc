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

#include "Label.h"
#include "util.h"

Label::~Label() {
    this->mID2Name.clear();
    this->mName2ID.clear();
}

Label::Label() {
    this->mCurLabelID = -1;
}

label_id_t Label::mapLabel(string label) {
    map<string, label_id_t>::iterator it;
    it = this->mName2ID.find(label);

    if (it == this->mName2ID.end()) {
        // sanity check
        string test(label);
        test[0] = (test[0] == '?') ? '!' : '?';
        if (this->mName2ID.find(test) != this->mName2ID.end()) {
            abort(3, "%s is used as input and output-interface", label.c_str());
        }

        this->nextLabelID();
        this->mID2Name.insert(make_pair(this->mCurLabelID, label));
        this->mName2ID.insert(make_pair(label, this->mCurLabelID));
        return this->mCurLabelID;
    }
    return it->second;
}

std::string Label::toString() const {
    std::stringstream tmpStr;
    tmpStr << std::endl << "Interface:" << std::endl;
    //for (map<label_id_t, string>::const_iterator it = this->mID2Name.begin(); it != this->mID2Name.end(); ++it) {
    FOREACH(it, this->mID2Name) {
        tmpStr << " " << static_cast<unsigned int>(it->first) << ": " << it->second << std::endl;
    }

    return tmpStr.str();
}
