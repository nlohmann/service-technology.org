package org.st.sam.mine;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Comparator;
import java.util.Date;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.Random;
import java.util.Set;
import java.util.SortedSet;
import java.util.TreeSet;


import org.deckfour.xes.model.XLog;
import org.deckfour.xes.model.XTrace;
import org.st.sam.log.Log_ChoiceConsistency;
import org.st.sam.log.SLogTree.TreeStatistics;
import org.st.sam.log.SLogTreeNode;
import org.st.sam.log.SScenario;
import org.st.sam.log.XESImport;
import org.st.sam.mine.MineLSC.Configuration;
import org.st.sam.mine.collect.SimpleArrayList;
import org.st.sam.mine.datastructure.LSC;
import org.st.sam.mine.datastructure.LSCEvent;
import org.st.sam.util.SAMOutput;

import com.google.gwt.dev.util.collect.HashMap;
import com.google.gwt.dev.util.collect.HashSet;

public class RunExperimentCompare {

  private static String SLASH = System.getProperty("file.separator");
  
  protected Properties props;
  
  private String mscRenderer;
  private String dotRenderer;
  
  private String lscFileFormat = "png";
  
  private static final int RENDER_ALWAYS = 0;
  private static final int RENDER_AUTO = 1;
  private static final int RENDER_NEVER = 2;
  private int renderTrees = RENDER_AUTO;

  public RunExperimentCompare() throws IOException {
    props = new Properties();
    props.load(new FileInputStream("sam_mine.properties"));
    
    String os = System.getProperty("os.name");
    String bin_ext = (os.startsWith("Windows")) ? ".exe" : "";

    dotRenderer = props.getProperty("sam.dot") + bin_ext;
    mscRenderer = props.getProperty("sam.mscgen") + bin_ext;
    
    lscFileFormat = props.getProperty("sam.lsc_img", "png");
    if (!(lscFileFormat.equals("svg")  || lscFileFormat.equals("png"))) lscFileFormat = "png";
    
    String treeRender = props.getProperty("sam.render_trees", "auto");
    if (treeRender.equals("always")) renderTrees = RENDER_ALWAYS;
    else if (treeRender.equals("never")) renderTrees = RENDER_NEVER;
    else renderTrees = RENDER_AUTO;
  }
  
  public RunExperimentCompare(String dotBinary, String mscGenBinary) throws IOException {
    mscRenderer = mscGenBinary;
    dotRenderer = dotBinary;
  }
  

  
  public static void systemCall(String command) {
    Runtime r = Runtime.getRuntime();

    try {
      /*
       * Here we are executing the UNIX command ls for directory listing. The
       * format returned is the long format which includes file information and
       * permissions.
       */
      //System.out.println("calling "+command);
      Process p = r.exec(command);
      InputStream in = p.getInputStream();
      BufferedInputStream buf = new BufferedInputStream(in);
      InputStreamReader inread = new InputStreamReader(buf);
      BufferedReader bufferedreader = new BufferedReader(inread);

      // Read the ls output
      String line;
      while ((line = bufferedreader.readLine()) != null) {
        System.out.println(line);
      }
      // Check for ls failure
      try {
        if (p.waitFor() != 0) {
          System.err.println("exit value = " + p.exitValue());
        }
      } catch (InterruptedException e) {
        System.err.println(e);
      } finally {
        // Close the InputStream
        bufferedreader.close();
        inread.close();
        buf.close();
        in.close();
      }
    } catch (IOException e) {
      System.err.println(e.getMessage());
    }
  }

  public static boolean lsc_equals(LSC l1, LSC l2) {
    if (l1.getPreChart().length != l2.getPreChart().length) return false;
    
    for (int i=0; i<l1.getPreChart().length; i++) {
      if (   l1.getPreChart()[i].getCaller() != l2.getPreChart()[i].getCaller()
          || l1.getPreChart()[i].getCallee() != l2.getPreChart()[i].getCallee()
          || l1.getPreChart()[i].getMethod() != l2.getPreChart()[i].getMethod()) {
        return false;
      }
    }
    
    if (l1.getMainChart().length != l2.getMainChart().length) return false;
    
    for (int i=0; i<l1.getMainChart().length; i++) {
      if (   l1.getMainChart()[i].getCaller() != l2.getMainChart()[i].getCaller()
          || l1.getMainChart()[i].getCallee() != l2.getMainChart()[i].getCallee()
          || l1.getMainChart()[i].getMethod() != l2.getMainChart()[i].getMethod()) {
        return false;
      }
    }
    return true;
  }
  
  public Comparator< LSCEvent[] > pre_chart_comp = new Comparator<LSCEvent[]>() {

    @Override
    public int compare(LSCEvent[] o1, LSCEvent[] o2) {
      for (int i=0; i<o1.length; i++) {
        if (i >= o2.length) return -1;
        int diff = o1[i].toString().compareTo(o2[i].toString());
        if (diff != 0) return diff;
      }
      if (o2.length > o1.length) return 1;
      else return 0;
    }
  };
  
  protected XLog readLog(final String logFile, final double traceFraction) throws IOException {
    XLog xlog = XESImport.readXLog(logFile);
    
    // filter traces to create a smaller log with less branching
    Random r = new Random();
    List<XTrace> toRemove = new LinkedList<XTrace>();
    for (XTrace t : xlog) {
      if (r.nextDouble() > traceFraction) {
        toRemove.add(t);
      }
    }
    for (XTrace t : toRemove) xlog.remove(t);
    return xlog;
  }
  

  
  protected MineLSC minerBranch = null;
  protected MineLSC minerLinear = null;
  protected Map<LSC, SScenario> originalScenarios_linear;
  protected Map<LSC, SScenario> originalScenarios_branch;
  
  private long runTime_minerBranch;
  
