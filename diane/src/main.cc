#include <fstream>
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
using std::string;
using std::stringstream;
using std::vector;

using pnapi::PetriNet;


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

    // check whether at most one file is given
    if (args_info.inputs_num > 1) {
        fprintf(stderr, "%s: at most one input file must be given -- aborting\n", PACKAGE);
        exit(EXIT_FAILURE);
    }
    free(params);
}


int main(int argc, char *argv[])
{
  evaluateParameters(argc, argv);

  // reading Petri net via API
  PetriNet net;
  try
  {
    if (!args_info.input_given)
      switch(args_info.mode_arg)
      {
      case (mode_arg_standard):
        cin >> pnapi::io::lola >> net;
        break;
      case (mode_arg_freechoice):
        cin >> pnapi::io::owfn >> net;
        break;
      default: break;
      }
    else
    {
      ifstream inputfile;
      inputfile.open(args_info.input_arg);
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
  }
  catch (pnapi::io::InputError e)
  {
    cerr << PACKAGE << e << endl;
    exit(EXIT_FAILURE);
  }

  /*!
   * Determine the mode and compute the service parts.
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
    n = unionfind::computeComponentsByUnionFind(net, tree, size, psize, reremap);
    break;
  case (mode_arg_freechoice):
    n = 0;
    break;
  default: break; /* do nothing */
  }

  /*!
   * Result output: number of components.
   */
  cout << "Number of components: " << n << endl;

  if (!args_info.quiet_flag)
  {
    vector<PetriNet *> nets(size);
    for (int i = 0; i < size; i++)
      nets[i] = NULL;

    switch (args_info.mode_arg)
    {
    case (mode_arg_standard):
      unionfind::createOpenNetComponentsByUnionFind(nets, tree, size, psize, reremap);
      break;
    case (mode_arg_freechoice):
      break;
    default: break;
    }

    // writing file output
    string fileprefix;
    string filepostfix = ".owfn";
    ofstream outputfiles[n];
    if (args_info.output_given)
      fileprefix += args_info.output_arg;
    else
      if (args_info.input_given)
      {
        fileprefix += args_info.input_arg;
        fileprefix += "_part";
      }
      else
        fileprefix += "netpart";

    int netnumber = 0;

    int digits, z;
    string digits_s, num, zs;
    stringstream ds;
    ds << n;
    ds >> digits_s;
    digits = digits_s.length();

    for (int i = 0; i < (int) nets.size(); i++)
      if (nets[i] == NULL)
        continue;
      else
      {
        stringstream ss;
        ss << netnumber;
        ss >> num;
        /// leading zeros
        z = digits-num.length();
        while (z-- > 0)
          zs += "0";

        outputfiles[netnumber].open((fileprefix+zs+num+filepostfix).c_str());
        outputfiles[netnumber] << pnapi::io::owfn << *nets[i];
        outputfiles[netnumber].close();
        netnumber++;
        zs.clear();
      }
  }

  // memory cleaner
  delete [] tree;

  return 0;
}
