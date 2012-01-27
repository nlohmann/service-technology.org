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

import lscminer.datastructure.LSC;
import lscminer.datastructure.LSCEvent;

import org.deckfour.xes.model.XLog;
import org.deckfour.xes.model.XTrace;
import org.st.sam.log.SLogTree.TreeStatistics;
import org.st.sam.log.SLogTreeNode;
import org.st.sam.log.SScenario;
import org.st.sam.log.XESImport;
import org.st.sam.util.SAMOutput;

import com.google.gwt.dev.util.collect.HashMap;
import com.google.gwt.dev.util.collect.HashSet;

public class RunExperimentCompare {

  private static String SLASH = System.getProperty("file.separator");
  private boolean allowed_to_renderTrees = true;
  
  private Properties props;

  public RunExperimentCompare() throws IOException {
    props = new Properties();
    props.load(new FileInputStream("sam_mine.properties"));

    dotRenderer = props.getProperty("sam.dot");
    mscRenderer = props.getProperty("sam.mscgen");
  }
  
  public RunExperimentCompare(String dotBinary, String mscGenBinary) throws IOException {
    mscRenderer = mscGenBinary;
    dotRenderer = dotBinary;
  }
  
  private String mscRenderer;
  private String dotRenderer;
  
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
      if (   !l1.getPreChart()[i].getCaller().equals(l2.getPreChart()[i].getCaller())
          || !l1.getPreChart()[i].getCallee().equals(l2.getPreChart()[i].getCallee())
          || !l1.getPreChart()[i].getMethod().equals(l2.getPreChart()[i].getMethod())) {
        return false;
      }
    }
    
    if (l1.getMainChart().length != l2.getMainChart().length) return false;
    
    for (int i=0; i<l1.getMainChart().length; i++) {
      if (   !l1.getMainChart()[i].getCaller().equals(l2.getMainChart()[i].getCaller())
          || !l1.getMainChart()[i].getCallee().equals(l2.getMainChart()[i].getCallee())
          || !l1.getMainChart()[i].getMethod().equals(l2.getMainChart()[i].getMethod())) {
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
  protected Map<LSC, SScenario> originalScenarios;
  
  private long runTime_minerBranch;
  
  public void runMiners(final String logFile, final XLog xlog, final int minSupportThreshold, final double confidence) throws IOException {

    runTime_minerBranch = System.currentTimeMillis();
    minerBranch = new MineBranchingLSC();
    minerBranch.OPTIONS_WEIGHTED_OCCURRENCE = true;
    System.out.println("mining branching lscs from "+logFile);
    minerBranch.mineLSCs(xlog, minSupportThreshold, confidence, logFile);
    runTime_minerBranch = System.currentTimeMillis()-runTime_minerBranch;
    
    minerLinear = new MineLinearLSC(minerBranch.getSupportedWords());
    minerLinear.OPTIONS_WEIGHTED_OCCURRENCE = true;
    System.out.println("mining linear lscs from "+logFile);
    //LSCEvent[][] dataSet = MineLinearLSC.readInput(experimentFileRoot+"/"+inputFile);
    //if (dataSet == null)
    //  return;
    //LSCMiner minerLinear = new LSCMiner();
    //minerLinear.mineLSCs(dataSet, support, confidence, density);
    minerLinear.mineLSCs(xlog, minSupportThreshold, confidence, logFile);
    
    originalScenarios = new HashMap<LSC, SScenario>();
    for (LSC l : minerBranch.getScenarios().keySet()) {
      originalScenarios.put(l, minerBranch.getScenarios().get(l));
    }
    for (LSC l : minerLinear.getScenarios().keySet()) {
      originalScenarios.put(l, minerLinear.getScenarios().get(l));
    }
  }

  protected ArrayList<LSC> both;
  protected ArrayList<LSC> onlyBranching;
  protected ArrayList<LSC> onlyLinear; 
  
  public void separateResults() {
    ArrayList<LSC> branching_lscs = minerBranch.getLSCs();
    ArrayList<LSC> linear_lscs = minerLinear.getLSCs();
    
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
      else both.add(b);
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
  }
  
  protected Date now;
  private String resultsDir;
  
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
  
  public String experiment(final String dir, final String inputFile, final XLog xlog, final int minSupportThreshold, final double confidence) throws IOException {
    
    System.out.println("---[ LSC Miner ] ------------------------------------------------------------");
    System.out.println("   "+inputFile);
    System.out.println("   size: "+xlog.size());
    System.out.println("   support: "+minSupportThreshold);
    System.out.println("   confidence: "+confidence);
    System.out.println("-----------------------------------------------------------------------------");
    
    String logFile = dir+"/"+inputFile;
    runMiners(logFile, xlog, minSupportThreshold, confidence);
    
    boolean render_trees = allowed_to_renderTrees;
    if (minerBranch.getTree().nodes.size() > 3000) {
      render_trees = false;
    }
    
    separateResults();
    
    for (LSC l : both) {
      SAMOutput.shortenLSCnames(l.getPreChart());
      SAMOutput.shortenLSCnames(l.getMainChart());
    }
    for (LSC l : onlyBranching) {
      SAMOutput.shortenLSCnames(l.getPreChart());
      SAMOutput.shortenLSCnames(l.getMainChart());
    }
    for (LSC l : onlyLinear) {
      SAMOutput.shortenLSCnames(l.getPreChart());
      SAMOutput.shortenLSCnames(l.getMainChart());
    }
    
    MineLSC.sortLSCs(both);
    MineLSC.sortLSCs(onlyBranching);
    MineLSC.sortLSCs(onlyLinear);

    if (!createOutputDirectory(dir, inputFile, xlog.size(), minSupportThreshold, confidence)) return "could not write results";

    // resulting html
    StringBuilder r = new StringBuilder();
    r.append("<html>\n");
    r.append("<head>\n");
    r.append("  <title>Results for "+dir+"/"+inputFile+"</title>\n");
    r.append("</head>\n");
    r.append("<body>\n");
    r.append("<a name='general' />\n");
    r.append("<h1>Results for "+dir+"/"+inputFile+"</h1>\n");
    
    
    r.append("<h2>General</h2>\n");
    
    DateFormat dateFormat2 = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
    r.append("executed on: "+dateFormat2.format(now)+"<br/>\n");
    r.append("input file: "+dir+"/"+inputFile+"<br/>\n");
    r.append("min. support threshold: "+minSupportThreshold+"<br/>\n");
    r.append("confidence: "+confidence+"<br/>\n");


    r.append("<h2>Input</h2>\n");
    
    TreeStatistics stat = minerBranch.getTree().getStatistics();
    r.append("number of events: "+stat.alphabetSize+"<br/>\n");
    r.append("avg. out degree: "+stat.averageOutDegree+"<br/>\n");
    r.append("max. out degree: "+stat.maxOutDegree+"<br/>\n");
    r.append("depth: "+stat.depth+"<br/>\n");
    r.append("width: "+stat.width+"<br/>\n");
    
    int treeFigHeight = stat.depth*5;
    if (treeFigHeight > 600) treeFigHeight = 600;
    
    r.append("<h2>Discovered Scenarios (general statistics)</h2>\n");
    
    minerBranch.getCoverageTreeGlobal();
    minerLinear.getCoverageTreeGlobal();
    
    r.append("tree coverage (branch): "+minerBranch.getTree().getCoverage()+"<br/>\n");
    r.append("tree coverage (linear): "+minerLinear.getTree().getCoverage()+"<br/>\n");
    
    Set<Short> coveredEvents_branch = new HashSet<Short>();
    for (SScenario s : minerBranch.getScenarios().values()) {
      for (short e : s.pre) coveredEvents_branch.add(e);
      for (short e : s.main) coveredEvents_branch.add(e);
    }
    double coveredAlphabet_branch = (double)coveredEvents_branch.size()/minerBranch.getSLog().originalNames.length;
    
    Set<Short> coveredEvents_linear = new HashSet<Short>();
    for (SScenario s : minerLinear.getScenarios().values()) {
      for (short e : s.pre) coveredEvents_linear.add(e);
      for (short e : s.main) coveredEvents_linear.add(e);
    }
    double coveredAlphabet_linear = (double)coveredEvents_linear.size()/minerLinear.getSLog().originalNames.length;
    r.append("alphabet coverage (branch): "+coveredAlphabet_branch+"<br/>\n");
    r.append("alphabet coverage (linear): "+coveredAlphabet_linear+"<br/>\n");
    
    ScenarioStatistics[] stat_branch_only = getStatistics(onlyBranching);
    ScenarioStatistics[] stat_both = getStatistics(minerLinear.getLSCs());
    
    r.append("<br/>\n");
    r.append("<b>strictly branching scenarios</b><br/>\n");
    r.append("found "+onlyBranching.size()+" strictly branching scenarios<br/>\n");
    r.append("length pre-chart (min/avg/max): "+stat_branch_only[0].preChartLength+"/"+stat_branch_only[1].preChartLength+"/"+stat_branch_only[2].preChartLength+"<br/>\n");
    r.append("length main-chart (min/avg/max): "+stat_branch_only[0].mainChartLength+"/"+stat_branch_only[1].mainChartLength+"/"+stat_branch_only[2].mainChartLength+"<br/>\n");
    r.append("#components (min/avg/max): "+stat_branch_only[0].components+"/"+stat_branch_only[1].components+"/"+stat_branch_only[2].components+"<br/>\n");

    r.append("<br/>\n");
    r.append("<b>linear and branching scenarios</b><br/>\n");
    r.append("found "+both.size()+" linear and branching scenarios<br/>\n");
    r.append("length pre-chart (min/avg/max): "+stat_both[0].preChartLength+"/"+stat_both[1].preChartLength+"/"+stat_both[2].preChartLength+"<br/>\n");
    r.append("length main-chart (min/avg/max): "+stat_both[0].mainChartLength+"/"+stat_both[1].mainChartLength+"/"+stat_both[2].mainChartLength+"<br/>\n");
    r.append("#components (min/avg/max): "+stat_both[0].components+"/"+stat_both[1].components+"/"+stat_both[2].components+"<br/>\n");
    r.append("<br/>\n");
    

    if (/*render_trees*/true) {
      String input_tree_dot = inputFile+".dot";
      String input_tree_svg = inputFile+".svg";
      String input_tree_png = inputFile+".png";
      
      minerBranch.getTree().clearCoverageMarking();
      minerBranch.getCoverageTreeGlobal();
      //SAMOutput.writeToFile(minerBranch.getTree().toDot_ScenarioCoverage(minerBranch.getShortenedNames()), resultsDir+SLASH+input_tree_dot);
      SAMOutput.writeToFile(minerBranch.getTree().toDot(minerBranch.getShortenedNames()), resultsDir+SLASH+input_tree_dot);
      systemCall(dotRenderer+" -Tsvg "+resultsDir+SLASH+input_tree_dot+" -o"+resultsDir+SLASH+input_tree_svg);
      //systemCall(dotRenderer+" -Tpng -Gsize=30 "+resultsDir+SLASH+input_tree_dot+" -o"+resultsDir+SLASH+input_tree_png);
      
      //r.append("<object style='height: 300px' data='"+input_tree_svg+"' type='image/svg+xml'></object><br/>\n");
      //r.append("<a href='"+input_tree_svg+"'><img style='height: "+treeFigHeight+"px' src='"+input_tree_png+"'/></a><br/>\n");
      r.append("<a href='"+input_tree_svg+"'><b>tree with complete scenario coverage</b></a><br/>\n");
    } else {
      r.append("<i>original tree not shown due to size</i><br/>\n");
    }
    
    writeResults_allCharts(r, resultsDir, treeFigHeight, render_trees);

    r.append("</body>\n");
    
    SAMOutput.writeToFile(r.toString(), resultsDir+"/results.html");    
    SAMOutput.writeToFile(minerLinear.getLSCs().toString(), resultsDir+"/lscs_linear.txt");
    SAMOutput.writeToFile(minerBranch.getLSCs().toString(), resultsDir+"/lscs_branching.txt");
    
    System.out.println("finished.");
    
    minerBranch.getTree().clearCoverageMarking();
    minerBranch.getCoverageTreeGlobal();
    minerLinear.getTree().clearCoverageMarking();
    minerLinear.getCoverageTreeGlobal();
    
    String stat_summary = inputFile+";"+xlog.size()+";"+minSupportThreshold+";"+confidence+";"
        +stat.alphabetSize+";"+stat.averageOutDegree+";"+stat.maxOutDegree+";"+stat.depth+";"+stat.width+";"
        +minerBranch.getTree().getCoverage()+";"+minerLinear.getTree().getCoverage()+";"+coveredAlphabet_branch+";"+coveredAlphabet_linear+";"
        +onlyBranching.size()+";"+both.size()+";"
        +stat_branch_only[0].preChartLength+";"+stat_branch_only[1].preChartLength+";"+stat_branch_only[2].preChartLength+";"
        +stat_branch_only[0].mainChartLength+";"+stat_branch_only[1].mainChartLength+";"+stat_branch_only[2].mainChartLength+";"
        +stat_branch_only[0].components+";"+stat_branch_only[1].components+";"+stat_branch_only[2].components+";"
        +stat_both[0].preChartLength+";"+stat_both[1].preChartLength+";"+stat_both[2].preChartLength+";"
        +stat_both[0].mainChartLength+";"+stat_both[1].mainChartLength+";"+stat_both[2].mainChartLength+";"
        +stat_both[0].components+";"+stat_both[1].components+";"+stat_both[2].components+";"
        +runTime_minerBranch;

    return stat_summary;
  }
  
  private String getCSVheader() {
    String stat_header = "inputFile;traceFraction;support;confidence;"
        +"alphabetSize;avg degree;max degree;depth;width;"
        +"coverage (B);coverage (L);alphabet coverage(B);alphabet coverage(L);"
        +"#scen (B);#scen (B+L);"
        +"pre (min)(B);pre (avg)(B);pre (max)(B);"
        +"main (min)(B);main (avg)(B);main (max)(B);"
        +"#comp (min)(B);#comp (avg)(B);#comp (max)(B);"
        +"pre (min)(L);pre (avg)(L);pre (max)(L);"
        +"main (min)(L);main (avg)(L);main (max)(L);"
        +"#comp (min)(L);#comp (avg)(L);#comp (max)(L);"
        +"time";
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
    for (LSCEvent e : preChart) title += " "+e.getMethod();
    
    ArrayList<LSC> both_match = new ArrayList<LSC>();
    ArrayList<LSC> branching_match = new ArrayList<LSC>();
    ArrayList<LSC> linear_match = new ArrayList<LSC>();
    
    for (LSC l : both) {
      if (pre_chart_comp.compare(l.getPreChart(), preChart) == 0
          && !isSubsumed(l, minerBranch, originalScenarios))
      {
        both_match.add(l);
        System.out.println(originalScenarios.get(l));
      }
    }
    for (LSC l : onlyLinear) {
      if (pre_chart_comp.compare(l.getPreChart(), preChart) == 0
          && !isSubsumed(l, minerBranch, originalScenarios))
      {
        //linear_match.add(l);
        // strictly linear scenarios (not discovered by branching miner)
        // are subsumed by branching scenarios by construction, i.e. they
        // are also discovered by the branching miner: add to both_match
        both_match.add(l);
      }
    }
    for (LSC l : onlyBranching) {
      if (pre_chart_comp.compare(l.getPreChart(), preChart) == 0
          && !isSubsumed(l, minerBranch, originalScenarios))
      {
        branching_match.add(l);
      }
    }
    
    if (branching_match.size() == 0 && both_match.size() == 0) return;
    
    r.append("<h2>"+title+"</h2>\n");
    if (branching_match.size() > 0) {
      r.append("<b>"+branching_match.size()+" strictly branching LSC</b></br>");
      appendResults(r, branching_match, resultsDir, treeFigHeight, render_trees,
          "strictly branching LSC",
          "lsc_branch");
    }
    if (both_match.size() > 0) {
      r.append("<b>"+both_match.size()+" linear and branching LSC</b></br>");
      appendResults(r, both_match, resultsDir, treeFigHeight, render_trees,
          "linear and branching LSC",
          "lsc_both");
    }
    
    r.append("<a href='#general'>top</a><br/>\n");
    r.append("<hr/>\n");
  }
  
  private int lsc_num=0; 
  
  protected void appendResults(
      StringBuilder r,
      ArrayList<LSC> lscs,
      final String resultsDir,
      int treeFigHeight,
       boolean render_trees,
       String type,
       String ref_prefix) throws IOException
  {
    StringBuilder found_lscs = new StringBuilder();
    
    int totalWidth = 0;
    
    minerBranch.getTree().clearCoverageMarking();
    minerLinear.getTree().clearCoverageMarking();
    
    ArrayList<LSC> nonSubsumed = new ArrayList<LSC>();
    for (LSC l : lscs) {
      if (!isSubsumed(l, minerBranch, originalScenarios)) {
        nonSubsumed.add(l);
      }
    }
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

      String lsc_string = SAMOutput.toMSCRenderer("LSC "+(lsc_num+1)+" conf="+l.getConfidence()+" supp="+l.getSupport(), l);
      final String lsc_resultfile = "lsc_"+(lsc_num+1)+"_"+ref_prefix+".lsc.txt";
      final String lsc_renderfile = "lsc_"+(lsc_num+1)+"_"+ref_prefix+".lsc.svg";
      SAMOutput.writeToFile(lsc_string, resultsDir+SLASH+lsc_resultfile);
      
      //Thread t = new Thread() {
      //  public void run() {
          systemCall(mscRenderer+" -Tsvg -i"+resultsDir+SLASH+lsc_resultfile+" -o "+resultsDir+SLASH+lsc_renderfile);
      //  };
      //};
      //t.start();
      //runningThreads.add(t);

      //r.append("<a name='"+ref_prefix+"_"+(i+1)+"'/>\n");

      //r.append("<h3>LSC "+(i+1)+"</h3>\n");
      //    r.append("<div>\n");
      //    r.append("<div style=\"float:right;clear: right;\">\n");
          
      int width = SAMOutput.getMSCWitdth(SAMOutput.getComponents(l))+10;
          
      r.append("<td width=\""+width+"\">\n");

        //r.append("</div>\n");
        //r.append("<div>\n");
          r.append("<object style='width:"+width+"px' data='"+lsc_renderfile+"' type='image/svg+xml'></object><br/>\n");
        //r.append("</div>\n");

        //  if (isSubsumed(l, minerBranch, originalScenarios)) {
        //    r.append("<i>subsumed</i><br/>\n");
        //    //continue;
        //  }

          r.append("<b>scenario #"+originalScenarios.get(l).id+"</b><br/>\n");
          r.append("confidence: "+l.getConfidence()+"<br/>\n");
          r.append("support: "+l.getSupport()+"<br/>\n");
          r.append("scenario: "+originalScenarios.get(l).toString().replace('>', '-')+"<br/>\n");
          
          List<SLogTreeNode[]> occ = minerBranch.getTree().countOccurrences(originalScenarios.get(l).getWord(), null, null);
          int total_occurrences = minerBranch.getTotalOccurrences(occ);
          //r.append("support (re-compute) "+total_occurrences+"/"+occ.size()+"<br/>\n");
          r.append("confidence (branch) "+minerBranch.getTree().confidence(originalScenarios.get(l), true)+"<br/>\n");
          r.append("confidence (linear) "+minerLinear.getTree().confidence(originalScenarios.get(l), true)+"<br/>\n");
          

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
        
        //Thread t2 = new Thread() {
        //  public void run() {
            systemCall(dotRenderer+" -Tsvg "+resultsDir+SLASH+ct_dotfile+" -o"+resultsDir+SLASH+ct_svgfile);
            //systemCall(dotRenderer+" -Tpng -Gsize=30 "+resultsDir+SLASH+ct_dotfile+" -o"+resultsDir+SLASH+ct_pngfile);
        //  };
        //};
        //t2.start();
        //runningThreads.add(t2);
        
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
  
  private boolean isSubsumed(LSC l, MineLSC minerBranch, Map<LSC, SScenario> originalScenarios) {
    boolean subsumed = false;
    for (SScenario s : originalScenarios.values()) {
      if (originalScenarios.get(l) == s) continue;
      if (originalScenarios.get(l).weakerThan(s) && !s.weakerThan(originalScenarios.get(l))) {
        //r.append("<b>scenario "+originalScenarios.get(l).toString().replace('>', '-')+" is subsumed by "+s.toString().replace('>', '-')+"</b><br/>\n");
        subsumed = true;
        continue;
      }
      if (minerBranch.implies(s, originalScenarios.get(l)) && !minerBranch.implies(originalScenarios.get(l), s)) {
        //r.append("<b>scenario "+originalScenarios.get(l).toString().replace('>', '-')+" is implied by "+s.toString().replace('>', '-')+"</b><br/>\n");
        subsumed = true;
        continue;
      }
    }
    return subsumed;
  }

  private void writeResultsSection(StringBuilder r, ArrayList<LSC> lscs, final String resultsDir, int treeFigHeight,
                                   MineLSC minerBranch, Map<LSC, SScenario> originalScenarios, boolean render_trees,
                                   String title, String ref_prefix) throws IOException {
    
    if (minerBranch == null) render_trees = false;
    
    r.append("<h2>"+title+"</h2>\n");
    
    r.append("found "+lscs.size()+" LSCs<br/>\n");
    for (int i=0; i<lscs.size(); i++) {
      r.append("<a href='#"+ref_prefix+"_"+(i+1)+"'>["+(i+1)+"]</a> ");
      if (((i+1) % 20) == 0) r.append("<br/>\n");
    }
    r.append("<br/>\n");
    
    String output_tree_dot = inputFile+"_cov.dot";
    String output_tree_svg = inputFile+"_cov.svg";
    String output_tree_png = inputFile+"_cov.png";
    
    SAMOutput.writeToFile(minerBranch.getCoverageTreeGlobal(), resultsDir+SLASH+output_tree_dot);
    systemCall(dotRenderer+" -Tsvg "+resultsDir+SLASH+output_tree_dot+" -o"+resultsDir+SLASH+output_tree_svg);
    systemCall(dotRenderer+" -Tpng -Gsize=30 "+resultsDir+SLASH+output_tree_dot+" -o"+resultsDir+SLASH+output_tree_png);
    
    //r.append("<object style='height: 300px' data='"+output_tree_svg+"' type='image/svg+xml'></object><br/>\n");
    r.append("<a href='"+output_tree_svg+"'><img style='height: "+treeFigHeight+"px' src='"+output_tree_png+"'/></a><br/>\n");
    r.append("<hr />");

    
    StringBuilder found_lscs = new StringBuilder();
    
    for (int i=0; i<lscs.size(); i++) {
      
      LSC l = lscs.get(i);
      
      found_lscs.append(l.toString());
      found_lscs.append("\n");

      String lsc_string = SAMOutput.toMSCRenderer("LSC "+(i+1)+" conf="+l.getConfidence()+" supp="+l.getSupport(), l);
      final String lsc_resultfile = "lsc_"+(i+1)+"_"+ref_prefix+".lsc.txt";
      final String lsc_renderfile = "lsc_"+(i+1)+"_"+ref_prefix+".lsc.svg";
      SAMOutput.writeToFile(lsc_string, resultsDir+SLASH+lsc_resultfile);
      
      //Thread t = new Thread() {
      //  public void run() {
          systemCall(mscRenderer+" -Tsvg -i"+resultsDir+SLASH+lsc_resultfile+" -o "+resultsDir+SLASH+lsc_renderfile);
      //  };
      //};
      //t.start();
      //runningThreads.add(t);

      r.append("<a name='"+ref_prefix+"_"+(i+1)+"'/>\n");
      r.append("<h2>LSC "+(i+1)+"</h2>\n");
      r.append("confidence: "+l.getConfidence()+"<br/>\n");
      r.append("support: "+l.getSupport()+"<br/>\n");
      r.append("scenario: "+originalScenarios.get(l).toString().replace('>', '-')+"<br/>\n");
      
      List<SLogTreeNode[]> occ = minerBranch.getTree().countOccurrences(originalScenarios.get(l).getWord(), null, null);
      int total_occurrences = minerBranch.getTotalOccurrences(occ);
      r.append("support (re-compute) "+total_occurrences+"/"+occ.size()+"<br/>\n");
      r.append("confidence (re-compute) "+minerBranch.getTree().confidence(originalScenarios.get(l), false)+"<br/>\n");
      
      if (isSubsumed(l, minerBranch, originalScenarios)) {
        r.append("<i>subsumed</i><br/>\n");
        continue;
      }
      
      r.append("<object data='"+lsc_renderfile+"' type='image/svg+xml'></object><br/>\n");
      
      if (render_trees) {
        if (originalScenarios.containsKey(l)) {
          String ct_string = minerBranch.getCoverageTreeFor(originalScenarios.get(l));
          final String ct_dotfile = "tree_cov_"+ref_prefix+"_"+(i+1)+".dot";
          final String ct_svgfile = "tree_cov_"+ref_prefix+"_"+(i+1)+".svg";
          final String ct_pngfile = "tree_cov_"+ref_prefix+"_"+(i+1)+".png";
          SAMOutput.writeToFile(ct_string, resultsDir+SLASH+ct_dotfile);
          
          //Thread t2 = new Thread() {
          //  public void run() {
              systemCall(dotRenderer+" -Tsvg "+resultsDir+SLASH+ct_dotfile+" -o"+resultsDir+SLASH+ct_svgfile);
              systemCall(dotRenderer+" -Tpng -Gsize=30 "+resultsDir+SLASH+ct_dotfile+" -o"+resultsDir+SLASH+ct_pngfile);
          //  };
          //};
          //t2.start();
          //runningThreads.add(t2);
          
          //r.append("<object style='height: 300px' data='"+ct_svgfile+"' type='image/svg+xml'></object><br/>\n");
          r.append("<a href='"+ct_svgfile+"'><img style='height: "+treeFigHeight+"px' src='"+ct_pngfile+"'/></a><br/>\n");
        } else {
          r.append("<i>coverage tree could not be rendered</i><br/>\n");
        }
      } else {
        r.append("<i>scenario coverage tree not shown due to size</i><br/>\n");
      }
      
      r.append("<a href='#general'>top</a><br/>\n");
      r.append("<hr/>\n");
    }    
  }

  private String experimentFileRoot;
  private String outputGroupDir = "";
  private String inputFile;
  private int support;
  private double confidence;
  private double density;
  private double fraction = 1.0;
  
  private void printHelp() {
    System.out.println("Sam/Mine version "+props.getProperty("sam.version"));
    System.out.println("usage:  sam_mine <inputfile.xes.gz> <support> <confidence>");
    System.out.println("  <inputfile>     path to log file");
    System.out.println("  <support>       minimum support threshold (integers > 0)");
    System.out.println("  <confidence>    minimum confidence (between 0.0 and 1.0)");
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
      confidence = Double.parseDouble(args[2]);
      density = 1.0;
      
      if (support < 1) throw new NumberFormatException("support must be larger than 0");
      if (confidence < 0.0 || confidence > 1.0) throw new NumberFormatException("confidence must be between 0.0 and 1.0");
      if (density < 0.0 || density > 1.0) throw new NumberFormatException("density must be between 0.0 and 1.0");
      
    } catch (NumberFormatException e) {
      System.err.println(e);
      printHelp();
      return false;
    }
    
    return true;
  }
  
  public void setParameters(String experimentFileRoot, String inputFile, double fraction, int support, double confidence) {
    this.experimentFileRoot = experimentFileRoot;
    this.inputFile = inputFile;
    this.fraction = fraction;
    this.support = support;
    this.confidence = confidence;
  }
  
  public void experiment() throws IOException {
    XLog xlog = readLog(experimentFileRoot+SLASH+inputFile, fraction);
    experiment(experimentFileRoot, inputFile, xlog, support, confidence);
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
    
    this.allowed_to_renderTrees = false;

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
          double confidence_fract = (double)(confidence*((double)step_confidence/steps));
          
          String stat = experiment(experimentFileRoot, inputFile, xlog, support_fract, confidence_fract);
          log(logfile, stat+"\n");
        }

      }

    }

  }

  public static void main(String[] args) throws IOException {
    
    RunExperimentCompare exp = new RunExperimentCompare();
    // if (!exp.readCommandLine(args)) return;
    
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
    exp.setParameters("./experiments/columba_ext/", "columba_ext_resampled2_agg.xes.gz", 1.0 /*fract*/, 10 /*supp*/, 1.0 /* conf */);

    exp.experiment();

  }

}
