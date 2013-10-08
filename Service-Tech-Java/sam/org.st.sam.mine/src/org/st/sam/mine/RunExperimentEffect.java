package org.st.sam.mine;

import java.io.File;
import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;


import org.deckfour.xes.model.XLog;
import org.st.sam.log.SLog;
import org.st.sam.log.SLogTree;
import org.st.sam.log.SLogTree.SupportedWord;
import org.st.sam.log.SLogTreeNode;
import org.st.sam.log.SScenario;
import org.st.sam.mine.MineLSC.Configuration;
import org.st.sam.mine.collect.SimpleArrayList;
import org.st.sam.mine.datastructure.LSC;
import org.st.sam.mine.datastructure.LSCEvent;

import com.google.gwt.dev.util.collect.HashMap;

public class RunExperimentEffect extends RunExperimentCompare {

  private static String SLASH = System.getProperty("file.separator");
  
  public RunExperimentEffect() throws IOException {
    super();
  }
  
  public RunExperimentEffect(String dotBinary, String mscGenBinary) throws IOException {
    super(dotBinary, mscGenBinary);
  }

  
  public List<short[]> getCanonicalEffects(XLog xlog) {
    List<short[]> triggers = new LinkedList<short[]>();
    
    SLog log = new SLog(xlog);
    SLogTree tree = new SLogTree(log, true);
    
    // add all leaf events to the effects
    for (SLogTreeNode n : tree.nodes) {
      if (n.post.length == 0) {
        triggers.add(new short[] { n.id } );  
      }
    }
    
    return triggers;
  }
  
  List<short[]> effects = null;
  
  
  
  @Override
  public ArrayList<LSC> runMiner_Branch(String logFile, XLog xlog, int minSupportThreshold, double confidence) throws IOException {

    Configuration c_br = Configuration.mineBranching();
    c_br.effects = effects;
    c_br.optimizedSearch = true;
    c_br.removeSubsumed = true;
    minerBranch = new MineLSC(c_br, supportedWords);
    minerBranch.OPTIONS_WEIGHTED_OCCURRENCE = true;
    System.out.println("mining branching lscs from "+logFile);
    minerBranch.mineLSCs(logFile, minSupportThreshold, confidence);
    
    originalScenarios_branch = new HashMap<LSC, SScenario>();
    for (LSC l : minerBranch.getScenarios().keySet()) {
      originalScenarios_branch.put(l, minerBranch.getScenarios().get(l));
    }
    
    minerBranch.time_candiate_words = time_supported_words;
    
    return minerBranch.getLSCs();
  }
  
  @Override
  public ArrayList<LSC> runMiner_Linear(String logFile, XLog xlog, int minSupportThreshold, double confidence) throws IOException {

    Configuration c_lin = Configuration.mineLinear();
    c_lin.effects = effects;
    c_lin.optimizedSearch = true;
    c_lin.removeSubsumed = true;
    minerLinear = new MineLSC(c_lin, supportedWords);
    minerLinear.OPTIONS_WEIGHTED_OCCURRENCE = true;
    System.out.println("mining linear lscs from "+logFile);
    minerLinear.mineLSCs(logFile, minSupportThreshold, confidence);
    
    originalScenarios_linear = new HashMap<LSC, SScenario>();

    for (LSC l : minerLinear.getScenarios().keySet()) {
      originalScenarios_linear.put(l, minerLinear.getScenarios().get(l));
    }
    
    return minerLinear.getLSCs();
  }
  
  private SimpleArrayList<SupportedWord> supportedWords;
  private long time_supported_words;
  
  private void computeSupportedWordsOnInvertedTree(String logFile, XLog xlog, int minSupportThreshold, double confidence_branch, double confidence_linear) throws IOException {
    SLog invertedLog = new SLog(xlog, true);
    MineBranchingTree inverseTree = new MineBranchingTree(invertedLog, false);
    
    List<short[]> inverted_effects = new LinkedList<short[]>();
    for (short[] effect : effects) {
      inverted_effects.add(SLog.invert(effect));
    }
    
    Configuration c_br = Configuration.mineBranching();
    c_br.triggers = inverted_effects; // inverted effects are triggers on inverted trees
    c_br.optimizedSearch = true;
    c_br.discoverScenarios = false;
    MineLSC miner = new MineLSC(c_br);
    miner.OPTIONS_WEIGHTED_OCCURRENCE = true;
    System.out.println("discovering words from inverted tree of "+logFile);
    miner.mineLSCs(invertedLog, inverseTree, minSupportThreshold, confidence_branch, null);
    
    supportedWords = new SimpleArrayList<SLogTree.SupportedWord>();
    for (SupportedWord w : miner.getSupportedWords()) {
      SupportedWord _w = new SupportedWord(SLog.invert(w.word), w.support);
      supportedWords.add(_w);
    }
    
    time_supported_words = miner.time_candiate_words;
    
  }

  @Override
  public void runMiners(String logFile, XLog xlog, int minSupportThreshold, double confidence_branch, double confidence_linear) throws IOException {
    if (effects == null) effects = getCanonicalEffects(xlog);
    
    computeSupportedWordsOnInvertedTree(logFile, xlog, minSupportThreshold, confidence_branch, confidence_linear);
    super.runMiners(logFile, xlog, minSupportThreshold, confidence_branch, confidence_linear);
  }
  
