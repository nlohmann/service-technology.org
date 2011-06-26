grammar CLSC;

options {
     language = Java;
}

@header {
/*****************************************************************************\
 * SAM (Scenario-based Analysis Methods and tools) [AGPL3.0]
 * Copyright (c) 2011.
 *     AIS Group, Eindhoven University of Technology,
 *     service-technology.org,
 *     Prof. Harel research lab at the Weizmann Institute of Science   
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

package org.st.scenarios.clsc;

import java.util.HashMap;

}

@members {

	private Specification spec;
	private Scenario currentScen;
	private Chart currentChart;
	private HashMap<String, Event> eventMap;
	
    public static void main(String[] args) throws Exception {
        CLSCLexer lex = new CLSCLexer(new ANTLRFileStream(args[0]));
       	CommonTokenStream tokens = new CommonTokenStream(lex);

        CLSCParser parser = new CLSCParser(tokens);

        try {
            Specification result = parser.net();
            System.out.println(result);
            
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
    package org.st.scenarios.clsc;
}


/*------------------------------------------------------------------
 * PARSER RULES
 *------------------------------------------------------------------*/
 
net returns [Specification resultSpec]: { spec = new Specification(); }
	scenarios
	components
	{$resultSpec = spec; }
	;

node_name returns [String text]:
  IDENT   { $text = $IDENT.text;  }
| NUMBER  { $text = $NUMBER.text; }
;

/* --------------- SCENARIOS ---------------- */

scenarios:
	(
		KEY_CLSC name1=node_name
			{
				currentScen = new Scenario($name1.text, null, null);
				spec.scenarios.add(currentScen);
				currentChart = null;
			}
		(KEY_PRECHART chart
			{
				currentScen.preChart = currentChart;
			}
		)?
		KEY_MAINCHART chart
			{
				currentScen.mainChart = currentChart;
			}
	)+
;

/* ----------------- CHARTS ----------------- */

chart:
	{
		currentChart = new Chart();
		eventMap = new HashMap<String, Event>();
	}
	(
		KEY_EVENT eventId=node_name eventLabel=node_name SEMICOLON
		{
			Event e = currentChart.addEvent($eventLabel.text);
			eventMap.put($eventId.text, e);
		}
	)+
	KEY_DEPENDENCIES (dependency_list)? SEMICOLON
;
	
			

/* -------------- DEPENDENCIES -------------- */


dependency_list:
    name1=node_name DEPENDS name2=node_name
	  {
		if (!eventMap.containsKey($name1.text)) {
			System.err.println("Event with id '"+$name1.text+"' is not declared in Scenario '"+currentScen.getName()+"'");
		}
		if (!eventMap.containsKey($name2.text)) {
			System.err.println("Event with id '"+$name2.text+"' is not declared in Scenario '"+currentScen.getName()+"'");
		}
	    currentChart.addDependency(eventMap.get($name1.text), eventMap.get($name2.text));
	  }
  (	
  	COMMA name3=node_name DEPENDS name4=node_name
	  {
		if (!eventMap.containsKey($name3.text)) {
			System.err.println("Event with id '"+$name3.text+"' is not declared in Scenario '"+currentScen.getName()+"'");
		}
		if (!eventMap.containsKey($name4.text)) {
			System.err.println("Event with id '"+$name4.text+"' is not declared in Scenario '"+currentScen.getName()+"'");
		}
	    currentChart.addDependency(eventMap.get($name3.text), eventMap.get($name4.text));
	  }
  )*
;


/* --------------- SCENARIOS ---------------- */

components:
	(
		KEY_COMPONENT name1=node_name
		(name2=node_name COMMA
			{
				spec.eventToComponent.put(name2,name1);
			}
		)*
		name3=node_name
			{
				spec.eventToComponent.put(name3,name1);
			}
		SEMICOLON
	)*
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


KEY_CLSC : 'cLSC' ;
KEY_PRECHART : 'PRE' ;
KEY_MAINCHART : 'MAIN' ;
KEY_EVENT : 'EVENT' ;
KEY_DEPENDENCIES : 'DEPENDENCIES' ;
DEPENDS : '->' ;

KEY_COMPONENT : 'COMPONENT' ;

COLON : ':' ;
COMMA : ',' ;
SEMICOLON : ';' ;

fragment DIGIT	: '0'..'9' ;
NUMBER	: (DIGIT)+ ;
IDENT : ~( ',' | ';' | ':' | '(' | ')' | '\t' | ' ' | '\n' | '\r' | '\{' | '\}' )+ ;

WHITESPACE : ( '\t' | ' ' | '\r' | '\n'| '\u000C' )+ 	{ $channel = HIDDEN; } ;
