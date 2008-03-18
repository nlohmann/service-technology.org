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

class GasTexGraph;
extern GasTexGraph* gastexGraph;

string texformat(string);

const string node_stmt_str[7] = {"\t\\node[Nw=", ", Nmarks=", "](", ")(", ",", "){$\\mathsf{", "}$}"};
const string edge_stmt_str[4] = {"\t\\drawedge(", ",", "){$\\mathsf{", "}$}"};

const int texHeaderCount = 33;
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
    "  \\gasset{ilength=5,iangle=130}                     % arc to initial node\n",
    "  \\gasset{Nw=5.5,Nh=5.5}                            % node width, node height\n",
    "  \\gasset{Nmr=4.0}                                  % maximum radius of a node\n",
    "                                                     % (less than Nw/2 results in a rectangle)\n",
    "  \\gasset{AHangle=25,AHLength=2.7,AHlength=1.8}     % arrow shape\n",
    "  \\gasset{ELdist=1.5,ELside=r}                      % distance from edge to label, side of label\n",
    "  %\\gasset{ELpos=25}                                % position of edge label (0 = source, ..., 100 = des}\n",
    "\n",
    "  %\\drawedge[dash={2.0 2.0}{0.0},AHnb=0](n1,n2){ }  % a dashed line\n",
    "  %\\drawedge[\\curvedepth=3.0](n1,n2){ }             % a curved line\n",
    "\n",
    "  %\\node[Nfill=y,fillgray=0.8](p32)(112.0,-42.0){ } % a grey node\n",
    "\n",
    "  %\\node[Nw=6,Nh=6,Nmarks=r]                        % a final node\n",
     "\n"
};

const int texFooterCount = 3;
const string texFooter[texFooterCount] = {
    "\n\\end{picture}\n",
    "\\end{center}\n\n",

    "\\end{document}\n" 
};

const bool strip_command_sequence = true;
const float char_width_ratio = 30.0;        // ratio between length of node label and node width
const float scale_factor = .2;

const int border_left = 10;
const int border_right = 10;
const int border_top = 10;
const int border_bottom = 10;

#endif // _DOT2TEX_H_