  @Override
  public String getResultsDirName(String dir, String logFileName, final int traceNum, int minSupportThreshold, double confidence) {

    DateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd_HH-mm-ss");
    return dir+SLASH+"results_"+logFileName+"_"+traceNum+"_EFF_"+minSupportThreshold+"_"+confidence+"_"+dateFormat.format(now);
  
  }
  
  @Override
  protected ArrayList<LSC> getNonSubsumedScenarios(ArrayList<LSC> lscs, Map<LSC, SScenario> originalScenarios, MineLSC miner_for_subsumption) {
    return lscs;
  }
  
  @Override
  protected void writeResults_perPreChart(StringBuilder r,
      LSCEvent[] preChart,
      final String resultsDir,
      int treeFigHeight,
      boolean render_trees) throws IOException
  {
    if (minerBranch == null) render_trees = false;

    String title = "";
    for (LSCEvent e : preChart) title += " "+e.getMethodName();
    
    ArrayList<LSC> both_match = new ArrayList<LSC>();
    ArrayList<LSC> branching_match = new ArrayList<LSC>();
    ArrayList<LSC> linear_match = new ArrayList<LSC>();
    
    for (LSC l : both) {
      if (ml && pre_chart_comp.compare(l.getPreChart(), preChart) == 0)
      {
        both_match.add(l);
        //System.out.println(originalScenarios_branch.get(l));
      }
    }
    for (LSC l : onlyLinear) {
      if (ml && pre_chart_comp.compare(l.getPreChart(), preChart) == 0)
      {
        //linear_match.add(l);
        // strictly linear scenarios (not discovered by branching miner)
        // are subsumed by branching scenarios by construction, i.e. they
        // are also discovered by the branching miner: add to both_match
        both_match.add(l);
      }
    }
    for (LSC l : onlyBranching) {
      if (mb && pre_chart_comp.compare(l.getPreChart(), preChart) == 0)
      {
        branching_match.add(l);
      }
    }
    
    if (branching_match.size() == 0 && both_match.size() == 0) return;
    
    r.append("<h2>"+title+"</h2>\n");
    if (branching_match.size() > 0) {
      r.append("<b>"+branching_match.size()+" strict branching LSC</b></br>");
      appendResults(r, branching_match, minerBranch, originalScenarios_branch, resultsDir, treeFigHeight, render_trees,
          "strictly branching LSC",
          "lsc_branch");
    }
    if (both_match.size() > 0) {
      r.append("<b>"+both_match.size()+" linear and branching LSC</b></br>");
      appendResults(r, both_match, minerLinear, originalScenarios_linear, resultsDir, treeFigHeight, render_trees,
          "linear and branching LSC",
          "lsc_both");
    }
    
    r.append("<a href='#general'>top</a><br/>\n");
    r.append("<hr/>\n");
  }
  
  
  @Override
  protected void printHelp() {
    System.out.println("Sam/Mine for Effects, version "+props.getProperty("sam.version"));
    System.out.println("usage:  sam_mine_effect <inputfile.xes.gz> <effect> <support> <confidence>");
    System.out.println("  <inputfile>     path to log file");
    System.out.println("  <effect>        effect as a sequence of event ids, e.g. 3,2,12");
    System.out.println("  <support>       minimum support threshold (integers > 0)");
    System.out.println("  <confidence>    minimum confidence (between 0.0 and 1.0)");
  }
  
  @Override
  public boolean readCommandLine(String[] args) {
    if (args.length != 4) {
      printHelp();
      return false;
    }
    
    File f = new File(args[0]);
    
    experimentFileRoot = f.getParent();
    inputFile = f.getName();
    try {
      effects = new LinkedList<short[]>();
      effects.add(SScenario.getWord(args[1]));
      support = Integer.parseInt(args[2]);
      confidence_branch = Double.parseDouble(args[3]);
      confidence_linear = confidence_branch;
      density = 1.0;
      fraction = 1.0;
      
      if (support < 1) throw new NumberFormatException("support must be larger than 0");
      if (confidence_branch < 0.0 || confidence_branch > 1.0) throw new NumberFormatException("confidence must be between 0.0 and 1.0");
      if (density < 0.0 || density > 1.0) throw new NumberFormatException("density must be between 0.0 and 1.0");
      
    } catch (Exception e) {
      System.err.println(e);
      printHelp();
      return false;
    }
    
    return true;
  }
  
  public static void main(String[] args) throws IOException {
    
    RunExperimentEffect exp = new RunExperimentEffect();
    if (!exp.readCommandLine(args)) return;

//    exp.setParameters("./experiments_fse2012/", "crossftp_filtered.xes.gz", 1.0 /*fract*/, 10 /*supp*/, .45 /* conf B */, .45 /* conf L */);
//    exp.effects = new LinkedList<short[]>();
//    //exp.effects.add(new short[] { 49, 50, 51, 52 });
//    //exp.effects.add(new short[] { 40, 41, 45 });
//    //exp.effects.add(new short[] {41, 27});
//    exp.effects.add(new short[] { 19 });
    
    exp.experiment();
  }

}
