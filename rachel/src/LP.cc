/*****************************************************************************\
 Rachel -- Repairing Automata for Choreographies by Editing Labels
 
 Copyright (C) 2008  Niels Lohmann <niels.lohmann@uni-rostock.de>
 
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

#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

#include "costfunction.h"
#include "LP.h"
#include "helpers.h"
//#include "lp_solve/lp_lib.h"

using std::endl;
using std::string;
using std::vector;
using std::sort;
using std::ofstream;



/*!
 * \brief generates variables and constraints to calculate simulation
 *
 * \note This function was derived from LpGen.java, provided by Oleg Sokolsky.
 */
void LP::lp_gen(Graph g) {

    // containers to collect the variables and constraints of the problem
    vector<string> vars;
    vector<string> constraints;
    
    for (unsigned int niter1 = 0; niter1 != g.nodes.size(); niter1++) {
        Node id1 = g.nodes[niter1];
        unsigned int tCount1 = g.outEdges(g.nodes[niter1]).size();
        
        for (unsigned int niter2 = 0; niter2 != g.nodes.size(); niter2++) {
            Node id2 = g.nodes[niter2];
            unsigned int tCount2 = g.outEdges(g.nodes[niter1]).size();
        
            // create a q-variable and a bound constraint for the current state pair
            string qVar = "q_" + toString(id1) + "_" + toString(id2);
            string qBound = "0 <= " + qVar + " <= 1;";
            vars.push_back( qVar );
            constraints.push_back( qBound );
            
            string qConstr;
            
            // case 1: node 1 has successors
            if ( !g.outEdges(id1).empty() ) {
                
                qConstr = qVar + " = " + toString(discount() * N(id1, id2));

                // traverse node 1's edges
                for (unsigned int eiter1 = 0; eiter1 < g.outEdges(id1).size(); eiter1++) {
                    Node target1 = g.outEdges(id1)[eiter1].target;
                    
                    // node 1 and node 2 have successors
                    if (tCount2 > 0)
                    {
                        // create an x-variable and a bound constraint for the current edge
                        string xVar = "x_" + toString(id1) + "_" + toString(id2) + "_" + toString(target1);
                        string xBound = "0 <= " + xVar + " <= 1;";
                        vars.push_back( xVar );
                        constraints.push_back( xBound );

                        // add the x-variable to the current constraint
                        qConstr += " + " + toString( (1-discount())/(g.outEdges(id1).size()) ) + " " + xVar;
                    }
                    
                    // traverse node 2's edges
                    for (unsigned int eiter2 = 0; eiter2 < g.outEdges(id2).size(); eiter2++) {
                        Node target2 = g.outEdges(id2)[eiter2].target;
                        
                        // the similarity between the edge labels
                        Value qn = L( g.outEdges(id1)[eiter1].label, g.outEdges(id2)[eiter2].label );
                        
                        if (qn != 0) {
                            string qTarget = "q_" + toString(target1) + "_" + toString(target2);
                            
                            // node 1 and node 2 have successors
                            if ( tCount1 > 1 && tCount2 > 1 ) {
                                // create constraint on x-variable
                                string xConstr = "x_" + toString(id1) + "_" + toString(id2) + "_" + toString(target1) + " >= ";
                                
                                // avoid multiplication by 1
                                if (qn != 1) {
                                    xConstr += toString(qn) + string(" ");
                                }
                                
                                xConstr += qTarget + string(";");
                                constraints.push_back(xConstr);
                            } else if ( tCount1 == 1 && tCount2 > 1 ) {
                                string xConstr = qVar + " >= " + toString(discount()*N(id1, id2)) + " + " + toString((1-discount())*qn) + " " + qTarget + ";";
                                constraints.push_back( xConstr );
                            } else
                                qConstr += " + " + toString( (1-discount())/(g.outEdges(id1).size())*qn ) + " " + qTarget;
                        }
                    }
                }
                
                qConstr += ";";
            } else {
                // case 2: node 1 has no successors
                qConstr = qVar + " = " + toString(N(id1, id2)) + ";";
            }
            
            
            if ( !(tCount1 == 1 && tCount2 > 1) ) {
                constraints.push_back( qConstr );
            } else {
                fprintf(stderr, "discarding constraint %s\n", qConstr.c_str());
            }
        }
    }
    

    
    // statistical output
    fprintf(stderr, "%u variables, %u constraints\n",
            static_cast<unsigned int>(vars.size()),
            static_cast<unsigned int>(constraints.size()));
    

    write_lp(vars, constraints);
    
//    calc_lp();
}


/*!
 * \brief writes lp problem, variables, and constraints to a file
 */
void LP::write_lp(const vector<string> & vars, const vector<string> & constraints) {
    ofstream lp_file;
    lp_file.open("problem.lp");
    if (!lp_file) {
        fprintf(stderr, "could not create file 'problem.lp'\n");
        exit( EXIT_FAILURE );
    }    
    
    
    // output target function
    lp_file << "min: ";
    unsigned int line_lenght = 0;
    for (unsigned int i = 0; i < vars.size(); i++) {       
        line_lenght += vars[i].length() + 3;
        if (line_lenght > 70) {
            lp_file << endl << "    ";
            line_lenght = 0;
        }
        
        if (i != 0)
            lp_file << " + ";
        lp_file << vars[i];        
    }
    lp_file << ";" << endl;
    
        
    // output constraints
    lp_file << endl;
    for (unsigned int i = 0; i < constraints.size(); i++) {
        lp_file << constraints[i] << endl;
    }
    
    lp_file.close();
}



/*!
 * \brief solves the linear programming problem with the lp_solve library
 *
 * \note  See http://lpsolve.sourceforge.net/5.5/lp_solveAPIreference.htm for
 *        a complete lp_solve API reference.
 */
/*
void LP::calc_lp() {
    // a pointer to the LP problem
    lprec *lp;

    // read LP problem
    lp = read_LP("problem.lp", CRITICAL, "test model");
    if(lp == NULL) {
        cerr << "Unable to read model 'problem.lp'" << endl;
        exit(EXIT_FAILURE);
    }
    
    // solve the LP problem
    int lp_result = solve(lp);
    
    // abort if no solution was found or problem is infeasible
    if (lp_result != OPTIMAL) {
        cerr << "no optimal solution was found!" << endl;
        return;
    }

    // the values of the variables
    REAL *ptr_var;
    
    // get the variable values and store them in ptr_var
    get_ptr_variables(lp, &ptr_var);

    
    //for (unsigned int i = 1; i <= get_Ncolumns(lp); i++) {    
    //    cerr << get_col_name(lp, i) << " = " << ptr_var[i-1] << endl;
    //}
    
    
    // remove the LP problem from memory
    delete_lp(lp);
    
    // delete the intermediate file
    remove("problem.lp");
}
*/
