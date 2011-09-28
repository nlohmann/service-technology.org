package org.st.sam.mine;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import com.google.gwt.dev.util.collect.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Properties;

import lscminer.datastructure.LSC;

import org.st.sam.log.SLogTree.TreeStatistics;
import org.st.sam.log.SLogTreeNode;
import org.st.sam.log.SScenario;
import org.st.sam.util.LSCOutput;

public class RunExperimentCompare {

  private static String SLASH = System.getProperty("file.separator");
  
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

  private static boolean lsc_equals(LSC l1, LSC l2) {
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
  
  private List<Thread> runningThreads = new LinkedList<Thread>();
  
  public void experiment(final String dir, final String inputFile, final int minSupportThreshold, final double confidence) throws IOException {
    
    final MineLSC minerBranch = new MineBranchingLSC();
    System.out.println("mining branching lscs from "+dir+"/"+inputFile);
    minerBranch.mineLSCs(dir+"/"+inputFile, minSupportThreshold, confidence);
    
    final MineLSC minerLinear = new MineLinearLSC(minerBranch.getSupportedWords()); 
    System.out.println("mining linear lscs from "+dir+"/"+inputFile);
    //LSCEvent[][] dataSet = MineLinearLSC.readInput(experimentFileRoot+"/"+inputFile);
    //if (dataSet == null)
    //  return;
    //LSCMiner minerLinear = new LSCMiner();
    //minerLinear.mineLSCs(dataSet, support, confidence, density);
    minerLinear.mineLSCs(dir+"/"+inputFile, minSupportThreshold, confidence);

    boolean render_trees = true;
    
    if (minerBranch.getTree().nodes.size() > 3000) {
      render_trees = false;
    }

    Date now = new Date();
    DateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd_HH-mm-ss");
    String resultsDir = dir+SLASH+"results_BL_"+minSupportThreshold+"_"+confidence+"_"+dateFormat.format(now);
    
    File f = new File(resultsDir);
    
    if (!f.exists()) {
      if (!f.mkdir()) {
        System.err.println("Couldn't create results directory: "+resultsDir);
        return;
      }
    }
    
    ArrayList<LSC> branching_lscs = minerBranch.getLSCs();
    ArrayList<LSC> linear_lscs = minerLinear.getLSCs();
    
    ArrayList<LSC> onlyBranching = new ArrayList<LSC>();
    ArrayList<LSC> both = new ArrayList<LSC>();
    for (LSC b : branching_lscs) {
      boolean is_linear = false;
      for (LSC l : linear_lscs) {
        if (lsc_equals(l, b)) {
          is_linear = true;
          System.out.println("comparing confidence l:"+l.getConfidence()+" b:"+b.getConfidence());
          break;
        }
      }
      if (!is_linear) onlyBranching.add(b);
      else both.add(b);
    }
    ArrayList<LSC> onlyLinear = new ArrayList<LSC>(); 
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
    
    if (render_trees) {
      String input_tree_dot = inputFile+".dot";
      String input_tree_svg = inputFile+".svg";
      String input_tree_png = inputFile+".png";
      
      minerBranch.getTree().clearCoverageMarking();
      LSCOutput.writeToFile(minerBranch.getTree().toDot(minerBranch.getShortenedNames()), resultsDir+SLASH+input_tree_dot);
      systemCall(dotRenderer+" -Tsvg "+resultsDir+SLASH+input_tree_dot+" -o"+resultsDir+SLASH+input_tree_svg);
      systemCall(dotRenderer+" -Tpng -Gsize=30 "+resultsDir+SLASH+input_tree_dot+" -o"+resultsDir+SLASH+input_tree_png);
      
      //r.append("<object style='height: 300px' data='"+input_tree_svg+"' type='image/svg+xml'></object><br/>\n");
      r.append("<a href='"+input_tree_svg+"'><img style='height: "+treeFigHeight+"px' src='"+input_tree_png+"'/></a><br/>\n");
    } else {
      r.append("<i>original tree not shown due to size</i><br/>\n");
    }
    
    Map<LSC, SScenario> originalScenarios = new HashMap<LSC, SScenario>();
    for (LSC l : minerBranch.getScenarios().keySet()) {
      originalScenarios.put(l, minerBranch.getScenarios().get(l));
    }
    for (LSC l : minerLinear.getScenarios().keySet()) {
      originalScenarios.put(l, minerLinear.getScenarios().get(l));
    }
    
    writeResultsSection(r, onlyBranching, resultsDir, treeFigHeight, minerBranch, originalScenarios, render_trees, "Strictly Branching LSCs", "lsc_branch");
    writeResultsSection(r, onlyLinear, resultsDir, treeFigHeight, minerBranch, originalScenarios, render_trees, "Strictly Linear LSCs", "lsc_linear");
    writeResultsSection(r, both, resultsDir, treeFigHeight, minerBranch, originalScenarios, false, "Linear and Branching LSCs", "lsc_both");

    r.append("</body>\n");
    LSCOutput.writeToFile(r.toString(), resultsDir+"/results.html");    
    LSCOutput.writeToFile(linear_lscs.toString(), resultsDir+"/lscs_linear.txt");
    LSCOutput.writeToFile(branching_lscs.toString(), resultsDir+"/lscs_branching.txt");
    
    System.out.println("waiting for diagram renderers to complete: "+runningThreads.size());
    for (Thread t : runningThreads) {
      try {
        t.join();
      } catch (InterruptedException e) {
        System.out.println(e);
        e.printStackTrace();
      }
      System.out.print(".");
    }
    System.out.println(".");
    
    System.out.println("finished.");
  }
  
  private void writeResultsSection(StringBuilder r, ArrayList<LSC> lscs, final String resultsDir, int treeFigHeight,
                                   MineLSC minerBranch, Map<LSC, SScenario> originalScenarios, boolean render_trees,
                                   String title, String ref_prefix) throws IOException {
    
    if (minerBranch == null) render_trees = false;
    
    for (LSC l : lscs) {
      LSCOutput.shortenLSCnames(l.getPreChart());
      LSCOutput.shortenLSCnames(l.getMainChart());
    }
    
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
    
    LSCOutput.writeToFile(minerBranch.getCoverageTreeGlobal(), resultsDir+SLASH+output_tree_dot);
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

      String lsc_string = LSCOutput.toMSCRenderer("LSC "+(i+1)+" conf="+l.getConfidence()+" supp="+l.getSupport(), l);
      final String lsc_resultfile = "lsc_"+(i+1)+"_"+ref_prefix+".lsc.txt";
      final String lsc_renderfile = "lsc_"+(i+1)+"_"+ref_prefix+".lsc.svg";
      LSCOutput.writeToFile(lsc_string, resultsDir+SLASH+lsc_resultfile);
      
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
      int total_occurrences = 0;
      for (SLogTreeNode[] o : occ) {
        // total number of occurrences = number of different occurrences * number of traces having this occurrence until the end of the word
        total_occurrences += minerBranch.getTree().nodeCount.get(o[o.length-1]);
      }
      r.append("support (re-compute) "+total_occurrences+"/"+occ.size()+"<br/>\n");
      r.append("confidence (re-compute) "+minerBranch.getTree().confidence(originalScenarios.get(l).pre, originalScenarios.get(l).main, false)+"<br/>\n");
      
      boolean subsumed = false;
      for (SScenario s : originalScenarios.values()) {
        if (originalScenarios.get(l).weakerThan(s) && !s.weakerThan(originalScenarios.get(l))) {
              r.append("<b>scenario "+originalScenarios.get(l).toString().replace('>', '-')+" is subsumed by "+s.toString().replace('>', '-')+"</b><br/>\n");
              subsumed = true;
        }
      }
      if (subsumed) continue;
      
      r.append("<object data='"+lsc_renderfile+"' type='image/svg+xml'></object><br/>\n");
      
      if (render_trees) {
        if (originalScenarios.containsKey(l)) {
          String ct_string = minerBranch.getCoverageTreeFor(originalScenarios.get(l));
          final String ct_dotfile = "tree_cov_"+ref_prefix+"_"+(i+1)+".dot";
          final String ct_svgfile = "tree_cov_"+ref_prefix+"_"+(i+1)+".svg";
          final String ct_pngfile = "tree_cov_"+ref_prefix+"_"+(i+1)+".png";
          LSCOutput.writeToFile(ct_string, resultsDir+SLASH+ct_dotfile);
          
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
  private String inputFile;
  private int support;
  private double confidence;
  private double density;
  
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
  
  public void experiment() throws IOException {
    experiment(experimentFileRoot, inputFile, support, confidence);
  }
  
  public static void main(String[] args) throws IOException {
    
    RunExperimentCompare exp = new RunExperimentCompare();
    if (!exp.readCommandLine(args)) return;
    exp.experiment();
  }

}
