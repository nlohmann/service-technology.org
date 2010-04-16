// $ANTLR 3.2 Sep 23, 2009 12:02:23 D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g 2010-04-16 22:38:34

    package hub.top.petrinet;


import org.antlr.runtime.*;
import java.util.Stack;
import java.util.List;
import java.util.ArrayList;

public class OWFNLexer extends Lexer {
    public static final int COMMA=13;
    public static final int IDENT=10;
    public static final int KEY_PLACE=4;
    public static final int NUMBER=11;
    public static final int KEY_FINALCONDITION=9;
    public static final int KEY_INITIALMARKING=8;
    public static final int KEY_TRANSITION=18;
    public static final int KEY_SAFE=15;
    public static final int WHITESPACE=23;
    public static final int KEY_INPUT=6;
    public static final int KEY_OUTPUT=7;
    public static final int SEMICOLON=14;
    public static final int KEY_ROLES=12;
    public static final int EOF=-1;
    public static final int KEY_INTERNAL=5;
    public static final int KEY_PRODUCE=20;
    public static final int COLON=16;
    public static final int COMMENT_CONTENTS=21;
    public static final int DIGIT=22;
    public static final int KEY_CONSUME=19;
    public static final int KEY_TRUE=17;

    // delegates
    // delegators

    public OWFNLexer() {;} 
    public OWFNLexer(CharStream input) {
        this(input, new RecognizerSharedState());
    }
    public OWFNLexer(CharStream input, RecognizerSharedState state) {
        super(input,state);

    }
    public String getGrammarFileName() { return "D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g"; }

