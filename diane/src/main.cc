/*****************************************************************************\
 Diane -- Decomposition of Petri nets.

 Copyright (C) 2009  Stephan Mennicke <stephan.mennicke@uni-rostock.de>

 Diane is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Diane is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Diane.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "cmdline.h"
#include "config.h"
#include "config-log.h"
#include "decomposition.h"
#include "pnapi/pnapi.h"
#include "verbose.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::map;
using std::ofstream;
using std::setfill;
using std::setw;
using std::string;
using std::stringstream;
using std::vector;
using pnapi::PetriNet;


/// statistics structure
struct Statistics {
  // runtime
  time_t t_total_;
  time_t t_computingComponents_;
  time_t t_buildingComponents_;

  // net statistics
  int netP_;
  int netT_;
  int netF_;

  // biggest fragment
  int largestNodes_;
  int largestP_;
  int largestT_;
  int largestF_;
  int largestPi_;
  int largestPo_;

  // average fragment
  // each must be devided by #fragments
  int averageP_;
  int averageT_;
  int averageF_;
  int averagePi_;
  int averagePo_;

  // #fragments
  int fragments_;
  // #trivial fragments
  int trivialFragments_;

  // clearing the statistics
  void clear()
  {
    t_total_ = t_computingComponents_ = t_buildingComponents_ = 0;
    netP_ = netT_ = netF_ = 0;
    largestP_ = largestT_ = largestF_ = largestPi_ = largestPo_ = 0;
    averageP_ = averageT_ = averageF_ = averagePi_ = averagePo_ = 0;
    fragments_ = trivialFragments_ = 0;
    largestNodes_ = 0;
  }
} _statistics;


/// the command line parameters
gengetopt_args_info args_info;

/// evaluate the command line parameters
void evaluateParameters(int, char **);
/// statistical output
void statistics(int, PetriNet &, vector<PetriNet *> &);


int main(int argc, char *argv[])
{
  evaluateParameters(argc, argv);
  // initial clearing
  _statistics.clear();

  string invocation;
  // store invocation in a string for meta information in file output
  for (int i = 0; i < argc; ++i) {
    invocation += string(argv[i]) + " ";
  }

  for (unsigned int i = 0; i < args_info.inputs_num; i++)
  {
    time_t start_total = time(NULL);
    /*
     * 1. Reading Petri net via PNAPI
     */
    PetriNet net;
    try
    {
      ifstream inputfile;
      inputfile.open(args_info.inputs[i]);
      if (inputfile.fail())
        abort(1, "could not open file '%s'", args_info.inputs[i]);
      switch (args_info.mode_arg)
      {
      case (mode_arg_standard):
        inputfile >> pnapi::io::lola >> net;
        break;
      case (mode_arg_freechoice):
        inputfile >> pnapi::io::owfn >> net;
        break;
      default: break;
      }
      inputfile.close();
    }
    catch (pnapi::io::InputError e)
    {
      abort(2, "could not parse file '%s': %s", args_info.inputs[i], e.message.c_str());
    }
    status("read file '%s' (%d of %d)", args_info.inputs[i], i+1, args_info.inputs_num);

    /*
     * 2. Determine the mode and compute the service parts.
     */
    int *tree = NULL;
    int size, psize;
    map<int, Node *> reremap;
    size = net.getNodes().size();
    psize = net.getPlaces().size();
    tree = new int [size];

    time_t start_computing = time(NULL);
    switch (args_info.mode_arg)
    {
    case (mode_arg_standard):
      _statistics.fragments_ = decomposition::computeComponentsByUnionFind(net,
              tree, size, psize, reremap);
      break;
    case (mode_arg_freechoice):
      _statistics.fragments_ = 0;
      break;
    default: break; /* do nothing */
    }
    time_t finish_computing = time(NULL);
    _statistics.t_computingComponents_ = finish_computing - start_computing;
    status("computed components of net '%s'", args_info.inputs[i]);

    /*
     * 3. Build Fragments.
     */
    vector<PetriNet *> nets(size);
    for (int j = 0; j < size; j++)
      nets[j] = NULL;

    // save start time of building
    time_t start_building = time(NULL);
    switch (args_info.mode_arg)
    {
    case (mode_arg_standard):
      decomposition::createOpenNetComponentsByUnionFind(nets, tree, size, psize, reremap);
      break;
    case (mode_arg_freechoice):
      break;
    default: break;
    }
    // save the finish time and make statistics
    time_t finish_building = time(NULL);
    _statistics.t_buildingComponents_ = finish_building - start_building;
    status("created net fragments of '%s'", args_info.inputs[i]);

    /*
     * 4. Write output files.
     */
    if (!args_info.quiet_flag)
    {
      // writing file output
      string fileprefix;
      string filepostfix = ".owfn";
      if (args_info.prefix_given)
        fileprefix += args_info.prefix_arg;
      fileprefix += args_info.inputs[i];

      int netnumber = 0;

      int digits;
      string digits_s;
      stringstream ds;
      ds << _statistics.fragments_;
      ds >> digits_s;
      digits = digits_s.length();

      for (int j = 0; j < (int) nets.size(); j++)
        if (nets[j] == NULL)
          continue;
        else
        {
          stringstream ss;
          string num;
          /// leading zeros
          int z = digits-num.length();
          ss << setfill('0') << setw(z) << netnumber;
          ss >> num;
          ofstream outputfile((fileprefix+num+filepostfix).c_str());
          outputfile << pnapi::io::owfn
              << meta(pnapi::io::INPUTFILE, args_info.inputs[i])
              << meta(pnapi::io::CREATOR, PACKAGE_STRING)
              << meta(pnapi::io::INVOCATION, invocation)
              << *nets[j];
          netnumber++;
        }
    }

    time_t finish_total = time(NULL);
    _statistics.t_total_ = finish_total - start_total;

    /*
     * 5. Make statistical output.
     */
    statistics(i, net, nets);

    // memory cleaner
    delete [] tree;
    _statistics.clear();
  }

  exit(EXIT_SUCCESS);
}


