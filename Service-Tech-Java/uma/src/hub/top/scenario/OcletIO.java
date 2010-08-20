package hub.top.scenario;

import java.io.IOException;

import org.antlr.runtime.ANTLRFileStream;
import org.antlr.runtime.CommonTokenStream;
import org.antlr.runtime.EarlyExitException;
import org.antlr.runtime.MismatchedTokenException;
import org.antlr.runtime.RecognitionException;

import hub.top.petrinet.LoLALexer;
import hub.top.petrinet.LoLAParser;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;

public class OcletIO extends PetriNetIO {

  public static PetriNet readNetFromFile(String fileName) throws IOException {
    
    String ext = fileName.substring(fileName.lastIndexOf('.')+1);
    
    if ("oclets".equals(ext)) {
      
      LoLALexer lex = new LoLALexer(new ANTLRFileStream(fileName));
      CommonTokenStream tokens = new CommonTokenStream(lex);
  
      LoLAParser parser = new LoLAParser(tokens);
  
      try {
        PetriNet result = parser.net();
        return result;
          
      } catch (RecognitionException e)  {
        if (e instanceof EarlyExitException) {
          EarlyExitException e2 = (EarlyExitException)e;
          System.err.println("failed parsing "+fileName);
          System.err.println("found unexpected '"+e2.token.getText()+"' in "
              +"line "+e2.line+ " at column "+e2.charPositionInLine);
          System.exit(1);
        } else if (e instanceof MismatchedTokenException) {
          MismatchedTokenException e2 = (MismatchedTokenException)e;
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
