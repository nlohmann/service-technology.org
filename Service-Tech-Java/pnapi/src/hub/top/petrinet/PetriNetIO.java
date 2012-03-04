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

import java.io.File;
import java.io.IOException;

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
    int format = PetriNetIO_Out.getFormatForFileExtension(ext);
    
    if (format == PetriNetIO_Out.FORMAT_LOLA) {
    
      LoLALexer lex = new LoLALexer(new org.antlr.runtime.ANTLRFileStream(fileName));
      org.antlr.runtime.CommonTokenStream tokens = new org.antlr.runtime.CommonTokenStream(lex);
  
      LoLAParser parser = new LoLAParser(tokens);
  
      try {
        PetriNet result = parser.net();
        return result;
          
      } catch (org.antlr.runtime.RecognitionException e)  {
        handleParsingException(e, fileName);
      }

      
    } else if (format == PetriNetIO_Out.FORMAT_OWFN) {
    	
      OWFNLexer lex = new OWFNLexer(new org.antlr.runtime.ANTLRFileStream(fileName));
      org.antlr.runtime.CommonTokenStream tokens = new org.antlr.runtime.CommonTokenStream(lex);
  
      OWFNParser parser = new OWFNParser(tokens);
  
      try {
        PetriNet result = parser.net();
        return result;
          
      } catch (org.antlr.runtime.RecognitionException e)  {
        handleParsingException(e, fileName);
      }

      
    } else if (format == PetriNetIO_Out.FORMAT_WOFLAN) {

      TPNLexer lex = new TPNLexer(new org.antlr.runtime.ANTLRFileStream(fileName));
      org.antlr.runtime.CommonTokenStream tokens = new org.antlr.runtime.CommonTokenStream(lex);
  
      TPNParser parser = new TPNParser(tokens);
  
      try {
        PetriNet result = parser.net();
        return result;
          
      } catch (org.antlr.runtime.RecognitionException e)  {
        handleParsingException(e, fileName);
      }
    }
    
    return null;
  }
  
  /**
   * Print diagnostic information in case an exception occurred during parsing.
   * Called by {@link #readNetFromFile(String)}.
   * 
   * @param e
   * @param fileName
   */
  private static void handleParsingException(org.antlr.runtime.RecognitionException e, String fileName) {
    if (e instanceof org.antlr.runtime.EarlyExitException) {
      org.antlr.runtime.EarlyExitException e2 = (org.antlr.runtime.EarlyExitException)e;
      System.err.println("failed parsing "+fileName);
      System.err.println("found unexpected '"+e2.token.getText()+"' in "
          +"line "+e2.line+ " at column "+(e2.charPositionInLine+1));
    } else if (e instanceof org.antlr.runtime.MismatchedTokenException) {
      org.antlr.runtime.MismatchedTokenException e2 = (org.antlr.runtime.MismatchedTokenException)e;
      System.err.println("failed parsing "+fileName);
      System.err.println("line "+e2.line+":"+(e2.charPositionInLine+1)+" found unexpected "+e2.token.getText());
    } else {
      System.err.println("Error parsing file "+fileName);
      e.printStackTrace();
    }
  }
  
  private String options_inFile = null;
  private int options_outputFormat = 0;
  
  /**
   * Set file or directory to read from.
   * @param fileName
   */
  protected void setInputFile(String fileName) {
    options_inFile = fileName;
  }

  /**
   * @return name of file/directory read from
   */
  protected String getInputFile() {
    return options_inFile;
  }
  
  private void parseCommandLine(String args[]) {
    for (int i=0; i<args.length; i++) {
      if ("-i".equals(args[i])) {
        setInputFile(args[++i]);
      }
      
      if ("-f".equals(args[i])) {
        ++i;
        if ("dot".equals(args[i]))
          options_outputFormat = PetriNetIO_Out.FORMAT_DOT;
        else if ("lola".equals(args[i]))
          options_outputFormat = PetriNetIO_Out.FORMAT_LOLA;
        else if ("owfn".equals(args[i]))
          options_outputFormat = PetriNetIO_Out.FORMAT_OWFN;
      }
    }
  }
  
  /**
   * Process a single input file according to the set input parameters,
   * see {@link #parseCommandLine(String[])}.
   * 
   * @param dirName
   */
  protected void processFile(String fileName) throws IOException {
    if (fileName == null) return;
    
    PetriNet net = readNetFromFile(fileName);
    if (net == null) return;

    //net.anonymizeNet();
    PetriNetIO_Out.writeToFile(net, fileName, options_outputFormat, 0);
  }
  
  /**
   * @param fileName
   * @return <code>true</code> iff we have a parser that can read the
   * file contents (decision is based on the file extension)
   */
  public static boolean isParseableFileType(String fileName) {
    String ext = fileName.substring(fileName.lastIndexOf('.')+1);
    int type = PetriNetIO_Out.getFormatForFileExtension(ext);
    if (type == PetriNetIO_Out.FORMAT_LOLA || type == PetriNetIO_Out.FORMAT_OWFN || type == PetriNetIO_Out.FORMAT_WOFLAN)
      return true;
    else
      return false;
  }
  
  /**
   * Process all files in the given directory according to the set input parameters,
   * see {@link #parseCommandLine(String[])}.
   * 
   * @param dirName
   */
  protected void processDirectory(String dirName) {
    File dir = new File(dirName);
    if (dir.isDirectory()) {
      for (String child : dir.list()) {
        File childFile = new File(dir, child);
        if (childFile.isFile()) {
          if (isParseableFileType(childFile.getPath())) {
            
            try {
              processFile(childFile.getPath());
            } catch (IOException e) {
              System.err.println(e);
            }
            
          }
        }
      }
    }
  }
  
  /**
   * Run this {@link PetriNetIO} object and convert files or
   * compute values as set.
   */
  public void run() {
    if (getInputFile() != null) {
      if (isParseableFileType(getInputFile())) {
        try {
          processFile(getInputFile());
        } catch (IOException e) {
          System.err.println(e);
        }
      }
      else
        processDirectory(getInputFile());
    }
  }

  /**
   * Read Petri net from a text-file and write a GraphViz Dot representation
   * of the net to standard out.
   * 
   * @param args
   * @throws Exception
   */
  public static void main(String args[]) throws Exception {
    
    PetriNetIO pn_process = new PetriNetIO();
    pn_process.parseCommandLine(args);
    
    if (pn_process.options_outputFormat == 0)
      pn_process.options_outputFormat = PetriNetIO_Out.FORMAT_DOT;

    pn_process.run();
  }
}
