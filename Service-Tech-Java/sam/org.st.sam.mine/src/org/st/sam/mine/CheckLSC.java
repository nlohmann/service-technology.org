package org.st.sam.mine;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.LinkedList;
import java.util.List;
import java.util.Properties;

import org.deckfour.xes.model.XLog;
import org.st.sam.log.SLog;
import org.st.sam.log.SLogTreeNode;
import org.st.sam.log.SScenario;
import org.st.sam.log.XESImport;
import org.st.sam.mine.collect.SimpleArrayList;
import org.st.sam.util.SAMOutput;

public class CheckLSC {
  
  private static String SLASH = System.getProperty("file.separator");
  
  private Properties props;
  
  private String mscRenderer;
  private String dotRenderer;
  
  private int mode = MineLSC.Configuration.MODE_BRANCHING;


  public CheckLSC() throws IOException {
    this(MineLSC.Configuration.MODE_BRANCHING);
  }
  
  public CheckLSC(int mode) throws FileNotFoundException, IOException {
    this.mode = mode;
    
    props = new Properties();
    props.load(new FileInputStream("sam_mine.properties"));

    dotRenderer = props.getProperty("sam.dot");
    mscRenderer = props.getProperty("sam.mscgen");
  }
  
  public void checkLSCs(String logFile, String scenario) throws IOException {
    loadXLog(logFile);
    XLog xlog = getXLog();
    checkLSC(xlog, logFile, scenario);
  }
  
  public void checkLSCs_writeResults(String logFile, String scenario) throws IOException {
    
    checkLSCs(logFile, scenario);
    
    String targetFilePrefix = logFile;
    
    /*
    LSCOutput.writeToFile(getTree().toDot(getShortenedNames()), targetFilePrefix+".dot");
    LSCOutput.writeToFile(getCoverageTreeGlobal(), targetFilePrefix+"_cov.dot");
    for (int lscNum=0; lscNum<getLSCs().size(); lscNum++) {
      LSC l = getLSCs().get(lscNum);
      LSCOutput.writeToFile(getCoverageTreeFor(originalScenarios.get(l)), targetFilePrefix+"_cov_"+(lscNum+1)+".dot");
    }
    
    StringBuilder found_lscs = new StringBuilder();
    for (int i=0; i<lscs.size(); i++) {
      
      LSC l = lscs.get(i);
      found_lscs.append(l.toString());
      found_lscs.append("\n");
    }
    LSCOutput.writeToFile(found_lscs.toString(), targetFilePrefix+"_all_lscs.txt");
    */
    
    
    final String ct_dotfile = "tree_cov_"+scenario+".dot";
    final String ct_svgfile = "tree_cov_"+scenario+".svg";
    final String ct_pngfile = "tree_cov_"+scenario+".png";
    SAMOutput.writeToFile(tree.toDot(), targetFilePrefix+"."+ct_dotfile);
    
    //Thread t2 = new Thread() {
    //  public void run() {
    SAMOutput.systemCall(dotRenderer+" -Tsvg "+targetFilePrefix+"."+ct_dotfile+" -o"+targetFilePrefix+"."+ct_svgfile);
  }
  
  
  private SScenario getScenario(String scenario) {
    
    String[] charts = scenario.split("---");
    String[] preEvents = charts[0].split(",");
    String[] mainEvents = charts[1].split(",");
    
    short[] pre = new short[preEvents.length];
    for (int e=0; e<pre.length; e++) {
      pre[e] = Short.parseShort(preEvents[e]);
    }
    short[] main = new short[mainEvents.length];
    for (int e=0; e<main.length; e++) {
      main[e] = Short.parseShort(mainEvents[e]);
    }
    
    return new SScenario(pre, main);
  }
  
  public void checkLSC(XLog xlog, String targetFilePrefix, String scenario) throws IOException {
    System.out.println("log contains "+xlog.size()+" traces");
    setSLog(new SLog(xlog));
    
    boolean mergeTraces = (mode == MineLSC.Configuration.MODE_BRANCHING) ? true : false;
    
    tree = new MineBranchingTree(getSLog(), mergeTraces);
    SScenario s = getScenario(scenario);
    
    System.out.println("checking "+s);
    
    short[] word = new short[s.pre.length+s.main.length];
    for (int e=0; e<s.pre.length; e++) {
      word[e] = s.pre[e];
    }
    for (int e=0; e<s.main.length; e++) {
      word[e+s.pre.length] = s.main[e];
    }
    
    SimpleArrayList<SLogTreeNode[]> occ = tree.countOccurrences(word, null, null);
    double conf = tree.confidence(s, true);
    int total_occurrences = getTotalOccurrences(occ);

    
    System.out.println("occurrences: "+total_occurrences);
    System.out.println("confidence: "+conf);

  }
  
  public int getTotalOccurrences(SimpleArrayList<SLogTreeNode[]> occ) {
    int total_occurrences = 0;
    for (SLogTreeNode[] o : occ) {
      // total number of occurrences = number of different occurrences * number of traces having this occurrence until the end of the word
      total_occurrences += tree.nodeCount.get(o[o.length-1]);
    }
    return total_occurrences;
  }
  
  private MineBranchingTree tree;

  private XLog xlog;
  private SLog slog;
  
  private void setXLog(XLog xlog) {
    this.xlog = xlog;
  }
  
  private XLog getXLog() {
    return xlog;
  }
  
  private XLog loadXLog(String logFile) throws IOException {
    XESImport read = new XESImport();
    setXLog(read.readLog(logFile));
    return getXLog();
  }
  
  private void setSLog(SLog slog) {
    this.slog = slog;
  }
  
  public SLog getSLog() {
    return slog;
  }
  
  public static void main(String args[]) throws Exception {
    if (args.length != 2) {
      System.out.println("error, wrong number of arguments");
      System.out.println("usage: java "+MineLSC.class.getCanonicalName()+" <logfile.xes> <chart>");
      System.out.println("    <chart>  1,2,3---4,5,6,7");
      return;
    }
    
    String srcFile = args[0];
    String scenario = args[1];
    CheckLSC check = new CheckLSC();
    check.checkLSCs_writeResults(srcFile, scenario);
  }
  
}