/*!
 * Evaluates gengetopt command-line parameters
 */
void evaluateParameters(int argc, char** argv) {
    // set default values
    cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    cmdline_parser(argc, argv, &args_info);

    // debug option
    if (args_info.bug_flag) {
        FILE *debug_output = fopen("bug.log", "w");
        fprintf(debug_output, "%s\n", CONFIG_LOG);
        fclose(debug_output);
        fprintf(stderr, "Please send file 'bug.log' to %s.\n", PACKAGE_BUGREPORT);
        exit(EXIT_SUCCESS);
    }

    free(params);
}


/*!
 * Provides statistical output.
 */
void statistics(int i, PetriNet &net, vector<PetriNet *> &nets)
{
  if (args_info.statistics_flag)
  {
    /*
     * 1. Net Statistics
     */
    _statistics.netP_ = (int) net.getPlaces().size();
    _statistics.netT_ = (int) net.getTransitions().size();
    _statistics.netF_ = (int) net.getArcs().size();
    /*
     * 2. Fragment Statistics
     */
    for (int j = 0; j < (int) nets.size(); j++)
    {
      if (nets[j] == NULL)
        continue;
      if (nets[j]->getNodes().size() > _statistics.largestNodes_)
      {
        _statistics.largestNodes_ = nets[j]->getNodes().size();
        _statistics.largestP_ = nets[j]->getPlaces().size();
        _statistics.largestT_ = nets[j]->getTransitions().size();
        _statistics.largestF_ = nets[j]->getArcs().size();
        _statistics.largestPi_ = nets[j]->getInputPlaces().size();
        _statistics.largestPo_ = nets[j]->getOutputPlaces().size();
      }

      if (nets[j]->getNodes().size()-nets[j]->getInterfacePlaces().size() == 1)
        _statistics.trivialFragments_++;

      _statistics.averageP_ += nets[j]->getPlaces().size();
      _statistics.averagePi_ += nets[j]->getInputPlaces().size();
      _statistics.averagePo_ += nets[j]->getOutputPlaces().size();
      _statistics.averageT_ += nets[j]->getTransitions().size();
      _statistics.averageF_ += nets[j]->getArcs().size();
    }
    /*
     * 3. Statistics Output
     */
    if (!args_info.csv_flag)
    {
      cout << "*******************************************************" << endl;
      cout << "* " << PACKAGE << ": " << args_info.inputs[i] << endl;
      cout << "* Statistics:" << endl;
      cout << "*******************************************************" << endl;
      cout << "* Petri net Statistics:" << endl;
      cout << "*   |P|= " << net.getPlaces().size()
           << " |T|= " << net.getTransitions().size()
           << " |F|= " << net.getArcs().size() << endl;
      cout << "*******************************************************" << endl;
      cout << "* Number of fragments:         " << _statistics.fragments_ << endl;
      cout << "* Number of trivial fragments: " << _statistics.trivialFragments_ << endl;
      cout << "* Largest fragment:" << endl;
      cout << "*   |P|= " << _statistics.largestP_
           << " |P_in|= " << _statistics.largestPi_
           << " |P_out|= " << _statistics.largestPo_
           << " |T|= " << _statistics.largestT_
           << " |F|= " << _statistics.largestF_ << endl;
      cout << "* Average fragment:" << endl;
      cout << "*   |P|= " << std::setprecision(2) << (float) _statistics.averageP_/_statistics.fragments_
           << " |P_in|= " << std::setprecision(2) << (float) _statistics.averagePi_/_statistics.fragments_
           << " |P_out|= " << std::setprecision(2) << (float) _statistics.averagePo_/_statistics.fragments_
           << " |T|= " << std::setprecision(2) << (float) _statistics.averageT_/_statistics.fragments_
           << " |F|= " << std::setprecision(2) << (float) _statistics.averageF_/_statistics.fragments_ << endl;
      cout << "*******************************************************" << endl;
      cout << "* Runtime:" << endl;
      cout << "*   Total:     " << _statistics.t_total_ << endl;
      cout << "*   Computing: " << _statistics.t_computingComponents_ << endl;
      cout << "*   Buildung:  " << _statistics.t_buildingComponents_ << endl;
      cout << "*******************************************************"
           << endl << endl;
    }
    else
    {
      /*
       * filename,|P| net,|T| net,|F| net,#fragments,#trivial fragments,
       * |P| lf, |P_in| lf, |P_out| lf, |T| lf, |F| lf,
       * |P| af, |P_in| af, |P_out| af, |T| af, |F| af,
       * total runtime, computing runtime, building runtime
       *
       * where lf = largest fragment, af = average fragment
       */
      cout << args_info.inputs[i] << ","
           << net.getPlaces().size() << ","
           << net.getTransitions().size() << ","
           << net.getArcs().size() << ","
           << _statistics.fragments_ << ","
           << _statistics.trivialFragments_ << ","
           << _statistics.largestP_ << ","
           << _statistics.largestPi_ << ","
           << _statistics.largestPo_ << ","
           << _statistics.largestT_ << ","
           << _statistics.largestF_ << ","
           << std::setprecision(2) << (float) _statistics.averageP_/_statistics.fragments_ << ","
           << std::setprecision(2) << (float) _statistics.averagePi_/_statistics.fragments_ << ","
           << std::setprecision(2) << (float) _statistics.averagePo_/_statistics.fragments_ << ","
           << std::setprecision(2) << (float) _statistics.averageT_/_statistics.fragments_ << ","
           << std::setprecision(2) << (float) _statistics.averageF_/_statistics.fragments_ << ","
           << _statistics.t_total_ << ","
           << _statistics.t_computingComponents_ << ","
           << _statistics.t_buildingComponents_
           << endl;
    }
  }
  else
  {
    cout << PACKAGE_NAME << ": " << args_info.inputs[i]
         << " - number of components: " << _statistics.fragments_
         << endl;
  }
}
