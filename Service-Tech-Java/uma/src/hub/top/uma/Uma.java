package hub.top.uma;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;

import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.unfold.DNodeSys_PetriNet;
import hub.top.scenario.DNodeSys_OcletSpecification;
import hub.top.scenario.OcletIO;
import hub.top.scenario.OcletSpecification;

public class Uma {

  public static final int MODE_COMPUTE_PREFIX = 0;
  public static final int MODE_RENDER_SYSTEM = 1;

  private static String options_inFile = null;
  private static String options_inFile_Ext = null;
  private static int options_outputFormat = 0;
  private static int options_mode = MODE_COMPUTE_PREFIX;

  
  private static void parseCommandLine(String args[]) {
    for (int i=0; i<args.length; i++) {
      if ("-i".equals(args[i])) {
        options_inFile = args[++i];
        options_inFile_Ext = options_inFile.substring(options_inFile.lastIndexOf('.')+1);
      }
      
      if ("-f".equals(args[i])) {
        ++i;
        if ("dot".equals(args[i]))
          options_outputFormat = PetriNetIO.FORMAT_DOT;
        else if ("lola".equals(args[i]))
          options_outputFormat = PetriNetIO.FORMAT_LOLA;
        else if ("owfn".equals(args[i]))
          options_outputFormat = PetriNetIO.FORMAT_OWFN;
      }
      
      if ("-m".equals(args[i])) {
        ++i;
        if ("prefix".equals(args[i]))
          options_mode = MODE_COMPUTE_PREFIX;
        else if ("system".equals(args[i]))
          options_mode = MODE_RENDER_SYSTEM;
      }
    }
  }
  
  private static void computePrefix() throws Exception {
    PetriNet net = OcletIO.readNetFromFile(options_inFile);
    
    if (net == null) {
      System.err.println("Parsed empty net from "+options_inFile);
      return;
    }
    
    DNodeSys sys = null;
    
    if ("oclets".equals(options_inFile_Ext)) {
      OcletSpecification os = new OcletSpecification(net);
      sys = new DNodeSys_OcletSpecification(os); 
    } else {
      sys = new DNodeSys_PetriNet(net);
    }
    DNodeBP build = new DNodeBP(sys);
    build.configure_buildOnly();
    build.configure_PetriNet();
    build.configure_stopIfUnSafe();
    
    while (build.step() > 0) {
    }

    if (options_outputFormat == PetriNetIO.FORMAT_DOT) {
      FileWriter fstream = new FileWriter(options_inFile+".bp.dot");
      BufferedWriter out = new BufferedWriter(fstream);
      out.write(build.toDot());
      out.close();
    }
    
    System.out.println(build.getStatistics());
  }
  
  private static void renderSystem() throws Exception {
    PetriNet net = OcletIO.readNetFromFile(options_inFile);
    
    if (net == null) {
      System.err.println("Parsed empty net from "+options_inFile);
      return;
    }
    
    if ("oclets".equals(options_inFile_Ext)) {
      OcletSpecification os = new OcletSpecification(net);
      PetriNetIO.writeToFile(net, options_inFile, options_outputFormat, 0);
    } else {
      PetriNetIO.writeToFile(net, options_inFile, options_outputFormat, 0);
    }
  }

  /**
   * @param args
   */
  public static void main(String[] args) throws Exception {
    parseCommandLine(args);
    
    if (options_inFile == null) return;
    
    if (options_mode == MODE_COMPUTE_PREFIX)
      computePrefix();
    else if (options_mode == MODE_RENDER_SYSTEM)
      renderSystem();

  }

}
