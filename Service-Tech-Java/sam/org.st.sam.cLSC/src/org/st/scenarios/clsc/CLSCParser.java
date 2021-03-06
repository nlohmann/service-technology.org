// $ANTLR 3.2 Sep 23, 2009 12:02:23 D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g 2012-06-08 09:16:31

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



import org.antlr.runtime.*;
import java.util.Stack;
import java.util.List;
import java.util.ArrayList;

public class CLSCParser extends Parser {
    public static final String[] tokenNames = new String[] {
        "<invalid>", "<EOR>", "<DOWN>", "<UP>", "IDENT", "NUMBER", "KEY_CLSC", "KEY_PRECHART", "KEY_MAINCHART", "KEY_EVENT", "SEMICOLON", "KEY_DEPENDENCIES", "DEPENDS", "COMMA", "KEY_COMPONENT", "COMMENT_CONTENTS", "COLON", "DIGIT", "WHITESPACE"
    };
    public static final int COLON=16;
    public static final int KEY_COMPONENT=14;
    public static final int COMMA=13;
    public static final int KEY_EVENT=9;
    public static final int NUMBER=5;
    public static final int WHITESPACE=18;
    public static final int IDENT=4;
    public static final int SEMICOLON=10;
    public static final int DEPENDS=12;
    public static final int DIGIT=17;
    public static final int COMMENT_CONTENTS=15;
    public static final int KEY_DEPENDENCIES=11;
    public static final int KEY_PRECHART=7;
    public static final int EOF=-1;
    public static final int KEY_CLSC=6;
    public static final int KEY_MAINCHART=8;

    // delegates
    // delegators


        public CLSCParser(TokenStream input) {
            this(input, new RecognizerSharedState());
        }
        public CLSCParser(TokenStream input, RecognizerSharedState state) {
            super(input, state);
             
        }
        

    public String[] getTokenNames() { return CLSCParser.tokenNames; }
    public String getGrammarFileName() { return "D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g"; }



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



    // $ANTLR start "net"
    // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:71:1: net returns [Specification resultSpec] : scenarios components ;
    public final Specification net() throws RecognitionException {
        Specification resultSpec = null;

        try {
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:71:39: ( scenarios components )
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:71:41: scenarios components
            {
             spec = new Specification(); 
            pushFollow(FOLLOW_scenarios_in_net64);
            scenarios();

            state._fsp--;

            pushFollow(FOLLOW_components_in_net67);
            components();

            state._fsp--;

            resultSpec = spec; 

            }

        }

            catch (RecognitionException e){
                    throw e;
              }
        finally {
        }
        return resultSpec;
    }
    // $ANTLR end "net"


    // $ANTLR start "node_name"
    // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:77:1: node_name returns [String text] : ( IDENT | NUMBER );
    public final String node_name() throws RecognitionException {
        String text = null;

        Token IDENT1=null;
        Token NUMBER2=null;

        try {
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:77:32: ( IDENT | NUMBER )
            int alt1=2;
            int LA1_0 = input.LA(1);

            if ( (LA1_0==IDENT) ) {
                alt1=1;
            }
            else if ( (LA1_0==NUMBER) ) {
                alt1=2;
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 1, 0, input);

                throw nvae;
            }
            switch (alt1) {
                case 1 :
                    // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:78:3: IDENT
                    {
                    IDENT1=(Token)match(input,IDENT,FOLLOW_IDENT_in_node_name85); 
                     text = (IDENT1!=null?IDENT1.getText():null);  

                    }
                    break;
                case 2 :
                    // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:79:3: NUMBER
                    {
                    NUMBER2=(Token)match(input,NUMBER,FOLLOW_NUMBER_in_node_name93); 
                     text = (NUMBER2!=null?NUMBER2.getText():null); 

                    }
                    break;

            }
        }

