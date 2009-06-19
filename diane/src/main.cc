#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
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
using std::set;
using std::string;
using std::stringstream;
using std::vector;

using pnapi::Arc;
using pnapi::Node;
using pnapi::PetriNet;
using pnapi::Place;
using pnapi::Transition;


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

  // read Petri net via API
  PetriNet net;
  try
  {
    if (!args_info.input_given)
      cin >> pnapi::io::lola >> net;
    else
    {
      ifstream inputfile;
      inputfile.open(args_info.input_arg);
      inputfile >> pnapi::io::lola >> net;
      inputfile.close();
    }
  }
  catch (pnapi::io::InputError e)
  {
    cerr << PACKAGE << e << endl;
  }

  // "size" of the Petri net
  int size = (int) net.getNodes().size();
  // size of the Petri net's place set
  int psize = (int) net.getPlaces().size();
  // the search tree of sets
  int *tree = new int [size];
  // map to remap the integers to the Petri net's nodes
  map<Node *, int> remap;
  map<int, Node *> reremap;

  // init MakeSet calls
  int q = 0;
  set<Place *> places = net.getPlaces();
  for (set<Place *>::iterator it = places.begin(); it != places.end(); it++)
  {
    remap[*it] = q;
    reremap[q] = *it;
    MakeSet(q, tree);
    q++;
  }
  assert(q == psize);
  set<Transition *> transitions = net.getTransitions();
  for (set<Transition *>::iterator it = transitions.begin(); it != transitions.end(); it++)
  {
    remap[*it] = q;
    reremap[q] = *it;
    MakeSet(q, tree);
    q++;
  }
  assert(q == size);

  // usage of rules 1 and 2
  for (set<Place *>::iterator p = places.begin(); p != places.end(); p++)
  {
    set<Node *> preset = (*p)->getPreset();
    set<Node *>::iterator first = preset.begin();
    int f = remap[*first];
    for (set<Node *>::iterator t = preset.begin(); t != preset.end(); t++)
    {
      Union(f, remap[*t], tree);
    }
    set<Node *> postset = (*p)->getPostset();
    first = postset.begin();
    f = remap[*first];
    for (set<Node *>::iterator t = postset.begin(); t != postset.end(); t++)
    {
      Union(f, remap[*t], tree);
    }
  }

  // usage of rule 3 until nothing changes
  bool hasChanged;
  do
  {
    hasChanged = false;
    for (set<Place *>::iterator p = places.begin(); p != places.end(); p++)
    {
      set<Node *> preset = (*p)->getPreset();
      set<Node *> postset = (*p)->getPostset();
      int proot = Find(remap[*p], tree);
      for (set<Node *>::iterator pt = preset.begin(); pt != preset.end(); pt++)
      {
        int ptroot = Find(remap[*pt], tree);
        if (proot == ptroot)
          continue;
        bool localChange = false;
        for (set<Node *>::iterator tp = postset.begin(); tp != postset.end(); tp++)
        {
          int tproot = Find(remap[*tp], tree);
          if (tproot == ptroot)
          {
            hasChanged = localChange = true;
            Union(tproot, proot, tree);
            break;
          }
        }
        if (localChange)
          break;
      }
    }
  } while (hasChanged);

  // output: number of components
  int n = 0;
  for (int i = 0; i < psize; i++)
    if (tree[i] < -1)
      n++;
  for (int i = psize; i < size; i++)
    if (tree[i] < 0)
      n++;
  cout << "Number of components: " << n << endl;

  if (!args_info.quiet_flag)
  {
    vector<PetriNet *> nets(size);
    for (int i = 0; i < size; i++)
      nets[i] = NULL;
    for (int i = 0; i < psize; i++)
    {
      if (tree[i] == -1)
        continue; // interface place found

      int x = Find(i, tree);
      if (nets[x] == NULL)
        nets[x] = new PetriNet();
      nets[x]->createPlace(reremap[i]->getName());
    }
    for (int i = psize; i < size; i++)
    {
      int x = Find(i, tree);
      if (nets[x] == NULL)
        nets[x] = new PetriNet();
      Transition *t = &nets[x]->createTransition(reremap[i]->getName());
      /// creating arcs and interface
      set<Arc *> preset = reremap[i]->getPresetArcs();
      for (set<Arc *>::iterator f = preset.begin(); f != preset.end(); f++)
      {
        Place *place = &(*f)->getPlace();
        Place *netPlace;
        netPlace = nets[x]->findPlace(place->getName());
        if (netPlace == NULL)
          netPlace = &nets[x]->createPlace(place->getName(), Node::INPUT);
        nets[x]->createArc(*netPlace, *t, (*f)->getWeight());
      }
      set<Arc *> postset = reremap[i]->getPostsetArcs();
      for (set<Arc *>::iterator f = postset.begin(); f != postset.end(); f++)
      {
        Place *place = &(*f)->getPlace();
        Place *netPlace = nets[x]->findPlace(place->getName());
        if (netPlace == NULL)
          netPlace = &nets[x]->createPlace(place->getName(), Node::OUTPUT);
        nets[x]->createArc(*t, *netPlace, (*f)->getWeight());
      }
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
    for (int i = 0; i < (int) nets.size(); i++)
      if (nets[i] == NULL)
        continue;
      else
      {
        string num;
        stringstream ss;
        ss << netnumber;
        ss >> num;
        outputfiles[netnumber].open((fileprefix+num+filepostfix).c_str());
        outputfiles[netnumber] << pnapi::io::owfn << *nets[i];
        outputfiles[netnumber].close();
        netnumber++;
      }
  }

  delete [] tree;

  return 0;
}
