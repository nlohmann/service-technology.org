package org.st.sam.mine;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

import lscminer.datastructure.LSC;

import org.st.sam.log.SLogTree.TreeStatistics;
import org.st.sam.mine.MineBranchingLSC;

public class RunExperimentBranching {

  private static String SLASH = System.getProperty("file.separator");

  public RunExperimentBranching() {
    this(defaultDotBinary(), defaultMscGenBinary());
  }
  
  public RunExperimentBranching(String dotBinary, String mscGenBinary) {
    mscRenderer = mscGenBinary;
    dotRenderer = dotBinary;
  }
  
  public static String defaultDotBinary() {
    return System.getProperty("sam.dot", "dot");
  }
  
  public static String defaultMscGenBinary() {
    return System.getProperty("sam.mscgen", "."+SLASH+"resources"+SLASH+"mscgen"+SLASH+"mscgen");
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

  
  public void experiment(String dir, String inputFile, int minSupportThreshold, double confidence) throws IOException {
    MineBranchingLSC miner = new MineBranchingLSC();
    miner.mineLSCs(dir+"/"+inputFile, minSupportThreshold, confidence);
    
    String SLASH = System.getProperty("file.separator");    
    String resultsDir = dir+SLASH+"results";
    
    File f = new File(resultsDir);
    
    if (!f.exists()) {
      if (!f.mkdir()) {
        System.err.println("Couldn't create results directory: "+resultsDir);
        return;
      }
    }
    
    ArrayList<LSC> lscs = miner.getLSCs();


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
    
    DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
    Date date = new Date();
    r.append("executed on: "+dateFormat.format(date)+"<br/>\n");
    r.append("input file: "+dir+"/"+inputFile+"<br/>\n");
    r.append("min. support threshold: "+minSupportThreshold+"<br/>\n");
    r.append("confidence: "+confidence+"<br/>\n");
    
    r.append("<h2>Input</h2>\n");
    
    TreeStatistics stat = miner.getTree().getStatistics();
    r.append("number of events: "+stat.alphabetSize+"<br/>\n");
    r.append("avg. out degree: "+stat.averageOutDegree+"<br/>\n");
    r.append("max. out degree: "+stat.maxOutDegree+"<br/>\n");
    r.append("depth: "+stat.depth+"<br/>\n");
    r.append("width: "+stat.width+"<br/>\n");
    
    String input_tree_dot = inputFile+".dot";
    String input_tree_svg = inputFile+".svg";
    
    miner.getTree().clearCoverageMarking();
    MineBranchingLSC.writeToFile(miner.getTree().toDot(miner.getShortenedNames()), resultsDir+SLASH+input_tree_dot);
    systemCall(dotRenderer+" -Tsvg "+resultsDir+SLASH+input_tree_dot+" -o"+resultsDir+SLASH+input_tree_svg);
    
    r.append("<object style='height: 300px' data='"+input_tree_svg+"' type='image/svg+xml'></object><br/>\n");
    r.append("<a href='"+input_tree_svg+"'>show input tree in fullsize</a><br/>\n");

    r.append("<h2>Output</h2>\n");
    
    r.append("found "+lscs.size()+" LSCs<br/>\n");
    for (int i=0; i<lscs.size(); i++) {
      r.append("<a href='#lsc_"+(i+1)+"'>["+(i+1)+"]</a> ");
      if (((i+1) % 20) == 0) r.append("<br/>\n");
    }
    r.append("<br/>\n");
    
    String output_tree_dot = inputFile+"_cov.dot";
    String output_tree_svg = inputFile+"_cov.svg";
    
    MineBranchingLSC.writeToFile(miner.getCoverageTreeGlobal(), resultsDir+SLASH+output_tree_dot);
    systemCall(dotRenderer+" -Tsvg "+resultsDir+SLASH+output_tree_dot+" -o"+resultsDir+SLASH+output_tree_svg);
    
    r.append("<object style='height: 300px' data='"+output_tree_svg+"' type='image/svg+xml'></object><br/>\n");
    r.append("<a href='"+output_tree_svg+"'>show covered tree in fullsize</a><br/>\n");
    r.append("<hr />");

    
    StringBuilder found_lscs = new StringBuilder();
    
    for (int i=0; i<lscs.size(); i++) {
      
      LSC l = lscs.get(i);
      
      found_lscs.append(l.toString());
      found_lscs.append("\n");

      String lsc_string = MineBranchingLSC.toMSCRenderer("LSC "+(i+1)+" conf="+l.getConfidence(), l);
      String lsc_resultfile = "lsc_"+(i+1)+".lsc.txt";
      String lsc_renderfile = "lsc_"+(i+1)+".lsc.svg";
      MineBranchingLSC.writeToFile(lsc_string, resultsDir+SLASH+lsc_resultfile);
      
      systemCall(mscRenderer+" -Tsvg -i"+resultsDir+SLASH+lsc_resultfile+" -o "+resultsDir+SLASH+lsc_renderfile);

      String ct_string = miner.getCoverageTreeFor(miner.getScenarios().get(i));
      String ct_dotfile = "tree_cov_"+(i+1)+".dot";
      String ct_svgfile = "tree_cov_"+(i+1)+".svg";
      MineBranchingLSC.writeToFile(ct_string, resultsDir+SLASH+ct_dotfile);
      
      systemCall(dotRenderer+" -Tsvg "+resultsDir+SLASH+ct_dotfile+" -o"+resultsDir+SLASH+ct_svgfile);
      

      r.append("<a name='lsc_"+(i+1)+"'/>\n");
      r.append("<h2>LSC "+(i+1)+"</h2>\n");
      r.append("confidence: "+l.getConfidence()+"<br/>\n");
      r.append("support: "+l.getSupport()+"<br/>\n");
      r.append("<object data='"+lsc_renderfile+"' type='image/svg+xml'></object><br/>\n");
      r.append("<object style='height: 300px' data='"+ct_svgfile+"' type='image/svg+xml'></object><br/>\n");
      r.append("<a href='"+ct_svgfile+"'>show coverage tree in fullsize</a><br/>\n");
      r.append("<a href='#general'>top</a><br/>\n");
      r.append("<hr/>\n");
    }
    r.append("</body>\n");
    MineBranchingLSC.writeToFile(r.toString(), resultsDir+"/results.html");    
    MineBranchingLSC.writeToFile(found_lscs.toString(), resultsDir+"/lscs.txt");
    System.out.println("finished.");
  }
  
  public static void main(String[] args) throws IOException {
    
    String experimentFileRoot = "./experiments";

    RunExperimentBranching e = new RunExperimentBranching();
    
    e.experiment(experimentFileRoot+"/crossftp_succinct", "crossftp_succinct_traceset.xes.gz", 10, 1.0);
    
  }

}
