// $ANTLR 3.2 Sep 23, 2009 12:02:23 D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g 2011-12-08 14:01:12

    package hub.top.petrinet;


import org.antlr.runtime.*;
import java.util.Stack;
import java.util.List;
import java.util.ArrayList;

public class TPNLexer extends Lexer {
    public static final int STRING_LITERAL=4;
    public static final int KEY_INIT=7;
    public static final int KEY_TRANSITION=9;
    public static final int NUMBER=5;
    public static final int TILDE=10;
    public static final int KEY_OUT=12;
    public static final int KEY_IN=11;
    public static final int WHITESPACE=15;
    public static final int SEMICOLON=8;
    public static final int DIGIT=14;
    public static final int COMMENT_CONTENTS=13;
    public static final int EOF=-1;
    public static final int KEY_PLACE=6;

    // delegates
    // delegators

    public TPNLexer() {;} 
    public TPNLexer(CharStream input) {
        this(input, new RecognizerSharedState());
    }
    public TPNLexer(CharStream input, RecognizerSharedState state) {
        super(input,state);

    }
    public String getGrammarFileName() { return "D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g"; }

    // $ANTLR start "COMMENT_CONTENTS"
    public final void mCOMMENT_CONTENTS() throws RecognitionException {
        try {
            int _type = COMMENT_CONTENTS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:149:18: ( '--' (~ ( '\\n' | '\\r' ) )* '\\n' )
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:150:2: '--' (~ ( '\\n' | '\\r' ) )* '\\n'
            {
            match("--"); 


                	_channel=98;
                
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:154:5: (~ ( '\\n' | '\\r' ) )*
            loop1:
            do {
                int alt1=2;
                int LA1_0 = input.LA(1);

                if ( ((LA1_0>='\u0000' && LA1_0<='\t')||(LA1_0>='\u000B' && LA1_0<='\f')||(LA1_0>='\u000E' && LA1_0<='\uFFFF')) ) {
                    alt1=1;
                }


                switch (alt1) {
            	case 1 :
            	    // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:154:7: ~ ( '\\n' | '\\r' )
            	    {
            	    if ( (input.LA(1)>='\u0000' && input.LA(1)<='\t')||(input.LA(1)>='\u000B' && input.LA(1)<='\f')||(input.LA(1)>='\u000E' && input.LA(1)<='\uFFFF') ) {
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

            match('\n'); 

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
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:157:11: ( 'place' )
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:157:13: 'place'
            {
            match("place"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_PLACE"

    // $ANTLR start "KEY_INIT"
    public final void mKEY_INIT() throws RecognitionException {
        try {
            int _type = KEY_INIT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:158:10: ( 'init' )
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:158:12: 'init'
            {
            match("init"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_INIT"

    // $ANTLR start "KEY_TRANSITION"
    public final void mKEY_TRANSITION() throws RecognitionException {
        try {
            int _type = KEY_TRANSITION;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:159:17: ( 'trans' )
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:159:19: 'trans'
            {
            match("trans"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_TRANSITION"

    // $ANTLR start "KEY_IN"
    public final void mKEY_IN() throws RecognitionException {
        try {
            int _type = KEY_IN;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:160:9: ( 'in' )
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:160:11: 'in'
            {
            match("in"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_IN"

    // $ANTLR start "KEY_OUT"
    public final void mKEY_OUT() throws RecognitionException {
        try {
            int _type = KEY_OUT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:161:10: ( 'out' )
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:161:12: 'out'
            {
            match("out"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_OUT"

    // $ANTLR start "SEMICOLON"
    public final void mSEMICOLON() throws RecognitionException {
        try {
            int _type = SEMICOLON;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:163:11: ( ';' )
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:163:13: ';'
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

    // $ANTLR start "TILDE"
    public final void mTILDE() throws RecognitionException {
        try {
            int _type = TILDE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:164:7: ( '~' )
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:164:9: '~'
            {
            match('~'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "TILDE"

    // $ANTLR start "DIGIT"
    public final void mDIGIT() throws RecognitionException {
        try {
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:166:16: ( '0' .. '9' )
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:166:18: '0' .. '9'
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
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:167:8: ( ( DIGIT )+ )
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:167:10: ( DIGIT )+
            {
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:167:10: ( DIGIT )+
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
            	    // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:167:11: DIGIT
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

    // $ANTLR start "STRING_LITERAL"
    public final void mSTRING_LITERAL() throws RecognitionException {
        try {
            int _type = STRING_LITERAL;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:170:3: ( '\"' (~ ( '\"' | '\\n' | '\\r' ) )* '\"' )
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:170:5: '\"' (~ ( '\"' | '\\n' | '\\r' ) )* '\"'
            {
            match('\"'); 
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:171:5: (~ ( '\"' | '\\n' | '\\r' ) )*
            loop3:
            do {
                int alt3=2;
                int LA3_0 = input.LA(1);

                if ( ((LA3_0>='\u0000' && LA3_0<='\t')||(LA3_0>='\u000B' && LA3_0<='\f')||(LA3_0>='\u000E' && LA3_0<='!')||(LA3_0>='#' && LA3_0<='\uFFFF')) ) {
                    alt3=1;
                }


                switch (alt3) {
            	case 1 :
            	    // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:171:7: ~ ( '\"' | '\\n' | '\\r' )
            	    {
            	    if ( (input.LA(1)>='\u0000' && input.LA(1)<='\t')||(input.LA(1)>='\u000B' && input.LA(1)<='\f')||(input.LA(1)>='\u000E' && input.LA(1)<='!')||(input.LA(1)>='#' && input.LA(1)<='\uFFFF') ) {
            	        input.consume();

            	    }
            	    else {
            	        MismatchedSetException mse = new MismatchedSetException(null,input);
            	        recover(mse);
            	        throw mse;}


            	    }
            	    break;

            	default :
            	    break loop3;
                }
            } while (true);

            match('\"'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "STRING_LITERAL"

    // $ANTLR start "WHITESPACE"
    public final void mWHITESPACE() throws RecognitionException {
        try {
            int _type = WHITESPACE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:176:12: ( ( '\\t' | ' ' | '\\r' | '\\n' | '\\u000C' )+ )
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:176:14: ( '\\t' | ' ' | '\\r' | '\\n' | '\\u000C' )+
            {
            // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:176:14: ( '\\t' | ' ' | '\\r' | '\\n' | '\\u000C' )+
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
            	    // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:
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
        // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:1:8: ( COMMENT_CONTENTS | KEY_PLACE | KEY_INIT | KEY_TRANSITION | KEY_IN | KEY_OUT | SEMICOLON | TILDE | NUMBER | STRING_LITERAL | WHITESPACE )
        int alt5=11;
        alt5 = dfa5.predict(input);
        switch (alt5) {
            case 1 :
                // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:1:10: COMMENT_CONTENTS
                {
                mCOMMENT_CONTENTS(); 

                }
                break;
            case 2 :
                // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:1:27: KEY_PLACE
                {
                mKEY_PLACE(); 

                }
                break;
            case 3 :
                // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:1:37: KEY_INIT
                {
                mKEY_INIT(); 

                }
                break;
            case 4 :
                // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:1:46: KEY_TRANSITION
                {
                mKEY_TRANSITION(); 

                }
                break;
            case 5 :
                // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:1:61: KEY_IN
                {
                mKEY_IN(); 

                }
                break;
            case 6 :
                // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:1:68: KEY_OUT
                {
                mKEY_OUT(); 

                }
                break;
            case 7 :
                // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:1:76: SEMICOLON
                {
                mSEMICOLON(); 

                }
                break;
            case 8 :
                // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:1:86: TILDE
                {
                mTILDE(); 

                }
                break;
            case 9 :
                // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:1:92: NUMBER
                {
                mNUMBER(); 

                }
                break;
            case 10 :
                // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:1:99: STRING_LITERAL
                {
                mSTRING_LITERAL(); 

                }
                break;
            case 11 :
                // D:\\LinuxShared\\eclipseWorkspace-greta-validate\\hub.top.pnapi\\src\\hub\\top\\petrinet\\TPN.g:1:114: WHITESPACE
                {
                mWHITESPACE(); 

                }
                break;

        }

    }


    protected DFA5 dfa5 = new DFA5(this);
    static final String DFA5_eotS =
        "\13\uffff\1\15\2\uffff";
    static final String DFA5_eofS =
        "\16\uffff";
    static final String DFA5_minS =
        "\1\11\2\uffff\1\156\7\uffff\1\151\2\uffff";
    static final String DFA5_maxS =
        "\1\176\2\uffff\1\156\7\uffff\1\151\2\uffff";
    static final String DFA5_acceptS =
        "\1\uffff\1\1\1\2\1\uffff\1\4\1\6\1\7\1\10\1\11\1\12\1\13\1\uffff"+
        "\1\3\1\5";
    static final String DFA5_specialS =
        "\16\uffff}>";
    static final String[] DFA5_transitionS = {
            "\2\12\1\uffff\2\12\22\uffff\1\12\1\uffff\1\11\12\uffff\1\1"+
            "\2\uffff\12\10\1\uffff\1\6\55\uffff\1\3\5\uffff\1\5\1\2\3\uffff"+
            "\1\4\11\uffff\1\7",
            "",
            "",
            "\1\13",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "\1\14",
            "",
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
            return "1:1: Tokens : ( COMMENT_CONTENTS | KEY_PLACE | KEY_INIT | KEY_TRANSITION | KEY_IN | KEY_OUT | SEMICOLON | TILDE | NUMBER | STRING_LITERAL | WHITESPACE );";
        }
    }
 

}