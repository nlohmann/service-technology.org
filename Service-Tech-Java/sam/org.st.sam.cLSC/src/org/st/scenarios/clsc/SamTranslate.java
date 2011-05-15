package org.st.scenarios.clsc;

import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;

import javax.naming.OperationNotSupportedException;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.TransformerException;

import org.antlr.runtime.ANTLRFileStream;
import org.antlr.runtime.CommonTokenStream;
import org.antlr.runtime.EarlyExitException;
import org.antlr.runtime.MismatchedTokenException;
import org.antlr.runtime.RecognitionException;
import org.cpntools.accesscpn.model.PetriNet;
import org.cpntools.accesscpn.model.exporter.DOMGenerator;
import org.st.scenarios.clsc.export.ExportToCPN;

public class SamTranslate {
  
  private static String options_inFile = null;
  private static String options_inFile_Ext = null;
  private static int options_outputFormat = 0;
  
  private static final int FORMAT_DOT = 1;
  private static final int FORMAT_CPN = 2;


  private static void parseCommandLine(String args[]) {
    for (int i = 0; i < args.length; i++) {
      if ("-i".equals(args[i])) {
        options_inFile = args[++i];
        options_inFile_Ext = options_inFile.substring(options_inFile
            .lastIndexOf('.') + 1);
      }

      if ("-f".equals(args[i])) {
        ++i;
        if ("dot".equals(args[i]))
          options_outputFormat = SamTranslate.FORMAT_DOT;
        if ("cpn".equals(args[i]))
          options_outputFormat = SamTranslate.FORMAT_CPN;
      }

      /*
      if ("-m".equals(args[i])) {
        ++i;
        if ("prefix".equals(args[i]))
          options_mode = MODE_COMPUTE_PREFIX;
        else if ("system".equals(args[i]))
          options_mode = MODE_RENDER_SYSTEM;
      }

      if ("-b".equals(args[i])) {
        ++i;
        options_bound = Integer.parseInt(args[i]);
      }
      */
    }
  }
  
  public static Specification readSpecificationFromFile(String fileName) throws IOException {
    
    String ext = fileName.substring(fileName.lastIndexOf('.')+1);
    
    if ("clsc".equals(ext)) {
      
      CLSCLexer lex = new CLSCLexer(new ANTLRFileStream(fileName));
      CommonTokenStream tokens = new CommonTokenStream(lex);
  
      CLSCParser parser = new CLSCParser(tokens);
  
      try {
        Specification result = parser.net();
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
          System.err.println("found "+e2.token.getText()+" expected "+CLSCParser.tokenNames[e2.expecting]);
          System.err.println(" in line "+e2.line+" at column "+e2.charPositionInLine);
        } else {
          e.printStackTrace();
        }
      }
    }
    
    return null;
  }
  
  /**
   * @param format
   * @return standard file extension for given file format
   */
  public static String getFileExtension(int format) {
    switch(format) {
      case FORMAT_DOT: return "dot";
      case FORMAT_CPN: return "cpn";
    }
    return "unknown";
  }
  
  public static void writeToFile(Specification spec, String fileName, int format, int parameter) throws IOException {

    if (format == FORMAT_DOT) {
        // Create file 
        FileWriter fstream = new FileWriter(fileName+"."+getFileExtension(format));
        BufferedWriter out = new BufferedWriter(fstream);
        out.write(spec.toDot());
        // Close the output stream
        out.close();
        System.out.println("Result written to "+fileName+"."+getFileExtension(format));
        
    } else if (format == FORMAT_CPN) {
      
      try {
        ExportToCPN e = new ExportToCPN(spec);
        PetriNet net = e.translate();
        DOMGenerator.export(net, fileName+"."+getFileExtension(format));
        System.out.println("Result written to "+fileName+"."+getFileExtension(format));
        
      } catch (OperationNotSupportedException e) {
        System.err.println("Failed to export specification. The translated net uses features that have not been implemented for export.");
        System.err.println(e);
      } catch (FileNotFoundException e) {
        System.err.println("Could not write file "+fileName+"."+getFileExtension(format));
        System.err.println(e);
      } catch (ParserConfigurationException e) {
        System.err.println("Failed to generate XML for resulting CPN files.");
        System.err.println(e);
      } catch (TransformerException e) {
        System.err.println("Failed to generate XML for resulting CPN files.");
        System.err.println(e);
      }
    }
  }
  
  public static void renderSystem(String fileName, int outputFormat) throws Exception {
    String ext = fileName.substring(fileName.lastIndexOf('.')+1);
    Specification spec = readSpecificationFromFile(fileName);
    
    if (spec == null) {
      System.err.println("Parsed empty specification from "+fileName);
      return;
    }
    
    writeToFile(spec, fileName, outputFormat, 0);
  }
  
  public static void renderSystem() throws Exception {
    renderSystem(options_inFile, options_outputFormat);
  }
  
  public static void main(String[] args) throws Exception {
    parseCommandLine(args);
    
    if (options_inFile == null) {
      return;
    }
    
    renderSystem();
  }
}
