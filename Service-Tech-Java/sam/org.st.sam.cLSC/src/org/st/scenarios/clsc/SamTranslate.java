package org.st.scenarios.clsc;

import java.io.BufferedWriter;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Properties;

import javax.naming.OperationNotSupportedException;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.TransformerException;

import org.antlr.runtime.ANTLRFileStream;
import org.antlr.runtime.CommonTokenStream;
import org.antlr.runtime.EarlyExitException;
import org.antlr.runtime.MismatchedTokenException;
import org.antlr.runtime.RecognitionException;
import org.cpntools.accesscpn.model.Arc;
import org.cpntools.accesscpn.model.Node;
import org.cpntools.accesscpn.model.Object;
import org.cpntools.accesscpn.model.Page;
import org.cpntools.accesscpn.model.PetriNet;
import org.cpntools.accesscpn.model.PlaceNode;
import org.cpntools.accesscpn.model.exporter.CPNtoDot;
import org.cpntools.accesscpn.model.exporter.DOMGenerator;
import org.st.scenarios.clsc.export.ExportToCPN;
import org.st.scenarios.clsc.export.ExportToCPN2;

public class SamTranslate {
  
  private static final int FORMAT_DOT = 1;
  private static final int FORMAT_CPN = 2;
  
  private static final int MODE_TRANSLATE = 1;
  private static final int MODE_RENDER = 2;

  private String options_inFile = null;
  private String options_inFile_Ext = null;
  private int options_mode = MODE_TRANSLATE;
  private int options_outputFormat = FORMAT_CPN;
  private boolean options_flat = false;
  private boolean options_remove_self_loops = false;
  private boolean options_components = false;
  private boolean options_multiply_activation = false;
  private boolean options_hierarchy = false;
  
  private Properties props;
  
  public SamTranslate() throws FileNotFoundException, IOException {
    props = new Properties();
    props.load(new FileInputStream("sam.properties"));
  }
  
  private String helpText() {
    StringBuilder sb = new StringBuilder();
    
    sb.append("Sam - Scenario-based Analysis Methods and tools, version "+props.getProperty("sam.version")+"\n");
    sb.append("\n");
    sb.append("usage: sam <options>\n");
    sb.append("\n");
    sb.append("available options:\n");
    sb.append("  -i <inputfile>      system model file (*.clsc)\n");
    sb.append("\n");
    sb.append("  -m <mode>           'translate' : translate specification to Colored Petri net\n");
    sb.append("                                    (default)\n");
    sb.append("                      'render'    : write system model in another format\n");
    sb.append("\n");
    sb.append("  -f <format>         output format for result files ('dot', 'cpn')\n");
    sb.append("                      (default: 'cpn')\n");
    sb.append("\n");
    //         0123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789
    sb.append("  -s <switch>         'flat' - produce a flat Petri net without hierarchies\n");
    sb.append("                      'noselfloops' - remove places with self-loops from the\n");
    sb.append("                                      model applies only with '-f dot'\n");
    sb.append("                      'components' - produce Petri net separated into\n");
    sb.append("                                     components\n");
    sb.append("                      'control' - each component may control activation\n");
    sb.append("                                  of any scenario it is involved\n");
    sb.append("                      'hierarchical' - render components as subpages\n");
    return sb.toString();
  }
  
  private boolean checkOptions() {
    
    if (options_inFile == null) {
      return false;
    }

    return true;
  }

  private boolean parseCommandLine(String args[]) {
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
      
      if ("-s".equals(args[i])) {
        ++i;
        if ("flat".equals(args[i]))
          options_flat = true;
        if ("noselfloops".equals(args[i]))
          options_remove_self_loops = true;
        if ("components".equals(args[i]))
          options_components = true;
        if ("control".equals(args[i]))
          options_multiply_activation = true;
        if ("hierarchical".equals(args[i]))
          options_hierarchy = true;
      }

      if ("-m".equals(args[i])) {
        ++i;
        if ("render".equals(args[i]))
          options_mode = MODE_RENDER;
        else if ("translate".equals(args[i]))
          options_mode = MODE_TRANSLATE;
      }

      /*
      if ("-b".equals(args[i])) {
        ++i;
        options_bound = Integer.parseInt(args[i]);
      }
      */
    }
    return checkOptions();
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
  
