package hub.top.uma;

import hub.top.petrinet.ISystemModel;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO_Out;
import hub.top.petrinet.unfold.DNodeSys_PetriNet;
import hub.top.scenario.DNodeSys_OcletSpecification;
import hub.top.scenario.OcletIO;
import hub.top.scenario.OcletIO_Out;
import hub.top.scenario.OcletSpecification;

import java.io.BufferedWriter;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintStream;
import java.util.Properties;

public class Uma {

  public static final int MODE_COMPUTE_PREFIX = 0;
  public static final int MODE_RENDER_SYSTEM = 1;

  private String options_inFile = null;
  private String options_inFile_Ext = null;
  private int options_outputFormat = PetriNetIO_Out.FORMAT_DOT;
  private int options_mode = MODE_COMPUTE_PREFIX;
  private int options_bound = 1;
  
  private Properties props;
  
  public Uma () throws FileNotFoundException, IOException {
    props = new Properties();
    props.load(new FileInputStream("uma.properties"));
  }

  private String helpText() {
    StringBuilder sb = new StringBuilder();
    
    sb.append("Uma - an Unfoding-based Model Analyzer, version "+props.getProperty("uma.version")+"\n");
    sb.append("\n");
    sb.append("usage: uma <options>\n");
    sb.append("\n");
    sb.append("available options:\n");
    sb.append("  -i <inputfile>      system model file (.lola .oclets)\n");
    sb.append("\n");
    sb.append("  -m <mode>           'prefix' : compute finite complete prefix of the system\n");
    sb.append("                                 behavior (default)\n");
    sb.append("                      'system' : write system model in another format\n");
    sb.append("\n");
    sb.append("  -f <format>         output format for result files ('dot', 'lola', 'owfn')\n");
    sb.append("                      (default: 'dot')\n");
    sb.append("\n");
    //         0123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789
    sb.append("  -b <number>         stop construction if prefix has more than the given\n");
    sb.append("                      number of tokens in a place (default: 1)\n");
    
    return sb.toString();
  }
  
  private boolean checkOptions() {
    if (options_inFile == null) return false;
    return true;
  }
  
  private boolean parseCommandLine(String args[]) {
    for (int i=0; i<args.length; i++) {
      if ("-i".equals(args[i])) {
        options_inFile = args[++i];
        options_inFile_Ext = options_inFile.substring(options_inFile.lastIndexOf('.')+1);
      }
      
      if ("-f".equals(args[i])) {
        ++i;
        if ("dot".equals(args[i]))
          options_outputFormat = PetriNetIO_Out.FORMAT_DOT;
        else if ("lola".equals(args[i]))
          options_outputFormat = PetriNetIO_Out.FORMAT_LOLA;
        else if ("owfn".equals(args[i]))
          options_outputFormat = PetriNetIO_Out.FORMAT_OWFN;
        else {
          System.out.println("unknown output file format: "+args[i]);
          return false;
        }
      }
      
      if ("-m".equals(args[i])) {
        ++i;
        if ("prefix".equals(args[i]))
          options_mode = MODE_COMPUTE_PREFIX;
        else if ("system".equals(args[i]))
          options_mode = MODE_RENDER_SYSTEM;
        else {
          System.out.println("unknown mode: "+args[i]);
          return false;          
        }
      }
      
      if ("-b".equals(args[i])) {
        ++i;
        try {
          options_bound = Integer.parseInt(args[i]);
        } catch (NumberFormatException e) {
          System.out.println("the bound value must be a number: "+args[i]);
          return false;
        }
      }
    }
    
    return checkOptions();
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
  
  public static DNodeRefold initBuildPrefix_Synthesis(DNodeSys sys, int bound) {
    
    Options o = new Options(sys);
    o.configure_synthesis();
    o.configure_setBound(bound);
    
    DNodeRefold build = new DNodeRefold(sys, o);
    
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
  
  private void computePrefix(int bound) throws IOException, InvalidModelException {
    
    ISystemModel sysModel = readSystemFromFile(options_inFile);
    DNodeSys sys = getBehavioralSystemModel(sysModel);
    DNodeBP build = buildPrefix(initBuildPrefix(sys, bound));

    if (options_outputFormat == PetriNetIO_Out.FORMAT_DOT) {
      System.out.println("writing to "+options_inFile+".bp.dot");
      FileWriter fstream = new FileWriter(options_inFile+".bp.dot");
      BufferedWriter out = new BufferedWriter(fstream);
      out.write(build.toDot());
      out.close();
    }
    
    System.out.println(build.getStatistics(false));
  }
  
  public static void renderSystem(String fileName, int outputFormat) throws IOException {
    String ext = fileName.substring(fileName.lastIndexOf('.')+1);
    PetriNet net = OcletIO.readNetFromFile(fileName);
    
    if (net == null) {
      System.err.println("Parsed empty net from "+fileName);
      return;
    }
    
    if ("oclets".equals(ext)) {
      OcletSpecification os = new OcletSpecification(net);
      OcletIO_Out.writeToFile(os, fileName, outputFormat, 0);
    } else {
      PetriNetIO_Out.writeToFile(net, fileName, outputFormat, 0);
    }
  }
  
  private void renderSystem() throws IOException {
    renderSystem(options_inFile, options_outputFormat);
  }
  
  private void run() throws IOException, InvalidModelException {
    if (options_inFile == null) return;
    
    if (options_mode == MODE_COMPUTE_PREFIX)
      computePrefix(options_bound);
    else if (options_mode == MODE_RENDER_SYSTEM)
      renderSystem();    
  }

  /**
   * @param args
   */
  public static void main(String[] args) throws Exception {
    
    Uma uma = new Uma();
    if (!uma.parseCommandLine(args)) {
      System.out.println(uma.helpText());
    }
    uma.run();
  }
  
  public static PrintStream out = System.out;
  public static PrintStream err = System.err;

}
