package hub.top.uma;

import hub.top.petrinet.ISystemModel;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.unfold.DNodeSys_PetriNet;
import hub.top.scenario.DNodeSys_OcletSpecification;
import hub.top.scenario.OcletIO;
import hub.top.scenario.OcletSpecification;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintStream;

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
  
  public static ISystemModel readSystemFromFile(String fileName) throws IOException {
    String ext = fileName.substring(fileName.lastIndexOf('.')+1);
    PetriNet net = OcletIO.readNetFromFile(fileName);
    
    if (net == null) {
      System.err.println("Parsed empty net from "+fileName);
      return null;
    }
    
    if ("oclets".equals(ext)) {
      OcletSpecification os = new OcletSpecification(net);
      return os;
    } else {
      return net;
    }
  }
  
  public static DNodeSys getBehavioralSystemModel(ISystemModel sysModel) throws InvalidModelException {
    DNodeSys sys = null;

    if (sysModel instanceof OcletSpecification) {
      sys = new DNodeSys_OcletSpecification((OcletSpecification)sysModel); 
    } else if (sysModel instanceof PetriNet) {
      sys = new DNodeSys_PetriNet((PetriNet)sysModel);
    }
    return sys;
  }
  
  public static DNodeBP initBuildPrefix(DNodeSys sys, int bound) {
    
    Options o = new Options(sys);
    o.configure_buildOnly();
    o.configure_setBound(bound);
    
    DNodeBP build = new DNodeBP(sys, o);
    
    return build;
  }
  
  public static DNodeBP initBuildPrefix_Synthesis(DNodeSys sys, int bound) {
    
    Options o = new Options(sys);
    o.configure_synthesis();
    o.configure_setBound(bound);
    
    DNodeBP build = new DNodeBP(sys, o);
    
    return build;
  }
  
  public static DNodeRefold initBuildPrefix_View(DNodeSys sys, int bound) {
    Options o = new Options(sys);
    o.configure_buildOnly();
    o.configure_setBound(bound);
    o.configure_simplifyNet();
    
    DNodeRefold build = new DNodeRefold(sys, o);
    
    return build;
  }
  
  public static DNodeBP buildPrefix(DNodeBP build) {
    
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
    
    ISystemModel sysModel = readSystemFromFile(options_inFile);
    DNodeSys sys = getBehavioralSystemModel(sysModel);
    DNodeBP build = buildPrefix(initBuildPrefix(sys, bound));

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
  
  public static PrintStream out = System.out;
  public static PrintStream err = System.err;

}
