// $ANTLR 3.2 Sep 23, 2009 12:02:23 D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g 2010-08-25 15:13:56

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


import org.antlr.runtime.*;
import java.util.Stack;
import java.util.List;
import java.util.ArrayList;

public class TPNParser extends Parser {
    public static final String[] tokenNames = new String[] {
        "<invalid>", "<EOR>", "<DOWN>", "<UP>", "STRING_LITERAL", "NUMBER", "KEY_PLACE", "KEY_INIT", "SEMICOLON", "KEY_TRANSITION", "TILDE", "KEY_IN", "KEY_OUT", "COMMENT_CONTENTS", "DIGIT", "WHITESPACE"
    };
    public static final int SEMICOLON=8;
    public static final int KEY_PLACE=6;
    public static final int EOF=-1;
    public static final int NUMBER=5;
    public static final int STRING_LITERAL=4;
    public static final int KEY_INIT=7;
    public static final int KEY_IN=11;
    public static final int KEY_TRANSITION=9;
    public static final int COMMENT_CONTENTS=13;
    public static final int DIGIT=14;
    public static final int KEY_OUT=12;
    public static final int TILDE=10;
    public static final int WHITESPACE=15;

    // delegates
    // delegators


        public TPNParser(TokenStream input) {
            this(input, new RecognizerSharedState());
        }
        public TPNParser(TokenStream input, RecognizerSharedState state) {
            super(input, state);
             
        }
        

    public String[] getTokenNames() { return TPNParser.tokenNames; }
    public String getGrammarFileName() { return "D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g"; }



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



    // $ANTLR start "net"
    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:71:1: net returns [PetriNet resultNet] : places transitions ;
    public final PetriNet net() throws RecognitionException {
        PetriNet resultNet = null;

        try {
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:71:33: ( places transitions )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:71:35: places transitions
            {
             net = new PetriNet(); 
            pushFollow(FOLLOW_places_in_net64);
            places();

            state._fsp--;

            pushFollow(FOLLOW_transitions_in_net67);
            transitions();

            state._fsp--;

            resultNet = net; 

            }

        }

            catch (RecognitionException e){
                    throw e;
              }
        finally {
        }
        return resultNet;
    }
    // $ANTLR end "net"


