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

#ifndef __RBTREESTACK_H
#define __RBTREESTACK_H

#include <config.h>
#include <limits>
#include <iostream>
#include <stdint.h>
#include <cstdlib>
#include <cstdio>
#include "types.h"

typedef struct rbts_node_key_t {
    og_service_index_t mIndexOne;
    og_service_index_t mIndexTwo;
} rbts_node_key_t;

typedef struct rbts_node_t {
    rbts_node_key_t mKey;
    unsigned mRed;
    rbts_node_t* mTreeLeft;
    rbts_node_t* mTreeRight;
    rbts_node_t* mTreeParent;
    rbts_node_t* mStackParent;
} rbts_node_t;

class RBTreeStack {
    public:
        RBTreeStack();
        ~RBTreeStack();

        bool add(og_service_index_t indexOne, og_service_index_t indexTwo);
        bool pop(og_service_index_t& indexOne, og_service_index_t& indexTwo);
        inline og_service_index_t size() const {
            return this->mSize;
        }

    private:
        inline void rightRotate(rbts_node_t* node);
        inline void leftRotate(rbts_node_t* node);
        inline bool addHelper(rbts_node_t* node);

        void deleteHelper(rbts_node_t* node);

    private:
        rbts_node_t* mRoot;
        rbts_node_t* mNil;
        rbts_node_t* mTop;
        og_service_index_t mSize;

};

#endif //__RBTREESTACK_H
