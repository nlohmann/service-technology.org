%token KEY_PEER KEY_NODES KEY_INITIAL KEY_FINAL KEY_IN KEY_OUT
%token SEMICOLON COMMA COLON ARROW CHI_LABEL
%token NUMBER NAME SYNCLABEL RCVLABEL SENDLABEL

%defines
%name-prefix="chor_"

%{
#include <string>
#include <map>

#define YYDEBUG 1
#define YYERROR_VERBOSE 1

/// the current NAME token as string
std::string NAME_token;

std::string current_peer;
unsigned int current_node;
std::map<std::string, std::string> output_channels;
std::map<std::string, std::string> input_channels;

bool input;

extern int chor_lex();
extern int chor_error(const char *);
%}

%union {
  unsigned int val;
}

%type <val> NUMBER

%%

chorography:
    { printf("digraph D {\n");
      printf("  graph [fontname=\"Helvetica\"];\n");
      printf("  node [fontname=\"Helvetica\"];\n");
      printf("  edge [fontname=\"Helvetica\"];\n");
      printf("  subgraph cluster0 {\n");
      printf("    label=\"collaboration\";\n");
      printf("    node [shape=square];\n");
      }
  peers
  { for (std::map<std::string, std::string>::const_iterator it = input_channels.begin(); it != input_channels.end(); ++it) {
      printf("    %s -> %s [label=\"%s\"]\n", it->second.c_str(), output_channels[it->first].c_str(), it->first.c_str());
  }
      printf("  }\n");
      printf("  subgraph cluster1 {\n");
      printf("    label=\"choreography\";\n");
      printf("    node [shape=circle];\n");
      printf("    INIT [label=\"\" height=0.01 width=0.01 style=invis];\n");
      }
  KEY_NODES nodes
    { printf("  }\n");
      printf("}\n"); }
;

peers:
  peer
| peers peer
;

peer:
  KEY_PEER NAME
    { current_peer = NAME_token;
      printf("    %s;\n", NAME_token.c_str()); }
  inputs
  outputs
;

inputs:
  /* empty */
| KEY_IN SEMICOLON
| KEY_IN { input = true; } label_list SEMICOLON
;

outputs:
  /* empty */
| KEY_OUT SEMICOLON
| KEY_OUT { input = false; } label_list SEMICOLON
;

label_list:
  NAME
    { if(input) {
        input_channels[NAME_token] = current_peer;
    } else {
        output_channels[NAME_token] = current_peer;
    }}
| label_list COMMA NAME
    { if(input) {
        input_channels[NAME_token] = current_peer;
    } else {
        output_channels[NAME_token] = current_peer;
    }}
;

nodes:
  node
| nodes node
;

node:
  NUMBER
  { current_node = $1;
    printf("    n%d [label=\"q%d\"];\n", $1, $1); }
  annotation successors
;

annotation:
  /* empty */
| COLON KEY_INITIAL
    { printf("    INIT -> n%d [minlen=0.5];\n", current_node); }
| COLON KEY_FINAL
    { printf("    n%d [peripheries=2];\n", current_node);}
| COLON KEY_INITIAL COMMA KEY_FINAL
    { printf("    INIT -> n%d [minlen=0.5, maxlen=0.5];\n", current_node);
      printf("    n%d [peripheries=2];\n", current_node); }
;

successors:
  /* empty */
| successors SENDLABEL ARROW NUMBER
    { printf("    n%d -> n%d [label=\"%s\"]\n", current_node, $4, NAME_token.c_str()); }
| successors RCVLABEL ARROW NUMBER
    { printf("    n%d -> n%d [label=\"%s\"]\n", current_node, $4, NAME_token.c_str()); }
| successors SYNCLABEL ARROW NUMBER
    { printf("    n%d -> n%d [label=\"%s\"]\n", current_node, $4, NAME_token.c_str()); }
| successors CHI_LABEL ARROW NUMBER
    { printf("    n%d -> n%d [label=<&#967;> style=dashed]\n", current_node, $4); }
;

%%

int main() {
    return chor_parse();
}