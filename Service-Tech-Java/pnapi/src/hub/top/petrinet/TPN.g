grammar TPN;

options {
     language = Java;
}

@header {
/*****************************************************************************\
 * Copyright (c) 2010. All rights reserved. Dirk Fahland. AGPL3.0
 * 
 * ServiceTechnology.org - Petri Net API/Java
 * 
 * This program and the accompanying materials are made available under
 * the terms of the GNU Affero General Public License Version 3 or later,
 * which accompanies this distribution, and is available at 
 * http://www.gnu.org/licenses/agpl.txt
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 * 
\*****************************************************************************/

package hub.top.petrinet;
}

@members {

	private PetriNet 	net;
	
	private boolean		produceArcs;	
	private Transition  currentTransition;

    public static void main(String[] args) throws Exception {
        TPNLexer lex = new TPNLexer(new ANTLRFileStream(args[0]));
       	CommonTokenStream tokens = new CommonTokenStream(lex);

        TPNParser parser = new TPNParser(tokens);

        try {
            PetriNet result = parser.net();
            System.out.println(result.toDot());
            
        } catch (RecognitionException e)  {
            System.err.println(e.line+":"+e.charPositionInLine+" found "+TPNParser.tokenNames[e.getUnexpectedType()]+" expected "+e.token.getText());
            e.printStackTrace();
        }
    }
    
    private static String stripQuotes(String s) {
        return s.substring(1,s.length()-1);
    }
}

@rulecatch{
    catch (RecognitionException e){
            throw e;
      }
}

@lexer::header {
    package hub.top.petrinet;
}


/*------------------------------------------------------------------
 * PARSER RULES
 *------------------------------------------------------------------*/
 
net returns [PetriNet resultNet]: { net = new PetriNet(); }
	places
	transitions 
	{$resultNet = net; }
	;

node_name returns [String text]:
  STRING_LITERAL   { $text = $STRING_LITERAL.text;  }
| NUMBER  { $text = $NUMBER.text; }
;

/* ----------------- PLACES ----------------- */

places:
	place ( place )*
;

place:
	KEY_PLACE name1=node_name
	{
	  net.addPlace_unique(stripQuotes($name1.text));
	}
	(
		KEY_INIT NUMBER 
		{
			net.setTokens(stripQuotes($name1.text), Integer.parseInt($NUMBER.text));
		}
	)?
	SEMICOLON 
; 

/* ----------------- TRANSITION ----------------- */

transitions: 
  transition ( transition )*             
;

transition: 
  KEY_TRANSITION name1=node_name ( TILDE name2=node_name )?
  {
  	currentTransition = net.addTransition_unique(stripQuotes($name1.text));
  }
  KEY_IN
  {
    produceArcs = false; 
  } 
  arcs
  KEY_OUT 
  { 
    produceArcs = true;
  }
  arcs
  SEMICOLON
;

arcs:
	( arc )*
;

arc: 
  node_name
  {
	// FIXME: add arc weight if arc is already present
    Place p = net.findPlace(stripQuotes($node_name.text));
    if (p == null) System.err.println("ERROR: place "+$node_name.text+" not found.");
    else {
    	if (produceArcs)
    		net.addArc(currentTransition, p);
    	else
    		net.addArc(p, currentTransition);
    }
  }
;
 
 /*------------------------------------------------------------------
 * LEXER RULES
 *------------------------------------------------------------------*/

COMMENT_CONTENTS :
	'--'
	{
    	$channel=98;
    }
    ( ~( '\n'|'\r')  )* '\n' ;


KEY_PLACE : 'place' ;
KEY_INIT : 'init' ;
KEY_TRANSITION  : 'trans' ;
KEY_IN  : 'in' ;
KEY_OUT  : 'out' ;

SEMICOLON : ';' ;
TILDE : '~' ;

fragment DIGIT	: '0'..'9' ;
NUMBER	: (DIGIT)+ ;

STRING_LITERAL
  : '"'
    ( ~('"'|'\n'|'\r') )*
    '"'
   ;


WHITESPACE : ( '\t' | ' ' | '\r' | '\n'| '\u000C' )+ 	{ $channel = HIDDEN; } ;
