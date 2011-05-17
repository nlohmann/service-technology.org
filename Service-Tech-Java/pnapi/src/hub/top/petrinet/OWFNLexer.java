// $ANTLR 3.2 Sep 23, 2009 12:02:23 src/hub/top/petrinet/OWFN.g 2011-05-17 10:06:52

    package hub.top.petrinet;


import org.antlr.runtime.*;
import java.util.Stack;
import java.util.List;
import java.util.ArrayList;

public class OWFNLexer extends Lexer {
    public static final int KEY_TRUE=23;
    public static final int KEY_INPUT=6;
    public static final int KEY_EMPTY=18;
    public static final int KEY_OUTPUT=7;
    public static final int NUMBER=11;
    public static final int WHITESPACE=25;
    public static final int SEMICOLON=14;
    public static final int COMMENT_CONTENTS=22;
    public static final int EOF=-1;
    public static final int COLON=16;
    public static final int KEY_ROLES=12;
    public static final int KEY_TRANSITION=19;
    public static final int COMMA=13;
    public static final int KEY_PRODUCE=21;
    public static final int KEY_SAFE=15;
    public static final int KEY_AND=17;
    public static final int KEY_INITIALMARKING=8;
    public static final int IDENT=10;
    public static final int KEY_FINALCONDITION=9;
    public static final int KEY_CONSUME=20;
    public static final int DIGIT=24;
    public static final int KEY_PLACE=4;
    public static final int KEY_INTERNAL=5;

    // delegates
    // delegators

    public OWFNLexer() {;} 
    public OWFNLexer(CharStream input) {
        this(input, new RecognizerSharedState());
    }
    public OWFNLexer(CharStream input, RecognizerSharedState state) {
        super(input,state);

    }
    public String getGrammarFileName() { return "src/hub/top/petrinet/OWFN.g"; }

