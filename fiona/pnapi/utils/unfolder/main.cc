#include <config.h>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>

extern int hlowfn_initial_parse();
extern FILE *hlowfn_initial_in;
extern FILE *hlowfn_initial_out;

extern int hlowfn_final_parse();
extern FILE *hlowfn_final_in;
extern FILE *hlowfn_final_out;

extern int llowfn_initial_parse();
extern FILE *llowfn_initial_in;
extern FILE *llowfn_initial_out;

extern int llowfn_final_parse();
extern FILE *llowfn_final_in;
extern FILE *llowfn_final_out;

FILE *pipe_out_initial;
FILE *pipe_out_final;

FILE *inputFile;

int main(int argc, char *argv[]) {
    if (argc == 2 && !strcmp(argv[1], "--version")) {
        printf("High level open net unfolder\n");
        printf("  part of %s\n\n", PACKAGE_STRING);
        return EXIT_SUCCESS;
    }

    if (argc == 2 && !strcmp(argv[1], "--help")) {
        printf("call %s < input.owfn\n", argv[0]);
        printf("  input.owfn : open net (always read from stdin)\n");
        return EXIT_SUCCESS;
    }

     /*
      The high-level net is being unfolded twice with LoLa - once using the
      initial marking, the second time using the final marking. Then the unfolded 
      nets are merged.
     */

    // write input from stdin to file
    int ch;
    inputFile = fopen("/tmp/input", "w");
    ch = getchar();
    while(ch != EOF){
        fputc(ch, inputFile);
        ch = getchar(); 
    }
    fclose(inputFile);
	
    
    // parse high-level net and pipe it to LoLA (initial marking)
    pipe_out_initial = popen("lola-statespace -nllnet1 2> /dev/null", "w");
    hlowfn_initial_in = fopen("/tmp/input", "r");
    hlowfn_initial_out = pipe_out_initial;
    hlowfn_initial_parse();
    pclose(pipe_out_initial);
    fclose(hlowfn_initial_in);
      
    // parse high-level net and pipe it to LoLA (final marking)
    pipe_out_final = popen("lola-statespace -nllnet2 2> /dev/null", "w");
    hlowfn_final_in = fopen("/tmp/input", "r");
    hlowfn_final_out = pipe_out_final;
    hlowfn_final_parse();
    pclose(pipe_out_final);
    fclose(hlowfn_final_in);
       
    // parse LoLA's output and pipe it to standard output (initial marking)
    llowfn_initial_out = fopen("net1", "w");
    llowfn_initial_in = fopen("llnet1", "r");
    llowfn_initial_parse();
    fclose(llowfn_initial_in);
    fclose(llowfn_initial_out);
    
    // parse LoLA's output and pipe it to standard output (final marking)
    llowfn_final_out = fopen("net2", "w");
    llowfn_final_in = fopen("llnet2", "r");
    llowfn_final_parse();
    fclose(llowfn_final_in);
    fclose(llowfn_final_out);
    
    // merge both nets
    std::ifstream ll1("net1"); 
    std::ifstream ll2("net2");
    std::string patternStr;
   
    // copy everything until the end of initial marking from first net
    getline(ll1, patternStr);
    while(patternStr.compare("INITIALMARKING") != 0){
        std::cout << patternStr << std::endl;
    	getline(ll1, patternStr);
    }
    std::cout << patternStr << std::endl;
    char c;
    do {
    	c = ll1.get();
        std::cout << c;
    } while (c != ';');
    std::cout << std::endl;

    // copy everything from final marking until the end of the second net
    getline(ll2, patternStr);
    while(patternStr.compare("FINALMARKING") != 0){
	getline(ll2, patternStr);
    }
    while(!ll2.eof()){
        std::cout << patternStr << std::endl;
	getline(ll2, patternStr);	
    }
  
    // clean up
    remove("llnet1");
    remove("llnet2");
    remove("llnet1.pnml");
    remove("llnet2.pnml");
    remove("net1");
    remove("net2");
    
   

    return EXIT_SUCCESS;
}
