#include "config.h"
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <sstream>
#include <cstdarg>
//#include <libgen.h>
#include <fstream>
#include <string>

#include "cmdline.h"
#include "helpers.h"
#include "Output.h"
#include "verbose.h"
#include <cmath>

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



bool checkComposite(std::vector<int>* composite) {
  std::set<int> inputs, outputs;
  for (int i = 0; i < composite->size(); ++i) {
    for (int j = 0; j < YasminaLog::models[(*composite)[i]]->inputs.size(); ++j) {
      if (inputs.find(YasminaLog::models[(*composite)[i]]->inputs[j]) != inputs.end()) {
        return false;
      } else {
        inputs.insert(YasminaLog::models[(*composite)[i]]->inputs[j]);
      }
    }
    for (int j = 0; j < YasminaLog::models[(*composite)[i]]->outputs.size(); ++j) {

      if (outputs.find(YasminaLog::models[(*composite)[i]]->outputs[j]) != outputs.end()) {
        return false;
      } else {
        outputs.insert(YasminaLog::models[(*composite)[i]]->outputs[j]);

      }
    }
    
    
  }
  return true;
}

ServiceModel* createModelFromOpenNet(pnapi::PetriNet* net) {
  
  ServiceModel* result = new ServiceModel();
  
  // Get interface 
  std::set<pnapi::Label *> labels = net->getInterface().getAsynchronousLabels();
  std::map<pnapi::Label*, int> labelMap;
  for (std::set<pnapi::Label *>::iterator it =
	    labels.begin(); it != labels.end(); ++it) {
	    std::string* labelName = new std::string((*it)->getName());
      int id = Universe::addIdentifier(labelName);
      delete labelName;
      labelMap[*it] = id;
      if ((*it)->getType() == 1) {
        result->inputs.push_back(id);
      } else {
        result->outputs.push_back(id);
      }
  }
  // Add all transitions as identifiers and store their numIDs in a map.
  std::map<pnapi::Transition*, int> transitionMap;
  
  for (std::set<pnapi::Transition*>::iterator it =
				net->getTransitions().begin(); it
				!= net->getTransitions().end(); ++it) {
    transitionMap[(*it)] = Universe::addIdentifier(&(*it)->getName());
  }

  // Now we need final markings.

  status("Computing final markings...");
	std::pair<BinaryTree<pnapi::Place*,std::pair<int,int> >**,int> translated = translateFinalCondition(net);
	
	status("Computed %i final markings",translated.second);
	
	for (int i = 0; i < translated.second; ++i) {
    
    result->constraints.push_back(std::vector<Constraint*>());
    int current = result->constraints.size()-1;
    
    // For each place, add the constraints. 
  
    for (std::set<pnapi::Place*>::iterator it =
				  net->getPlaces().begin(); it
				  != net->getPlaces().end(); ++it) {
      
      BinaryTreeNode<pnapi::Place*,std::pair<int,int> >* node = translated.first[i]->find((*it));

      int op = node->value.first;
			int value = node->value.second - (*it)->getTokenCount();

      std::vector<int>* domain = new std::vector<int>();
      std::vector<int>* values = new std::vector<int>();            
      
      for (std::set<pnapi::Arc *>::iterator pIt =
					  (*it)->getPresetArcs().begin(); pIt
					  != (*it)->getPresetArcs().end(); ++pIt) {
            pnapi::Transition& t = (*pIt)->getTransition();
            domain->push_back(transitionMap[&t]);
            values->push_back((*pIt)->getWeight());
      }
      
      for (std::set<pnapi::Arc *>::iterator pIt =
					  (*it)->getPostsetArcs().begin(); pIt
					  != (*it)->getPostsetArcs().end(); ++pIt) {
            pnapi::Transition& t = (*pIt)->getTransition();
            int id = transitionMap[&t];
            bool set = false;
            for (int x = 0; x < domain->size(); ++x) {
              if (!set && (*domain)[x] == id) {
                (*values)[x] -= (*pIt)->getWeight();
              }
            }
            
            if (!set) {
              domain->push_back(transitionMap[&t]);
              values->push_back(-1*(*pIt)->getWeight());
            }
      }
      
      CustomConstraint* newConstraint = new CustomConstraint(domain,values,op,value);
      delete domain; delete values;
      result->constraints[current].push_back(newConstraint);
    }
    
    // for each label, add a constraint
    for (std::set<pnapi::Label *>::iterator it =
	    labels.begin(); it != labels.end(); ++it) {
      std::vector<int>* domain = new std::vector<int>();
      std::vector<int>* values = new std::vector<int>();            
      int id = labelMap[(*it)];
      domain->push_back(id);
      values->push_back(-1);      
      for (std::set<pnapi::Transition*>::iterator tIt = (*it)->getTransitions().begin(); tIt != (*it)->getTransitions().end(); ++tIt) {
        domain->push_back(transitionMap[*tIt]);
        values->push_back(1);
      }
      CustomConstraint* newConstraint = new CustomConstraint(domain,values,EQ,0);
      result->constraints[current].push_back(newConstraint);
    }
    
  }

  return result;


}



