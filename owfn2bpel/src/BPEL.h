/*
   oWFN2BPEL - translate open workflow net models into BPEL processes

   Copyright (C) 2008       Niels Lohmann <niels.lohmann@uni-rostock.de>
   Copyright (C) 2006, 2007 Jens Kleine <jkleine@informatik.hu-berlin.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/


#ifndef _CLASS_BPEL_H_
#define _CLASS_BPEL_H_

#include <fstream>
#include <string>

class Links;
class Branch;
class plists;

class BPEL {
    public:
        int activity;
        std::string name;
        Links* source;
        Links* target;
        Branch* branches;
        int link_is_or;
        BPEL* next;

        BPEL(int act, BPEL* ptr = NULL, std::string str = "");

        //branch functions
        std::string remove_fullstop(std::string str);
        void delete_lists();
        void add_branch();
        void out();
        void links_out(std::ofstream* bpel_file, int in = 0);
        void code(std::ofstream* bpel_file, plists* ins = NULL, plists* outs = NULL, int in = 0);
        void add_source();
        void add_target();
        void add_target(int i);
        BPEL* is_target();
};

#endif