  public ArrayList<LSC> runMiner_Branch(final String logFile, final XLog xlog, final int minSupportThreshold, final double confidence) throws IOException {
    runTime_minerBranch = System.currentTimeMillis();
    
    Configuration config_branch = Configuration.mineBranching();
    config_branch.allowEventRepetitions = false;
    minerBranch = new MineLSC(config_branch);
    minerBranch.OPTIONS_WEIGHTED_OCCURRENCE = true;
    System.out.println("mining branching lscs from "+logFile);
    minerBranch.mineLSCs(xlog, minSupportThreshold, confidence, logFile);
    runTime_minerBranch = System.currentTimeMillis()-runTime_minerBranch;
    
    originalScenarios_branch = new HashMap<LSC, SScenario>();
    for (LSC l : minerBranch.getScenarios().keySet()) {
      originalScenarios_branch.put(l, minerBranch.getScenarios().get(l));
    }

    return minerBranch.getLSCs(); 
  }
  
  public ArrayList<LSC> runMiner_Linear(final String logFile, final XLog xlog, final int minSupportThreshold, final double confidence) throws IOException {
    Configuration config_linear = Configuration.mineLinear();
    config_linear.allowEventRepetitions = false;
    minerLinear = new MineLSC(config_linear, minerBranch.getSupportedWords());
    minerLinear.OPTIONS_WEIGHTED_OCCURRENCE = true;
    System.out.println("mining linear lscs from "+logFile);
    //LSCEvent[][] dataSet = MineLinearLSC.readInput(experimentFileRoot+"/"+inputFile);
    //if (dataSet == null)
    //  return;
    //LSCMiner minerLinear = new LSCMiner();
    //minerLinear.mineLSCs(dataSet, support, confidence, density);
    minerLinear.mineLSCs(xlog, minSupportThreshold, confidence, logFile);
    
    originalScenarios_linear = new HashMap<LSC, SScenario>();
    // comment out the following loop to consider only linear mining results
    for (LSC l : minerLinear.getScenarios().keySet()) {
      originalScenarios_linear.put(l, minerLinear.getScenarios().get(l));
    }
    
    return minerLinear.getLSCs();
  }
  
  public void runMiners(final String logFile, final XLog xlog, final int minSupportThreshold, final double confidence_branch, final double confidence_linear) throws IOException {
    branching_lscs = runMiner_Branch(logFile, xlog, minSupportThreshold, confidence_branch);
    linear_lscs = runMiner_Linear(logFile, xlog, minSupportThreshold, confidence_linear);
  }

  protected ArrayList<LSC> branching_lscs;
  protected ArrayList<LSC> linear_lscs;
  
  protected ArrayList<LSC> both;
  protected ArrayList<LSC> onlyBranching;
  protected ArrayList<LSC> onlyLinear; 
  
  public void separateResults() {
     
    both = new ArrayList<LSC>();
    onlyBranching = new ArrayList<LSC>();
    onlyLinear = new ArrayList<LSC>(); 
    for (LSC b : branching_lscs) {
      boolean is_linear = false;
      for (LSC l : linear_lscs) {
        if (lsc_equals(l, b)) {
          is_linear = true;
          break;
        }
      }
      if (!is_linear) onlyBranching.add(b);
      else {
        both.add(b);
        originalScenarios_branch.put(b, minerBranch.getScenarios().get(b));
        originalScenarios_linear.put(b, minerBranch.getScenarios().get(b));
      }
    }
    
    onlyLinear = new ArrayList<LSC>(); 
    for (LSC l : linear_lscs) {
      boolean is_branching = false;
      for (LSC b : branching_lscs) {
        if (lsc_equals(l, b)) {
          is_branching = true;
          break;
        }
      }
      if (!is_branching) onlyLinear.add(l);
    }
    
    /* uncomment this to get linear-only results
    both = linear_lscs;
    onlyBranching.clear();
    onlyLinear.clear();
    */
  }
  
  protected Date now;
  protected String resultsDir;
  
  public void setResultsDirNameGroupInfix(String dir, String logFileName) {
    DateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd_HH-mm");
    outputGroupDir = logFileName+"_"+dateFormat.format(new Date())+SLASH;
  }
  
  public String getResultsDirName(String dir, String logFileName, final int traceNum, final int minSupportThreshold, final double confidence) {
    DateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd_HH-mm-ss");
    return dir+SLASH+outputGroupDir+"results_"+logFileName+"_"+traceNum+"_BL_"+minSupportThreshold+"_"+confidence+"_"+dateFormat.format(now);
  }
  
  public boolean createOutputDirectory(final String dir, final String inputFile, final int traceNum, final int minSupportThreshold, final double confidence) {
    
    now = new Date();
    
    String logFileName = inputFile.substring(inputFile.lastIndexOf('/')+1, inputFile.indexOf('.',inputFile.lastIndexOf('/')));
    resultsDir = getResultsDirName(dir, logFileName, traceNum, minSupportThreshold, confidence);
    
    File f = new File(resultsDir);
    
    if (!f.exists()) {
      if (!f.mkdir()) {
        System.err.println("Couldn't create results directory: "+resultsDir);
        return false;
      }
    }
    return true;
    
  }
  
  public static class ScenarioStatistics {
    public double preChartLength;
    public double mainChartLength;
    public double components;
  }
  
