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

import java.io.IOException;

import org.antlr.runtime.ANTLRFileStream;
import org.antlr.runtime.CommonTokenStream;
import org.antlr.runtime.EarlyExitException;
import org.antlr.runtime.MismatchedTokenException;
import org.antlr.runtime.RecognitionException;

/**
 * 
 * Provides front-end methods for reading and writing Petri nets.
 * 
 * @author Dirk Fahland
 */
public class PetriNetIO {
  
  /**
   * Read Petri net from a text-file.
   * 
   * @param fileName
   * @return the Petri net written in <code>fileName</code>
   * @throws IOException
   */
  public static PetriNet readNetFromFile(String fileName) throws IOException {
    
    String ext = fileName.substring(fileName.lastIndexOf('.')+1);
    
    if ("lola".equals(ext)) {
    
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
        } else {
          e.printStackTrace();
        }
      }
      
    } else if ("owfn".equals(ext)) {

      OWFNLexer lex = new OWFNLexer(new ANTLRFileStream(fileName));
      CommonTokenStream tokens = new CommonTokenStream(lex);
  
      OWFNParser parser = new OWFNParser(tokens);
  
      try {
        PetriNet result = parser.net();
        return result;
          
      } catch (RecognitionException e)  {
          if (e instanceof EarlyExitException) {
            EarlyExitException e2 = (EarlyExitException)e;
            System.err.println("failed parsing "+fileName);
            System.err.println("found unexpected '"+e2.token.getText()+"' in "
                +"line "+e2.line+ " at column "+(e2.charPositionInLine+1));
            System.exit(1);
          } else if (e instanceof MismatchedTokenException) {
            MismatchedTokenException e2 = (MismatchedTokenException)e;
            System.err.println("failed parsing "+fileName);
            System.err.println("line "+e2.line+":"+(e2.charPositionInLine+1)+" found unexpected "+e2.token.getText());
            System.exit(1);
          } else {
            e.printStackTrace();
          }
      }
    }
    
    return null;
  }

  /**
   * Read Petri net from a text-file and write a GraphViz Dot representation
   * of the net to standard out.
   * 
   * @param args
   * @throws Exception
   */
  public static void main(String args[]) throws Exception {
    if (args.length == 0) return;
    
    PetriNet net = readNetFromFile(args[0]);
    if (net != null) System.out.println(net.toDot());
  }
}
