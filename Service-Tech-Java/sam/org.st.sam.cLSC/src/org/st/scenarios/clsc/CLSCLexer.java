// $ANTLR 3.2 Sep 23, 2009 12:02:23 D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g 2012-06-08 09:16:31

    package org.st.scenarios.clsc;


import org.antlr.runtime.*;
import java.util.Stack;
import java.util.List;
import java.util.ArrayList;

public class CLSCLexer extends Lexer {
    public static final int COLON=16;
    public static final int KEY_COMPONENT=14;
    public static final int COMMA=13;
    public static final int KEY_EVENT=9;
    public static final int NUMBER=5;
    public static final int IDENT=4;
    public static final int WHITESPACE=18;
    public static final int SEMICOLON=10;
    public static final int DEPENDS=12;
    public static final int DIGIT=17;
    public static final int COMMENT_CONTENTS=15;
    public static final int KEY_PRECHART=7;
    public static final int KEY_DEPENDENCIES=11;
    public static final int EOF=-1;
    public static final int KEY_CLSC=6;
    public static final int KEY_MAINCHART=8;

    // delegates
    // delegators

    public CLSCLexer() {;} 
    public CLSCLexer(CharStream input) {
        this(input, new RecognizerSharedState());
    }
    public CLSCLexer(CharStream input, RecognizerSharedState state) {
        super(input,state);

    }
    public String getGrammarFileName() { return "D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g"; }