  private void writeToFile(Specification spec, String fileName, int format, int parameter) throws IOException {

    if (format == FORMAT_DOT) {
        // Create file 
        FileWriter fstream = new FileWriter(fileName+"."+getFileExtension(format));
        BufferedWriter out = new BufferedWriter(fstream);
        out.write(spec.toDot());
        // Close the output stream
        out.close();
        System.out.println("Result written to "+fileName+"."+getFileExtension(format));
        
    } else {
      System.out.println("Cannot render specification in format "+getFileExtension(format));
    }
  }
  
  public PetriNet translateSpec(Specification spec) {
    ExportToCPN2 e = new ExportToCPN2(spec);
    PetriNet net = e.translate();
    
    e.assignRoles(spec.eventToComponent);
    if (options_multiply_activation)
      e.multiplyActivationLogic();
    if (options_hierarchy)
      e.splitRolesToSubpages(net.getPage().get(0));
    
    return net;
  }
  
  public void writeToFile(PetriNet net, String fileName, int format, int parameter) throws IOException {

    if (format == FORMAT_DOT) {
      
      if (options_remove_self_loops) {
        for (Page pg : net.getPage()) {
          LinkedList<PlaceNode> toRemove = new LinkedList<PlaceNode>();
          for (Object o : pg.getObject()) {
            if (o instanceof PlaceNode) {
              HashSet<Node> non_intersect = new HashSet<Node>();
              for (Arc a : ((PlaceNode) o).getSourceArc()) {
                non_intersect.add(a.getTarget());
              }
              for (Arc a : ((PlaceNode) o).getTargetArc()) {
                if (non_intersect.contains(a.getSource()))
                    non_intersect.remove(a.getSource());
                else
                  non_intersect.add(a.getSource());
                        
              }
              if (non_intersect.isEmpty()) {
                // pre-set of this place equals its postset
                toRemove.add((PlaceNode)o);
              }
            }
          }
          for (PlaceNode p : toRemove) {
            for (Arc a : p.getSourceArc()) {
              pg.getArc().remove(a);
            }
            for (Arc a : p.getTargetArc()) {
              pg.getArc().remove(a);
            }
            pg.getObject().remove(p);
          }
        }
      }
      
      CPNtoDot c2d = new CPNtoDot(net);
      FileWriter fstream = new FileWriter(fileName+".net."+getFileExtension(format));
      BufferedWriter out = new BufferedWriter(fstream);
      out.write(c2d.toDot());
      out.close();
        
      System.out.println("Result written to "+fileName+".net."+getFileExtension(format));
        
    } else if (format == FORMAT_CPN) {
      
      try {
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
  
  public void run(String fileName, int outputFormat) throws Exception {
    String ext = fileName.substring(fileName.lastIndexOf('.')+1);
    Specification spec = readSpecificationFromFile(fileName);
    
    if (spec == null) {
      System.err.println("Parsed empty specification from "+fileName);
      return;
    }
    
    if (options_mode == MODE_RENDER) {
      writeToFile(spec, fileName, outputFormat, 0);
    } else if (options_mode == MODE_TRANSLATE) {
      PetriNet net = translateSpec(spec);
      writeToFile(net, fileName, outputFormat, 0);
    }
  }
  
  public void run() throws Exception {
    run(options_inFile, options_outputFormat);
  }
  
  public static void main(String[] args) throws Exception {
    
    SamTranslate sam = new SamTranslate();
    
    if (!sam.parseCommandLine(args)) {
      System.out.println(sam.helpText());
      return;
    }
    
    sam.run();
  }
}
