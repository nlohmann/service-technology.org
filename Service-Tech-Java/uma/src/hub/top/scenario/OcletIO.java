package hub.top.scenario;

import hub.top.petrinet.LoLALexer;
import hub.top.petrinet.LoLAParser;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;

import java.io.IOException;

public class OcletIO extends PetriNetIO {

  public static PetriNet readNetFromFile(String fileName) throws IOException {
    
    String ext = fileName.substring(fileName.lastIndexOf('.')+1);
    
    if ("oclets".equals(ext)) {
      
      LoLALexer lex = new LoLALexer(new org.antlr.runtime.ANTLRFileStream(fileName));
      org.antlr.runtime.CommonTokenStream tokens = new org.antlr.runtime.CommonTokenStream(lex);
  
      LoLAParser parser = new LoLAParser(tokens);
  
      try {
        PetriNet result = parser.net();
        return result;
          
      } catch (org.antlr.runtime.RecognitionException e)  {
        if (e instanceof org.antlr.runtime.EarlyExitException) {
          org.antlr.runtime.EarlyExitException e2 = (org.antlr.runtime.EarlyExitException)e;
          System.err.println("failed parsing "+fileName);
          System.err.println("found unexpected '"+e2.token.getText()+"' in "
              +"line "+e2.line+ " at column "+e2.charPositionInLine);
          System.exit(1);
        } else if (e instanceof org.antlr.runtime.MismatchedTokenException) {
          org.antlr.runtime.MismatchedTokenException e2 = (org.antlr.runtime.MismatchedTokenException)e;
          System.err.println("found "+e2.token.getText()+" expected "+LoLAParser.tokenNames[e2.expecting]);
          System.err.println(" in line "+e2.line+" at column "+e2.charPositionInLine);
        } else {
          e.printStackTrace();
        }
      }
    }
    
    return PetriNetIO.readNetFromFile(fileName);
  }
  
}
