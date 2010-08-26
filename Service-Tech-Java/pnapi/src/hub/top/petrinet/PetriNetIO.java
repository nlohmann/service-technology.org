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

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Iterator;

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
    int format = getFormatForFileExtension(ext);
    
    if (format == FORMAT_LOLA) {
    
      LoLALexer lex = new LoLALexer(new ANTLRFileStream(fileName));
      CommonTokenStream tokens = new CommonTokenStream(lex);
  
      LoLAParser parser = new LoLAParser(tokens);
  
      try {
        PetriNet result = parser.net();
        return result;
          
      } catch (RecognitionException e)  {
        handleParsingException(e, fileName);
      }

      
    } else if (format == FORMAT_OWFN) {

      OWFNLexer lex = new OWFNLexer(new ANTLRFileStream(fileName));
      CommonTokenStream tokens = new CommonTokenStream(lex);
  
      OWFNParser parser = new OWFNParser(tokens);
  
      try {
        PetriNet result = parser.net();
        return result;
          
      } catch (RecognitionException e)  {
        handleParsingException(e, fileName);
      }

      
    } else if (format == FORMAT_WOFLAN) {

      TPNLexer lex = new TPNLexer(new ANTLRFileStream(fileName));
      CommonTokenStream tokens = new CommonTokenStream(lex);
  
      TPNParser parser = new TPNParser(tokens);
  
      try {
        PetriNet result = parser.net();
        return result;
          
      } catch (RecognitionException e)  {
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
  private static void handleParsingException(RecognitionException e, String fileName) {
    if (e instanceof EarlyExitException) {
      EarlyExitException e2 = (EarlyExitException)e;
      System.err.println("failed parsing "+fileName);
      System.err.println("found unexpected '"+e2.token.getText()+"' in "
          +"line "+e2.line+ " at column "+(e2.charPositionInLine+1));
    } else if (e instanceof MismatchedTokenException) {
      MismatchedTokenException e2 = (MismatchedTokenException)e;
      System.err.println("failed parsing "+fileName);
      System.err.println("line "+e2.line+":"+(e2.charPositionInLine+1)+" found unexpected "+e2.token.getText());
    } else {
      System.err.println("Error parsing file "+fileName);
      e.printStackTrace();
    }
  }
  
  public static final int FORMAT_DOT = 1;
  public static final int FORMAT_LOLA = 2;
  public static final int FORMAT_OWFN = 3;
  public static final int FORMAT_WOFLAN = 4;
  
  public static String toLoLA(String ident) {
    String result = ident.replace(' ', '_');
    result = result.replace('(', '_');
    result = result.replace(')', '_');
    result = result.replace('[', '_');
    result = result.replace(']', '_');
    result = result.replace('=', '_');
    result = result.replace(':', '_');
    return result;
  }
  
  public static String toLoLA(PetriNet net) {
    StringBuilder b = new StringBuilder();
    
    b.append("{\n");
    b.append("  input file:\n");
    b.append("  invocation:\n");
    b.append("  net size:     "+net.getInfo()+"\n");
    b.append("}\n\n");
    
    // ---------------------- places ------------------------
    b.append("PLACE");
      b.append("    ");
      Iterator<Place> place = net.getPlaces().iterator();
      while (place.hasNext()) {
        b.append(" "+toLoLA(place.next().getUniqueIdentifier()));
        if (place.hasNext()) b.append(",");
      }
      b.append(";\n");
    b.append("\n");
    
    // ---------------------- markings ------------------------
    b.append("MARKING\n");
    b.append("  ");
    boolean firstPlace = true;
    for (Place p : net.getPlaces()) {
      if (p.getTokens() > 0) {
        if (!firstPlace) b.append(",");
        b.append(" "+toLoLA(p.getUniqueIdentifier())+":"+p.getTokens());
        firstPlace = false;
      }
    }
    b.append(";\n\n");
    
    // ---------------------- transitions ------------------------
    for (Transition t : net.getTransitions()) {
      b.append("TRANSITION "+toLoLA(t.getUniqueIdentifier())+"\n");
        // pre-places of the transition
        b.append("  CONSUME");
        place = t.getPreSet().iterator();
        while (place.hasNext()) {
          b.append(" "+toLoLA(place.next().getUniqueIdentifier())+":1");
          if (place.hasNext()) b.append(",");
        }
        b.append(";\n");
        // post-places of the transition
        b.append("  PRODUCE");
        place = t.getPostSet().iterator();
        while (place.hasNext()) {
          b.append(" "+toLoLA(place.next().getUniqueIdentifier())+":1");
          if (place.hasNext()) b.append(",");
        }
        b.append(";\n");
      b.append("\n");
    }
    return b.toString();
  }
  
  public static String toOWFN(PetriNet net) {
    StringBuilder b = new StringBuilder();
    
    b.append("{\n");
    b.append("  input file:\n");
    b.append("  invocation:\n");
    b.append("  net size:     "+net.getInfo()+"\n");
    b.append("}\n\n");
    
    // ---------------------- places ------------------------
    b.append("PLACE");
      // print roles
      if (net.getRoles().size() > 0) {
        b.append("  ROLES");
        Iterator<String> role = net.getRoles().iterator();
        while (role.hasNext()) {
          b.append(" "+toLoLA(role.next()));
          if (role.hasNext()) b.append(",");
        }
        b.append(";\n");
      }
      // print internal places
      b.append("  INTERNAL\n");
        b.append("    ");
        Iterator<Place> place = net.getPlaces().iterator();
        while (place.hasNext()) {
          b.append(" "+toLoLA(place.next().getUniqueIdentifier()));
          if (place.hasNext()) b.append(",");
        }
        b.append(";\n");

      // print input places
      b.append("  INPUT\n");
        b.append("    "); /*
        place = net.getPlaces().iterator();
        while (place.hasNext()) {
          b.append(" "+place.next());
          if (place.hasNext()) b.append(",");
        } */
        b.append(";\n");

      // print input places
      b.append("  OUTPUT\n");
        b.append("    "); /*
        place = net.getPlaces().iterator();
        while (place.hasNext()) {
          b.append(" "+place.next());
          if (place.hasNext()) b.append(",");
        } */
        b.append(";\n");
    b.append("\n");
    
    // ---------------------- markings ------------------------
    b.append("INITIALMARKING\n");
    b.append("  ");
    boolean firstPlace = true;
    for (Place p : net.getPlaces()) {
      if (p.getTokens() > 0) {
        if (!firstPlace) b.append(",");
        b.append(" "+toLoLA(p.getUniqueIdentifier())+":"+p.getTokens());
        firstPlace = false;
      }
    }
    b.append(";\n\n");
    
    b.append("FINALCONDITION\n");
    b.append("  TRUE;\n");
    b.append("\n");
    
    // ---------------------- transitions ------------------------
    for (Transition t : net.getTransitions()) {
      b.append("TRANSITION "+toLoLA(t.getUniqueIdentifier())+"\n");
        // print roles of the transition
        if (t.getRoles().size() > 0) {
          b.append("  ROLES");
          Iterator<String> role = t.getRoles().iterator();
          while (role.hasNext()) {
            b.append(" "+toLoLA(role.next()));
            if (role.hasNext()) b.append(",");
          }
          b.append(";\n");
        }
        // pre-places of the transition
        b.append("  CONSUME");
        place = t.getPreSet().iterator();
        while (place.hasNext()) {
          b.append(" "+toLoLA(place.next().getUniqueIdentifier())+":1");
          if (place.hasNext()) b.append(",");
        }
        b.append(";\n");
        // post-places of the transition
        b.append("  PRODUCE");
        place = t.getPostSet().iterator();
        while (place.hasNext()) {
          b.append(" "+toLoLA(place.next().getUniqueIdentifier())+":1");
          if (place.hasNext()) b.append(",");
        }
        b.append(";\n");
      b.append("\n");
    }
    
    return b.toString();
  }
  
  /**
   * @param format
   * @return standard file extension for given file format
   */
  public static String getFileExtension(int format) {
    switch(format) {
    case FORMAT_DOT: return "dot";
    case FORMAT_LOLA: return "lola";
    case FORMAT_OWFN: return "owfn";
    case FORMAT_WOFLAN: return "tpn";
    }
    return "unknown";
  }
  
  /**
   * @param format
   * @return standard file extension for given file format
   */
  public static int getFormatForFileExtension(String ext) {
    if ("dot".equals(ext)) return FORMAT_DOT;
    else if ("lola".equals(ext)) return FORMAT_LOLA;
    else if ("owfn".equals(ext)) return FORMAT_OWFN;
    else if ("tpn".equals(ext)) return FORMAT_WOFLAN;
    else return 0;
  }
  
  /**
   * @param fileName
   * @return <code>true</code> iff we have a parser that can read the
   * file contents (decision is based on the file extension)
   */
  public static boolean isParseableFileType(String fileName) {
    String ext = fileName.substring(fileName.lastIndexOf('.')+1);
    int type = getFormatForFileExtension(ext);
    if (type == FORMAT_LOLA || type == FORMAT_OWFN || type == FORMAT_WOFLAN)
      return true;
    else
      return false;
  }
  
  public static final int PARAM_SWIMLANE = 1;
  
  public static void writeToFile(PetriNet net, String fileName, int format, int parameter) throws IOException {

    // Create file 
    FileWriter fstream = new FileWriter(fileName+"."+getFileExtension(format));
    BufferedWriter out = new BufferedWriter(fstream);
    
    if (format == FORMAT_DOT) {
      if ((parameter & PARAM_SWIMLANE) == 1)
        out.write(net.toDot_swimlanes());
      else
        out.write(net.toDot());
    } else if (format == FORMAT_LOLA) {
      out.write(PetriNetIO.toLoLA(net));
    } else if (format == FORMAT_OWFN) {
      out.write(PetriNetIO.toOWFN(net));
    }

    //Close the output stream
    out.close();
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
          options_outputFormat = FORMAT_DOT;
        else if ("lola".equals(args[i]))
          options_outputFormat = FORMAT_LOLA;
        else if ("owfn".equals(args[i]))
          options_outputFormat = FORMAT_OWFN;
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
    writeToFile(net, fileName, options_outputFormat, 0);
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
      pn_process.options_outputFormat = FORMAT_DOT;

    pn_process.run();
  }
}
