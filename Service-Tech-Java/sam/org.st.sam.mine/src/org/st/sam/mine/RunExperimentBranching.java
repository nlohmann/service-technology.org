package org.st.sam.mine;

import java.io.File;
import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Properties;

import lscminer.datastructure.LSC;

import org.deckfour.xes.model.XLog;

public class RunExperimentBranching extends RunExperimentCompare {

  private static String SLASH = System.getProperty("file.separator");
  
  private Properties props;

  public RunExperimentBranching() throws IOException {
    super();
  }
  
  public RunExperimentBranching(String dotBinary, String mscGenBinary) throws IOException {
    super(dotBinary, mscGenBinary);
  }
  
  @Override
  public void runMiners(String logFile, XLog xlog, int minSupportThreshold, double confidence_branch, double confidence_linear) throws IOException {
    branching_lscs = runMiner_Branch(logFile, xlog, minSupportThreshold, confidence_branch);
    linear_lscs = new ArrayList<LSC>();
  }
  
  @Override
  public String getResultsDirName(String dir, String logFileName, int traceNum, int minSupportThreshold, double confidence)
  {
    DateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd_HH-mm-ss");
    return dir+SLASH+outputGroupDir+"results_"+logFileName+"_"+traceNum+"_B_"+minSupportThreshold+"_"+confidence+"_"+dateFormat.format(now);
  }
  
  
  @Override
  protected void printHelp() {
    System.out.println("Sam/Mine version "+props.getProperty("sam.version"));
    System.out.println("usage:  sam_mine <inputfile.xes.gz> <support> <confidence>");
    System.out.println("  <inputfile>     path to log file");
    System.out.println("  <support>       minimum support threshold (integers > 0)");
    System.out.println("  <confidence>    minimum confidence (between 0.0 and 1.0)");
  }
  
  @Override
  public boolean readCommandLine(String[] args) {
    if (args.length != 3) {
      printHelp();
      return false;
    }
    
    File f = new File(args[0]);
    
    experimentFileRoot = f.getParent();
    inputFile = f.getName();
    try {
      support = Integer.parseInt(args[1]);
      confidence_branch = Double.parseDouble(args[2]);
      
      if (support < 1) throw new NumberFormatException("support must be larger than 0");
      if (confidence_branch < 0.0 || confidence_branch > 1.0) throw new NumberFormatException("confidence must be between 0.0 and 1.0");
      
    } catch (NumberFormatException e) {
      System.err.println(e);
      printHelp();
      return false;
    }
    
    return true;
  }
  
  public static void main(String[] args) throws IOException {
    
    RunExperimentBranching exp = new RunExperimentBranching();
    //if (!exp.readCommandLine(args)) return;
    
  
    //exp.setParameters("./experiments/crossftp_invariants/", "crossftp_invariants.xes.gz", 1.0 /*fract*/, 120 /*supp*/, 1.0 /* conf B */, 1.0 /* conf L */);
    exp.setParameters("./experiments_fse2012/", "columba_filtered.xes.gz", 1.0 /*fract*/, 20 /*supp*/, 1.0 /* conf B */, 1.0 /* conf L */);
    
    
    exp.experiment();
  }

}
