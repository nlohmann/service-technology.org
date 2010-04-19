grammar OWFN;

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
	private Transition  currentTransition = null;

    public static void main(String[] args) throws Exception {
        OWFNLexer lex = new OWFNLexer(new ANTLRFileStream(args[0]));
       	CommonTokenStream tokens = new CommonTokenStream(lex);

        OWFNParser parser = new OWFNParser(tokens);

        try {
            PetriNet result = parser.net();
            System.out.println(result.toDot());
            
        } catch (RecognitionException e)  {
            e.printStackTrace();
        }
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
	KEY_PLACE
		(roles)?
	KEY_INTERNAL
	  places
	KEY_INPUT
	  places
	KEY_OUTPUT
	  places

	KEY_INITIALMARKING marking_list
	KEY_FINALCONDITION final_condition 
	transitions 
	{$resultNet = net; }
	;

node_name returns [String text]:
  IDENT   { $text = $IDENT.text;  }
| NUMBER  { $text = $NUMBER.text; }
;

/* ----------------- ROLES ----------------- */

roles:
	KEY_ROLES
	  name1=node_name
        {
          if (currentTransition != null)
            currentTransition.addRole($name1.text);
          else
            net.addRole($name1.text);
        }
	(
	  COMMA name2=node_name
        {
          if (currentTransition != null)
            currentTransition.addRole($name2.text);
          else
            net.addRole($name2.text);
        }
	)*
	SEMICOLON
;

/* ----------------- PLACES ----------------- */

places:
	(
		capacity place_list SEMICOLON { /*capacity_ = 0;*/ } 
	)+
;
 
capacity:
  /* empty */
| KEY_SAFE COLON        { /*capacity_ = 1;*/ }
| KEY_SAFE NUMBER COLON { /*capacity_ = $2;*/ }
;

place_list:
 (
    name1=node_name
	  {/*
	    check(places_[nodeName_.str()] == NULL, "node name already used");
	    places_[nodeName_.str()] = &pnapi_lola_yynet.createPlace(nodeName_.str(), 0, capacity_);
	    */
	    net.addPlace_unique($name1.text);
	  }
  (	
  	COMMA name2=node_name
	  {/*
	    check(places_[nodeName_.str()] == NULL, "node name already used");
	    places_[nodeName_.str()] = &pnapi_lola_yynet.createPlace(nodeName_.str(), 0, capacity_);
	    */
	    net.addPlace_unique($name2.text);
	  }
  )*
 )?
;

/* ----------------- MARKING ----------------- */

marking_list:
  ( marking ( COMMA marking )* )? SEMICOLON
;

marking:
  node_name 
  {
    net.setTokens($node_name.text, 1);
  }
 | 
  node_name COLON NUMBER 
  {
    net.setTokens($node_name.text, Integer.parseInt($NUMBER.text));
  }
;

/* ----------------- FINALCONDITION ----------------- */

final_condition:
	KEY_TRUE SEMICOLON
;

/* ----------------- TRANSITION ----------------- */

transitions: 
  ( transition )*             
;

transition: 
  KEY_TRANSITION node_name 
  {
  	currentTransition = net.addTransition_unique($node_name.text);
  	/* 
    check(!pnapi_lola_yynet.containsNode(nodeName_.str()), "node name already used");
    transition_ = &pnapi_lola_yynet.createTransition(nodeName_.str());
    */
  }
  (roles)?
  KEY_CONSUME
  {
  	/*
    target_ = reinterpret_cast<Node * *>(&transition_);
    source_ = reinterpret_cast<Node * *>(&place_);
    */
    produceArcs = false; 
  } 
  arcs
  KEY_PRODUCE 
  { 
  	/*
    source_ = reinterpret_cast<Node * *>(&transition_);
    target_ = reinterpret_cast<Node * *>(&place_);
    */
    produceArcs = true;
  }
  arcs
;

arcs:
	( arc (COMMA arc)* )? SEMICOLON
;

arc:
	  node_name 
	  {
		// FIXME: add arc weight if arc is already present
	    Place p = net.findPlace($node_name.text);
	    if (p == null) System.err.println("ERROR: place "+$node_name.text+" not found.");
	    else {
	    	if (produceArcs)
	    		net.addArc(currentTransition, p);
	    	else
	    		net.addArc(p, currentTransition);
	    }
	  }
	|
	  node_name COLON NUMBER 
	  {
		// FIXME: add arc weight if arc is already present
	    Place p = net.findPlace($node_name.text);
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
	'\{'
	{
    	$channel=98;
    }
    ( ~('\}') )* '\}' ;


KEY_PLACE : 'PLACE' ;
KEY_ROLES : 'ROLES' ;
KEY_INTERNAL : 'INTERNAL' ;
KEY_INPUT : 'INPUT' ;
KEY_OUTPUT : 'OUTPUT' ;
KEY_SAFE : 'SAFE' ;
KEY_INITIALMARKING  : 'INITIALMARKING' ;
KEY_TRANSITION  : 'TRANSITION' ;
KEY_CONSUME  : 'CONSUME' ;
KEY_PRODUCE  : 'PRODUCE' ;

KEY_FINALCONDITION  : 'FINALCONDITION' ;
KEY_TRUE : 'TRUE' ;

COLON : ':' ;
COMMA : ',' ;
SEMICOLON : ';' ;

fragment DIGIT	: '0'..'9' ;
NUMBER	: (DIGIT)+ ;
IDENT : ~( ',' | ';' | ':' | '(' | ')' | '\t' | ' ' | '\n' | '\r' | '\{' | '\}' )+ ;

WHITESPACE : ( '\t' | ' ' | '\r' | '\n'| '\u000C' )+ 	{ $channel = HIDDEN; } ;