  public ScenarioStatistics[] getStatistics(Collection<LSC> scenarios) {
    
    if (scenarios.size() == 0) {
      ScenarioStatistics null_stat = new ScenarioStatistics() {{
        preChartLength=0;
        mainChartLength=0;
        components=0;
      }};
      ScenarioStatistics[] result = new ScenarioStatistics[3];

      result[0] = null_stat;
      result[1] = null_stat;
      result[2] = null_stat;
      
      return result;
 
    }
    
    ScenarioStatistics min = new ScenarioStatistics() {{
      preChartLength=Double.MAX_VALUE;
      mainChartLength=Double.MAX_VALUE;
      components=Double.MAX_VALUE;
    }};
    ScenarioStatistics avg = new ScenarioStatistics() {{
      preChartLength=0;
      mainChartLength=0;
      components=0;
    }};
    ScenarioStatistics max = new ScenarioStatistics() {{
      preChartLength=Double.MIN_VALUE;
      mainChartLength=Double.MIN_VALUE;
      components=Double.MIN_VALUE;
    }};
    
    for (LSC l : scenarios) {
      min.preChartLength = (min.preChartLength > l.getPrechartEventNo()) ? l.getPrechartEventNo() : min.preChartLength;
      min.mainChartLength = (min.mainChartLength > l.getMainchartEventNo()) ? l.getMainchartEventNo() : min.mainChartLength;
      min.components = (min.components > l.getDistinctObjectNo()) ? l.getDistinctObjectNo() : min.components;
      
      avg.preChartLength += l.getPrechartEventNo();
      avg.mainChartLength += l.getMainchartEventNo();
      avg.components += l.getDistinctObjectNo();
      
      max.preChartLength = (max.preChartLength < l.getPrechartEventNo()) ? l.getPrechartEventNo() : max.preChartLength;
      max.mainChartLength = (max.mainChartLength < l.getMainchartEventNo()) ? l.getMainchartEventNo() : max.mainChartLength;
      max.components = (max.components < l.getDistinctObjectNo()) ? l.getDistinctObjectNo() : max.components;

    }
    
    avg.preChartLength /= scenarios.size();
    avg.mainChartLength /= scenarios.size();
    avg.components /= scenarios.size();
    
    ScenarioStatistics[] result = new ScenarioStatistics[3];
    result[0] = min;
    result[1] = avg;
    result[2] = max;
    
    return result;
  }
  
  /**
   * list of system calls that still have to be made to render all remaining graphics
   */
  private LinkedList<String> renderJobs;
  
  protected boolean mb = false; // branching miner executed
  protected boolean ml = false; // linear miner executed
  