    // $ANTLR start "COMMENT_CONTENTS"
    public final void mCOMMENT_CONTENTS() throws RecognitionException {
        try {
            int _type = COMMENT_CONTENTS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // src/hub/top/petrinet/OWFN.g:241:18: ( '\\{' (~ ( '\\}' ) )* '\\}' )
            // src/hub/top/petrinet/OWFN.g:242:2: '\\{' (~ ( '\\}' ) )* '\\}'
            {
            match('{'); 

                	_channel=98;
                
            // src/hub/top/petrinet/OWFN.g:246:5: (~ ( '\\}' ) )*
            loop1:
            do {
                int alt1=2;
                int LA1_0 = input.LA(1);

                if ( ((LA1_0>='\u0000' && LA1_0<='|')||(LA1_0>='~' && LA1_0<='\uFFFF')) ) {
                    alt1=1;
                }


                switch (alt1) {
            	case 1 :
            	    // src/hub/top/petrinet/OWFN.g:246:7: ~ ( '\\}' )
            	    {
            	    if ( (input.LA(1)>='\u0000' && input.LA(1)<='|')||(input.LA(1)>='~' && input.LA(1)<='\uFFFF') ) {
            	        input.consume();

            	    }
            	    else {
            	        MismatchedSetException mse = new MismatchedSetException(null,input);
            	        recover(mse);
            	        throw mse;}


            	    }
            	    break;

            	default :
            	    break loop1;
                }
            } while (true);

            match('}'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "COMMENT_CONTENTS"

    // $ANTLR start "KEY_PLACE"
    public final void mKEY_PLACE() throws RecognitionException {
        try {
            int _type = KEY_PLACE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // src/hub/top/petrinet/OWFN.g:249:11: ( 'PLACE' )
            // src/hub/top/petrinet/OWFN.g:249:13: 'PLACE'
            {
            match("PLACE"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_PLACE"

    // $ANTLR start "KEY_ROLES"
    public final void mKEY_ROLES() throws RecognitionException {
        try {
            int _type = KEY_ROLES;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // src/hub/top/petrinet/OWFN.g:250:11: ( 'ROLES' )
            // src/hub/top/petrinet/OWFN.g:250:13: 'ROLES'
            {
            match("ROLES"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_ROLES"

    // $ANTLR start "KEY_INTERNAL"
    public final void mKEY_INTERNAL() throws RecognitionException {
        try {
            int _type = KEY_INTERNAL;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // src/hub/top/petrinet/OWFN.g:251:14: ( 'INTERNAL' )
            // src/hub/top/petrinet/OWFN.g:251:16: 'INTERNAL'
            {
            match("INTERNAL"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_INTERNAL"

    // $ANTLR start "KEY_INPUT"
    public final void mKEY_INPUT() throws RecognitionException {
        try {
            int _type = KEY_INPUT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // src/hub/top/petrinet/OWFN.g:252:11: ( 'INPUT' )
            // src/hub/top/petrinet/OWFN.g:252:13: 'INPUT'
            {
            match("INPUT"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_INPUT"

    // $ANTLR start "KEY_OUTPUT"
    public final void mKEY_OUTPUT() throws RecognitionException {
        try {
            int _type = KEY_OUTPUT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // src/hub/top/petrinet/OWFN.g:253:12: ( 'OUTPUT' )
            // src/hub/top/petrinet/OWFN.g:253:14: 'OUTPUT'
            {
            match("OUTPUT"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_OUTPUT"

    // $ANTLR start "KEY_SAFE"
    public final void mKEY_SAFE() throws RecognitionException {
        try {
            int _type = KEY_SAFE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // src/hub/top/petrinet/OWFN.g:254:10: ( 'SAFE' )
            // src/hub/top/petrinet/OWFN.g:254:12: 'SAFE'
            {
            match("SAFE"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_SAFE"

    // $ANTLR start "KEY_INITIALMARKING"
    public final void mKEY_INITIALMARKING() throws RecognitionException {
        try {
            int _type = KEY_INITIALMARKING;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // src/hub/top/petrinet/OWFN.g:255:21: ( 'INITIALMARKING' )
            // src/hub/top/petrinet/OWFN.g:255:23: 'INITIALMARKING'
            {
            match("INITIALMARKING"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_INITIALMARKING"

    // $ANTLR start "KEY_TRANSITION"
    public final void mKEY_TRANSITION() throws RecognitionException {
        try {
            int _type = KEY_TRANSITION;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // src/hub/top/petrinet/OWFN.g:256:17: ( 'TRANSITION' )
            // src/hub/top/petrinet/OWFN.g:256:19: 'TRANSITION'
            {
            match("TRANSITION"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_TRANSITION"

    // $ANTLR start "KEY_CONSUME"
    public final void mKEY_CONSUME() throws RecognitionException {
        try {
            int _type = KEY_CONSUME;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // src/hub/top/petrinet/OWFN.g:257:14: ( 'CONSUME' )
            // src/hub/top/petrinet/OWFN.g:257:16: 'CONSUME'
            {
            match("CONSUME"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_CONSUME"

    // $ANTLR start "KEY_PRODUCE"
    public final void mKEY_PRODUCE() throws RecognitionException {
        try {
            int _type = KEY_PRODUCE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // src/hub/top/petrinet/OWFN.g:258:14: ( 'PRODUCE' )
            // src/hub/top/petrinet/OWFN.g:258:16: 'PRODUCE'
            {
            match("PRODUCE"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_PRODUCE"

    // $ANTLR start "KEY_FINALCONDITION"
    public final void mKEY_FINALCONDITION() throws RecognitionException {
        try {
            int _type = KEY_FINALCONDITION;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // src/hub/top/petrinet/OWFN.g:260:21: ( 'FINALCONDITION' )
            // src/hub/top/petrinet/OWFN.g:260:23: 'FINALCONDITION'
            {
            match("FINALCONDITION"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_FINALCONDITION"

    // $ANTLR start "KEY_TRUE"
    public final void mKEY_TRUE() throws RecognitionException {
        try {
            int _type = KEY_TRUE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // src/hub/top/petrinet/OWFN.g:261:10: ( 'TRUE' )
            // src/hub/top/petrinet/OWFN.g:261:12: 'TRUE'
            {
            match("TRUE"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_TRUE"

    // $ANTLR start "KEY_AND"
    public final void mKEY_AND() throws RecognitionException {
        try {
            int _type = KEY_AND;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // src/hub/top/petrinet/OWFN.g:262:9: ( 'AND' )
            // src/hub/top/petrinet/OWFN.g:262:11: 'AND'
            {
            match("AND"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_AND"

    // $ANTLR start "KEY_EMPTY"
    public final void mKEY_EMPTY() throws RecognitionException {
        try {
            int _type = KEY_EMPTY;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // src/hub/top/petrinet/OWFN.g:263:11: ( 'ALL_OTHER_PLACES_EMPTY' )
            // src/hub/top/petrinet/OWFN.g:263:13: 'ALL_OTHER_PLACES_EMPTY'
            {
            match("ALL_OTHER_PLACES_EMPTY"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_EMPTY"

    // $ANTLR start "COLON"
    public final void mCOLON() throws RecognitionException {
        try {
            int _type = COLON;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // src/hub/top/petrinet/OWFN.g:265:7: ( ':' )
            // src/hub/top/petrinet/OWFN.g:265:9: ':'
            {
            match(':'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "COLON"

    // $ANTLR start "COMMA"
    public final void mCOMMA() throws RecognitionException {
        try {
            int _type = COMMA;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // src/hub/top/petrinet/OWFN.g:266:7: ( ',' )
            // src/hub/top/petrinet/OWFN.g:266:9: ','
            {
            match(','); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "COMMA"

    // $ANTLR start "SEMICOLON"
    public final void mSEMICOLON() throws RecognitionException {
        try {
            int _type = SEMICOLON;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // src/hub/top/petrinet/OWFN.g:267:11: ( ';' )
            // src/hub/top/petrinet/OWFN.g:267:13: ';'
            {
            match(';'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "SEMICOLON"

    // $ANTLR start "DIGIT"
    public final void mDIGIT() throws RecognitionException {
        try {
            // src/hub/top/petrinet/OWFN.g:269:16: ( '0' .. '9' )
            // src/hub/top/petrinet/OWFN.g:269:18: '0' .. '9'
            {
            matchRange('0','9'); 

            }

        }
        finally {
        }
    }
    // $ANTLR end "DIGIT"

    // $ANTLR start "NUMBER"
    public final void mNUMBER() throws RecognitionException {
        try {
            int _type = NUMBER;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // src/hub/top/petrinet/OWFN.g:270:8: ( ( DIGIT )+ )
            // src/hub/top/petrinet/OWFN.g:270:10: ( DIGIT )+
            {
            // src/hub/top/petrinet/OWFN.g:270:10: ( DIGIT )+
            int cnt2=0;
            loop2:
            do {
                int alt2=2;
                int LA2_0 = input.LA(1);

                if ( ((LA2_0>='0' && LA2_0<='9')) ) {
                    alt2=1;
                }


                switch (alt2) {
            	case 1 :
            	    // src/hub/top/petrinet/OWFN.g:270:11: DIGIT
            	    {
            	    mDIGIT(); 

            	    }
            	    break;

            	default :
            	    if ( cnt2 >= 1 ) break loop2;
                        EarlyExitException eee =
                            new EarlyExitException(2, input);
                        throw eee;
                }
                cnt2++;
            } while (true);


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "NUMBER"

    // $ANTLR start "IDENT"
    public final void mIDENT() throws RecognitionException {
        try {
            int _type = IDENT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // src/hub/top/petrinet/OWFN.g:271:7: ( (~ ( ',' | ';' | ':' | '(' | ')' | '\\t' | ' ' | '\\n' | '\\r' | '\\{' | '\\}' ) )+ )
            // src/hub/top/petrinet/OWFN.g:271:9: (~ ( ',' | ';' | ':' | '(' | ')' | '\\t' | ' ' | '\\n' | '\\r' | '\\{' | '\\}' ) )+
            {
            // src/hub/top/petrinet/OWFN.g:271:9: (~ ( ',' | ';' | ':' | '(' | ')' | '\\t' | ' ' | '\\n' | '\\r' | '\\{' | '\\}' ) )+
            int cnt3=0;
            loop3:
            do {
                int alt3=2;
                int LA3_0 = input.LA(1);

                if ( ((LA3_0>='\u0000' && LA3_0<='\b')||(LA3_0>='\u000B' && LA3_0<='\f')||(LA3_0>='\u000E' && LA3_0<='\u001F')||(LA3_0>='!' && LA3_0<='\'')||(LA3_0>='*' && LA3_0<='+')||(LA3_0>='-' && LA3_0<='9')||(LA3_0>='<' && LA3_0<='z')||LA3_0=='|'||(LA3_0>='~' && LA3_0<='\uFFFF')) ) {
                    alt3=1;
                }


                switch (alt3) {
            	case 1 :
            	    // src/hub/top/petrinet/OWFN.g:271:9: ~ ( ',' | ';' | ':' | '(' | ')' | '\\t' | ' ' | '\\n' | '\\r' | '\\{' | '\\}' )
            	    {
            	    if ( (input.LA(1)>='\u0000' && input.LA(1)<='\b')||(input.LA(1)>='\u000B' && input.LA(1)<='\f')||(input.LA(1)>='\u000E' && input.LA(1)<='\u001F')||(input.LA(1)>='!' && input.LA(1)<='\'')||(input.LA(1)>='*' && input.LA(1)<='+')||(input.LA(1)>='-' && input.LA(1)<='9')||(input.LA(1)>='<' && input.LA(1)<='z')||input.LA(1)=='|'||(input.LA(1)>='~' && input.LA(1)<='\uFFFF') ) {
            	        input.consume();

            	    }
            	    else {
            	        MismatchedSetException mse = new MismatchedSetException(null,input);
            	        recover(mse);
            	        throw mse;}


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

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "IDENT"

    // $ANTLR start "WHITESPACE"
    public final void mWHITESPACE() throws RecognitionException {
        try {
            int _type = WHITESPACE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // src/hub/top/petrinet/OWFN.g:273:12: ( ( '\\t' | ' ' | '\\r' | '\\n' | '\\u000C' )+ )
            // src/hub/top/petrinet/OWFN.g:273:14: ( '\\t' | ' ' | '\\r' | '\\n' | '\\u000C' )+
            {
            // src/hub/top/petrinet/OWFN.g:273:14: ( '\\t' | ' ' | '\\r' | '\\n' | '\\u000C' )+
            int cnt4=0;
            loop4:
            do {
                int alt4=2;
                int LA4_0 = input.LA(1);

                if ( ((LA4_0>='\t' && LA4_0<='\n')||(LA4_0>='\f' && LA4_0<='\r')||LA4_0==' ') ) {
                    alt4=1;
                }


                switch (alt4) {
            	case 1 :
            	    // src/hub/top/petrinet/OWFN.g:
            	    {
            	    if ( (input.LA(1)>='\t' && input.LA(1)<='\n')||(input.LA(1)>='\f' && input.LA(1)<='\r')||input.LA(1)==' ' ) {
            	        input.consume();

            	    }
            	    else {
            	        MismatchedSetException mse = new MismatchedSetException(null,input);
            	        recover(mse);
            	        throw mse;}


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

             _channel = HIDDEN; 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "WHITESPACE"

    public void mTokens() throws RecognitionException {
        // src/hub/top/petrinet/OWFN.g:1:8: ( COMMENT_CONTENTS | KEY_PLACE | KEY_ROLES | KEY_INTERNAL | KEY_INPUT | KEY_OUTPUT | KEY_SAFE | KEY_INITIALMARKING | KEY_TRANSITION | KEY_CONSUME | KEY_PRODUCE | KEY_FINALCONDITION | KEY_TRUE | KEY_AND | KEY_EMPTY | COLON | COMMA | SEMICOLON | NUMBER | IDENT | WHITESPACE )
        int alt5=21;
        alt5 = dfa5.predict(input);
        switch (alt5) {
            case 1 :
                // src/hub/top/petrinet/OWFN.g:1:10: COMMENT_CONTENTS
                {
                mCOMMENT_CONTENTS(); 

                }
                break;
            case 2 :
                // src/hub/top/petrinet/OWFN.g:1:27: KEY_PLACE
                {
                mKEY_PLACE(); 

                }
                break;
            case 3 :
                // src/hub/top/petrinet/OWFN.g:1:37: KEY_ROLES
                {
                mKEY_ROLES(); 

                }
                break;
            case 4 :
                // src/hub/top/petrinet/OWFN.g:1:47: KEY_INTERNAL
                {
                mKEY_INTERNAL(); 

                }
                break;
            case 5 :
                // src/hub/top/petrinet/OWFN.g:1:60: KEY_INPUT
                {
                mKEY_INPUT(); 

                }
                break;
            case 6 :
                // src/hub/top/petrinet/OWFN.g:1:70: KEY_OUTPUT
                {
                mKEY_OUTPUT(); 

                }
                break;
            case 7 :
                // src/hub/top/petrinet/OWFN.g:1:81: KEY_SAFE
                {
                mKEY_SAFE(); 

                }
                break;
            case 8 :
                // src/hub/top/petrinet/OWFN.g:1:90: KEY_INITIALMARKING
                {
                mKEY_INITIALMARKING(); 

                }
                break;
            case 9 :
                // src/hub/top/petrinet/OWFN.g:1:109: KEY_TRANSITION
                {
                mKEY_TRANSITION(); 

                }
                break;
            case 10 :
                // src/hub/top/petrinet/OWFN.g:1:124: KEY_CONSUME
                {
                mKEY_CONSUME(); 

                }
                break;
            case 11 :
                // src/hub/top/petrinet/OWFN.g:1:136: KEY_PRODUCE
                {
                mKEY_PRODUCE(); 

                }
                break;
            case 12 :
                // src/hub/top/petrinet/OWFN.g:1:148: KEY_FINALCONDITION
                {
                mKEY_FINALCONDITION(); 

                }
                break;
            case 13 :
                // src/hub/top/petrinet/OWFN.g:1:167: KEY_TRUE
                {
                mKEY_TRUE(); 

                }
                break;
            case 14 :
                // src/hub/top/petrinet/OWFN.g:1:176: KEY_AND
                {
                mKEY_AND(); 

                }
                break;
            case 15 :
                // src/hub/top/petrinet/OWFN.g:1:184: KEY_EMPTY
                {
                mKEY_EMPTY(); 

                }
                break;
            case 16 :
                // src/hub/top/petrinet/OWFN.g:1:194: COLON
                {
                mCOLON(); 

                }
                break;
            case 17 :
                // src/hub/top/petrinet/OWFN.g:1:200: COMMA
                {
                mCOMMA(); 

                }
                break;
            case 18 :
                // src/hub/top/petrinet/OWFN.g:1:206: SEMICOLON
                {
                mSEMICOLON(); 

                }
                break;
            case 19 :
                // src/hub/top/petrinet/OWFN.g:1:216: NUMBER
                {
                mNUMBER(); 

                }
                break;
            case 20 :
                // src/hub/top/petrinet/OWFN.g:1:223: IDENT
                {
                mIDENT(); 

                }
                break;
            case 21 :
                // src/hub/top/petrinet/OWFN.g:1:229: WHITESPACE
                {
                mWHITESPACE(); 

                }
                break;

        }

    }


    protected DFA5 dfa5 = new DFA5(this);
    static final String DFA5_eotS =
        "\2\uffff\11\20\3\uffff\1\35\1\20\2\uffff\13\20\1\uffff\14\20\1\70"+
        "\10\20\1\101\1\20\1\103\2\20\1\uffff\1\20\1\107\1\20\1\111\1\20"+
        "\1\113\2\20\1\uffff\1\20\1\uffff\3\20\1\uffff\1\20\1\uffff\1\20"+
        "\1\uffff\1\20\1\125\4\20\1\132\2\20\1\uffff\1\20\1\136\2\20\1\uffff"+
        "\1\141\2\20\1\uffff\2\20\1\uffff\5\20\1\153\3\20\1\uffff\10\20\1"+
        "\167\1\170\1\20\2\uffff\7\20\1\u0081\1\uffff";
    static final String DFA5_eofS =
        "\u0082\uffff";
    static final String DFA5_minS =
        "\1\0\1\uffff\1\114\1\117\1\116\1\125\1\101\1\122\1\117\1\111\1\114"+
        "\3\uffff\1\0\1\11\2\uffff\1\101\1\117\1\114\1\111\1\124\1\106\1"+
        "\101\2\116\1\104\1\114\1\uffff\1\103\1\104\2\105\1\125\1\124\1\120"+
        "\1\105\1\116\1\105\1\123\1\101\1\0\1\137\1\105\1\125\1\123\1\122"+
        "\1\124\1\111\1\125\1\0\1\123\1\0\1\125\1\114\1\uffff\1\117\1\0\1"+
        "\103\1\0\1\116\1\0\1\101\1\124\1\uffff\1\111\1\uffff\1\115\1\103"+
        "\1\124\1\uffff\1\105\1\uffff\1\101\1\uffff\1\114\1\0\1\124\1\105"+
        "\1\117\1\110\1\0\1\114\1\115\1\uffff\1\111\1\0\1\116\1\105\1\uffff"+
        "\1\0\1\101\1\117\1\uffff\1\104\1\122\1\uffff\1\122\1\116\1\111\1"+
        "\137\1\113\1\0\1\124\1\120\1\111\1\uffff\1\111\1\114\1\116\1\117"+
        "\1\101\1\107\1\116\1\103\2\0\1\105\2\uffff\1\123\1\137\1\105\1\115"+
        "\1\120\1\124\1\131\1\0\1\uffff";
    static final String DFA5_maxS =
        "\1\uffff\1\uffff\1\122\1\117\1\116\1\125\1\101\1\122\1\117\1\111"+
        "\1\116\3\uffff\1\uffff\1\40\2\uffff\1\101\1\117\1\114\2\124\1\106"+
        "\1\125\2\116\1\104\1\114\1\uffff\1\103\1\104\2\105\1\125\1\124\1"+
        "\120\1\105\1\116\1\105\1\123\1\101\1\uffff\1\137\1\105\1\125\1\123"+
        "\1\122\1\124\1\111\1\125\1\uffff\1\123\1\uffff\1\125\1\114\1\uffff"+
        "\1\117\1\uffff\1\103\1\uffff\1\116\1\uffff\1\101\1\124\1\uffff\1"+
        "\111\1\uffff\1\115\1\103\1\124\1\uffff\1\105\1\uffff\1\101\1\uffff"+
        "\1\114\1\uffff\1\124\1\105\1\117\1\110\1\uffff\1\114\1\115\1\uffff"+
        "\1\111\1\uffff\1\116\1\105\1\uffff\1\uffff\1\101\1\117\1\uffff\1"+
        "\104\1\122\1\uffff\1\122\1\116\1\111\1\137\1\113\1\uffff\1\124\1"+
        "\120\1\111\1\uffff\1\111\1\114\1\116\1\117\1\101\1\107\1\116\1\103"+
        "\2\uffff\1\105\2\uffff\1\123\1\137\1\105\1\115\1\120\1\124\1\131"+
        "\1\uffff\1\uffff";
    static final String DFA5_acceptS =
        "\1\uffff\1\1\11\uffff\1\20\1\21\1\22\2\uffff\1\24\1\25\13\uffff"+
        "\1\23\32\uffff\1\16\10\uffff\1\7\1\uffff\1\15\3\uffff\1\2\1\uffff"+
        "\1\3\1\uffff\1\5\11\uffff\1\6\4\uffff\1\13\3\uffff\1\12\2\uffff"+
        "\1\4\11\uffff\1\11\13\uffff\1\10\1\14\10\uffff\1\17";
    static final String DFA5_specialS =
        "\1\16\15\uffff\1\4\33\uffff\1\11\10\uffff\1\17\1\uffff\1\6\4\uffff"+
        "\1\10\1\uffff\1\12\1\uffff\1\14\16\uffff\1\15\4\uffff\1\3\4\uffff"+
        "\1\2\3\uffff\1\13\13\uffff\1\1\14\uffff\1\0\1\5\12\uffff\1\7\1\uffff}>";
    static final String[] DFA5_transitionS = {
            "\11\20\2\21\1\20\1\17\1\21\22\20\1\21\7\20\2\uffff\2\20\1\14"+
            "\3\20\12\16\1\13\1\15\5\20\1\12\1\20\1\10\2\20\1\11\2\20\1\4"+
            "\5\20\1\5\1\2\1\20\1\3\1\6\1\7\46\20\1\1\1\20\1\uffff\uff82"+
            "\20",
            "",
            "\1\22\5\uffff\1\23",
            "\1\24",
            "\1\25",
            "\1\26",
            "\1\27",
            "\1\30",
            "\1\31",
            "\1\32",
            "\1\34\1\uffff\1\33",
            "",
            "",
            "",
            "\11\20\2\uffff\2\20\1\uffff\22\20\1\uffff\7\20\2\uffff\2\20"+
            "\1\uffff\3\20\12\16\2\uffff\77\20\1\uffff\1\20\1\uffff\uff82"+
            "\20",
            "\2\21\1\uffff\1\17\1\21\22\uffff\1\21",
            "",
            "",
            "\1\36",
            "\1\37",
            "\1\40",
            "\1\43\6\uffff\1\42\3\uffff\1\41",
            "\1\44",
            "\1\45",
            "\1\46\23\uffff\1\47",
            "\1\50",
            "\1\51",
            "\1\52",
            "\1\53",
            "",
            "\1\54",
            "\1\55",
            "\1\56",
            "\1\57",
            "\1\60",
            "\1\61",
            "\1\62",
            "\1\63",
            "\1\64",
            "\1\65",
            "\1\66",
            "\1\67",
            "\11\20\2\uffff\2\20\1\uffff\22\20\1\uffff\7\20\2\uffff\2\20"+
            "\1\uffff\15\20\2\uffff\77\20\1\uffff\1\20\1\uffff\uff82\20",
            "\1\71",
            "\1\72",
            "\1\73",
            "\1\74",
            "\1\75",
            "\1\76",
            "\1\77",
            "\1\100",
            "\11\20\2\uffff\2\20\1\uffff\22\20\1\uffff\7\20\2\uffff\2\20"+
            "\1\uffff\15\20\2\uffff\77\20\1\uffff\1\20\1\uffff\uff82\20",
            "\1\102",
            "\11\20\2\uffff\2\20\1\uffff\22\20\1\uffff\7\20\2\uffff\2\20"+
            "\1\uffff\15\20\2\uffff\77\20\1\uffff\1\20\1\uffff\uff82\20",
            "\1\104",
            "\1\105",
            "",
            "\1\106",
            "\11\20\2\uffff\2\20\1\uffff\22\20\1\uffff\7\20\2\uffff\2\20"+
            "\1\uffff\15\20\2\uffff\77\20\1\uffff\1\20\1\uffff\uff82\20",
            "\1\110",
            "\11\20\2\uffff\2\20\1\uffff\22\20\1\uffff\7\20\2\uffff\2\20"+
            "\1\uffff\15\20\2\uffff\77\20\1\uffff\1\20\1\uffff\uff82\20",
            "\1\112",
            "\11\20\2\uffff\2\20\1\uffff\22\20\1\uffff\7\20\2\uffff\2\20"+
            "\1\uffff\15\20\2\uffff\77\20\1\uffff\1\20\1\uffff\uff82\20",
            "\1\114",
            "\1\115",
            "",
            "\1\116",
            "",
            "\1\117",
            "\1\120",
            "\1\121",
            "",
            "\1\122",
            "",
            "\1\123",
            "",
            "\1\124",
            "\11\20\2\uffff\2\20\1\uffff\22\20\1\uffff\7\20\2\uffff\2\20"+
            "\1\uffff\15\20\2\uffff\77\20\1\uffff\1\20\1\uffff\uff82\20",
            "\1\126",
            "\1\127",
            "\1\130",
            "\1\131",
            "\11\20\2\uffff\2\20\1\uffff\22\20\1\uffff\7\20\2\uffff\2\20"+
            "\1\uffff\15\20\2\uffff\77\20\1\uffff\1\20\1\uffff\uff82\20",
            "\1\133",
            "\1\134",
            "",
            "\1\135",
            "\11\20\2\uffff\2\20\1\uffff\22\20\1\uffff\7\20\2\uffff\2\20"+
            "\1\uffff\15\20\2\uffff\77\20\1\uffff\1\20\1\uffff\uff82\20",
            "\1\137",
            "\1\140",
            "",
            "\11\20\2\uffff\2\20\1\uffff\22\20\1\uffff\7\20\2\uffff\2\20"+
            "\1\uffff\15\20\2\uffff\77\20\1\uffff\1\20\1\uffff\uff82\20",
            "\1\142",
            "\1\143",
            "",
            "\1\144",
            "\1\145",
            "",
            "\1\146",
            "\1\147",
            "\1\150",
            "\1\151",
            "\1\152",
            "\11\20\2\uffff\2\20\1\uffff\22\20\1\uffff\7\20\2\uffff\2\20"+
            "\1\uffff\15\20\2\uffff\77\20\1\uffff\1\20\1\uffff\uff82\20",
            "\1\154",
            "\1\155",
            "\1\156",
            "",
            "\1\157",
            "\1\160",
            "\1\161",
            "\1\162",
            "\1\163",
            "\1\164",
            "\1\165",
            "\1\166",
            "\11\20\2\uffff\2\20\1\uffff\22\20\1\uffff\7\20\2\uffff\2\20"+
            "\1\uffff\15\20\2\uffff\77\20\1\uffff\1\20\1\uffff\uff82\20",
            "\11\20\2\uffff\2\20\1\uffff\22\20\1\uffff\7\20\2\uffff\2\20"+
            "\1\uffff\15\20\2\uffff\77\20\1\uffff\1\20\1\uffff\uff82\20",
            "\1\171",
            "",
            "",
            "\1\172",
            "\1\173",
            "\1\174",
            "\1\175",
            "\1\176",
            "\1\177",
            "\1\u0080",
            "\11\20\2\uffff\2\20\1\uffff\22\20\1\uffff\7\20\2\uffff\2\20"+
            "\1\uffff\15\20\2\uffff\77\20\1\uffff\1\20\1\uffff\uff82\20",
            ""
    };

    static final short[] DFA5_eot = DFA.unpackEncodedString(DFA5_eotS);
    static final short[] DFA5_eof = DFA.unpackEncodedString(DFA5_eofS);
    static final char[] DFA5_min = DFA.unpackEncodedStringToUnsignedChars(DFA5_minS);
    static final char[] DFA5_max = DFA.unpackEncodedStringToUnsignedChars(DFA5_maxS);
    static final short[] DFA5_accept = DFA.unpackEncodedString(DFA5_acceptS);
    static final short[] DFA5_special = DFA.unpackEncodedString(DFA5_specialS);
    static final short[][] DFA5_transition;

    static {
        int numStates = DFA5_transitionS.length;
        DFA5_transition = new short[numStates][];
        for (int i=0; i<numStates; i++) {
            DFA5_transition[i] = DFA.unpackEncodedString(DFA5_transitionS[i]);
        }
    }

    class DFA5 extends DFA {

        public DFA5(BaseRecognizer recognizer) {
            this.recognizer = recognizer;
            this.decisionNumber = 5;
            this.eot = DFA5_eot;
            this.eof = DFA5_eof;
            this.min = DFA5_min;
            this.max = DFA5_max;
            this.accept = DFA5_accept;
            this.special = DFA5_special;
            this.transition = DFA5_transition;
        }
        public String getDescription() {
            return "1:1: Tokens : ( COMMENT_CONTENTS | KEY_PLACE | KEY_ROLES | KEY_INTERNAL | KEY_INPUT | KEY_OUTPUT | KEY_SAFE | KEY_INITIALMARKING | KEY_TRANSITION | KEY_CONSUME | KEY_PRODUCE | KEY_FINALCONDITION | KEY_TRUE | KEY_AND | KEY_EMPTY | COLON | COMMA | SEMICOLON | NUMBER | IDENT | WHITESPACE );";
        }
        public int specialStateTransition(int s, IntStream _input) throws NoViableAltException {
            IntStream input = _input;
        	int _s = s;
            switch ( s ) {
                    case 0 : 
                        int LA5_116 = input.LA(1);

                        s = -1;
                        if ( ((LA5_116>='\u0000' && LA5_116<='\b')||(LA5_116>='\u000B' && LA5_116<='\f')||(LA5_116>='\u000E' && LA5_116<='\u001F')||(LA5_116>='!' && LA5_116<='\'')||(LA5_116>='*' && LA5_116<='+')||(LA5_116>='-' && LA5_116<='9')||(LA5_116>='<' && LA5_116<='z')||LA5_116=='|'||(LA5_116>='~' && LA5_116<='\uFFFF')) ) {s = 16;}

                        else s = 119;

                        if ( s>=0 ) return s;
                        break;
                    case 1 : 
                        int LA5_103 = input.LA(1);

                        s = -1;
                        if ( ((LA5_103>='\u0000' && LA5_103<='\b')||(LA5_103>='\u000B' && LA5_103<='\f')||(LA5_103>='\u000E' && LA5_103<='\u001F')||(LA5_103>='!' && LA5_103<='\'')||(LA5_103>='*' && LA5_103<='+')||(LA5_103>='-' && LA5_103<='9')||(LA5_103>='<' && LA5_103<='z')||LA5_103=='|'||(LA5_103>='~' && LA5_103<='\uFFFF')) ) {s = 16;}

                        else s = 107;

                        if ( s>=0 ) return s;
                        break;
                    case 2 : 
                        int LA5_87 = input.LA(1);

                        s = -1;
                        if ( ((LA5_87>='\u0000' && LA5_87<='\b')||(LA5_87>='\u000B' && LA5_87<='\f')||(LA5_87>='\u000E' && LA5_87<='\u001F')||(LA5_87>='!' && LA5_87<='\'')||(LA5_87>='*' && LA5_87<='+')||(LA5_87>='-' && LA5_87<='9')||(LA5_87>='<' && LA5_87<='z')||LA5_87=='|'||(LA5_87>='~' && LA5_87<='\uFFFF')) ) {s = 16;}

                        else s = 94;

                        if ( s>=0 ) return s;
                        break;
                    case 3 : 
                        int LA5_82 = input.LA(1);

                        s = -1;
                        if ( ((LA5_82>='\u0000' && LA5_82<='\b')||(LA5_82>='\u000B' && LA5_82<='\f')||(LA5_82>='\u000E' && LA5_82<='\u001F')||(LA5_82>='!' && LA5_82<='\'')||(LA5_82>='*' && LA5_82<='+')||(LA5_82>='-' && LA5_82<='9')||(LA5_82>='<' && LA5_82<='z')||LA5_82=='|'||(LA5_82>='~' && LA5_82<='\uFFFF')) ) {s = 16;}

                        else s = 90;

                        if ( s>=0 ) return s;
                        break;
                    case 4 : 
                        int LA5_14 = input.LA(1);

                        s = -1;
                        if ( ((LA5_14>='0' && LA5_14<='9')) ) {s = 14;}

                        else if ( ((LA5_14>='\u0000' && LA5_14<='\b')||(LA5_14>='\u000B' && LA5_14<='\f')||(LA5_14>='\u000E' && LA5_14<='\u001F')||(LA5_14>='!' && LA5_14<='\'')||(LA5_14>='*' && LA5_14<='+')||(LA5_14>='-' && LA5_14<='/')||(LA5_14>='<' && LA5_14<='z')||LA5_14=='|'||(LA5_14>='~' && LA5_14<='\uFFFF')) ) {s = 16;}

                        else s = 29;

                        if ( s>=0 ) return s;
                        break;
                    case 5 : 
                        int LA5_117 = input.LA(1);

                        s = -1;
                        if ( ((LA5_117>='\u0000' && LA5_117<='\b')||(LA5_117>='\u000B' && LA5_117<='\f')||(LA5_117>='\u000E' && LA5_117<='\u001F')||(LA5_117>='!' && LA5_117<='\'')||(LA5_117>='*' && LA5_117<='+')||(LA5_117>='-' && LA5_117<='9')||(LA5_117>='<' && LA5_117<='z')||LA5_117=='|'||(LA5_117>='~' && LA5_117<='\uFFFF')) ) {s = 16;}

                        else s = 120;

                        if ( s>=0 ) return s;
                        break;
                    case 6 : 
                        int LA5_53 = input.LA(1);

                        s = -1;
                        if ( ((LA5_53>='\u0000' && LA5_53<='\b')||(LA5_53>='\u000B' && LA5_53<='\f')||(LA5_53>='\u000E' && LA5_53<='\u001F')||(LA5_53>='!' && LA5_53<='\'')||(LA5_53>='*' && LA5_53<='+')||(LA5_53>='-' && LA5_53<='9')||(LA5_53>='<' && LA5_53<='z')||LA5_53=='|'||(LA5_53>='~' && LA5_53<='\uFFFF')) ) {s = 16;}

                        else s = 67;

                        if ( s>=0 ) return s;
                        break;
                    case 7 : 
                        int LA5_128 = input.LA(1);

                        s = -1;
                        if ( ((LA5_128>='\u0000' && LA5_128<='\b')||(LA5_128>='\u000B' && LA5_128<='\f')||(LA5_128>='\u000E' && LA5_128<='\u001F')||(LA5_128>='!' && LA5_128<='\'')||(LA5_128>='*' && LA5_128<='+')||(LA5_128>='-' && LA5_128<='9')||(LA5_128>='<' && LA5_128<='z')||LA5_128=='|'||(LA5_128>='~' && LA5_128<='\uFFFF')) ) {s = 16;}

                        else s = 129;

                        if ( s>=0 ) return s;
                        break;
                    case 8 : 
                        int LA5_58 = input.LA(1);

                        s = -1;
                        if ( ((LA5_58>='\u0000' && LA5_58<='\b')||(LA5_58>='\u000B' && LA5_58<='\f')||(LA5_58>='\u000E' && LA5_58<='\u001F')||(LA5_58>='!' && LA5_58<='\'')||(LA5_58>='*' && LA5_58<='+')||(LA5_58>='-' && LA5_58<='9')||(LA5_58>='<' && LA5_58<='z')||LA5_58=='|'||(LA5_58>='~' && LA5_58<='\uFFFF')) ) {s = 16;}

                        else s = 71;

                        if ( s>=0 ) return s;
                        break;
                    case 9 : 
                        int LA5_42 = input.LA(1);

                        s = -1;
                        if ( ((LA5_42>='\u0000' && LA5_42<='\b')||(LA5_42>='\u000B' && LA5_42<='\f')||(LA5_42>='\u000E' && LA5_42<='\u001F')||(LA5_42>='!' && LA5_42<='\'')||(LA5_42>='*' && LA5_42<='+')||(LA5_42>='-' && LA5_42<='9')||(LA5_42>='<' && LA5_42<='z')||LA5_42=='|'||(LA5_42>='~' && LA5_42<='\uFFFF')) ) {s = 16;}

                        else s = 56;

                        if ( s>=0 ) return s;
                        break;
                    case 10 : 
                        int LA5_60 = input.LA(1);

                        s = -1;
                        if ( ((LA5_60>='\u0000' && LA5_60<='\b')||(LA5_60>='\u000B' && LA5_60<='\f')||(LA5_60>='\u000E' && LA5_60<='\u001F')||(LA5_60>='!' && LA5_60<='\'')||(LA5_60>='*' && LA5_60<='+')||(LA5_60>='-' && LA5_60<='9')||(LA5_60>='<' && LA5_60<='z')||LA5_60=='|'||(LA5_60>='~' && LA5_60<='\uFFFF')) ) {s = 16;}

                        else s = 73;

                        if ( s>=0 ) return s;
                        break;
                    case 11 : 
                        int LA5_91 = input.LA(1);

                        s = -1;
                        if ( ((LA5_91>='\u0000' && LA5_91<='\b')||(LA5_91>='\u000B' && LA5_91<='\f')||(LA5_91>='\u000E' && LA5_91<='\u001F')||(LA5_91>='!' && LA5_91<='\'')||(LA5_91>='*' && LA5_91<='+')||(LA5_91>='-' && LA5_91<='9')||(LA5_91>='<' && LA5_91<='z')||LA5_91=='|'||(LA5_91>='~' && LA5_91<='\uFFFF')) ) {s = 16;}

                        else s = 97;

                        if ( s>=0 ) return s;
                        break;
                    case 12 : 
                        int LA5_62 = input.LA(1);

                        s = -1;
                        if ( ((LA5_62>='\u0000' && LA5_62<='\b')||(LA5_62>='\u000B' && LA5_62<='\f')||(LA5_62>='\u000E' && LA5_62<='\u001F')||(LA5_62>='!' && LA5_62<='\'')||(LA5_62>='*' && LA5_62<='+')||(LA5_62>='-' && LA5_62<='9')||(LA5_62>='<' && LA5_62<='z')||LA5_62=='|'||(LA5_62>='~' && LA5_62<='\uFFFF')) ) {s = 16;}

                        else s = 75;

                        if ( s>=0 ) return s;
                        break;
                    case 13 : 
                        int LA5_77 = input.LA(1);

                        s = -1;
                        if ( ((LA5_77>='\u0000' && LA5_77<='\b')||(LA5_77>='\u000B' && LA5_77<='\f')||(LA5_77>='\u000E' && LA5_77<='\u001F')||(LA5_77>='!' && LA5_77<='\'')||(LA5_77>='*' && LA5_77<='+')||(LA5_77>='-' && LA5_77<='9')||(LA5_77>='<' && LA5_77<='z')||LA5_77=='|'||(LA5_77>='~' && LA5_77<='\uFFFF')) ) {s = 16;}

                        else s = 85;

                        if ( s>=0 ) return s;
                        break;
                    case 14 : 
                        int LA5_0 = input.LA(1);

                        s = -1;
                        if ( (LA5_0=='{') ) {s = 1;}

                        else if ( (LA5_0=='P') ) {s = 2;}

                        else if ( (LA5_0=='R') ) {s = 3;}

                        else if ( (LA5_0=='I') ) {s = 4;}

                        else if ( (LA5_0=='O') ) {s = 5;}

                        else if ( (LA5_0=='S') ) {s = 6;}

                        else if ( (LA5_0=='T') ) {s = 7;}

                        else if ( (LA5_0=='C') ) {s = 8;}

                        else if ( (LA5_0=='F') ) {s = 9;}

                        else if ( (LA5_0=='A') ) {s = 10;}

                        else if ( (LA5_0==':') ) {s = 11;}

                        else if ( (LA5_0==',') ) {s = 12;}

                        else if ( (LA5_0==';') ) {s = 13;}

                        else if ( ((LA5_0>='0' && LA5_0<='9')) ) {s = 14;}

                        else if ( (LA5_0=='\f') ) {s = 15;}

                        else if ( ((LA5_0>='\u0000' && LA5_0<='\b')||LA5_0=='\u000B'||(LA5_0>='\u000E' && LA5_0<='\u001F')||(LA5_0>='!' && LA5_0<='\'')||(LA5_0>='*' && LA5_0<='+')||(LA5_0>='-' && LA5_0<='/')||(LA5_0>='<' && LA5_0<='@')||LA5_0=='B'||(LA5_0>='D' && LA5_0<='E')||(LA5_0>='G' && LA5_0<='H')||(LA5_0>='J' && LA5_0<='N')||LA5_0=='Q'||(LA5_0>='U' && LA5_0<='z')||LA5_0=='|'||(LA5_0>='~' && LA5_0<='\uFFFF')) ) {s = 16;}

                        else if ( ((LA5_0>='\t' && LA5_0<='\n')||LA5_0=='\r'||LA5_0==' ') ) {s = 17;}

                        if ( s>=0 ) return s;
                        break;
                    case 15 : 
                        int LA5_51 = input.LA(1);

                        s = -1;
                        if ( ((LA5_51>='\u0000' && LA5_51<='\b')||(LA5_51>='\u000B' && LA5_51<='\f')||(LA5_51>='\u000E' && LA5_51<='\u001F')||(LA5_51>='!' && LA5_51<='\'')||(LA5_51>='*' && LA5_51<='+')||(LA5_51>='-' && LA5_51<='9')||(LA5_51>='<' && LA5_51<='z')||LA5_51=='|'||(LA5_51>='~' && LA5_51<='\uFFFF')) ) {s = 16;}

                        else s = 65;

                        if ( s>=0 ) return s;
                        break;
            }
            NoViableAltException nvae =
                new NoViableAltException(getDescription(), 5, _s, input);
            error(nvae);
            throw nvae;
        }
    }
 

}