ServiceModel* parsedModel;


int main(int argc, char** argv) {
	bool dirty = false; // Setting this bit means that yasmina should exit with an exit code != 0, but should be further executed.



	/*
	* The Yasmina Workflow in short:
	* ----------------------------
	* Evaluate command line parameters

	*/

	// Start the stopwatch
	clock_t start_clock = clock();

	// Evaluate parameters
	evaluateParameters(argc, argv);
	Output::setTempfileTemplate(args_info.tmpfile_arg);
	Output::setKeepTempfiles(args_info.noClean_flag);

  message("Parsing models...");    
  for (int i = 0; i < args_info.fingerprint_given;	++i) {  
  extern FILE * fingerprint_yyin;
  
	if ( not (fingerprint_yyin = fopen(args_info.fingerprint_arg[i], "r")))
		{
			status("File not found: %s", args_info.fingerprint_arg[i]);
			exit(1);
		}
  
	int fingerprint_yyparse();
	fingerprint_yyparse();
	fclose(fingerprint_yyin);
	
	parsedModel->name = std::string(args_info.fingerprint_arg[i]);
	YasminaLog::models.push_back(parsedModel);
	parsedModel->alias = YasminaLog::models.size();
	
	message("M%i: %s", 	parsedModel->alias, parsedModel->name.c_str());
	
	}
	
for (int i = 0; i < args_info.net_given;	++i) {  
		// Parsing the open net, using the PNAPI
		status("Processing %s", args_info.net_arg[0]);
		pnapi::PetriNet* net = new pnapi::PetriNet;

		try {

			// parse either from standard input or from a given file
				
				std::ifstream inputStream;
				inputStream.open(args_info.net_arg[i]);
				inputStream >> pnapi::io::owfn >> *(net);
				inputStream.close();
			

			std::stringstream pnstats;
			pnstats << pnapi::io::stat << *(net);

			status("read net %s", pnstats.str().c_str());
		} catch (pnapi::exception::InputError error) {
			std::stringstream inputerror;
			inputerror << error;
			abort(3, "pnapi error %i", inputerror.str().c_str());
		}	
		
		ServiceModel* netModel = createModelFromOpenNet(net);
		netModel->alias = YasminaLog::models.size()+1;
		netModel->name = args_info.net_arg[i];
		YasminaLog::models.push_back(netModel);
  	message("M%i: %s", 	netModel->alias, netModel->name.c_str());
	
}	
	
	
	
	status("Nr of models parsed: %i.",YasminaLog::models.size()); 
  
  
  
  // Find all compositions
  
  message("Performing compatibility checks...");    
  int incompatibles = 0;
  int inconclusives = 0;
  int syntacticallyIncompatibles = 0;
  int lpcounter = 0;

  unsigned int upper = (unsigned int) pow(2.0,(double) YasminaLog::models.size());


  unsigned int bad = 4;
  std::vector<unsigned int> composites;
  
  for (unsigned int i = 3; i < upper; ++i) {
    if (i == bad) {
      bad *= 2; 
    } else {
      composites.push_back(i);
    }  
  }
  
	
  for (int i = 0; i < composites.size(); ++i) {
    bool first = true;
    std::vector<int>* currentComposite = new std::vector<int>();
    std::string currentCompositeString = "";
    for (int j = 0; j < YasminaLog::models.size(); ++j) {
      if (getBit(composites[i],j) == 1) {
        if (!first) {
          currentCompositeString += " (+) ";
        } else {
          first = false;
        }
        currentCompositeString += "M" + intToStr(YasminaLog::models[j]->alias);
        currentComposite->push_back(j);        
     }
    }  
  


    if (checkComposite(currentComposite)) {
      message(currentCompositeString.c_str());    
      int* counter = new int[currentComposite->size()];
      
      for (int x = 0; x < currentComposite->size(); ++x) {
         counter[x] = 0;    
      }
      
      std::vector<std::vector<int> > currentCompositeFinals;
      
      int z = 0;
      while (z < currentComposite->size()) {
        if (z == 0) {
          currentCompositeFinals.push_back(std::vector<int>());
          for (int x = 0; x < currentComposite->size(); ++x) {
            currentCompositeFinals[currentCompositeFinals.size()-1].push_back(counter[x]);
          }
        }

        ++counter[z];

        if (counter[z] >= YasminaLog::models[(*currentComposite)[z]]->constraints.size()) {
          counter[z] = 0;
          ++z;
        }
        else {
          z = 0;
        }      
        
        
      }

      delete[] counter;


      unsigned int incompatible = 0;
      for (int x = 0; x < currentCompositeFinals.size(); ++x) {
        ++lpcounter;
        lprec* lp = make_lp(0,10);
        set_verbose(lp, 0);
        set_add_rowmode(lp, TRUE);
        for (int y = 0; y < currentComposite->size(); ++y) {

          for (int xy = 0; xy < YasminaLog::models[(*currentComposite)[y]]->constraints[currentCompositeFinals[x][y]].size(); ++xy) {
            YasminaLog::models[(*currentComposite)[y]]->constraints[currentCompositeFinals[x][y]][xy]->addToLP(lp);
          } 
        }
        set_add_rowmode(lp, FALSE);

        if (solve(lp) == INFEASIBLE) {
          ++incompatible;        
        }
        delete_lp(lp);
      }
      
      if (incompatible == currentCompositeFinals.size()) { 
        message("    Composite incompatible."); 
        ++incompatibles; 
        YasminaLog::results.push_back(Result(currentComposite,1));
      } else { 
        message("    Check inconclusive."); 
        ++inconclusives;
        YasminaLog::results.push_back(Result(currentComposite,0)); 
      }   
    } else {
    status("Composite syntactically incompatible: %s", currentCompositeString.c_str());
      YasminaLog::results.push_back(Result(currentComposite,-1));
      ++syntacticallyIncompatibles;
    }	
  } 

	message("Overall statistics:");
	message("    %i incompatible composites ", incompatibles);
	message("    %i inconclusive results ", inconclusives);
  if ((!args_info.verbose_given) && (syntacticallyIncompatibles != 0)) {
  	message("   [%i composites with incompatible interfaces (enable -v for details)]", syntacticallyIncompatibles);
	} else {
		status("    %i composites with incompatible interfaces", syntacticallyIncompatibles);
	}	
	message("    %i integer linear programs solved ", lpcounter);
  
	if (args_info.output_given) {

	  {
		  Output out( std::string(args_info.output_arg), "Yasmina log file" );
		  out << (*YasminaLog::toString((args_info.list_undefined_given)));
	  }

	  if (std::string(args_info.output_arg) != std::string("-")) {
		  // Creation complete.
		  message("Output file %s created.", args_info.output_arg);
	  } else {
		  // Output complete.
		  message("Output written to standard out.");
	  }

  } else {
	  message("No output file produced, since no output target was given.");
	  message("To produce output to a file, use \"-o <path/to/file>\", to output to standard out, use \"-o-\". ");
  }
  	
	// Verbose output the result of the stop watch
	status("runtime: %.2f sec", ((double(clock()) - double(start_clock))
	/ CLOCKS_PER_SEC));
	if (args_info.verbose_given) {
		fprintf(stderr, "%s: memory consumption: ", PACKAGE);
		system(
		(std::string("ps | ") + TOOL_GREP + " " + PACKAGE + " | "
		+ TOOL_AWK
		+ " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
	}
	
	if (dirty) return 1;
	return 0;
	
}