  public String experiment(final String dir, final String inputFile, final XLog xlog, final int minSupportThreshold, final double confidence_branch, final double confidence_linear) throws IOException {
    
    System.out.println("---[ LSC Miner ] ------------------------------------------------------------");
    System.out.println("   "+inputFile);
    System.out.println("   size: "+xlog.size());
    System.out.println("   support: "+minSupportThreshold);
    System.out.println("   confidence branching: "+confidence_branch);
    System.out.println("   confidence linear: "+confidence_linear);
    System.out.println("-----------------------------------------------------------------------------");
    
    String logFile = dir+"/"+inputFile;
    runMiners(logFile, xlog, minSupportThreshold, confidence_branch, confidence_linear);
    mb = (minerBranch != null);
    ml = (minerLinear != null);
    
    renderJobs = new LinkedList<String>();
    
    boolean render_trees = this.renderTrees == RENDER_AUTO || this.renderTrees == RENDER_ALWAYS;
    int nodeSize = 0;
    if (mb) nodeSize = minerBranch.getTree().nodes.size();
    else if (ml) nodeSize = minerLinear.getTree().nodes.size();
    
    if (this.renderTrees == RENDER_AUTO && nodeSize > 3000) {
      // disable tree rendering for large trees in case of automatic rendering
      render_trees = false;
    }
    
    separateResults();
    if (mb) minerBranch.getSLog().lscEventTable.shorteNames();
    if (ml) minerLinear.getSLog().lscEventTable.shorteNames();
    
    MineLSC.sortLSCs(both);
    MineLSC.sortLSCs(onlyBranching);
    MineLSC.sortLSCs(onlyLinear);

    if (!createOutputDirectory(dir, inputFile, xlog.size(), minSupportThreshold, confidence_branch)) return "could not write results";

    // resulting html
    StringBuilder r = new StringBuilder();
    r.append("<html>\n");
    r.append("<head>\n");
    r.append("  <title>Results for "+dir+"/"+inputFile+"</title>\n");
    r.append("</head>\n");
    r.append("<body style='font-family: sans-serif'>\n");
    r.append("<a name='general' />\n");
    r.append("<h1>Results for "+dir+"/"+inputFile+"</h1>\n");
    
    r.append("<h2>General</h2>\n");
    
    DateFormat dateFormat2 = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
    r.append("executed on: "+dateFormat2.format(now)+"<br/>\n");
    r.append("input file: "+dir+"/"+inputFile+"<br/>\n");
    r.append("min. support threshold: "+minSupportThreshold+"<br/>\n");
    if (mb) r.append("confidence branching scenarios: "+confidence_branch+"<br/>\n");
    if (ml) r.append("confidence linear scenarios: "+confidence_linear+"<br/>\n");


    r.append("<h2>Input</h2>\n");
    
    TreeStatistics stat = null;
    if (mb) stat = minerBranch.getTree().getStatistics();
    else if (ml) stat = minerLinear.getTree().getStatistics();
    r.append("number of nodes: "+stat.size+"<br/>\n");
    r.append("number of events: "+stat.alphabetSize+"<br/>\n");
    r.append("avg. out degree: "+stat.averageOutDegree+"<br/>\n");
    r.append("max. out degree: "+stat.maxOutDegree+"<br/>\n");
    r.append("depth: "+stat.depth+"<br/>\n");
    r.append("width: "+stat.width+"<br/>\n");
    
    Log_ChoiceConsistency choices = null;
    if (mb) choices = new Log_ChoiceConsistency(minerBranch.getTree());
    else if (ml) choices = new Log_ChoiceConsistency(minerLinear.getTree());
    choices.getChoiceConsistency();
    
    r.append("<b>max. support/confidence to find at least one alternative with k branches</b><br/>\n");
    r.append("<table>\n");
    r.append("  <tr style='border:1px'>\n");
    r.append("    <td>k</td>");
    for (int i=0;i<choices.min_consistency_high.length;i++) {
      if (choices.min_consistency_high[i] >= confidence_branch) {
        r.append("<td style='background-color: #AAAAAA;'><b>"+(i+1)+"</b></td>");
      } else {
        r.append("<td>"+(i+1)+"</td>");
      }
    }
    r.append("\n");
    r.append("  </tr>\n");
    
    r.append("  <tr>\n");
    r.append("    <td>support</td>");
    for (int i=0;i<choices.min_consistency_high.length;i++) {
      if (choices.min_support_high[i] >= minSupportThreshold) {
        r.append("<td><b>"+choices.min_support_high[i]+"</b></td>");
      } else {
        r.append("<td>"+choices.min_support_high[i]+"</td>");
      }
    }
    r.append("\n");
    r.append("  </tr>\n");    
    
    r.append("  <tr>\n");
    r.append("    <td>confidence</td>");
    for (int i=0;i<choices.min_consistency_high.length;i++) {
      if (choices.min_consistency_high[i] >= confidence_branch) {
        r.append("<td><b>"+choices.min_consistency_high[i]+"</b></td>");
      } else {
        r.append("<td>"+choices.min_consistency_high[i]+"</td>");
      }
    }
    r.append("\n");
    r.append("  </tr>\n");    
    r.append("</table>\n");
    
    
    r.append("<b>max. support/confidence to find all alternatives with k branches</b><br/>\n");
    r.append("<table>\n");
    r.append("  <tr>\n");
    r.append("    <td>k</td>");
    for (int i=0;i<choices.min_consistency_low.length;i++) {
      if (choices.min_consistency_low[i] >= confidence_branch) {
        r.append("<td style='background-color: #AAAAAA;'><b>"+(i+1)+"</b></td>");
      } else {
        r.append("<td>"+(i+1)+"</td>");
      }
    }
    r.append("\n");
    r.append("  </tr>\n");
    
    r.append("  <tr>\n");
    r.append("    <td>support</td>");
    for (int i=0;i<choices.min_consistency_low.length;i++) {
      if (choices.min_support_low[i] >= minSupportThreshold) {
        r.append("<td><b>"+choices.min_support_low[i]+"</b></td>");
      } else {
        r.append("<td>"+choices.min_support_low[i]+"</td>");
      }
    }
    r.append("\n");
    r.append("  </tr>\n");    
    
    r.append("  <tr>\n");
    r.append("    <td>confidence</td>");
    for (int i=0;i<choices.min_consistency_low.length;i++) {
      if (choices.min_consistency_low[i] >= confidence_branch) {
        r.append("<td><b>"+choices.min_consistency_low[i]+"</b></td>");
      } else {
        r.append("<td>"+choices.min_consistency_low[i]+"</td>");
      }
    }
    r.append("\n");
    r.append("  </tr>\n");    
    r.append("</table>\n");
    
    int treeFigHeight = stat.depth*5;
    if (treeFigHeight > 600) treeFigHeight = 600;
    
    r.append("<h2>Discovered Scenarios (general statistics)</h2>\n");
    
    double[] coverage_branch = null;
    double[] coverage_linear = null;
    
    if (mb) minerBranch.getCoverageTreeGlobal();
    if (ml) minerLinear.getCoverageTreeGlobal();
    
    if (mb) coverage_branch = minerBranch.getTree().getCoverage();
    if (ml) coverage_linear = minerLinear.getTree().getCoverage();
    if (mb) r.append("tree coverage (branch), main chart events: "+coverage_branch[1]+"<br/>\n");
    if (ml) r.append("tree coverage (linear), main chart events: "+coverage_linear[1]+"<br/>\n");
    r.append("<br/>");
    if (mb)r.append("tree coverage (branch), all chart events: "+coverage_branch[0]+"<br/>\n");
    if (ml) r.append("tree coverage (linear), all chart events: "+coverage_linear[0]+"<br/>\n");

    
    double coveredAlphabet_branch = 0;
    double coveredAlphabet_linear = 0;
    
    if (mb) {
      Set<Short> coveredEvents_branch = new HashSet<Short>();
      for (SScenario s : minerBranch.getScenarios().values()) {
        for (short e : s.pre) coveredEvents_branch.add(e);
        for (short e : s.main) coveredEvents_branch.add(e);
      }
      coveredAlphabet_branch = (double)coveredEvents_branch.size()/minerBranch.getSLog().originalNames.length;
      r.append("alphabet coverage (branch): "+coveredAlphabet_branch+"<br/>\n");
    }
    
    if (ml) {
      Set<Short> coveredEvents_linear = new HashSet<Short>();
      for (SScenario s : minerLinear.getScenarios().values()) {
        for (short e : s.pre) coveredEvents_linear.add(e);
        for (short e : s.main) coveredEvents_linear.add(e);
      }
      coveredAlphabet_linear = (double)coveredEvents_linear.size()/minerLinear.getSLog().originalNames.length;
      r.append("alphabet coverage (linear): "+coveredAlphabet_linear+"<br/>\n");
    }
    
    ScenarioStatistics[] stat_branch_only = getStatistics(onlyBranching);
    ScenarioStatistics[] stat_both = getStatistics(linear_lscs);
    
    r.append("<br/>\n");
    if (mb) {
      r.append("supported words: "+minerBranch.number_of_candidate_words+"<br/>\n");
      r.append("time to find supported words: "+(minerBranch.time_candiate_words)+"ms</br>");
      r.append("time to find branching scenarios: "+(minerBranch.time_scenario_discovery)+"ms<br/>");
    } else if (ml) {
      r.append("supported words: "+minerLinear.number_of_candidate_words+"<br/>\n");
      r.append("time to find supported words: "+(minerLinear.time_candiate_words)+"ms</br>");
    }
    if (ml) {
      r.append("time to find linear scenarios   : "+(minerLinear.time_scenario_discovery)+"ms<br/>");
    }
    
    r.append("<br/>\n");
    if (mb) r.append("total number of scenarios found by branching miner: "+minerBranch.total_number_of_scenarios+"<br/>\n");
    if (ml)r.append("total number of scenarios found by linear miner: "+minerLinear.total_number_of_scenarios+"<br/>\n");
    
    r.append("<br/>\n");
    r.append("<b>strictly branching scenarios (ignoring subsumed scenarios)</b><br/>\n");
    r.append("found <b>"+onlyBranching.size()+" strictly branching</b> scenarios<br/>\n");
    r.append("length pre-chart (min/avg/max): "+stat_branch_only[0].preChartLength+"/"+stat_branch_only[1].preChartLength+"/"+stat_branch_only[2].preChartLength+"<br/>\n");
    r.append("length main-chart (min/avg/max): "+stat_branch_only[0].mainChartLength+"/"+stat_branch_only[1].mainChartLength+"/"+stat_branch_only[2].mainChartLength+"<br/>\n");
    r.append("#components (min/avg/max): "+stat_branch_only[0].components+"/"+stat_branch_only[1].components+"/"+stat_branch_only[2].components+"<br/>\n");
   
    r.append("<br/>\n");
    r.append("<b>linear and branching scenarios (ignoring subsumed scenarios)</b><br/>\n");
    r.append("found <b>"+(both.size()+onlyLinear.size())+" linear</b> and branching scenarios<br/>\n");
    r.append("length pre-chart (min/avg/max): "+stat_both[0].preChartLength+"/"+stat_both[1].preChartLength+"/"+stat_both[2].preChartLength+"<br/>\n");
    r.append("length main-chart (min/avg/max): "+stat_both[0].mainChartLength+"/"+stat_both[1].mainChartLength+"/"+stat_both[2].mainChartLength+"<br/>\n");
    r.append("#components (min/avg/max): "+stat_both[0].components+"/"+stat_both[1].components+"/"+stat_both[2].components+"<br/>\n");
    r.append("<br/>\n");
    

    if (render_trees) {
      String input_tree_dot = inputFile+".dot";
      String input_tree_svg = inputFile+".svg";
      String input_tree_png = inputFile+".png";
      
      if (mb) {
        minerBranch.getTree().clearCoverageMarking();
        minerBranch.getCoverageTreeGlobal();
        //SAMOutput.writeToFile(minerBranch.getTree().toDot_ScenarioCoverage(minerBranch.getShortenedNames()), resultsDir+SLASH+input_tree_dot);
        SAMOutput.writeToFile(minerBranch.getTree().toDot(minerBranch.getShortenedNames()), resultsDir+SLASH+input_tree_dot);
      } else {
        minerLinear.getTree().clearCoverageMarking();
        minerLinear.getCoverageTreeGlobal();
        //SAMOutput.writeToFile(minerBranch.getTree().toDot_ScenarioCoverage(minerBranch.getShortenedNames()), resultsDir+SLASH+input_tree_dot);
        SAMOutput.writeToFile(minerLinear.getTree().toDot(minerLinear.getShortenedNames()), resultsDir+SLASH+input_tree_dot);
      }
      String call = dotRenderer+" -Tsvg "+resultsDir+SLASH+input_tree_dot+" -o"+resultsDir+SLASH+input_tree_svg;
      //systemCall(call);
      renderJobs.add(call); // queue rendering   

      //systemCall(dotRenderer+" -Tpng -Gsize=30 "+resultsDir+SLASH+input_tree_dot+" -o"+resultsDir+SLASH+input_tree_png);
      
      //r.append("<object style='height: 300px' data='"+input_tree_svg+"' type='image/svg+xml'></object><br/>\n");
      //r.append("<a href='"+input_tree_svg+"'><img style='height: "+treeFigHeight+"px' src='"+input_tree_png+"'/></a><br/>\n");
      r.append("<a href='"+input_tree_svg+"'><b>input with complete scenario coverage</b></a><br/>\n");
    } else {
      r.append("<i>original input not shown because of configuration</i><br/>\n");
    }
    
    writeResults_allCharts(r, resultsDir, treeFigHeight, render_trees);

    r.append("</body>\n");
    
    SAMOutput.writeToFile(r.toString(), resultsDir+"/results.html");
    if (mb) SAMOutput.writeToFile(minerBranch.getLSCs().toString(), resultsDir+"/lscs_branching.txt");
    if (ml) SAMOutput.writeToFile(minerLinear.getLSCs().toString(), resultsDir+"/lscs_linear.txt");

    
    System.out.println("finished mining.");
    if (mb) {
      System.out.println("discovering candidate words    : "+(minerBranch.time_candiate_words)+"ms");
      System.out.println("discovering branching scenarios: "+(minerBranch.time_scenario_discovery)+"ms");
    } else if (ml) {
      System.out.println("discovering candidate words    : "+(minerLinear.time_candiate_words)+"ms");
    }
    if (ml) {
      System.out.println("discovering linear scenarios   : "+(minerLinear.time_scenario_discovery)+"ms");
    }
    
    System.out.println("rendering "+renderJobs.size()+" graphics");
    int count = 0;
    for (String call : renderJobs) {
      count++;
      System.out.print(".");
      if (count % 70 == 0) System.out.println(count+"/"+renderJobs.size());
      
      systemCall(call);
    }
    System.out.println("\nfinished rendering.");

    if (mb && ml) {
      minerBranch.getTree().clearCoverageMarking();
      minerBranch.getCoverageTreeGlobal();
      minerLinear.getTree().clearCoverageMarking();
      minerLinear.getCoverageTreeGlobal();
      
      String stat_summary = inputFile+";"+xlog.size()+";"+minSupportThreshold+";"+confidence_branch+";"+confidence_linear+";"
          +stat.size+";"+stat.alphabetSize+";"+stat.averageOutDegree+";"+stat.maxOutDegree+";"+stat.depth+";"+stat.width+";"
          +minerBranch.getTree().getCoverage()[1]+";"+minerLinear.getTree().getCoverage()[1]+";"+coveredAlphabet_branch+";"+coveredAlphabet_linear+";"
          +onlyBranching.size()+";"+(both.size()+onlyLinear.size())+";"
          +stat_branch_only[0].preChartLength+";"+stat_branch_only[1].preChartLength+";"+stat_branch_only[2].preChartLength+";"
          +stat_branch_only[0].mainChartLength+";"+stat_branch_only[1].mainChartLength+";"+stat_branch_only[2].mainChartLength+";"
          +stat_branch_only[0].components+";"+stat_branch_only[1].components+";"+stat_branch_only[2].components+";"
          +stat_both[0].preChartLength+";"+stat_both[1].preChartLength+";"+stat_both[2].preChartLength+";"
          +stat_both[0].mainChartLength+";"+stat_both[1].mainChartLength+";"+stat_both[2].mainChartLength+";"
          +stat_both[0].components+";"+stat_both[1].components+";"+stat_both[2].components+";"
          +minerBranch.time_candiate_words+";"+minerBranch.time_scenario_discovery+";"+minerLinear.time_scenario_discovery;
      return stat_summary+"\n";
    } else {
      return null;
    }

    
  }
  