    // $ANTLR start "node_name"
    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:77:1: node_name returns [String text] : ( STRING_LITERAL | NUMBER );
    public final String node_name() throws RecognitionException {
        String text = null;

        Token STRING_LITERAL1=null;
        Token NUMBER2=null;

        try {
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:77:32: ( STRING_LITERAL | NUMBER )
            int alt1=2;
            int LA1_0 = input.LA(1);

            if ( (LA1_0==STRING_LITERAL) ) {
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
                    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:78:3: STRING_LITERAL
                    {
                    STRING_LITERAL1=(Token)match(input,STRING_LITERAL,FOLLOW_STRING_LITERAL_in_node_name86); 
                     text = (STRING_LITERAL1!=null?STRING_LITERAL1.getText():null);  

                    }
                    break;
                case 2 :
                    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:79:3: NUMBER
                    {
                    NUMBER2=(Token)match(input,NUMBER,FOLLOW_NUMBER_in_node_name94); 
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


    // $ANTLR start "places"
    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:84:1: places : place ( place )* ;
    public final void places() throws RecognitionException {
        try {
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:84:7: ( place ( place )* )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:85:2: place ( place )*
            {
            pushFollow(FOLLOW_place_in_places109);
            place();

            state._fsp--;

            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:85:8: ( place )*
            loop2:
            do {
                int alt2=2;
                int LA2_0 = input.LA(1);

                if ( (LA2_0==KEY_PLACE) ) {
                    alt2=1;
                }


                switch (alt2) {
            	case 1 :
            	    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:85:10: place
            	    {
            	    pushFollow(FOLLOW_place_in_places113);
            	    place();

            	    state._fsp--;


            	    }
            	    break;

            	default :
            	    break loop2;
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
    // $ANTLR end "places"


    // $ANTLR start "place"
    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:88:1: place : KEY_PLACE name1= node_name ( KEY_INIT NUMBER )? SEMICOLON ;
    public final void place() throws RecognitionException {
        Token NUMBER3=null;
        String name1 = null;


        try {
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:88:6: ( KEY_PLACE name1= node_name ( KEY_INIT NUMBER )? SEMICOLON )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:89:2: KEY_PLACE name1= node_name ( KEY_INIT NUMBER )? SEMICOLON
            {
            match(input,KEY_PLACE,FOLLOW_KEY_PLACE_in_place125); 
            pushFollow(FOLLOW_node_name_in_place129);
            name1=node_name();

            state._fsp--;


            	  net.addPlace_unique(stripQuotes(name1));
            	
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:93:2: ( KEY_INIT NUMBER )?
            int alt3=2;
            int LA3_0 = input.LA(1);

            if ( (LA3_0==KEY_INIT) ) {
                alt3=1;
            }
            switch (alt3) {
                case 1 :
                    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:94:3: KEY_INIT NUMBER
                    {
                    match(input,KEY_INIT,FOLLOW_KEY_INIT_in_place139); 
                    NUMBER3=(Token)match(input,NUMBER,FOLLOW_NUMBER_in_place141); 

                    			net.setTokens(stripQuotes(name1), Integer.parseInt((NUMBER3!=null?NUMBER3.getText():null)));
                    		

                    }
                    break;

            }

            match(input,SEMICOLON,FOLLOW_SEMICOLON_in_place153); 

            }

        }

            catch (RecognitionException e){
                    throw e;
              }
        finally {
        }
        return ;
    }
    // $ANTLR end "place"


    // $ANTLR start "transitions"
    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:104:1: transitions : transition ( transition )* ;
    public final void transitions() throws RecognitionException {
        try {
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:104:12: ( transition ( transition )* )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:105:3: transition ( transition )*
            {
            pushFollow(FOLLOW_transition_in_transitions169);
            transition();

            state._fsp--;

            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:105:14: ( transition )*
            loop4:
            do {
                int alt4=2;
                int LA4_0 = input.LA(1);

                if ( (LA4_0==KEY_TRANSITION) ) {
                    alt4=1;
                }


                switch (alt4) {
            	case 1 :
            	    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:105:16: transition
            	    {
            	    pushFollow(FOLLOW_transition_in_transitions173);
            	    transition();

            	    state._fsp--;


            	    }
            	    break;

            	default :
            	    break loop4;
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
    // $ANTLR end "transitions"


    // $ANTLR start "transition"
    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:108:1: transition : KEY_TRANSITION name1= node_name ( TILDE name2= node_name )? KEY_IN arcs KEY_OUT arcs SEMICOLON ;
    public final void transition() throws RecognitionException {
        String name1 = null;

        String name2 = null;


        try {
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:108:11: ( KEY_TRANSITION name1= node_name ( TILDE name2= node_name )? KEY_IN arcs KEY_OUT arcs SEMICOLON )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:109:3: KEY_TRANSITION name1= node_name ( TILDE name2= node_name )? KEY_IN arcs KEY_OUT arcs SEMICOLON
            {
            match(input,KEY_TRANSITION,FOLLOW_KEY_TRANSITION_in_transition200); 
            pushFollow(FOLLOW_node_name_in_transition204);
            name1=node_name();

            state._fsp--;

            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:109:34: ( TILDE name2= node_name )?
            int alt5=2;
            int LA5_0 = input.LA(1);

            if ( (LA5_0==TILDE) ) {
                alt5=1;
            }
            switch (alt5) {
                case 1 :
                    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:109:36: TILDE name2= node_name
                    {
                    match(input,TILDE,FOLLOW_TILDE_in_transition208); 
                    pushFollow(FOLLOW_node_name_in_transition212);
                    name2=node_name();

                    state._fsp--;


                    }
                    break;

            }


              	currentTransition = net.addTransition_unique(stripQuotes(name1));
              
            match(input,KEY_IN,FOLLOW_KEY_IN_in_transition223); 

                produceArcs = false; 
              
            pushFollow(FOLLOW_arcs_in_transition232);
            arcs();

            state._fsp--;

            match(input,KEY_OUT,FOLLOW_KEY_OUT_in_transition236); 
             
                produceArcs = true;
              
            pushFollow(FOLLOW_arcs_in_transition245);
            arcs();

            state._fsp--;

            match(input,SEMICOLON,FOLLOW_SEMICOLON_in_transition249); 

            }

        }

            catch (RecognitionException e){
                    throw e;
              }
        finally {
        }
        return ;
    }
    // $ANTLR end "transition"


    // $ANTLR start "arcs"
    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:126:1: arcs : ( arc )* ;
    public final void arcs() throws RecognitionException {
        try {
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:126:5: ( ( arc )* )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:127:2: ( arc )*
            {
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:127:2: ( arc )*
            loop6:
            do {
                int alt6=2;
                int LA6_0 = input.LA(1);

                if ( ((LA6_0>=STRING_LITERAL && LA6_0<=NUMBER)) ) {
                    alt6=1;
                }


                switch (alt6) {
            	case 1 :
            	    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:127:4: arc
            	    {
            	    pushFollow(FOLLOW_arc_in_arcs260);
            	    arc();

            	    state._fsp--;


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
    // $ANTLR end "arcs"


    // $ANTLR start "arc"
    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:130:1: arc : node_name ;
    public final void arc() throws RecognitionException {
        String node_name4 = null;


        try {
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:130:4: ( node_name )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:131:3: node_name
            {
            pushFollow(FOLLOW_node_name_in_arc274);
            node_name4=node_name();

            state._fsp--;


            	// FIXME: add arc weight if arc is already present
                Place p = net.findPlace(stripQuotes(node_name4));
                if (p == null) System.err.println("ERROR: place "+node_name4+" not found.");
                else {
                	if (produceArcs)
                		net.addArc(currentTransition, p);
                	else
                		net.addArc(p, currentTransition);
                }
              

            }

        }

            catch (RecognitionException e){
                    throw e;
              }
        finally {
        }
        return ;
    }
    // $ANTLR end "arc"

    // Delegated rules


 

    public static final BitSet FOLLOW_places_in_net64 = new BitSet(new long[]{0x0000000000000200L});
    public static final BitSet FOLLOW_transitions_in_net67 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_STRING_LITERAL_in_node_name86 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_NUMBER_in_node_name94 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_place_in_places109 = new BitSet(new long[]{0x0000000000000042L});
    public static final BitSet FOLLOW_place_in_places113 = new BitSet(new long[]{0x0000000000000042L});
    public static final BitSet FOLLOW_KEY_PLACE_in_place125 = new BitSet(new long[]{0x0000000000000030L});
    public static final BitSet FOLLOW_node_name_in_place129 = new BitSet(new long[]{0x0000000000000180L});
    public static final BitSet FOLLOW_KEY_INIT_in_place139 = new BitSet(new long[]{0x0000000000000020L});
    public static final BitSet FOLLOW_NUMBER_in_place141 = new BitSet(new long[]{0x0000000000000100L});
    public static final BitSet FOLLOW_SEMICOLON_in_place153 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_transition_in_transitions169 = new BitSet(new long[]{0x0000000000000202L});
    public static final BitSet FOLLOW_transition_in_transitions173 = new BitSet(new long[]{0x0000000000000202L});
    public static final BitSet FOLLOW_KEY_TRANSITION_in_transition200 = new BitSet(new long[]{0x0000000000000030L});
    public static final BitSet FOLLOW_node_name_in_transition204 = new BitSet(new long[]{0x0000000000000C00L});
    public static final BitSet FOLLOW_TILDE_in_transition208 = new BitSet(new long[]{0x0000000000000030L});
    public static final BitSet FOLLOW_node_name_in_transition212 = new BitSet(new long[]{0x0000000000000800L});
    public static final BitSet FOLLOW_KEY_IN_in_transition223 = new BitSet(new long[]{0x0000000000001030L});
    public static final BitSet FOLLOW_arcs_in_transition232 = new BitSet(new long[]{0x0000000000001000L});
    public static final BitSet FOLLOW_KEY_OUT_in_transition236 = new BitSet(new long[]{0x0000000000000130L});
    public static final BitSet FOLLOW_arcs_in_transition245 = new BitSet(new long[]{0x0000000000000100L});
    public static final BitSet FOLLOW_SEMICOLON_in_transition249 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_arc_in_arcs260 = new BitSet(new long[]{0x0000000000000032L});
    public static final BitSet FOLLOW_node_name_in_arc274 = new BitSet(new long[]{0x0000000000000002L});

}