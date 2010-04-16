// $ANTLR 3.2 Sep 23, 2009 12:02:23 D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g 2010-04-16 22:38:32

    package hub.top.petrinet;


import org.antlr.runtime.*;
import java.util.Stack;
import java.util.List;
import java.util.ArrayList;

public class LoLALexer extends Lexer {
    public static final int COMMA=11;
    public static final int KEY_MARKING=5;
    public static final int IDENT=6;
    public static final int SEMICOLON=8;
    public static final int KEY_PLACE=4;
    public static final int NUMBER=7;
    public static final int EOF=-1;
    public static final int KEY_PRODUCE=14;
    public static final int COLON=10;
    public static final int KEY_TRANSITION=12;
    public static final int COMMENT_CONTENTS=15;
    public static final int DIGIT=16;
    public static final int KEY_SAFE=9;
    public static final int KEY_CONSUME=13;
    public static final int WHITESPACE=17;

    // delegates
    // delegators

    public LoLALexer() {;} 
    public LoLALexer(CharStream input) {
        this(input, new RecognizerSharedState());
    }
    public LoLALexer(CharStream input, RecognizerSharedState state) {
        super(input,state);

    }
    public String getGrammarFileName() { return "D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g"; }

    // $ANTLR start "COMMENT_CONTENTS"
    public final void mCOMMENT_CONTENTS() throws RecognitionException {
        try {
            int _type = COMMENT_CONTENTS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:182:18: ( '\\{' (~ ( '\\}' ) )* '\\}' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:183:2: '\\{' (~ ( '\\}' ) )* '\\}'
            {
            match('{'); 

                	_channel=98;
                
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:187:5: (~ ( '\\}' ) )*
            loop1:
            do {
                int alt1=2;
                int LA1_0 = input.LA(1);

                if ( ((LA1_0>='\u0000' && LA1_0<='|')||(LA1_0>='~' && LA1_0<='\uFFFF')) ) {
                    alt1=1;
                }


                switch (alt1) {
            	case 1 :
            	    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:187:7: ~ ( '\\}' )
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:190:11: ( 'PLACE' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:190:13: 'PLACE'
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

    // $ANTLR start "KEY_SAFE"
    public final void mKEY_SAFE() throws RecognitionException {
        try {
            int _type = KEY_SAFE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:191:10: ( 'SAFE' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:191:12: 'SAFE'
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

    // $ANTLR start "KEY_MARKING"
    public final void mKEY_MARKING() throws RecognitionException {
        try {
            int _type = KEY_MARKING;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:192:14: ( 'MARKING' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:192:16: 'MARKING'
            {
            match("MARKING"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_MARKING"

    // $ANTLR start "KEY_TRANSITION"
    public final void mKEY_TRANSITION() throws RecognitionException {
        try {
            int _type = KEY_TRANSITION;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:193:17: ( 'TRANSITION' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:193:19: 'TRANSITION'
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:194:14: ( 'CONSUME' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:194:16: 'CONSUME'
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:195:14: ( 'PRODUCE' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:195:16: 'PRODUCE'
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

    // $ANTLR start "COLON"
    public final void mCOLON() throws RecognitionException {
        try {
            int _type = COLON;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:197:7: ( ':' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:197:9: ':'
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:198:7: ( ',' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:198:9: ','
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:199:11: ( ';' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:199:13: ';'
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:201:16: ( '0' .. '9' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:201:18: '0' .. '9'
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:202:8: ( ( DIGIT )+ )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:202:10: ( DIGIT )+
            {
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:202:10: ( DIGIT )+
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
            	    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:202:11: DIGIT
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:203:7: ( (~ ( ',' | ';' | ':' | '(' | ')' | '\\t' | ' ' | '\\n' | '\\r' | '\\{' | '\\}' ) )+ )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:203:9: (~ ( ',' | ';' | ':' | '(' | ')' | '\\t' | ' ' | '\\n' | '\\r' | '\\{' | '\\}' ) )+
            {
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:203:9: (~ ( ',' | ';' | ':' | '(' | ')' | '\\t' | ' ' | '\\n' | '\\r' | '\\{' | '\\}' ) )+
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
            	    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:203:9: ~ ( ',' | ';' | ':' | '(' | ')' | '\\t' | ' ' | '\\n' | '\\r' | '\\{' | '\\}' )
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:205:12: ( ( '\\t' | ' ' | '\\r' | '\\n' | '\\u000C' )+ )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:205:14: ( '\\t' | ' ' | '\\r' | '\\n' | '\\u000C' )+
            {
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:205:14: ( '\\t' | ' ' | '\\r' | '\\n' | '\\u000C' )+
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
            	    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:
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
        // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:1:8: ( COMMENT_CONTENTS | KEY_PLACE | KEY_SAFE | KEY_MARKING | KEY_TRANSITION | KEY_CONSUME | KEY_PRODUCE | COLON | COMMA | SEMICOLON | NUMBER | IDENT | WHITESPACE )
        int alt5=13;
        alt5 = dfa5.predict(input);
        switch (alt5) {
            case 1 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:1:10: COMMENT_CONTENTS
                {
                mCOMMENT_CONTENTS(); 

                }
                break;
            case 2 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:1:27: KEY_PLACE
                {
                mKEY_PLACE(); 

                }
                break;
            case 3 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:1:37: KEY_SAFE
                {
                mKEY_SAFE(); 

                }
                break;
            case 4 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:1:46: KEY_MARKING
                {
                mKEY_MARKING(); 

                }
                break;
            case 5 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:1:58: KEY_TRANSITION
                {
                mKEY_TRANSITION(); 

                }
                break;
            case 6 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:1:73: KEY_CONSUME
                {
                mKEY_CONSUME(); 

                }
                break;
            case 7 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:1:85: KEY_PRODUCE
                {
                mKEY_PRODUCE(); 

                }
                break;
            case 8 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:1:97: COLON
                {
                mCOLON(); 

                }
                break;
            case 9 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:1:103: COMMA
                {
                mCOMMA(); 

                }
                break;
            case 10 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:1:109: SEMICOLON
                {
                mSEMICOLON(); 

                }
                break;
            case 11 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:1:119: NUMBER
                {
                mNUMBER(); 

                }
                break;
            case 12 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:1:126: IDENT
                {
                mIDENT(); 

                }
                break;
            case 13 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\LoLA.g:1:132: WHITESPACE
                {
                mWHITESPACE(); 

                }
                break;

        }

    }


    protected DFA5 dfa5 = new DFA5(this);
    static final String DFA5_eotS =
        "\2\uffff\5\14\3\uffff\1\24\1\14\2\uffff\6\14\1\uffff\10\14\1\43"+
        "\3\14\1\47\1\14\1\uffff\3\14\1\uffff\4\14\1\60\1\61\1\14\1\63\2"+
        "\uffff\1\14\1\uffff\1\14\1\66\1\uffff";
    static final String DFA5_eofS =
        "\67\uffff";
    static final String DFA5_minS =
        "\1\0\1\uffff\1\114\2\101\1\122\1\117\3\uffff\1\0\1\11\2\uffff\1"+
        "\101\1\117\1\106\1\122\1\101\1\116\1\uffff\1\103\1\104\1\105\1\113"+
        "\1\116\1\123\1\105\1\125\1\0\1\111\1\123\1\125\1\0\1\103\1\uffff"+
        "\1\116\1\111\1\115\1\uffff\1\105\1\107\1\124\1\105\2\0\1\111\1\0"+
        "\2\uffff\1\117\1\uffff\1\116\1\0\1\uffff";
    static final String DFA5_maxS =
        "\1\uffff\1\uffff\1\122\2\101\1\122\1\117\3\uffff\1\uffff\1\40\2"+
        "\uffff\1\101\1\117\1\106\1\122\1\101\1\116\1\uffff\1\103\1\104\1"+
        "\105\1\113\1\116\1\123\1\105\1\125\1\uffff\1\111\1\123\1\125\1\uffff"+
        "\1\103\1\uffff\1\116\1\111\1\115\1\uffff\1\105\1\107\1\124\1\105"+
        "\2\uffff\1\111\1\uffff\2\uffff\1\117\1\uffff\1\116\1\uffff\1\uffff";
    static final String DFA5_acceptS =
        "\1\uffff\1\1\5\uffff\1\10\1\11\1\12\2\uffff\1\14\1\15\6\uffff\1"+
        "\13\16\uffff\1\3\3\uffff\1\2\10\uffff\1\7\1\4\1\uffff\1\6\2\uffff"+
        "\1\5";
    static final String DFA5_specialS =
        "\1\4\11\uffff\1\5\22\uffff\1\6\3\uffff\1\2\12\uffff\1\7\1\0\1\uffff"+
        "\1\3\5\uffff\1\1\1\uffff}>";
    static final String[] DFA5_transitionS = {
            "\11\14\2\15\1\14\1\13\1\15\22\14\1\15\7\14\2\uffff\2\14\1\10"+
            "\3\14\12\12\1\7\1\11\7\14\1\6\11\14\1\4\2\14\1\2\2\14\1\3\1"+
            "\5\46\14\1\1\1\14\1\uffff\uff82\14",
            "",
            "\1\16\5\uffff\1\17",
            "\1\20",
            "\1\21",
            "\1\22",
            "\1\23",
            "",
            "",
            "",
            "\11\14\2\uffff\2\14\1\uffff\22\14\1\uffff\7\14\2\uffff\2\14"+
            "\1\uffff\3\14\12\12\2\uffff\77\14\1\uffff\1\14\1\uffff\uff82"+
            "\14",
            "\2\15\1\uffff\1\13\1\15\22\uffff\1\15",
            "",
            "",
            "\1\25",
            "\1\26",
            "\1\27",
            "\1\30",
            "\1\31",
            "\1\32",
            "",
            "\1\33",
            "\1\34",
            "\1\35",
            "\1\36",
            "\1\37",
            "\1\40",
            "\1\41",
            "\1\42",
            "\11\14\2\uffff\2\14\1\uffff\22\14\1\uffff\7\14\2\uffff\2\14"+
            "\1\uffff\15\14\2\uffff\77\14\1\uffff\1\14\1\uffff\uff82\14",
            "\1\44",
            "\1\45",
            "\1\46",
            "\11\14\2\uffff\2\14\1\uffff\22\14\1\uffff\7\14\2\uffff\2\14"+
            "\1\uffff\15\14\2\uffff\77\14\1\uffff\1\14\1\uffff\uff82\14",
            "\1\50",
            "",
            "\1\51",
            "\1\52",
            "\1\53",
            "",
            "\1\54",
            "\1\55",
            "\1\56",
            "\1\57",
            "\11\14\2\uffff\2\14\1\uffff\22\14\1\uffff\7\14\2\uffff\2\14"+
            "\1\uffff\15\14\2\uffff\77\14\1\uffff\1\14\1\uffff\uff82\14",
            "\11\14\2\uffff\2\14\1\uffff\22\14\1\uffff\7\14\2\uffff\2\14"+
            "\1\uffff\15\14\2\uffff\77\14\1\uffff\1\14\1\uffff\uff82\14",
            "\1\62",
            "\11\14\2\uffff\2\14\1\uffff\22\14\1\uffff\7\14\2\uffff\2\14"+
            "\1\uffff\15\14\2\uffff\77\14\1\uffff\1\14\1\uffff\uff82\14",
            "",
            "",
            "\1\64",
            "",
            "\1\65",
            "\11\14\2\uffff\2\14\1\uffff\22\14\1\uffff\7\14\2\uffff\2\14"+
            "\1\uffff\15\14\2\uffff\77\14\1\uffff\1\14\1\uffff\uff82\14",
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
            return "1:1: Tokens : ( COMMENT_CONTENTS | KEY_PLACE | KEY_SAFE | KEY_MARKING | KEY_TRANSITION | KEY_CONSUME | KEY_PRODUCE | COLON | COMMA | SEMICOLON | NUMBER | IDENT | WHITESPACE );";
        }
        public int specialStateTransition(int s, IntStream _input) throws NoViableAltException {
            IntStream input = _input;
        	int _s = s;
            switch ( s ) {
                    case 0 : 
                        int LA5_45 = input.LA(1);

                        s = -1;
                        if ( ((LA5_45>='\u0000' && LA5_45<='\b')||(LA5_45>='\u000B' && LA5_45<='\f')||(LA5_45>='\u000E' && LA5_45<='\u001F')||(LA5_45>='!' && LA5_45<='\'')||(LA5_45>='*' && LA5_45<='+')||(LA5_45>='-' && LA5_45<='9')||(LA5_45>='<' && LA5_45<='z')||LA5_45=='|'||(LA5_45>='~' && LA5_45<='\uFFFF')) ) {s = 12;}

                        else s = 49;

                        if ( s>=0 ) return s;
                        break;
                    case 1 : 
                        int LA5_53 = input.LA(1);

                        s = -1;
                        if ( ((LA5_53>='\u0000' && LA5_53<='\b')||(LA5_53>='\u000B' && LA5_53<='\f')||(LA5_53>='\u000E' && LA5_53<='\u001F')||(LA5_53>='!' && LA5_53<='\'')||(LA5_53>='*' && LA5_53<='+')||(LA5_53>='-' && LA5_53<='9')||(LA5_53>='<' && LA5_53<='z')||LA5_53=='|'||(LA5_53>='~' && LA5_53<='\uFFFF')) ) {s = 12;}

                        else s = 54;

                        if ( s>=0 ) return s;
                        break;
                    case 2 : 
                        int LA5_33 = input.LA(1);

                        s = -1;
                        if ( ((LA5_33>='\u0000' && LA5_33<='\b')||(LA5_33>='\u000B' && LA5_33<='\f')||(LA5_33>='\u000E' && LA5_33<='\u001F')||(LA5_33>='!' && LA5_33<='\'')||(LA5_33>='*' && LA5_33<='+')||(LA5_33>='-' && LA5_33<='9')||(LA5_33>='<' && LA5_33<='z')||LA5_33=='|'||(LA5_33>='~' && LA5_33<='\uFFFF')) ) {s = 12;}

                        else s = 39;

                        if ( s>=0 ) return s;
                        break;
                    case 3 : 
                        int LA5_47 = input.LA(1);

                        s = -1;
                        if ( ((LA5_47>='\u0000' && LA5_47<='\b')||(LA5_47>='\u000B' && LA5_47<='\f')||(LA5_47>='\u000E' && LA5_47<='\u001F')||(LA5_47>='!' && LA5_47<='\'')||(LA5_47>='*' && LA5_47<='+')||(LA5_47>='-' && LA5_47<='9')||(LA5_47>='<' && LA5_47<='z')||LA5_47=='|'||(LA5_47>='~' && LA5_47<='\uFFFF')) ) {s = 12;}

                        else s = 51;

                        if ( s>=0 ) return s;
                        break;
                    case 4 : 
                        int LA5_0 = input.LA(1);

                        s = -1;
                        if ( (LA5_0=='{') ) {s = 1;}

                        else if ( (LA5_0=='P') ) {s = 2;}

                        else if ( (LA5_0=='S') ) {s = 3;}

                        else if ( (LA5_0=='M') ) {s = 4;}

                        else if ( (LA5_0=='T') ) {s = 5;}

                        else if ( (LA5_0=='C') ) {s = 6;}

                        else if ( (LA5_0==':') ) {s = 7;}

                        else if ( (LA5_0==',') ) {s = 8;}

                        else if ( (LA5_0==';') ) {s = 9;}

                        else if ( ((LA5_0>='0' && LA5_0<='9')) ) {s = 10;}

                        else if ( (LA5_0=='\f') ) {s = 11;}

                        else if ( ((LA5_0>='\u0000' && LA5_0<='\b')||LA5_0=='\u000B'||(LA5_0>='\u000E' && LA5_0<='\u001F')||(LA5_0>='!' && LA5_0<='\'')||(LA5_0>='*' && LA5_0<='+')||(LA5_0>='-' && LA5_0<='/')||(LA5_0>='<' && LA5_0<='B')||(LA5_0>='D' && LA5_0<='L')||(LA5_0>='N' && LA5_0<='O')||(LA5_0>='Q' && LA5_0<='R')||(LA5_0>='U' && LA5_0<='z')||LA5_0=='|'||(LA5_0>='~' && LA5_0<='\uFFFF')) ) {s = 12;}

                        else if ( ((LA5_0>='\t' && LA5_0<='\n')||LA5_0=='\r'||LA5_0==' ') ) {s = 13;}

                        if ( s>=0 ) return s;
                        break;
                    case 5 : 
                        int LA5_10 = input.LA(1);

                        s = -1;
                        if ( ((LA5_10>='0' && LA5_10<='9')) ) {s = 10;}

                        else if ( ((LA5_10>='\u0000' && LA5_10<='\b')||(LA5_10>='\u000B' && LA5_10<='\f')||(LA5_10>='\u000E' && LA5_10<='\u001F')||(LA5_10>='!' && LA5_10<='\'')||(LA5_10>='*' && LA5_10<='+')||(LA5_10>='-' && LA5_10<='/')||(LA5_10>='<' && LA5_10<='z')||LA5_10=='|'||(LA5_10>='~' && LA5_10<='\uFFFF')) ) {s = 12;}

                        else s = 20;

                        if ( s>=0 ) return s;
                        break;
                    case 6 : 
                        int LA5_29 = input.LA(1);

                        s = -1;
                        if ( ((LA5_29>='\u0000' && LA5_29<='\b')||(LA5_29>='\u000B' && LA5_29<='\f')||(LA5_29>='\u000E' && LA5_29<='\u001F')||(LA5_29>='!' && LA5_29<='\'')||(LA5_29>='*' && LA5_29<='+')||(LA5_29>='-' && LA5_29<='9')||(LA5_29>='<' && LA5_29<='z')||LA5_29=='|'||(LA5_29>='~' && LA5_29<='\uFFFF')) ) {s = 12;}

                        else s = 35;

                        if ( s>=0 ) return s;
                        break;
                    case 7 : 
                        int LA5_44 = input.LA(1);

                        s = -1;
                        if ( ((LA5_44>='\u0000' && LA5_44<='\b')||(LA5_44>='\u000B' && LA5_44<='\f')||(LA5_44>='\u000E' && LA5_44<='\u001F')||(LA5_44>='!' && LA5_44<='\'')||(LA5_44>='*' && LA5_44<='+')||(LA5_44>='-' && LA5_44<='9')||(LA5_44>='<' && LA5_44<='z')||LA5_44=='|'||(LA5_44>='~' && LA5_44<='\uFFFF')) ) {s = 12;}

                        else s = 48;

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