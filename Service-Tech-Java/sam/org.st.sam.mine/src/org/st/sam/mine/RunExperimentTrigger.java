package org.st.sam.mine;

import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

import lscminer.datastructure.LSC;

import org.deckfour.xes.model.XLog;
import org.st.sam.log.Log_ChoiceConsistency;
import org.st.sam.log.SLog;
import org.st.sam.log.SLogTree;
import org.st.sam.log.SLogTreeNode;
import org.st.sam.log.SScenario;
import org.st.sam.mine.MineLSC.Configuration;

import com.google.gwt.dev.util.collect.HashMap;

public class RunExperimentTrigger extends RunExperimentCompare {

  private static String SLASH = System.getProperty("file.separator");
  
  public RunExperimentTrigger() throws IOException {
    super();
  }
  
  public RunExperimentTrigger(String dotBinary, String mscGenBinary) throws IOException {
    super(dotBinary, mscGenBinary);
  }

  public List<short[]> getCanonicalTriggers(XLog xlog) {
    List<short[]> triggers = new LinkedList<short[]>();
    
    SLog log = new SLog(xlog);
    SLogTree tree = new SLogTree(log, true);
    
    // add all root events to the triggers
    for (SLogTreeNode r : tree.roots) {
      triggers.add(new short[] { r.id } );
    }
    
    // add all branching events to the triggers
    for (SLogTreeNode n : tree.nodes) {
      if (n.post.length > 1) {
        triggers.add(new short[] { n.id } );  
      }
    }
    
    return triggers;
  }
  
  List<short[]> triggers = null;
  
  @Override
  public ArrayList<LSC> runMiner_Branch(String logFile, XLog xlog, int minSupportThreshold, double confidence) throws IOException {

    Configuration c_br = Configuration.mineBranching();
    c_br.triggers = triggers;
    c_br.optimizedSearch = true;
    minerBranch = new MineLSC(c_br);
    minerBranch.OPTIONS_WEIGHTED_OCCURRENCE = true;
    System.out.println("mining branching lscs from "+logFile);
    minerBranch.mineLSCs(logFile, minSupportThreshold, confidence);
    
    originalScenarios_branch = new HashMap<LSC, SScenario>();
    for (LSC l : minerBranch.getScenarios().keySet()) {
      originalScenarios_branch.put(l, minerBranch.getScenarios().get(l));
    }
    
    return minerBranch.getLSCs();
  }
  
  @Override
  public ArrayList<LSC> runMiner_Linear(String logFile, XLog xlog, int minSupportThreshold, double confidence) throws IOException {

    Configuration c_lin = Configuration.mineLinear();
    c_lin.triggers = triggers;
    c_lin.optimizedSearch = true;
    minerLinear = new MineLSC(c_lin, minerBranch.getSupportedWords());
    minerLinear.OPTIONS_WEIGHTED_OCCURRENCE = true;
    System.out.println("mining linear lscs from "+logFile);
    //LSCEvent[][] dataSet = MineLinearLSC.readInput(experimentFileRoot+"/"+inputFile);
    //if (dataSet == null)
    //  return;
    //LSCMiner minerLinear = new LSCMiner();
    //minerLinear.mineLSCs(dataSet, support, confidence, density);
    minerLinear.mineLSCs(logFile, minSupportThreshold, confidence);
    
    originalScenarios_linear = new HashMap<LSC, SScenario>();

    for (LSC l : minerLinear.getScenarios().keySet()) {
      originalScenarios_linear.put(l, minerLinear.getScenarios().get(l));
    }
    
    return minerLinear.getLSCs();
  }
  
  @Override
  public void runMiners(String logFile, XLog xlog, int minSupportThreshold, double confidence_branch, double confidence_linear) throws IOException {
    if (triggers == null) triggers = getCanonicalTriggers(xlog);
    
    super.runMiners(logFile, xlog, minSupportThreshold, confidence_branch, confidence_linear);
  }
  
  @Override
  public String getResultsDirName(String dir, String logFileName, int traceNum, int minSupportThreshold, double confidence) {
    DateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd_HH-mm-ss");
    return dir+SLASH+"results_"+logFileName+"_"+traceNum+"_TR_"+minSupportThreshold+"_"+confidence+"_"+dateFormat.format(now);
  }
  
  @Override
  protected void printHelp() {
    System.out.println("Sam/Mine for Triggers, version "+props.getProperty("sam.version"));
    System.out.println("usage:  sam_mine_trigger <inputfile.xes.gz> <support> <confidence>");
    System.out.println("  <inputfile>     path to log file");
    System.out.println("  <support>       minimum support threshold (integers > 0)");
    System.out.println("  <confidence>    minimum confidence (between 0.0 and 1.0)");
  }
  
  public static void main(String[] args) throws IOException {
    
    RunExperimentTrigger exp = new RunExperimentTrigger();
    //if (!exp.readCommandLine(args)) return;
    //exp.setParameters("./experiments/columba_ext/", "columba_ext_resampled2_agg.xes.gz", 1.0 /*fract*/, 22 /*supp*/, 1.0 /* conf */);
    
    
//    short trigger = 20;
//    
//    Log_ChoiceConsistency cons = new Log_ChoiceConsistency("./experiments_fse2012/"+"crossftp_filtered.xes.gz");
//    cons.getChoiceConsistency();
//    double confidence = cons.min_consistency_high[cons.min_consistency_high.length*2/3];
//    if (trigger != -1 && cons.continuation_consistency[trigger] != null) {
//      for (double c : cons.continuation_consistency[trigger].values()) {
//        if (confidence > c) confidence = c;
//      }
//    }
//    int support = cons.min_support_high[cons.min_consistency_high.length*2/3];    
//    if (trigger != -1 && cons.continuation_consistency[trigger] != null) {
//      for (int s : cons.continuation_support[trigger].values()) {
//        if (support > s) support = s;
//      }
//    }
//    exp.setParameters("./experiments_fse2012/", "crossftp_filtered.xes.gz", 1.0 /*fract*/, support /*supp*/, confidence /* conf B */, confidence /* conf L */);
//    exp.triggers = new LinkedList<short[]>();
//    exp.triggers.add(new short[] { trigger });

    
    //exp.setParameters("./experiments_fse2012/", "columba_filtered.xes.gz", 1.0 /*fract*/, 10 /*supp*/, 1.0 /* conf */, 1.0);
    exp.setParameters("./experiments_fse2012/", "crossftp_filtered.xes.gz", 1.0 /*fract*/, 1 /*supp*/, .45 /* conf B */, .45 /* conf L */);
    exp.triggers = new LinkedList<short[]>();
    //exp.triggers.add(new short[] { 49, 52 });
    exp.triggers.add(new short[] { 19 });
    
    exp.experiment();
  }

}