  public static String getCSVheader() {
    String stat_header = "inputFile;traceFraction;support;confidence(B);confidence(L);"
        +"#nodes;alphabetSize;avg degree;max degree;depth;width;"
        +"coverage (B);coverage (L);alphabet coverage(B);alphabet coverage(L);"
        +"#scen (B);#scen (B+L);"
        +"pre (min)(B);pre (avg)(B);pre (max)(B);"
        +"main (min)(B);main (avg)(B);main (max)(B);"
        +"#comp (min)(B);#comp (avg)(B);#comp (max)(B);"
        +"pre (min)(L);pre (avg)(L);pre (max)(L);"
        +"main (min)(L);main (avg)(L);main (max)(L);"
        +"#comp (min)(L);#comp (avg)(L);#comp (max)(L);"
        +"time W;time B;time L";
    return stat_header;
  }
  
  private void writeResults_allCharts(StringBuilder r, String resultsDir, int treeFigHeight, boolean render_trees) throws IOException {
    SortedSet< LSCEvent[] > preCharts = new TreeSet<LSCEvent[]>(pre_chart_comp);
    
    for (LSC l : both) {
      preCharts.add(l.getPreChart());
    }
    for (LSC l : onlyBranching) {
      preCharts.add(l.getPreChart());
    }
    for (LSC l : onlyLinear) {
      preCharts.add(l.getPreChart());
    }

    //writeResultsSection(r, onlyBranching, resultsDir, treeFigHeight, minerBranch, originalScenarios, render_trees, "Strictly Branching LSCs", "lsc_branch");
    //writeResultsSection(r, onlyLinear, resultsDir, treeFigHeight, minerBranch, originalScenarios, render_trees, "Strictly Linear LSCs", "lsc_linear");
    //writeResultsSection(r, both, resultsDir, treeFigHeight, minerBranch, originalScenarios, false, "Linear and Branching LSCs", "lsc_both");
    for (LSCEvent[] preChart : preCharts) {
      writeResults_perPreChart(r, preChart, resultsDir, treeFigHeight, render_trees);
    }
  }
  
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
      if (ml && pre_chart_comp.compare(l.getPreChart(), preChart) == 0
          && !isSubsumed(originalScenarios_branch.get(l), minerLinear, originalScenarios_linear))
      {
        both_match.add(l);
        //System.out.println(originalScenarios_branch.get(l));
      }
    }
    for (LSC l : onlyLinear) {
      if (ml && pre_chart_comp.compare(l.getPreChart(), preChart) == 0
          && !isSubsumed(originalScenarios_linear.get(l),  minerLinear, originalScenarios_linear))
      {
        //linear_match.add(l);
        // strictly linear scenarios (not discovered by branching miner)
        // are subsumed by branching scenarios by construction, i.e. they
        // are also discovered by the branching miner: add to both_match
        both_match.add(l);
      }
    }
    for (LSC l : onlyBranching) {
      if (mb && pre_chart_comp.compare(l.getPreChart(), preChart) == 0
          && !isSubsumed(originalScenarios_branch.get(l), minerBranch, originalScenarios_branch))
      {
        branching_match.add(l);
      }
    }
    
    if (branching_match.size() == 0 && both_match.size() == 0) return;
    
    r.append("<h2>"+title+"</h2>\n");
    if (branching_match.size() > 0) {
      r.append("<b>"+branching_match.size()+" strictly branching LSC</b></br>");
      appendResults(r, branching_match, minerBranch, originalScenarios_branch, resultsDir, treeFigHeight, render_trees,
          "strictly branching LSC",
          "lsc_branch");
    }
    if (both_match.size() > 0) {
      r.append("<b>"+both_match.size()+" linear and branching LSC</b></br>");
      appendResults(r, both_match, minerLinear, originalScenarios_linear,resultsDir, treeFigHeight, render_trees,
          "linear and branching LSC",
          "lsc_both");
    }
    
    r.append("<a href='#general'>top</a><br/>\n");
    r.append("<hr/>\n");
  }
  
  private int lsc_num=0; 
  
 
  protected ArrayList<LSC> getNonSubsumedScenarios(ArrayList<LSC> lscs, Map<LSC, SScenario> originalScenarios, MineLSC miner_for_subsumption) {
    ArrayList<LSC> nonSubsumed = new ArrayList<LSC>();
    for (LSC l : lscs) {
      if (!isSubsumed(originalScenarios.get(l), miner_for_subsumption, originalScenarios)) {
        nonSubsumed.add(l);
      }
    }
    return nonSubsumed;
  }
  
  protected void appendResults(
      StringBuilder r,
      ArrayList<LSC> lscs,
      MineLSC miner_for_subsumption,
      Map<LSC, SScenario> originalScenarios,
      final String resultsDir,
      int treeFigHeight,
       boolean render_trees,
       String type,
       String ref_prefix) throws IOException
  {
    StringBuilder found_lscs = new StringBuilder();
    
    int totalWidth = 0;
    
    if (mb) minerBranch.getTree().clearCoverageMarking();
    if (ml) minerLinear.getTree().clearCoverageMarking();
    
    ArrayList<LSC> nonSubsumed = getNonSubsumedScenarios(lscs, originalScenarios, miner_for_subsumption);
    if (nonSubsumed.isEmpty()) return;
    
    for (LSC l : nonSubsumed) {
      totalWidth += SAMOutput.getMSCWitdth(SAMOutput.getComponents(l))+10;
    }

    r.append("<table style='width:"+totalWidth+"px'>\n");
    r.append("<tr>\n");
    
    for (LSC l : nonSubsumed) {
      
      lsc_num++;

      found_lscs.append(l.toString());
      found_lscs.append("\n");
      
      

      double conf = ((double)((int)(l.getConfidence()*100))/100);
      String lsc_string = SAMOutput.toMSCRenderer("LSC "+(lsc_num+1)+" conf="+conf+" supp="+l.getSupport(), l);
      final String lsc_resultfile = "lsc_"+(lsc_num+1)+"_"+ref_prefix+".lsc.txt";
      final String lsc_renderfile = "lsc_"+(lsc_num+1)+"_"+ref_prefix+".lsc."+lscFileFormat;
      SAMOutput.writeToFile(lsc_string, resultsDir+SLASH+lsc_resultfile);
      String call = mscRenderer+" -T"+lscFileFormat+" -i"+resultsDir+SLASH+lsc_resultfile+" -o "+resultsDir+SLASH+lsc_renderfile; 
      //systemCall(call);
      renderJobs.addFirst(call);
      //r.append("<a name='"+ref_prefix+"_"+(i+1)+"'/>\n");

      //r.append("<h3>LSC "+(i+1)+"</h3>\n");
      //    r.append("<div>\n");
      //    r.append("<div style=\"float:right;clear: right;\">\n");
          
      int width = SAMOutput.getMSCWitdth(SAMOutput.getComponents(l))+10;
          
      r.append("<td width=\""+width+"\">\n");

        //r.append("</div>\n");
        //r.append("<div>\n");
        if (lscFileFormat.equals("svg"))
          r.append("<object style='width:"+width+"px' data='"+lsc_renderfile+"' type='image/svg+xml'></object><br/>\n");
        else
          r.append("<img src='"+lsc_renderfile+"' /><br/>\n");
        //r.append("</div>\n");

        //  if (isSubsumed(l, minerBranch, originalScenarios)) {
        //    r.append("<i>subsumed</i><br/>\n");
        //    //continue;
        //  }

          r.append("<b>scenario #"+originalScenarios.get(l).id+"</b><br/>\n");
          r.append("scenario: "+originalScenarios.get(l).toString().replace('>', '-')+"<br/>\n");
          
          if (mb && isSubsumed(originalScenarios.get(l), minerBranch, originalScenarios_branch))
            r.append("subsumed by branching scenario<br/>\n");
          
          r.append("support: "+l.getSupport()+"<br/>\n");
          if (mb) r.append("confidence (branch) "+minerBranch.getTree().confidence(originalScenarios.get(l), true)+"<br/>\n");
          if (ml) r.append("confidence (linear) "+minerLinear.getTree().confidence(originalScenarios.get(l), true)+"<br/>\n");
          

      //r.append("</div><br/>\n");
      r.append("</td>\n");
    }
    r.append("</tr>\n");
    r.append("</table><br/>\n");
    
    
    if (render_trees) {
        String ct_string = minerBranch.getTree().toDot_ScenarioCoverage(minerBranch.getShortenedNames());
        final String ct_dotfile = "tree_cov_"+ref_prefix+"_"+(lsc_num+1)+".dot";
        final String ct_svgfile = "tree_cov_"+ref_prefix+"_"+(lsc_num+1)+".svg";
        //final String ct_pngfile = "tree_cov_"+ref_prefix+"_"+(lsc_num+1)+".png";
        SAMOutput.writeToFile(ct_string, resultsDir+SLASH+ct_dotfile);
        
        String call = dotRenderer+" -Tsvg "+resultsDir+SLASH+ct_dotfile+" -o"+resultsDir+SLASH+ct_svgfile;
        //systemCall(call);
        renderJobs.addLast(call);
        
        //r.append("<object style='height: 300px' data='"+ct_svgfile+"' type='image/svg+xml'></object><br/>\n");
        //r.append("<a href='"+ct_svgfile+"'><img style='height: "+treeFigHeight+"px' src='"+ct_pngfile+"'/></a><br/>\n");
        r.append("<a href='"+ct_svgfile+"'>tree with scenario coverage</a><br/>\n");
      //} else {
      //  r.append("<i>coverage tree could not be rendered</i><br/>\n");
      //}
    } else {
      r.append("<i>scenario coverage tree not shown due to size</i><br/>\n");
    }

  }
  
  protected static boolean isSubsumed(SScenario s2, MineLSC minerBranch, Map<LSC, SScenario> originalScenarios) {
    boolean subsumed = false;
    for (SScenario s : originalScenarios.values()) {
      if (s2 == s) continue;
      /*
      if (s2.weakerThan(s) && !s.weakerThan(s2) && s2.support <= s.support) {
        System.out.println(s2+" is subsumed by "+s);
        subsumed = true;
        continue;
      }
      */
      if (minerBranch.implies(s, s2) && !minerBranch.implies(s2, s)) {
        System.out.println(s2+" is implied by "+s);
        subsumed = true;
        continue;
      }
    }
    return subsumed;
  }

  protected String experimentFileRoot;
  protected String outputGroupDir = "";
  protected String inputFile;
  protected int support;
  protected double confidence_branch;
  protected double confidence_linear;
  protected double density;
  protected double fraction = 1.0;
  
  protected void printHelp() {
    System.out.println("Sam/Mine version "+props.getProperty("sam.version"));
    System.out.println("usage:  sam_mine <inputfile.xes.gz> <support> <confidence>");
    System.out.println("  <inputfile>     path to log file");
    System.out.println("  <support>       minimum support threshold (integers > 0)");
    System.out.println("  <confidence>    minimum confidence for scenarios (between 0.0 and 1.0)");
  }
  
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
      confidence_linear = confidence_branch;
      density = 1.0;
      fraction = 1.0;
      
      if (support < 1) throw new NumberFormatException("support must be larger than 0");
      if (confidence_branch < 0.0 || confidence_branch > 1.0) throw new NumberFormatException("confidence must be between 0.0 and 1.0");
      if (density < 0.0 || density > 1.0) throw new NumberFormatException("density must be between 0.0 and 1.0");
      
    } catch (NumberFormatException e) {
      System.err.println(e);
      printHelp();
      return false;
    }
    
    return true;
  }
  
  public void setParameters(String experimentFileRoot, String inputFile, double fraction, int support, double confidence_branch, double confidence_linear) {
    this.experimentFileRoot = experimentFileRoot;
    this.inputFile = inputFile;
    this.fraction = fraction;
    this.support = support;
    this.confidence_branch = confidence_branch;
    this.confidence_linear = confidence_linear;
  }
  
  public String experiment() throws IOException {
    XLog xlog = readLog(experimentFileRoot+SLASH+inputFile, fraction);
    return experiment(experimentFileRoot, inputFile, xlog, support, confidence_branch, confidence_linear);
  }
  
  public synchronized static void log_new(String logfile, String line) throws IOException {
    writeFile(logfile, line, false);
  }
  
  public synchronized static void log(String logfile, String line) throws IOException {
    writeFile(logfile, line, true);
  }

  /**
   * Helper function to write a string to a file.
   * 
   * @param fileName
   * @param contents
   * @throws IOException
   */
  protected static void writeFile(String fileName, String contents, boolean append) throws IOException {
    FileWriter fstream = new FileWriter(fileName, append);
    BufferedWriter out = new BufferedWriter(fstream);
    out.write(contents);
    out.close();
  }
  
  public void experimentMulti() throws IOException {
    int steps=4;
    
    this.renderTrees = RENDER_NEVER;

    this.setResultsDirNameGroupInfix(experimentFileRoot, inputFile);
    String logfileDir = experimentFileRoot+SLASH+outputGroupDir+SLASH;
        
    File f = new File(logfileDir);
    if (!f.exists()) {
      if (!f.mkdir()) {
        System.err.println("Couldn't create results directory: "+logfileDir);
      }
    }
    String logfile = logfileDir+inputFile+"_stat.csv";
    
    log_new(logfile, getCSVheader()+"\n");
    
    for (int step_fract=steps; step_fract>0; step_fract--) {
      double trace_fract = (fraction)*((double)step_fract/steps);
      
      XLog xlog = readLog(experimentFileRoot+SLASH+inputFile, trace_fract);
      
      for (int step_support=steps; step_support>0; step_support--) {
        int support_fract = (int)(support*((double)step_support/steps));
        
        for (int step_confidence=steps; step_confidence>0; step_confidence--) {
          double confidence_fract = (double)(confidence_branch*((double)step_confidence/steps));
          
          String stat = experiment(experimentFileRoot, inputFile, xlog, support_fract, confidence_fract, confidence_linear);
          log(logfile, stat+"\n");
        }

      }

    }

  }

  public static void main(String[] args) throws IOException {
    
    RunExperimentCompare exp = new RunExperimentCompare();
    //if (!exp.readCommandLine(args)) return;
    
    //exp.experimentFileRoot = "./experiments/columba_ext/";
    //exp.inputFile = "columba_ext_resampled2_filtered.xes.gz";
    //exp.fraction = 1.0;
    //exp.confidence = 1.0;
    //exp.support = 5;
    
//    exp.experimentFileRoot = "./experiments/columba_ext/";
//    exp.inputFile = "columba_ext_resampled2_agg.xes.gz";
//    exp.fraction = 1.0;
//    exp.confidence = 1.0;
//    exp.support = 4;
    
    //exp.setParameters("./experiments/p_Afschriften/", "Afschriften_agg.xes.gz", 1.0 /*fract*/, 1 /*supp*/, 1.0 /* conf */);
    //exp.setParameters("./experiments/columba_v3/", "columba_v3_resampled_agg.xes.gz", 1.0 /*fract*/, 10 /*supp*/, 1.0 /* conf */);
    //exp.setParameters("./experiments/columba_v3/", "columba_v3_resampled_agg_filtered3c.xes.gz", 1.0 /*fract*/, 10 /*supp*/, 0.5 /* conf B */, 0.5 /* conf L */);
    //exp.setParameters("./experiments_fse2012/", "columba_filtered.xes.gz", 1.0 /*fract*/, 10 /*supp*/, 1.0 /* conf */);
    //exp.setParameters("./experiments/crossftp_succinct/", "crossftp_succinct_traceset.xes.gz", 1.0 /*fract*/, 10 /*supp*/, 1.0 /* conf */);
    //exp.setParameters("./experiments/columba_ext/", "columba_ext_resampled2_agg.xes.gz", 1.0 /*fract*/, 5 /*supp*/, 1.0 /* conf */);
    
    //exp.setParameters("./experiments/jeti_invariants/", "jeti_invariants.xes.gz", 1.0 /*fract*/, 10 /*supp*/, 1.0 /* conf */);
    
    //exp.setParameters("./experiments_fse2012/", "columba.xes.gz", 1.0 /*fract*/, 100 /*supp*/, 1.0 /* conf */);
    //exp.setParameters("./experiments_fse2012/", "columba_filtered.xes.gz", 1.0 /*fract*/, 10 /*supp*/, 0.5 /* conf B */, .5 /* conf L */);
    //exp.setParameters("./experiments_fse2012/", "crossftp_filtered.xes.gz", 1.0 /*fract*/, 10 /*supp*/, .4 /* conf B */, 1.0 /* conf L */);
    exp.setParameters("./experiments_fse2012/", "crossftp.xes.gz", 1.0 /*fract*/, 10 /*supp*/, 2.0 /* conf B */, 1.0 /* conf L */);

    //exp.setParameters("./experiments/crossftp_invariants/", "crossftp_invariants.xes.gz", 1.0 /*fract*/, 80 /*supp*/, 1.0 /* conf B */, 1.0 /* conf L */);

    exp.experiment();

  }

}
