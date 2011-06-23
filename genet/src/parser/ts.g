
#header <<

#include <string>
#include <iostream>
#include <map>
#include "global.h"

>>

#token SL_COMMENT "\#(~[\n])*\n" << skip();>>
#token MODEL   "\.model"
#token STATE   "\.state"
#token GRAPH   "graph"
#token KINP    "\.inputs"
#token KOUT    "\.outputs"
#token KINT    "\.internal"
#token KDUM    "\.dummy"
#token IDEXT   "[a-zA-Z][a-zA-Z0-9\_\.]* { \. \_ [a-zA-Z0-9]+ }{[\+\-]}"
#token ID      "[a-zA-Z][a-zA-Z0-9]*"
#token SPACE   "[\ \n\r]" << skip(); >>


class TSParser {

<<  std::map <std::string, bool> estats; 	
>>

ats [Tts *ti] > [Tts *t]  :       
	 graph[ti] init_state[ti] end << $t = $ti;>>;


graph [Tts *ti]  :  model interface_decl STATE GRAPH graph_defs[ti];

model : MODEL IDEXT | ;

interface_decl: ( interface (IDEXT)+ ) *;

interface: KINP | KOUT | KDUM | KINT ;

graph_defs [Tts *ti]:  ( S1:IDEXT T:IDEXT S2:IDEXT <<  ti->insert(S1-> getText(),T-> getText(),S2-> getText());>> )* 
        ;

graph_line [Tts *ti]:  S1:IDEXT T:IDEXT S2:IDEXT 
				<<  ti->insert(S1-> getText(),T-> getText(),S2-> getText());
				>> ;				

init_state [Tts *ti] :     ".marking" "\{" S1:IDEXT "\}" 
				<< ti->initial = std::string(S1-> getText()); >>;


end     :       ".end" ;        

}
