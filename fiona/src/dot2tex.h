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
const string node_stmt2_str[3] = {"\t\\node[Nw=", "](", ")"};
const string edge_stmt_str[3] = {"\t\\drawedge(", ",", ")"};

const int texHeaderCount = 29;
const string texHeader[texHeaderCount] = {
    "\\documentclass{article}\n",
    "\\usepackage{gastex}\n",
    "\\usepackage[usenames]{color}% delete this line if you don't use colors\n",
    "\\pagestyle{empty}\n",
    "\n",
    "\\newcommand\\euro{{\\textsf{C}%\n",
    "  \\makebox[0pt][l]{\\kern-.73em\\mbox{--}}%\n",
    "  \\makebox[0pt][l]{\\kern-.71em\\raisebox{.25ex}{--}}}}\n",
    "\n",
    "\\begin{document}\n",
    "\n",
    "\\begin{center}\n",
    "\\begin{picture}(250, 0)(50, 200)\n",
    "%\\begin{picture}(oben-links)(unten-rechts)\n",
    "%\\begin{picture}(max-X, min-Y)(min-X, max-Y)\n",
    "%\\put(0,0){\\framebox(150, 200){}}\n",
    "\n",
    "  \\gasset{linewidth=0.3}                            % line width\n",
    "  \\gasset{ilength=7.65,iangle=129.47}               % arc to initial node\n",
    "  \\gasset{Nw=7.0,Nh=7.0}                            % node width, node height\n",
    "  \\gasset{Nmr=3.0}                                  %\n",
    "  \\gasset{AHangle=25,AHLength=3.5,AHlength=2}       % arrow shape\n",
    "  \\gasset{ELdist=1.5}                               % distance from edge to label\n",
    "  %\\gasset{ELpos=25}                                % position of edge label (0 = source, ..., 100 = des}\n",
    "\n",
    "  %\\drawedge[dash={2.0 2.0}{0.0},AHnb=0](n1,n2){ }  % a dashed line\n",
    "  %\\drawedge[\\curvedepth=3.0](n1,n2){ }             % a curved line\n",
    "  %\\node[Nfill=y,fillgray=0.8](p32)(112.0,-42.0){ } % a grey node\n",
    "\n"
};




const int texFooterCount = 3;
const string texFooter[texFooterCount] = {
    "\n\\end{picture}\n",
    "\\end{center}\n\n",

    "\\end{document}\n" 
};

const bool strip_command_sequence = true;

const double char_width_ratio = 1.0;  // ration between length of node label and node width
const int max_lines = 1024;
const double scale_factor = .2;

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
