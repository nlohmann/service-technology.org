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
