#include "main.h"

/// Filename of input file.
char* filename = NULL;


int yyerror(const char* msg)
{
  // display passed error message
  fprintf(stderr, "Error in '%s' in line %d:\n", filename, yylineno);
  fprintf(stderr, "  token/text last read was '%s'\n\n", yytext);

  // close input file
  fclose(yyin);


  fprintf(stderr, "-------------------------------------------------------------------------------\n");
  
  int firstShowedLine = ((yylineno-3)>0)?(yylineno-3):1;

  std::ifstream inputFile(filename);
  std::string errorLine;
  for (int i=0; i<firstShowedLine; i++)
    getline(inputFile, errorLine);
  
  // print the erroneous line (plus/minus three more)
  for (int i=firstShowedLine; i<=firstShowedLine+6; i++)
  {
    fprintf(stderr, "%d: %s\n", i, errorLine.c_str());
    getline(inputFile, errorLine);
    if (inputFile.eof())
      break;
  }
  
  fprintf(stderr, "-------------------------------------------------------------------------------\n");
  
  inputFile.close();
  
  
  exit(1);
}


int main( int argc, char *argv[])
{
  if (argc != 2)
    exit(2);

  // don't show debug messages from flex and Bison
  yydebug = 0;
  yy_flex_debug = 0;

  filename = argv[1];

  if (!(yyin = fopen(filename, "r")))
  {
    fprintf(stderr, "  File '%s' not found.\n", filename);
    exit(2);
  }

  fprintf(stderr, "Parsing '%s'...\n", filename);
  
  // invoke Bison parser
  int error = yyparse();

  if (!error)
  {
    fprintf(stderr, "Parsing complete.\n");
    TheProcess->print();
  }
  
  
  return error;
}
