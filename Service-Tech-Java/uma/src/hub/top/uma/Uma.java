package hub.top.uma;

import java.io.BufferedWriter;
import java.io.FileNotFoundException;
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
  private static int options_bound = 1;

  
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
      
      if ("-b".equals(args[i])) {
        ++i;
        options_bound = Integer.parseInt(args[i]);
      }
    }
  }
  
  public static DNodeSys readSystemFromFile(String fileName) throws IOException, InvalidModelException {
    String ext = fileName.substring(fileName.lastIndexOf('.')+1);
    PetriNet net = OcletIO.readNetFromFile(fileName);
    
    if (net == null) {
      System.err.println("Parsed empty net from "+fileName);
      return null;
    }
    
    DNodeSys sys = null;
    
    if ("oclets".equals(ext)) {
      OcletSpecification os = new OcletSpecification(net);
      sys = new DNodeSys_OcletSpecification(os); 
    } else {
      sys = new DNodeSys_PetriNet(net);
    }
    return sys;
  }
  
  public static DNodeBP initBuildPrefix(DNodeSys sys, int bound) {
    DNodeBP build = new DNodeBP(sys);
    build.configure_buildOnly();
    build.configure_setBound(bound);
    
    if (sys instanceof DNodeSys_PetriNet) build.configure_PetriNet();
    else build.configure_Scenarios();
    
    return build;
  }
  
  public static DNodeBP buildPrefix(DNodeSys sys, int bound) {
    
    DNodeBP build = initBuildPrefix(sys, bound);
    int fired, totalFired = 0, step = 0;
    while ((fired = build.step()) > 0) {
      totalFired += fired;
       System.out.print(totalFired+".. ");
      if (step > 10) { System.out.print("\n"); step = 0; }
    }
    System.out.println();
    
    return build;
  }
  
  private static void computePrefix(int bound) throws IOException, InvalidModelException {
    
    DNodeSys sys = readSystemFromFile(options_inFile);
    DNodeBP build = buildPrefix(sys, bound);

    if (options_outputFormat == PetriNetIO.FORMAT_DOT) {
      System.out.println("writing to "+options_inFile+".bp.dot");
      FileWriter fstream = new FileWriter(options_inFile+".bp.dot");
      BufferedWriter out = new BufferedWriter(fstream);
      out.write(build.toDot());
      out.close();
    }
    
    System.out.println(build.getStatistics());
  }
  
  public static void renderSystem(String fileName, int outputFormat) throws Exception {
    String ext = fileName.substring(fileName.lastIndexOf('.')+1);
    PetriNet net = OcletIO.readNetFromFile(fileName);
    
    if (net == null) {
      System.err.println("Parsed empty net from "+fileName);
      return;
    }
    
    if ("oclets".equals(ext)) {
      OcletSpecification os = new OcletSpecification(net);
      OcletIO.writeToFile(os, fileName, outputFormat, 0);
    } else {
      PetriNetIO.writeToFile(net, fileName, outputFormat, 0);
    }
  }
  
  private static void renderSystem() throws Exception {
    renderSystem(options_inFile, options_outputFormat);
  }

  /**
   * @param args
   */
  public static void main(String[] args) throws Exception {
    parseCommandLine(args);
    
    if (options_inFile == null) return;
    
    if (options_mode == MODE_COMPUTE_PREFIX)
      computePrefix(options_bound);
    else if (options_mode == MODE_RENDER_SYSTEM)
      renderSystem();

  }

}
