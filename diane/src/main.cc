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
#include "unionfind.h"

#include "pnapi/pnapi.h"

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

  // trivial fragments
  int trivialN_;

  // clearing the statistics
  void clear()
  {
    t_total_ = t_computingComponents_ = t_buildingComponents_ = 0;
    netP_ = netT_ = netF_ = 0;
    largestP_ = largestT_ = largestF_ = largestPi_ = largestPo_ = 0;
    averageP_ = averageT_ = averageF_ = averagePi_ = averagePo_ = 0;
    trivialN_ = 0;
    largestNodes_ = 0;
  }
} statistics;


/// the command line parameters
gengetopt_args_info args_info;

/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // set default values
    cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    cmdline_parser(argc, argv, &args_info);

    free(params);
}


int main(int argc, char *argv[])
{
  evaluateParameters(argc, argv);
  bool *exit_failure = new bool[args_info.inputs_num];
  statistics.clear();

  for (unsigned int i = 0; i < args_info.inputs_num; i++)
  {
    time_t start_total = time(NULL);
    exit_failure[i] = false;
    /*
     * 1. Reading Petri net via PNAPI
     */
    PetriNet net;
    try
    {
      ifstream inputfile;
      inputfile.open(args_info.inputs[i]);
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
      cerr << PACKAGE << e << endl;
      exit_failure[i] = true;
      continue;
    }

    /*
     * 2. Determine the mode and compute the service parts.
     */
    int *tree = NULL;
    int size, psize, n;
    map<int, Node *> reremap;
    size = net.getNodes().size();
    psize = net.getPlaces().size();
    tree = new int [size];

    switch (args_info.mode_arg)
    {
    case (mode_arg_standard):
    {
      time_t start_computing = time(NULL);
      n = unionfind::computeComponentsByUnionFind(net, tree, size, psize,
          reremap);
      time_t finish_computing = time(NULL);
      statistics.t_computingComponents_ = finish_computing - start_computing;
      break;
    }
    case (mode_arg_freechoice):
      n = 0;
      break;
    default: break; /* do nothing */
    }

    /*!
     * 3. Result output: number of components.
     */
    if (!args_info.verbose_given)
      cout << "Number of fragments: " << n << endl;

    /*
     * 4. Handle quiet and verbose flag.
     */
    if (!args_info.quiet_flag || args_info.verbose_given)
    {
      /*
       * 4.1 Create net fragments
       */
      vector<PetriNet *> nets(size);
      for (int j = 0; j < size; j++)
        nets[j] = NULL;

      switch (args_info.mode_arg)
      {
      case (mode_arg_standard):
      {
        time_t start_building = time(NULL);
        unionfind::createOpenNetComponentsByUnionFind(nets, tree, size, psize, reremap);
        time_t finish_building = time(NULL);
        statistics.t_buildingComponents_ = finish_building - start_building;
        break;
      }
      case (mode_arg_freechoice):
        break;
      default: break;
      }

      if (!args_info.quiet_flag)
      {
        // writing file output
        string fileprefix;
        string filepostfix = ".owfn";
        ofstream outputfiles[n];
        if (args_info.output_given)
          fileprefix += args_info.output_arg;
        fileprefix += args_info.inputs[i];

        int netnumber = 0;

        int digits;
        string digits_s;
        stringstream ds;
        ds << n;
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

            outputfiles[netnumber].open((fileprefix+num+filepostfix).c_str());
            outputfiles[netnumber] << pnapi::io::owfn << *nets[j];
            outputfiles[netnumber].close();
            netnumber++;
          }
      }

      time_t finish_total = time(NULL);
      statistics.t_total_ = finish_total - start_total;
      /*
       * 5. Creating Statistics
       */
      if (args_info.verbose_given)
      {
        /*
         * 5.1 Net Statistics
         */
        statistics.netP_ = (int) net.getPlaces().size();
        statistics.netT_ = (int) net.getTransitions().size();
        statistics.netF_ = (int) net.getArcs().size();
        /*
         * 5.2 Fragment Statistics
         */
        for (int j = 0; j < (int) nets.size(); j++)
        {
          if (nets[j] == NULL)
            continue;
          if (nets[j]->getNodes().size() > statistics.largestNodes_)
          {
            statistics.largestNodes_ = nets[j]->getNodes().size();
            statistics.largestP_ = nets[j]->getPlaces().size();
            statistics.largestT_ = nets[j]->getTransitions().size();
            statistics.largestF_ = nets[j]->getArcs().size();
            statistics.largestPi_ = nets[j]->getInputPlaces().size();
            statistics.largestPo_ = nets[j]->getOutputPlaces().size();
          }

          if (nets[j]->getNodes().size()-nets[j]->getInterfacePlaces().size() == 1)
            statistics.trivialN_++;

          statistics.averageP_ += nets[j]->getPlaces().size();
          statistics.averagePi_ += nets[j]->getInputPlaces().size();
          statistics.averagePo_ += nets[j]->getOutputPlaces().size();
          statistics.averageT_ += nets[j]->getTransitions().size();
          statistics.averageF_ += nets[j]->getArcs().size();
        }
        /*
         * 5.3 Statistics Output
         */
        if (!args_info.csv_flag)
        {
          cout << "*******************************************************" << endl;
          cout << "* " << PACKAGE_STRING << endl;
          cout << "* Statistics: " << args_info.inputs[i] << endl;
          cout << "*******************************************************" << endl;
          cout << "* Petri net Statistics:" << endl;
          cout << "*   |P|= " << net.getPlaces().size()
               << " |T|= " << net.getTransitions().size()
               << " |F|= " << net.getArcs().size() << endl;
          cout << "*******************************************************" << endl;
          cout << "* Number of fragments:         " << n << endl;
          cout << "* Number of trivial fragments: " << statistics.trivialN_ << endl;
          cout << "* Largest fragment:" << endl;
          cout << "*   |P|= " << statistics.largestP_
               << " |P_in|= " << statistics.largestPi_
               << " |P_out|= " << statistics.largestPo_
               << " |T|= " << statistics.largestT_
               << " |F|= " << statistics.largestF_ << endl;
          cout << "* Average fragment:" << endl;
          cout << "*   |P|= " << std::setprecision(2) << (float) statistics.averageP_/n
               << " |P_in|= " << std::setprecision(2) << (float) statistics.averagePi_/n
               << " |P_out|= " << std::setprecision(2) << (float) statistics.averagePo_/n
               << " |T|= " << std::setprecision(2) << (float) statistics.averageT_/n
               << " |F|= " << std::setprecision(2) << (float) statistics.averageF_/n << endl;
          cout << "*******************************************************" << endl;
          cout << "* Runtime:" << endl;
          cout << "*   Total:     " << statistics.t_total_ << endl;
          cout << "*   Computing: " << statistics.t_computingComponents_ << endl;
          cout << "*   Buildung:  " << statistics.t_buildingComponents_ << endl;
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
               << n << ","
               << statistics.trivialN_ << ","
               << statistics.largestP_ << ","
               << statistics.largestPi_ << ","
               << statistics.largestPo_ << ","
               << statistics.largestT_ << ","
               << statistics.largestF_ << ","
               << statistics.averageP_ << ","
               << statistics.averagePi_ << ","
               << statistics.averagePo_ << ","
               << statistics.averageT_ << ","
               << statistics.averageF_ << ","
               << statistics.t_total_ << ","
               << statistics.t_computingComponents_ << ","
               << statistics.t_buildingComponents_
               << endl;
        }
      }
    }

    // memory cleaner
    delete [] tree;
    statistics.clear();
  }

  for (int i = 0; i < args_info.inputs_num; i++)
    if (exit_failure[i])
      exit(EXIT_FAILURE);

  exit(EXIT_SUCCESS);
}