    // $ANTLR start "COMMENT_CONTENTS"
    public final void mCOMMENT_CONTENTS() throws RecognitionException {
        try {
            int _type = COMMENT_CONTENTS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:176:18: ( '\\{' (~ ( '\\}' ) )* '\\}' )
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:177:2: '\\{' (~ ( '\\}' ) )* '\\}'
            {
            match('{'); 

                	_channel=98;
                
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:181:5: (~ ( '\\}' ) )*
            loop1:
            do {
                int alt1=2;
                int LA1_0 = input.LA(1);

                if ( ((LA1_0>='\u0000' && LA1_0<='|')||(LA1_0>='~' && LA1_0<='\uFFFF')) ) {
                    alt1=1;
                }


                switch (alt1) {
            	case 1 :
            	    // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:181:7: ~ ( '\\}' )
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

    // $ANTLR start "KEY_CLSC"
    public final void mKEY_CLSC() throws RecognitionException {
        try {
            int _type = KEY_CLSC;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:184:10: ( 'cLSC' )
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:184:12: 'cLSC'
            {
            match("cLSC"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_CLSC"

    // $ANTLR start "KEY_PRECHART"
    public final void mKEY_PRECHART() throws RecognitionException {
        try {
            int _type = KEY_PRECHART;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:185:14: ( 'PRE' )
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:185:16: 'PRE'
            {
            match("PRE"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_PRECHART"

    // $ANTLR start "KEY_MAINCHART"
    public final void mKEY_MAINCHART() throws RecognitionException {
        try {
            int _type = KEY_MAINCHART;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:186:15: ( 'MAIN' )
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:186:17: 'MAIN'
            {
            match("MAIN"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_MAINCHART"

    // $ANTLR start "KEY_EVENT"
    public final void mKEY_EVENT() throws RecognitionException {
        try {
            int _type = KEY_EVENT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:187:11: ( 'EVENT' )
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:187:13: 'EVENT'
            {
            match("EVENT"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_EVENT"

    // $ANTLR start "KEY_DEPENDENCIES"
    public final void mKEY_DEPENDENCIES() throws RecognitionException {
        try {
            int _type = KEY_DEPENDENCIES;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:188:18: ( 'DEPENDENCIES' )
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:188:20: 'DEPENDENCIES'
            {
            match("DEPENDENCIES"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_DEPENDENCIES"

    // $ANTLR start "DEPENDS"
    public final void mDEPENDS() throws RecognitionException {
        try {
            int _type = DEPENDS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:189:9: ( '->' )
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:189:11: '->'
            {
            match("->"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "DEPENDS"

    // $ANTLR start "KEY_COMPONENT"
    public final void mKEY_COMPONENT() throws RecognitionException {
        try {
            int _type = KEY_COMPONENT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:191:15: ( 'COMPONENT' )
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:191:17: 'COMPONENT'
            {
            match("COMPONENT"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "KEY_COMPONENT"

    // $ANTLR start "COLON"
    public final void mCOLON() throws RecognitionException {
        try {
            int _type = COLON;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:193:7: ( ':' )
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:193:9: ':'
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
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:194:7: ( ',' )
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:194:9: ','
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
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:195:11: ( ';' )
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:195:13: ';'
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
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:197:16: ( '0' .. '9' )
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:197:18: '0' .. '9'
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
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:198:8: ( ( DIGIT )+ )
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:198:10: ( DIGIT )+
            {
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:198:10: ( DIGIT )+
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
            	    // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:198:11: DIGIT
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
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:199:7: ( (~ ( ',' | ';' | ':' | '(' | ')' | '\\t' | ' ' | '\\n' | '\\r' | '\\{' | '\\}' ) )+ )
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:199:9: (~ ( ',' | ';' | ':' | '(' | ')' | '\\t' | ' ' | '\\n' | '\\r' | '\\{' | '\\}' ) )+
            {
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:199:9: (~ ( ',' | ';' | ':' | '(' | ')' | '\\t' | ' ' | '\\n' | '\\r' | '\\{' | '\\}' ) )+
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
            	    // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:199:9: ~ ( ',' | ';' | ':' | '(' | ')' | '\\t' | ' ' | '\\n' | '\\r' | '\\{' | '\\}' )
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
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:201:12: ( ( '\\t' | ' ' | '\\r' | '\\n' | '\\u000C' )+ )
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:201:14: ( '\\t' | ' ' | '\\r' | '\\n' | '\\u000C' )+
            {
            // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:201:14: ( '\\t' | ' ' | '\\r' | '\\n' | '\\u000C' )+
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
            	    // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:
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
        // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:1:8: ( COMMENT_CONTENTS | KEY_CLSC | KEY_PRECHART | KEY_MAINCHART | KEY_EVENT | KEY_DEPENDENCIES | DEPENDS | KEY_COMPONENT | COLON | COMMA | SEMICOLON | NUMBER | IDENT | WHITESPACE )
        int alt5=14;
        alt5 = dfa5.predict(input);
        switch (alt5) {
            case 1 :
                // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:1:10: COMMENT_CONTENTS
                {
                mCOMMENT_CONTENTS(); 

                }
                break;
            case 2 :
                // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:1:27: KEY_CLSC
                {
                mKEY_CLSC(); 

                }
                break;
            case 3 :
                // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:1:36: KEY_PRECHART
                {
                mKEY_PRECHART(); 

                }
                break;
            case 4 :
                // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:1:49: KEY_MAINCHART
                {
                mKEY_MAINCHART(); 

                }
                break;
            case 5 :
                // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:1:63: KEY_EVENT
                {
                mKEY_EVENT(); 

                }
                break;
            case 6 :
                // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:1:73: KEY_DEPENDENCIES
                {
                mKEY_DEPENDENCIES(); 

                }
                break;
            case 7 :
                // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:1:90: DEPENDS
                {
                mDEPENDS(); 

                }
                break;
            case 8 :
                // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:1:98: KEY_COMPONENT
                {
                mKEY_COMPONENT(); 

                }
                break;
            case 9 :
                // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:1:112: COLON
                {
                mCOLON(); 

                }
                break;
            case 10 :
                // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:1:118: COMMA
                {
                mCOMMA(); 

                }
                break;
            case 11 :
                // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:1:124: SEMICOLON
                {
                mSEMICOLON(); 

                }
                break;
            case 12 :
                // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:1:134: NUMBER
                {
                mNUMBER(); 

                }
                break;
            case 13 :
                // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:1:141: IDENT
                {
                mIDENT(); 

                }
                break;
            case 14 :
                // D:\\LinuxShared\\eclipseWorkspace-cLSC\\org.st.sam.cLSC\\src\\org\\st\\scenarios\\clsc\\CLSC.g:1:147: WHITESPACE
                {
                mWHITESPACE(); 

                }
                break;

        }

    }


    protected DFA5 dfa5 = new DFA5(this);
    static final String DFA5_eotS =
        "\2\uffff\7\16\3\uffff\1\27\1\16\2\uffff\5\16\1\35\1\16\1\uffff"+
        "\1\16\1\40\3\16\1\uffff\1\16\1\45\1\uffff\1\46\3\16\2\uffff\1\52"+
        "\2\16\1\uffff\7\16\1\64\1\16\1\uffff\1\16\1\67\1\uffff";
    static final String DFA5_eofS =
        "\70\uffff";
    static final String DFA5_minS =
        "\1\0\1\uffff\1\114\1\122\1\101\1\126\1\105\1\76\1\117\3\uffff\1"+
        "\0\1\11\2\uffff\1\123\1\105\1\111\1\105\1\120\1\0\1\115\1\uffff"+
        "\1\103\1\0\2\116\1\105\1\uffff\1\120\1\0\1\uffff\1\0\1\124\1\116"+
        "\1\117\2\uffff\1\0\1\104\1\116\1\uffff\2\105\2\116\1\103\1\124\1"+
        "\111\1\0\1\105\1\uffff\1\123\1\0\1\uffff";
    static final String DFA5_maxS =
        "\1\uffff\1\uffff\1\114\1\122\1\101\1\126\1\105\1\76\1\117\3\uffff"+
        "\1\uffff\1\40\2\uffff\1\123\1\105\1\111\1\105\1\120\1\uffff\1\115"+
        "\1\uffff\1\103\1\uffff\2\116\1\105\1\uffff\1\120\1\uffff\1\uffff"+
        "\1\uffff\1\124\1\116\1\117\2\uffff\1\uffff\1\104\1\116\1\uffff\2"+
        "\105\2\116\1\103\1\124\1\111\1\uffff\1\105\1\uffff\1\123\1\uffff"+
        "\1\uffff";
    static final String DFA5_acceptS =
        "\1\uffff\1\1\7\uffff\1\11\1\12\1\13\2\uffff\1\15\1\16\7\uffff\1"+
        "\14\5\uffff\1\7\2\uffff\1\3\4\uffff\1\2\1\4\3\uffff\1\5\11\uffff"+
        "\1\10\2\uffff\1\6";
    static final String DFA5_specialS =
        "\1\2\13\uffff\1\1\10\uffff\1\10\3\uffff\1\4\5\uffff\1\0\1\uffff"+
        "\1\3\5\uffff\1\6\12\uffff\1\7\3\uffff\1\5\1\uffff}>";
    static final String[] DFA5_transitionS = {
            "\11\16\2\17\1\16\1\15\1\17\22\16\1\17\7\16\2\uffff\2\16\1\12"+
            "\1\7\2\16\12\14\1\11\1\13\7\16\1\10\1\6\1\5\7\16\1\4\2\16\1"+
            "\3\22\16\1\2\27\16\1\1\1\16\1\uffff\uff82\16",
            "",
            "\1\20",
            "\1\21",
            "\1\22",
            "\1\23",
            "\1\24",
            "\1\25",
            "\1\26",
            "",
            "",
            "",
            "\11\16\2\uffff\2\16\1\uffff\22\16\1\uffff\7\16\2\uffff\2\16"+
            "\1\uffff\3\16\12\14\2\uffff\77\16\1\uffff\1\16\1\uffff\uff82"+
            "\16",
            "\2\17\1\uffff\1\15\1\17\22\uffff\1\17",
            "",
            "",
            "\1\30",
            "\1\31",
            "\1\32",
            "\1\33",
            "\1\34",
            "\11\16\2\uffff\2\16\1\uffff\22\16\1\uffff\7\16\2\uffff\2\16"+
            "\1\uffff\15\16\2\uffff\77\16\1\uffff\1\16\1\uffff\uff82\16",
            "\1\36",
            "",
            "\1\37",
            "\11\16\2\uffff\2\16\1\uffff\22\16\1\uffff\7\16\2\uffff\2\16"+
            "\1\uffff\15\16\2\uffff\77\16\1\uffff\1\16\1\uffff\uff82\16",
            "\1\41",
            "\1\42",
            "\1\43",
            "",
            "\1\44",
            "\11\16\2\uffff\2\16\1\uffff\22\16\1\uffff\7\16\2\uffff\2\16"+
            "\1\uffff\15\16\2\uffff\77\16\1\uffff\1\16\1\uffff\uff82\16",
            "",
            "\11\16\2\uffff\2\16\1\uffff\22\16\1\uffff\7\16\2\uffff\2\16"+
            "\1\uffff\15\16\2\uffff\77\16\1\uffff\1\16\1\uffff\uff82\16",
            "\1\47",
            "\1\50",
            "\1\51",
            "",
            "",
            "\11\16\2\uffff\2\16\1\uffff\22\16\1\uffff\7\16\2\uffff\2\16"+
            "\1\uffff\15\16\2\uffff\77\16\1\uffff\1\16\1\uffff\uff82\16",
            "\1\53",
            "\1\54",
            "",
            "\1\55",
            "\1\56",
            "\1\57",
            "\1\60",
            "\1\61",
            "\1\62",
            "\1\63",
            "\11\16\2\uffff\2\16\1\uffff\22\16\1\uffff\7\16\2\uffff\2\16"+
            "\1\uffff\15\16\2\uffff\77\16\1\uffff\1\16\1\uffff\uff82\16",
            "\1\65",
            "",
            "\1\66",
            "\11\16\2\uffff\2\16\1\uffff\22\16\1\uffff\7\16\2\uffff\2\16"+
            "\1\uffff\15\16\2\uffff\77\16\1\uffff\1\16\1\uffff\uff82\16",
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
            return "1:1: Tokens : ( COMMENT_CONTENTS | KEY_CLSC | KEY_PRECHART | KEY_MAINCHART | KEY_EVENT | KEY_DEPENDENCIES | DEPENDS | KEY_COMPONENT | COLON | COMMA | SEMICOLON | NUMBER | IDENT | WHITESPACE );";
        }
        public int specialStateTransition(int s, IntStream _input) throws NoViableAltException {
            IntStream input = _input;
        	int _s = s;
            switch ( s ) {
                    case 0 : 
                        int LA5_31 = input.LA(1);

                        s = -1;
                        if ( ((LA5_31>='\u0000' && LA5_31<='\b')||(LA5_31>='\u000B' && LA5_31<='\f')||(LA5_31>='\u000E' && LA5_31<='\u001F')||(LA5_31>='!' && LA5_31<='\'')||(LA5_31>='*' && LA5_31<='+')||(LA5_31>='-' && LA5_31<='9')||(LA5_31>='<' && LA5_31<='z')||LA5_31=='|'||(LA5_31>='~' && LA5_31<='\uFFFF')) ) {s = 14;}

                        else s = 37;

                        if ( s>=0 ) return s;
                        break;
                    case 1 : 
                        int LA5_12 = input.LA(1);

                        s = -1;
                        if ( ((LA5_12>='0' && LA5_12<='9')) ) {s = 12;}

                        else if ( ((LA5_12>='\u0000' && LA5_12<='\b')||(LA5_12>='\u000B' && LA5_12<='\f')||(LA5_12>='\u000E' && LA5_12<='\u001F')||(LA5_12>='!' && LA5_12<='\'')||(LA5_12>='*' && LA5_12<='+')||(LA5_12>='-' && LA5_12<='/')||(LA5_12>='<' && LA5_12<='z')||LA5_12=='|'||(LA5_12>='~' && LA5_12<='\uFFFF')) ) {s = 14;}

                        else s = 23;

                        if ( s>=0 ) return s;
                        break;
                    case 2 : 
                        int LA5_0 = input.LA(1);

                        s = -1;
                        if ( (LA5_0=='{') ) {s = 1;}

                        else if ( (LA5_0=='c') ) {s = 2;}

                        else if ( (LA5_0=='P') ) {s = 3;}

                        else if ( (LA5_0=='M') ) {s = 4;}

                        else if ( (LA5_0=='E') ) {s = 5;}

                        else if ( (LA5_0=='D') ) {s = 6;}

                        else if ( (LA5_0=='-') ) {s = 7;}

                        else if ( (LA5_0=='C') ) {s = 8;}

                        else if ( (LA5_0==':') ) {s = 9;}

                        else if ( (LA5_0==',') ) {s = 10;}

                        else if ( (LA5_0==';') ) {s = 11;}

                        else if ( ((LA5_0>='0' && LA5_0<='9')) ) {s = 12;}

                        else if ( (LA5_0=='\f') ) {s = 13;}

                        else if ( ((LA5_0>='\u0000' && LA5_0<='\b')||LA5_0=='\u000B'||(LA5_0>='\u000E' && LA5_0<='\u001F')||(LA5_0>='!' && LA5_0<='\'')||(LA5_0>='*' && LA5_0<='+')||(LA5_0>='.' && LA5_0<='/')||(LA5_0>='<' && LA5_0<='B')||(LA5_0>='F' && LA5_0<='L')||(LA5_0>='N' && LA5_0<='O')||(LA5_0>='Q' && LA5_0<='b')||(LA5_0>='d' && LA5_0<='z')||LA5_0=='|'||(LA5_0>='~' && LA5_0<='\uFFFF')) ) {s = 14;}

                        else if ( ((LA5_0>='\t' && LA5_0<='\n')||LA5_0=='\r'||LA5_0==' ') ) {s = 15;}

                        if ( s>=0 ) return s;
                        break;
                    case 3 : 
                        int LA5_33 = input.LA(1);

                        s = -1;
                        if ( ((LA5_33>='\u0000' && LA5_33<='\b')||(LA5_33>='\u000B' && LA5_33<='\f')||(LA5_33>='\u000E' && LA5_33<='\u001F')||(LA5_33>='!' && LA5_33<='\'')||(LA5_33>='*' && LA5_33<='+')||(LA5_33>='-' && LA5_33<='9')||(LA5_33>='<' && LA5_33<='z')||LA5_33=='|'||(LA5_33>='~' && LA5_33<='\uFFFF')) ) {s = 14;}

                        else s = 38;

                        if ( s>=0 ) return s;
                        break;
                    case 4 : 
                        int LA5_25 = input.LA(1);

                        s = -1;
                        if ( ((LA5_25>='\u0000' && LA5_25<='\b')||(LA5_25>='\u000B' && LA5_25<='\f')||(LA5_25>='\u000E' && LA5_25<='\u001F')||(LA5_25>='!' && LA5_25<='\'')||(LA5_25>='*' && LA5_25<='+')||(LA5_25>='-' && LA5_25<='9')||(LA5_25>='<' && LA5_25<='z')||LA5_25=='|'||(LA5_25>='~' && LA5_25<='\uFFFF')) ) {s = 14;}

                        else s = 32;

                        if ( s>=0 ) return s;
                        break;
                    case 5 : 
                        int LA5_54 = input.LA(1);

                        s = -1;
                        if ( ((LA5_54>='\u0000' && LA5_54<='\b')||(LA5_54>='\u000B' && LA5_54<='\f')||(LA5_54>='\u000E' && LA5_54<='\u001F')||(LA5_54>='!' && LA5_54<='\'')||(LA5_54>='*' && LA5_54<='+')||(LA5_54>='-' && LA5_54<='9')||(LA5_54>='<' && LA5_54<='z')||LA5_54=='|'||(LA5_54>='~' && LA5_54<='\uFFFF')) ) {s = 14;}

                        else s = 55;

                        if ( s>=0 ) return s;
                        break;
                    case 6 : 
                        int LA5_39 = input.LA(1);

                        s = -1;
                        if ( ((LA5_39>='\u0000' && LA5_39<='\b')||(LA5_39>='\u000B' && LA5_39<='\f')||(LA5_39>='\u000E' && LA5_39<='\u001F')||(LA5_39>='!' && LA5_39<='\'')||(LA5_39>='*' && LA5_39<='+')||(LA5_39>='-' && LA5_39<='9')||(LA5_39>='<' && LA5_39<='z')||LA5_39=='|'||(LA5_39>='~' && LA5_39<='\uFFFF')) ) {s = 14;}

                        else s = 42;

                        if ( s>=0 ) return s;
                        break;
                    case 7 : 
                        int LA5_50 = input.LA(1);

                        s = -1;
                        if ( ((LA5_50>='\u0000' && LA5_50<='\b')||(LA5_50>='\u000B' && LA5_50<='\f')||(LA5_50>='\u000E' && LA5_50<='\u001F')||(LA5_50>='!' && LA5_50<='\'')||(LA5_50>='*' && LA5_50<='+')||(LA5_50>='-' && LA5_50<='9')||(LA5_50>='<' && LA5_50<='z')||LA5_50=='|'||(LA5_50>='~' && LA5_50<='\uFFFF')) ) {s = 14;}

                        else s = 52;

                        if ( s>=0 ) return s;
                        break;
                    case 8 : 
                        int LA5_21 = input.LA(1);

                        s = -1;
                        if ( ((LA5_21>='\u0000' && LA5_21<='\b')||(LA5_21>='\u000B' && LA5_21<='\f')||(LA5_21>='\u000E' && LA5_21<='\u001F')||(LA5_21>='!' && LA5_21<='\'')||(LA5_21>='*' && LA5_21<='+')||(LA5_21>='-' && LA5_21<='9')||(LA5_21>='<' && LA5_21<='z')||LA5_21=='|'||(LA5_21>='~' && LA5_21<='\uFFFF')) ) {s = 14;}

                        else s = 29;

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