            catch (RecognitionException e){
                    throw e;
              }
        finally {
        }
        return text;
    }
    // $ANTLR end "node_name"


    // $ANTLR start "scenarios"
    // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:84:1: scenarios : ( KEY_CLSC name1= node_name ( KEY_PRECHART chart )? KEY_MAINCHART chart )+ ;
    public final void scenarios() throws RecognitionException {
        String name1 = null;


        try {
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:84:10: ( ( KEY_CLSC name1= node_name ( KEY_PRECHART chart )? KEY_MAINCHART chart )+ )
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:85:2: ( KEY_CLSC name1= node_name ( KEY_PRECHART chart )? KEY_MAINCHART chart )+
            {
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:85:2: ( KEY_CLSC name1= node_name ( KEY_PRECHART chart )? KEY_MAINCHART chart )+
            int cnt3=0;
            loop3:
            do {
                int alt3=2;
                int LA3_0 = input.LA(1);

                if ( (LA3_0==KEY_CLSC) ) {
                    alt3=1;
                }


                switch (alt3) {
            	case 1 :
            	    // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:86:3: KEY_CLSC name1= node_name ( KEY_PRECHART chart )? KEY_MAINCHART chart
            	    {
            	    match(input,KEY_CLSC,FOLLOW_KEY_CLSC_in_scenarios112); 
            	    pushFollow(FOLLOW_node_name_in_scenarios116);
            	    name1=node_name();

            	    state._fsp--;


            	    				currentScen = new Scenario(name1, null, null);
            	    				spec.scenarios.add(currentScen);
            	    				currentChart = null;
            	    			
            	    // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:92:3: ( KEY_PRECHART chart )?
            	    int alt2=2;
            	    int LA2_0 = input.LA(1);

            	    if ( (LA2_0==KEY_PRECHART) ) {
            	        alt2=1;
            	    }
            	    switch (alt2) {
            	        case 1 :
            	            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:92:4: KEY_PRECHART chart
            	            {
            	            match(input,KEY_PRECHART,FOLLOW_KEY_PRECHART_in_scenarios126); 
            	            pushFollow(FOLLOW_chart_in_scenarios128);
            	            chart();

            	            state._fsp--;


            	            				currentScen.preChart = currentChart;
            	            			

            	            }
            	            break;

            	    }

            	    match(input,KEY_MAINCHART,FOLLOW_KEY_MAINCHART_in_scenarios142); 
            	    pushFollow(FOLLOW_chart_in_scenarios144);
            	    chart();

            	    state._fsp--;


            	    				currentScen.mainChart = currentChart;
            	    			

            	    }
            	    break;

            	default :
            	    if ( cnt3 >= 1 ) break loop3;
                        EarlyExitException eee =
                            new EarlyExitException(3, input);
                        throw eee;
                }
                cnt3++;
            } while (true);


            }

        }

            catch (RecognitionException e){
                    throw e;
              }
        finally {
        }
        return ;
    }
    // $ANTLR end "scenarios"


    // $ANTLR start "chart"
    // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:106:1: chart : ( KEY_EVENT eventId= node_name eventLabel= node_name SEMICOLON )+ KEY_DEPENDENCIES ( dependency_list )? SEMICOLON ;
    public final void chart() throws RecognitionException {
        String eventId = null;

        String eventLabel = null;


        try {
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:106:6: ( ( KEY_EVENT eventId= node_name eventLabel= node_name SEMICOLON )+ KEY_DEPENDENCIES ( dependency_list )? SEMICOLON )
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:107:2: ( KEY_EVENT eventId= node_name eventLabel= node_name SEMICOLON )+ KEY_DEPENDENCIES ( dependency_list )? SEMICOLON
            {

            		currentChart = new Chart();
            		eventMap = new HashMap<String, Event>();
            	
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:111:2: ( KEY_EVENT eventId= node_name eventLabel= node_name SEMICOLON )+
            int cnt4=0;
            loop4:
            do {
                int alt4=2;
                int LA4_0 = input.LA(1);

                if ( (LA4_0==KEY_EVENT) ) {
                    alt4=1;
                }


                switch (alt4) {
            	case 1 :
            	    // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:112:3: KEY_EVENT eventId= node_name eventLabel= node_name SEMICOLON
            	    {
            	    match(input,KEY_EVENT,FOLLOW_KEY_EVENT_in_chart172); 
            	    pushFollow(FOLLOW_node_name_in_chart176);
            	    eventId=node_name();

            	    state._fsp--;

            	    pushFollow(FOLLOW_node_name_in_chart180);
            	    eventLabel=node_name();

            	    state._fsp--;

            	    match(input,SEMICOLON,FOLLOW_SEMICOLON_in_chart182); 

            	    			Event e = currentChart.addEvent(eventLabel);
            	    			eventMap.put(eventId, e);
            	    		

            	    }
            	    break;

            	default :
            	    if ( cnt4 >= 1 ) break loop4;
                        EarlyExitException eee =
                            new EarlyExitException(4, input);
                        throw eee;
                }
                cnt4++;
            } while (true);

            match(input,KEY_DEPENDENCIES,FOLLOW_KEY_DEPENDENCIES_in_chart193); 
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:118:19: ( dependency_list )?
            int alt5=2;
            int LA5_0 = input.LA(1);

            if ( ((LA5_0>=IDENT && LA5_0<=NUMBER)) ) {
                alt5=1;
            }
            switch (alt5) {
                case 1 :
                    // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:118:20: dependency_list
                    {
                    pushFollow(FOLLOW_dependency_list_in_chart196);
                    dependency_list();

                    state._fsp--;


                    }
                    break;

            }

            match(input,SEMICOLON,FOLLOW_SEMICOLON_in_chart200); 

            }

        }

            catch (RecognitionException e){
                    throw e;
              }
        finally {
        }
        return ;
    }
    // $ANTLR end "chart"


    // $ANTLR start "dependency_list"
    // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:126:1: dependency_list : name1= node_name DEPENDS name2= node_name ( COMMA name3= node_name DEPENDS name4= node_name )* ;
    public final void dependency_list() throws RecognitionException {
        String name1 = null;

        String name2 = null;

        String name3 = null;

        String name4 = null;


        try {
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:126:16: (name1= node_name DEPENDS name2= node_name ( COMMA name3= node_name DEPENDS name4= node_name )* )
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:127:5: name1= node_name DEPENDS name2= node_name ( COMMA name3= node_name DEPENDS name4= node_name )*
            {
            pushFollow(FOLLOW_node_name_in_dependency_list224);
            name1=node_name();

            state._fsp--;

            match(input,DEPENDS,FOLLOW_DEPENDS_in_dependency_list226); 
            pushFollow(FOLLOW_node_name_in_dependency_list230);
            name2=node_name();

            state._fsp--;


            		if (!eventMap.containsKey(name1)) {
            			System.err.println("Event with id '"+name1+"' is not declared in Scenario '"+currentScen.getName()+"'");
            		}
            		if (!eventMap.containsKey(name2)) {
            			System.err.println("Event with id '"+name2+"' is not declared in Scenario '"+currentScen.getName()+"'");
            		}
            	    currentChart.addDependency(eventMap.get(name1), eventMap.get(name2));
            	  
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:137:3: ( COMMA name3= node_name DEPENDS name4= node_name )*
            loop6:
            do {
                int alt6=2;
                int LA6_0 = input.LA(1);

                if ( (LA6_0==COMMA) ) {
                    alt6=1;
                }


                switch (alt6) {
            	case 1 :
            	    // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:138:4: COMMA name3= node_name DEPENDS name4= node_name
            	    {
            	    match(input,COMMA,FOLLOW_COMMA_in_dependency_list245); 
            	    pushFollow(FOLLOW_node_name_in_dependency_list249);
            	    name3=node_name();

            	    state._fsp--;

            	    match(input,DEPENDS,FOLLOW_DEPENDS_in_dependency_list251); 
            	    pushFollow(FOLLOW_node_name_in_dependency_list255);
            	    name4=node_name();

            	    state._fsp--;


            	    		if (!eventMap.containsKey(name3)) {
            	    			System.err.println("Event with id '"+name3+"' is not declared in Scenario '"+currentScen.getName()+"'");
            	    		}
            	    		if (!eventMap.containsKey(name4)) {
            	    			System.err.println("Event with id '"+name4+"' is not declared in Scenario '"+currentScen.getName()+"'");
            	    		}
            	    	    currentChart.addDependency(eventMap.get(name3), eventMap.get(name4));
            	    	  

            	    }
            	    break;

            	default :
            	    break loop6;
                }
            } while (true);


            }

        }

            catch (RecognitionException e){
                    throw e;
              }
        finally {
        }
        return ;
    }
    // $ANTLR end "dependency_list"


    // $ANTLR start "components"
    // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:154:1: components : ( KEY_COMPONENT name1= node_name (name2= node_name COMMA )* name3= node_name SEMICOLON )* ;
    public final void components() throws RecognitionException {
        String name1 = null;

        String name2 = null;

        String name3 = null;


        try {
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:154:11: ( ( KEY_COMPONENT name1= node_name (name2= node_name COMMA )* name3= node_name SEMICOLON )* )
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:155:2: ( KEY_COMPONENT name1= node_name (name2= node_name COMMA )* name3= node_name SEMICOLON )*
            {
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:155:2: ( KEY_COMPONENT name1= node_name (name2= node_name COMMA )* name3= node_name SEMICOLON )*
            loop8:
            do {
                int alt8=2;
                int LA8_0 = input.LA(1);

                if ( (LA8_0==KEY_COMPONENT) ) {
                    alt8=1;
                }


                switch (alt8) {
            	case 1 :
            	    // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:156:3: KEY_COMPONENT name1= node_name (name2= node_name COMMA )* name3= node_name SEMICOLON
            	    {
            	    match(input,KEY_COMPONENT,FOLLOW_KEY_COMPONENT_in_components282); 
            	    pushFollow(FOLLOW_node_name_in_components286);
            	    name1=node_name();

            	    state._fsp--;

            	    // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:157:3: (name2= node_name COMMA )*
            	    loop7:
            	    do {
            	        int alt7=2;
            	        int LA7_0 = input.LA(1);

            	        if ( (LA7_0==IDENT) ) {
            	            int LA7_1 = input.LA(2);

            	            if ( (LA7_1==COMMA) ) {
            	                alt7=1;
            	            }


            	        }
            	        else if ( (LA7_0==NUMBER) ) {
            	            int LA7_2 = input.LA(2);

            	            if ( (LA7_2==COMMA) ) {
            	                alt7=1;
            	            }


            	        }


            	        switch (alt7) {
            	    	case 1 :
            	    	    // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:157:4: name2= node_name COMMA
            	    	    {
            	    	    pushFollow(FOLLOW_node_name_in_components293);
            	    	    name2=node_name();

            	    	    state._fsp--;

            	    	    match(input,COMMA,FOLLOW_COMMA_in_components295); 

            	    	    				spec.eventToComponent.put(name2,name1);
            	    	    			

            	    	    }
            	    	    break;

            	    	default :
            	    	    break loop7;
            	        }
            	    } while (true);

            	    pushFollow(FOLLOW_node_name_in_components311);
            	    name3=node_name();

            	    state._fsp--;


            	    				spec.eventToComponent.put(name3,name1);
            	    			
            	    match(input,SEMICOLON,FOLLOW_SEMICOLON_in_components320); 

            	    }
            	    break;

            	default :
            	    break loop8;
                }
            } while (true);


            }

        }

            catch (RecognitionException e){
                    throw e;
              }
        finally {
        }
        return ;
    }
    // $ANTLR end "components"

    // Delegated rules


 

    public static final BitSet FOLLOW_scenarios_in_net64 = new BitSet(new long[]{0x0000000000004000L});
    public static final BitSet FOLLOW_components_in_net67 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_IDENT_in_node_name85 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_NUMBER_in_node_name93 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KEY_CLSC_in_scenarios112 = new BitSet(new long[]{0x0000000000000030L});
    public static final BitSet FOLLOW_node_name_in_scenarios116 = new BitSet(new long[]{0x0000000000000180L});
    public static final BitSet FOLLOW_KEY_PRECHART_in_scenarios126 = new BitSet(new long[]{0x0000000000000200L});
    public static final BitSet FOLLOW_chart_in_scenarios128 = new BitSet(new long[]{0x0000000000000100L});
    public static final BitSet FOLLOW_KEY_MAINCHART_in_scenarios142 = new BitSet(new long[]{0x0000000000000200L});
    public static final BitSet FOLLOW_chart_in_scenarios144 = new BitSet(new long[]{0x0000000000000042L});
    public static final BitSet FOLLOW_KEY_EVENT_in_chart172 = new BitSet(new long[]{0x0000000000000030L});
    public static final BitSet FOLLOW_node_name_in_chart176 = new BitSet(new long[]{0x0000000000000030L});
    public static final BitSet FOLLOW_node_name_in_chart180 = new BitSet(new long[]{0x0000000000000400L});
    public static final BitSet FOLLOW_SEMICOLON_in_chart182 = new BitSet(new long[]{0x0000000000000A00L});
    public static final BitSet FOLLOW_KEY_DEPENDENCIES_in_chart193 = new BitSet(new long[]{0x0000000000000430L});
    public static final BitSet FOLLOW_dependency_list_in_chart196 = new BitSet(new long[]{0x0000000000000400L});
    public static final BitSet FOLLOW_SEMICOLON_in_chart200 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_node_name_in_dependency_list224 = new BitSet(new long[]{0x0000000000001000L});
    public static final BitSet FOLLOW_DEPENDS_in_dependency_list226 = new BitSet(new long[]{0x0000000000000030L});
    public static final BitSet FOLLOW_node_name_in_dependency_list230 = new BitSet(new long[]{0x0000000000002002L});
    public static final BitSet FOLLOW_COMMA_in_dependency_list245 = new BitSet(new long[]{0x0000000000000030L});
    public static final BitSet FOLLOW_node_name_in_dependency_list249 = new BitSet(new long[]{0x0000000000001000L});
    public static final BitSet FOLLOW_DEPENDS_in_dependency_list251 = new BitSet(new long[]{0x0000000000000030L});
    public static final BitSet FOLLOW_node_name_in_dependency_list255 = new BitSet(new long[]{0x0000000000002002L});
    public static final BitSet FOLLOW_KEY_COMPONENT_in_components282 = new BitSet(new long[]{0x0000000000000030L});
    public static final BitSet FOLLOW_node_name_in_components286 = new BitSet(new long[]{0x0000000000000030L});
    public static final BitSet FOLLOW_node_name_in_components293 = new BitSet(new long[]{0x0000000000002000L});
    public static final BitSet FOLLOW_COMMA_in_components295 = new BitSet(new long[]{0x0000000000000030L});
    public static final BitSet FOLLOW_node_name_in_components311 = new BitSet(new long[]{0x0000000000000400L});
    public static final BitSet FOLLOW_SEMICOLON_in_components320 = new BitSet(new long[]{0x0000000000004002L});

}