    // $ANTLR start "COMMENT_CONTENTS"
    public final void mCOMMENT_CONTENTS() throws RecognitionException {
        try {
            int _type = COMMENT_CONTENTS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:229:18: ( '\\{' (~ ( '\\}' ) )* '\\}' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:230:2: '\\{' (~ ( '\\}' ) )* '\\}'
            {
            match('{'); 

                	_channel=98;
                
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:234:5: (~ ( '\\}' ) )*
            loop1:
            do {
                int alt1=2;
                int LA1_0 = input.LA(1);

                if ( ((LA1_0>='\u0000' && LA1_0<='|')||(LA1_0>='~' && LA1_0<='\uFFFF')) ) {
                    alt1=1;
                }


                switch (alt1) {
            	case 1 :
            	    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:234:7: ~ ( '\\}' )
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:237:11: ( 'PLACE' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:237:13: 'PLACE'
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:238:11: ( 'ROLES' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:238:13: 'ROLES'
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:239:14: ( 'INTERNAL' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:239:16: 'INTERNAL'
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:240:11: ( 'INPUT' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:240:13: 'INPUT'
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:241:12: ( 'OUTPUT' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:241:14: 'OUTPUT'
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:242:10: ( 'SAFE' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:242:12: 'SAFE'
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:243:21: ( 'INITIALMARKING' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:243:23: 'INITIALMARKING'
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:244:17: ( 'TRANSITION' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:244:19: 'TRANSITION'
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:245:14: ( 'CONSUME' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:245:16: 'CONSUME'
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:246:14: ( 'PRODUCE' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:246:16: 'PRODUCE'
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:248:21: ( 'FINALCONDITION' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:248:23: 'FINALCONDITION'
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:249:10: ( 'TRUE' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:249:12: 'TRUE'
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

    // $ANTLR start "COLON"
    public final void mCOLON() throws RecognitionException {
        try {
            int _type = COLON;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:251:7: ( ':' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:251:9: ':'
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:252:7: ( ',' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:252:9: ','
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:253:11: ( ';' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:253:13: ';'
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:255:16: ( '0' .. '9' )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:255:18: '0' .. '9'
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:256:8: ( ( DIGIT )+ )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:256:10: ( DIGIT )+
            {
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:256:10: ( DIGIT )+
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
            	    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:256:11: DIGIT
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:257:7: ( (~ ( ',' | ';' | ':' | '(' | ')' | '\\t' | ' ' | '\\n' | '\\r' | '\\{' | '\\}' ) )+ )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:257:9: (~ ( ',' | ';' | ':' | '(' | ')' | '\\t' | ' ' | '\\n' | '\\r' | '\\{' | '\\}' ) )+
            {
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:257:9: (~ ( ',' | ';' | ':' | '(' | ')' | '\\t' | ' ' | '\\n' | '\\r' | '\\{' | '\\}' ) )+
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
            	    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:257:9: ~ ( ',' | ';' | ':' | '(' | ')' | '\\t' | ' ' | '\\n' | '\\r' | '\\{' | '\\}' )
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
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:259:12: ( ( '\\t' | ' ' | '\\r' | '\\n' | '\\u000C' )+ )
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:259:14: ( '\\t' | ' ' | '\\r' | '\\n' | '\\u000C' )+
            {
            // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:259:14: ( '\\t' | ' ' | '\\r' | '\\n' | '\\u000C' )+
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
            	    // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:
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
        // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:1:8: ( COMMENT_CONTENTS | KEY_PLACE | KEY_ROLES | KEY_INTERNAL | KEY_INPUT | KEY_OUTPUT | KEY_SAFE | KEY_INITIALMARKING | KEY_TRANSITION | KEY_CONSUME | KEY_PRODUCE | KEY_FINALCONDITION | KEY_TRUE | COLON | COMMA | SEMICOLON | NUMBER | IDENT | WHITESPACE )
        int alt5=19;
        alt5 = dfa5.predict(input);
        switch (alt5) {
            case 1 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:1:10: COMMENT_CONTENTS
                {
                mCOMMENT_CONTENTS(); 

                }
                break;
            case 2 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:1:27: KEY_PLACE
                {
                mKEY_PLACE(); 

                }
                break;
            case 3 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:1:37: KEY_ROLES
                {
                mKEY_ROLES(); 

                }
                break;
            case 4 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:1:47: KEY_INTERNAL
                {
                mKEY_INTERNAL(); 

                }
                break;
            case 5 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:1:60: KEY_INPUT
                {
                mKEY_INPUT(); 

                }
                break;
            case 6 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:1:70: KEY_OUTPUT
                {
                mKEY_OUTPUT(); 

                }
                break;
            case 7 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:1:81: KEY_SAFE
                {
                mKEY_SAFE(); 

                }
                break;
            case 8 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:1:90: KEY_INITIALMARKING
                {
                mKEY_INITIALMARKING(); 

                }
                break;
            case 9 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:1:109: KEY_TRANSITION
                {
                mKEY_TRANSITION(); 

                }
                break;
            case 10 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:1:124: KEY_CONSUME
                {
                mKEY_CONSUME(); 

                }
                break;
            case 11 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:1:136: KEY_PRODUCE
                {
                mKEY_PRODUCE(); 

                }
                break;
            case 12 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:1:148: KEY_FINALCONDITION
                {
                mKEY_FINALCONDITION(); 

                }
                break;
            case 13 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:1:167: KEY_TRUE
                {
                mKEY_TRUE(); 

                }
                break;
            case 14 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:1:176: COLON
                {
                mCOLON(); 

                }
                break;
            case 15 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:1:182: COMMA
                {
                mCOMMA(); 

                }
                break;
            case 16 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:1:188: SEMICOLON
                {
                mSEMICOLON(); 

                }
                break;
            case 17 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:1:198: NUMBER
                {
                mNUMBER(); 

                }
                break;
            case 18 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:1:205: IDENT
                {
                mIDENT(); 

                }
                break;
            case 19 :
                // D:\\LinuxShared\\eclipseWorkspace-greta\\hub.top.pnapi\\src\\hub\\top\\petrinet\\OWFN.g:1:211: WHITESPACE
                {
                mWHITESPACE(); 

                }
                break;

        }

    }


    protected DFA5 dfa5 = new DFA5(this);
    static final String DFA5_eotS =
        "\2\uffff\10\17\3\uffff\1\32\1\17\2\uffff\11\17\1\uffff\23\17\1"+
        "\72\1\17\1\74\2\17\1\77\1\17\1\101\1\17\1\103\2\17\1\uffff\1\17"+
        "\1\uffff\2\17\1\uffff\1\17\1\uffff\1\17\1\uffff\1\17\1\114\3\17"+
        "\1\120\2\17\1\uffff\1\17\1\124\1\17\1\uffff\1\126\2\17\1\uffff\1"+
        "\17\1\uffff\4\17\1\136\2\17\1\uffff\5\17\1\146\1\147\2\uffff";
    static final String DFA5_eofS =
        "\150\uffff";
    static final String DFA5_minS =
        "\1\0\1\uffff\1\114\1\117\1\116\1\125\1\101\1\122\1\117\1\111\3"+
        "\uffff\1\0\1\11\2\uffff\1\101\1\117\1\114\1\111\1\124\1\106\1\101"+
        "\2\116\1\uffff\1\103\1\104\2\105\1\125\1\124\1\120\1\105\1\116\1"+
        "\105\1\123\1\101\1\105\1\125\1\123\1\122\1\124\1\111\1\125\1\0\1"+
        "\123\1\0\1\125\1\114\1\0\1\103\1\0\1\116\1\0\1\101\1\124\1\uffff"+
        "\1\111\1\uffff\1\115\1\103\1\uffff\1\105\1\uffff\1\101\1\uffff\1"+
        "\114\1\0\1\124\1\105\1\117\1\0\1\114\1\115\1\uffff\1\111\1\0\1\116"+
        "\1\uffff\1\0\1\101\1\117\1\uffff\1\104\1\uffff\1\122\1\116\1\111"+
        "\1\113\1\0\1\124\1\111\1\uffff\1\111\1\116\1\117\1\107\1\116\2\0"+
        "\2\uffff";
    static final String DFA5_maxS =
        "\1\uffff\1\uffff\1\122\1\117\1\116\1\125\1\101\1\122\1\117\1\111"+
        "\3\uffff\1\uffff\1\40\2\uffff\1\101\1\117\1\114\2\124\1\106\1\125"+
        "\2\116\1\uffff\1\103\1\104\2\105\1\125\1\124\1\120\1\105\1\116\1"+
        "\105\1\123\1\101\1\105\1\125\1\123\1\122\1\124\1\111\1\125\1\uffff"+
        "\1\123\1\uffff\1\125\1\114\1\uffff\1\103\1\uffff\1\116\1\uffff\1"+
        "\101\1\124\1\uffff\1\111\1\uffff\1\115\1\103\1\uffff\1\105\1\uffff"+
        "\1\101\1\uffff\1\114\1\uffff\1\124\1\105\1\117\1\uffff\1\114\1\115"+
        "\1\uffff\1\111\1\uffff\1\116\1\uffff\1\uffff\1\101\1\117\1\uffff"+
        "\1\104\1\uffff\1\122\1\116\1\111\1\113\1\uffff\1\124\1\111\1\uffff"+
        "\1\111\1\116\1\117\1\107\1\116\2\uffff\2\uffff";
    static final String DFA5_acceptS =
        "\1\uffff\1\1\10\uffff\1\16\1\17\1\20\2\uffff\1\22\1\23\11\uffff"+
        "\1\21\37\uffff\1\7\1\uffff\1\15\2\uffff\1\2\1\uffff\1\3\1\uffff"+
        "\1\5\10\uffff\1\6\3\uffff\1\13\3\uffff\1\12\1\uffff\1\4\7\uffff"+
        "\1\11\7\uffff\1\10\1\14";
    static final String DFA5_specialS =
        "\1\13\14\uffff\1\2\40\uffff\1\6\1\uffff\1\3\2\uffff\1\7\1\uffff"+
        "\1\11\1\uffff\1\4\15\uffff\1\1\3\uffff\1\14\4\uffff\1\5\2\uffff"+
        "\1\0\11\uffff\1\15\10\uffff\1\10\1\12\2\uffff}>";
    static final String[] DFA5_transitionS = {
            "\11\17\2\20\1\17\1\16\1\20\22\17\1\20\7\17\2\uffff\2\17\1\13"+
            "\3\17\12\15\1\12\1\14\7\17\1\10\2\17\1\11\2\17\1\4\5\17\1\5"+
            "\1\2\1\17\1\3\1\6\1\7\46\17\1\1\1\17\1\uffff\uff82\17",
            "",
            "\1\21\5\uffff\1\22",
            "\1\23",
            "\1\24",
            "\1\25",
            "\1\26",
            "\1\27",
            "\1\30",
            "\1\31",
            "",
            "",
            "",
            "\11\17\2\uffff\2\17\1\uffff\22\17\1\uffff\7\17\2\uffff\2\17"+
            "\1\uffff\3\17\12\15\2\uffff\77\17\1\uffff\1\17\1\uffff\uff82"+
            "\17",
            "\2\20\1\uffff\1\16\1\20\22\uffff\1\20",
            "",
            "",
            "\1\33",
            "\1\34",
            "\1\35",
            "\1\40\6\uffff\1\37\3\uffff\1\36",
            "\1\41",
            "\1\42",
            "\1\43\23\uffff\1\44",
            "\1\45",
            "\1\46",
            "",
            "\1\47",
            "\1\50",
            "\1\51",
            "\1\52",
            "\1\53",
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
            "\1\70",
            "\1\71",
            "\11\17\2\uffff\2\17\1\uffff\22\17\1\uffff\7\17\2\uffff\2\17"+
            "\1\uffff\15\17\2\uffff\77\17\1\uffff\1\17\1\uffff\uff82\17",
            "\1\73",
            "\11\17\2\uffff\2\17\1\uffff\22\17\1\uffff\7\17\2\uffff\2\17"+
            "\1\uffff\15\17\2\uffff\77\17\1\uffff\1\17\1\uffff\uff82\17",
            "\1\75",
            "\1\76",
            "\11\17\2\uffff\2\17\1\uffff\22\17\1\uffff\7\17\2\uffff\2\17"+
            "\1\uffff\15\17\2\uffff\77\17\1\uffff\1\17\1\uffff\uff82\17",
            "\1\100",
            "\11\17\2\uffff\2\17\1\uffff\22\17\1\uffff\7\17\2\uffff\2\17"+
            "\1\uffff\15\17\2\uffff\77\17\1\uffff\1\17\1\uffff\uff82\17",
            "\1\102",
            "\11\17\2\uffff\2\17\1\uffff\22\17\1\uffff\7\17\2\uffff\2\17"+
            "\1\uffff\15\17\2\uffff\77\17\1\uffff\1\17\1\uffff\uff82\17",
            "\1\104",
            "\1\105",
            "",
            "\1\106",
            "",
            "\1\107",
            "\1\110",
            "",
            "\1\111",
            "",
            "\1\112",
            "",
            "\1\113",
            "\11\17\2\uffff\2\17\1\uffff\22\17\1\uffff\7\17\2\uffff\2\17"+
            "\1\uffff\15\17\2\uffff\77\17\1\uffff\1\17\1\uffff\uff82\17",
            "\1\115",
            "\1\116",
            "\1\117",
            "\11\17\2\uffff\2\17\1\uffff\22\17\1\uffff\7\17\2\uffff\2\17"+
            "\1\uffff\15\17\2\uffff\77\17\1\uffff\1\17\1\uffff\uff82\17",
            "\1\121",
            "\1\122",
            "",
            "\1\123",
            "\11\17\2\uffff\2\17\1\uffff\22\17\1\uffff\7\17\2\uffff\2\17"+
            "\1\uffff\15\17\2\uffff\77\17\1\uffff\1\17\1\uffff\uff82\17",
            "\1\125",
            "",
            "\11\17\2\uffff\2\17\1\uffff\22\17\1\uffff\7\17\2\uffff\2\17"+
            "\1\uffff\15\17\2\uffff\77\17\1\uffff\1\17\1\uffff\uff82\17",
            "\1\127",
            "\1\130",
            "",
            "\1\131",
            "",
            "\1\132",
            "\1\133",
            "\1\134",
            "\1\135",
            "\11\17\2\uffff\2\17\1\uffff\22\17\1\uffff\7\17\2\uffff\2\17"+
            "\1\uffff\15\17\2\uffff\77\17\1\uffff\1\17\1\uffff\uff82\17",
            "\1\137",
            "\1\140",
            "",
            "\1\141",
            "\1\142",
            "\1\143",
            "\1\144",
            "\1\145",
            "\11\17\2\uffff\2\17\1\uffff\22\17\1\uffff\7\17\2\uffff\2\17"+
            "\1\uffff\15\17\2\uffff\77\17\1\uffff\1\17\1\uffff\uff82\17",
            "\11\17\2\uffff\2\17\1\uffff\22\17\1\uffff\7\17\2\uffff\2\17"+
            "\1\uffff\15\17\2\uffff\77\17\1\uffff\1\17\1\uffff\uff82\17",
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
            return "1:1: Tokens : ( COMMENT_CONTENTS | KEY_PLACE | KEY_ROLES | KEY_INTERNAL | KEY_INPUT | KEY_OUTPUT | KEY_SAFE | KEY_INITIALMARKING | KEY_TRANSITION | KEY_CONSUME | KEY_PRODUCE | KEY_FINALCONDITION | KEY_TRUE | COLON | COMMA | SEMICOLON | NUMBER | IDENT | WHITESPACE );";
        }
        public int specialStateTransition(int s, IntStream _input) throws NoViableAltException {
            IntStream input = _input;
        	int _s = s;
            switch ( s ) {
                    case 0 : 
                        int LA5_81 = input.LA(1);

                        s = -1;
                        if ( ((LA5_81>='\u0000' && LA5_81<='\b')||(LA5_81>='\u000B' && LA5_81<='\f')||(LA5_81>='\u000E' && LA5_81<='\u001F')||(LA5_81>='!' && LA5_81<='\'')||(LA5_81>='*' && LA5_81<='+')||(LA5_81>='-' && LA5_81<='9')||(LA5_81>='<' && LA5_81<='z')||LA5_81=='|'||(LA5_81>='~' && LA5_81<='\uFFFF')) ) {s = 15;}

                        else s = 86;

                        if ( s>=0 ) return s;
                        break;
                    case 1 : 
                        int LA5_69 = input.LA(1);

                        s = -1;
                        if ( ((LA5_69>='\u0000' && LA5_69<='\b')||(LA5_69>='\u000B' && LA5_69<='\f')||(LA5_69>='\u000E' && LA5_69<='\u001F')||(LA5_69>='!' && LA5_69<='\'')||(LA5_69>='*' && LA5_69<='+')||(LA5_69>='-' && LA5_69<='9')||(LA5_69>='<' && LA5_69<='z')||LA5_69=='|'||(LA5_69>='~' && LA5_69<='\uFFFF')) ) {s = 15;}

                        else s = 76;

                        if ( s>=0 ) return s;
                        break;
                    case 2 : 
                        int LA5_13 = input.LA(1);

                        s = -1;
                        if ( ((LA5_13>='0' && LA5_13<='9')) ) {s = 13;}

                        else if ( ((LA5_13>='\u0000' && LA5_13<='\b')||(LA5_13>='\u000B' && LA5_13<='\f')||(LA5_13>='\u000E' && LA5_13<='\u001F')||(LA5_13>='!' && LA5_13<='\'')||(LA5_13>='*' && LA5_13<='+')||(LA5_13>='-' && LA5_13<='/')||(LA5_13>='<' && LA5_13<='z')||LA5_13=='|'||(LA5_13>='~' && LA5_13<='\uFFFF')) ) {s = 15;}

                        else s = 26;

                        if ( s>=0 ) return s;
                        break;
                    case 3 : 
                        int LA5_48 = input.LA(1);

                        s = -1;
                        if ( ((LA5_48>='\u0000' && LA5_48<='\b')||(LA5_48>='\u000B' && LA5_48<='\f')||(LA5_48>='\u000E' && LA5_48<='\u001F')||(LA5_48>='!' && LA5_48<='\'')||(LA5_48>='*' && LA5_48<='+')||(LA5_48>='-' && LA5_48<='9')||(LA5_48>='<' && LA5_48<='z')||LA5_48=='|'||(LA5_48>='~' && LA5_48<='\uFFFF')) ) {s = 15;}

                        else s = 60;

                        if ( s>=0 ) return s;
                        break;
                    case 4 : 
                        int LA5_55 = input.LA(1);

                        s = -1;
                        if ( ((LA5_55>='\u0000' && LA5_55<='\b')||(LA5_55>='\u000B' && LA5_55<='\f')||(LA5_55>='\u000E' && LA5_55<='\u001F')||(LA5_55>='!' && LA5_55<='\'')||(LA5_55>='*' && LA5_55<='+')||(LA5_55>='-' && LA5_55<='9')||(LA5_55>='<' && LA5_55<='z')||LA5_55=='|'||(LA5_55>='~' && LA5_55<='\uFFFF')) ) {s = 15;}

                        else s = 67;

                        if ( s>=0 ) return s;
                        break;
                    case 5 : 
                        int LA5_78 = input.LA(1);

                        s = -1;
                        if ( ((LA5_78>='\u0000' && LA5_78<='\b')||(LA5_78>='\u000B' && LA5_78<='\f')||(LA5_78>='\u000E' && LA5_78<='\u001F')||(LA5_78>='!' && LA5_78<='\'')||(LA5_78>='*' && LA5_78<='+')||(LA5_78>='-' && LA5_78<='9')||(LA5_78>='<' && LA5_78<='z')||LA5_78=='|'||(LA5_78>='~' && LA5_78<='\uFFFF')) ) {s = 15;}

                        else s = 84;

                        if ( s>=0 ) return s;
                        break;
                    case 6 : 
                        int LA5_46 = input.LA(1);

                        s = -1;
                        if ( ((LA5_46>='\u0000' && LA5_46<='\b')||(LA5_46>='\u000B' && LA5_46<='\f')||(LA5_46>='\u000E' && LA5_46<='\u001F')||(LA5_46>='!' && LA5_46<='\'')||(LA5_46>='*' && LA5_46<='+')||(LA5_46>='-' && LA5_46<='9')||(LA5_46>='<' && LA5_46<='z')||LA5_46=='|'||(LA5_46>='~' && LA5_46<='\uFFFF')) ) {s = 15;}

                        else s = 58;

                        if ( s>=0 ) return s;
                        break;
                    case 7 : 
                        int LA5_51 = input.LA(1);

                        s = -1;
                        if ( ((LA5_51>='\u0000' && LA5_51<='\b')||(LA5_51>='\u000B' && LA5_51<='\f')||(LA5_51>='\u000E' && LA5_51<='\u001F')||(LA5_51>='!' && LA5_51<='\'')||(LA5_51>='*' && LA5_51<='+')||(LA5_51>='-' && LA5_51<='9')||(LA5_51>='<' && LA5_51<='z')||LA5_51=='|'||(LA5_51>='~' && LA5_51<='\uFFFF')) ) {s = 15;}

                        else s = 63;

                        if ( s>=0 ) return s;
                        break;
                    case 8 : 
                        int LA5_100 = input.LA(1);

                        s = -1;
                        if ( ((LA5_100>='\u0000' && LA5_100<='\b')||(LA5_100>='\u000B' && LA5_100<='\f')||(LA5_100>='\u000E' && LA5_100<='\u001F')||(LA5_100>='!' && LA5_100<='\'')||(LA5_100>='*' && LA5_100<='+')||(LA5_100>='-' && LA5_100<='9')||(LA5_100>='<' && LA5_100<='z')||LA5_100=='|'||(LA5_100>='~' && LA5_100<='\uFFFF')) ) {s = 15;}

                        else s = 102;

                        if ( s>=0 ) return s;
                        break;
                    case 9 : 
                        int LA5_53 = input.LA(1);

                        s = -1;
                        if ( ((LA5_53>='\u0000' && LA5_53<='\b')||(LA5_53>='\u000B' && LA5_53<='\f')||(LA5_53>='\u000E' && LA5_53<='\u001F')||(LA5_53>='!' && LA5_53<='\'')||(LA5_53>='*' && LA5_53<='+')||(LA5_53>='-' && LA5_53<='9')||(LA5_53>='<' && LA5_53<='z')||LA5_53=='|'||(LA5_53>='~' && LA5_53<='\uFFFF')) ) {s = 15;}

                        else s = 65;

                        if ( s>=0 ) return s;
                        break;
                    case 10 : 
                        int LA5_101 = input.LA(1);

                        s = -1;
                        if ( ((LA5_101>='\u0000' && LA5_101<='\b')||(LA5_101>='\u000B' && LA5_101<='\f')||(LA5_101>='\u000E' && LA5_101<='\u001F')||(LA5_101>='!' && LA5_101<='\'')||(LA5_101>='*' && LA5_101<='+')||(LA5_101>='-' && LA5_101<='9')||(LA5_101>='<' && LA5_101<='z')||LA5_101=='|'||(LA5_101>='~' && LA5_101<='\uFFFF')) ) {s = 15;}

                        else s = 103;

                        if ( s>=0 ) return s;
                        break;
                    case 11 : 
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

                        else if ( (LA5_0==':') ) {s = 10;}

                        else if ( (LA5_0==',') ) {s = 11;}

                        else if ( (LA5_0==';') ) {s = 12;}

                        else if ( ((LA5_0>='0' && LA5_0<='9')) ) {s = 13;}

                        else if ( (LA5_0=='\f') ) {s = 14;}

                        else if ( ((LA5_0>='\u0000' && LA5_0<='\b')||LA5_0=='\u000B'||(LA5_0>='\u000E' && LA5_0<='\u001F')||(LA5_0>='!' && LA5_0<='\'')||(LA5_0>='*' && LA5_0<='+')||(LA5_0>='-' && LA5_0<='/')||(LA5_0>='<' && LA5_0<='B')||(LA5_0>='D' && LA5_0<='E')||(LA5_0>='G' && LA5_0<='H')||(LA5_0>='J' && LA5_0<='N')||LA5_0=='Q'||(LA5_0>='U' && LA5_0<='z')||LA5_0=='|'||(LA5_0>='~' && LA5_0<='\uFFFF')) ) {s = 15;}

                        else if ( ((LA5_0>='\t' && LA5_0<='\n')||LA5_0=='\r'||LA5_0==' ') ) {s = 16;}

                        if ( s>=0 ) return s;
                        break;
                    case 12 : 
                        int LA5_73 = input.LA(1);

                        s = -1;
                        if ( ((LA5_73>='\u0000' && LA5_73<='\b')||(LA5_73>='\u000B' && LA5_73<='\f')||(LA5_73>='\u000E' && LA5_73<='\u001F')||(LA5_73>='!' && LA5_73<='\'')||(LA5_73>='*' && LA5_73<='+')||(LA5_73>='-' && LA5_73<='9')||(LA5_73>='<' && LA5_73<='z')||LA5_73=='|'||(LA5_73>='~' && LA5_73<='\uFFFF')) ) {s = 15;}

                        else s = 80;

                        if ( s>=0 ) return s;
                        break;
                    case 13 : 
                        int LA5_91 = input.LA(1);

                        s = -1;
                        if ( ((LA5_91>='\u0000' && LA5_91<='\b')||(LA5_91>='\u000B' && LA5_91<='\f')||(LA5_91>='\u000E' && LA5_91<='\u001F')||(LA5_91>='!' && LA5_91<='\'')||(LA5_91>='*' && LA5_91<='+')||(LA5_91>='-' && LA5_91<='9')||(LA5_91>='<' && LA5_91<='z')||LA5_91=='|'||(LA5_91>='~' && LA5_91<='\uFFFF')) ) {s = 15;}

                        else s = 94;

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