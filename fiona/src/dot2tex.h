/* dot2tex.h */

#ifndef _DOT2TEX_H_
#define _DOT2TEX_H_

#define YY_NO_UNPUT

using namespace std;

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <climits>

const string a_list_label_str[2] = {"{", "}"};
const string a_list_pos_str[2] = {"(", ")"};
const string a_list_bb_str[2] = {"[", "]"};
const string node_stmt_str[2] = {"\t\\node(", ")"};
const string node_stmt2_str[3] = {"\t\\node[Nh=10, Nw=", ", Nmr=15](", ")"};
const string edge_stmt_str[3] = {"\t\\drawedge(", ",", ")"};

const int texHeaderCount = 5;
const string texHeader[texHeaderCount] = {
    "\\documentclass{article}\n", 
    "\\usepackage{gastex}\n\n", 

    "\\begin{document}\n\n", 

    "\\begin{center}\n",
    "\\begin{picture}(100, 100)(0, 0)\n"
};

const int texFooterCount = 3;
const string texFooter[texFooterCount] = {
    "\\end{picture}\n",
    "\\end{center}\n\n",

    "\\end{document}\n" 
};

const bool strip_command_sequence = true;

const double char_width_ratio = 1.5;
const int max_lines = 1024;
const double scale_factor = .5;

const int border_left = 10;
const int border_right = 10;
const int border_top = 10;
const int border_bottom = 10;

extern string texBuffer[max_lines];

extern int minx;
extern int maxx;
extern int miny;
extern int maxy;

#endif // _DOT2TEX_H_
