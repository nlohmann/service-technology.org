package org.st.sam.mine;


import java.awt.PageAttributes.OriginType;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import lscminer.datastructure.LSC;
import lscminer.datastructure.LSCEvent;

import org.deckfour.xes.model.XLog;
import org.st.sam.log.SLog;
import org.st.sam.log.SLogTree;
import org.st.sam.log.SLogTreeNode;
import org.st.sam.log.SScenario;
import org.st.sam.log.XESImport;

public class MineBranchingLSC {
  
  private ArrayList<LSC> lscs;
  private ArrayList<SScenario> scenarios;
  
  private MineBranchingTree tree;
  private String globalCoverageTree;
  
  public MineBranchingLSC() {
  }
  
  public void mineLSCs(String logfile) throws IOException {
    int minSupportThreshold = 10;
    float confidence = 1.0f;
    
    mineLSCs(logfile, minSupportThreshold, confidence);
  }
  
  public void mineLSCs(String logFile, int minSupportThreshold, double confidence) throws IOException {
    loadXLog(logFile);
    XLog xlog = getXLog();
    mineLSCs(xlog, minSupportThreshold, confidence, logFile);
  }
  
  public void mineLSCs_writeResults(String logFile) throws IOException {
    int minSupportThreshold = 10;
    float confidence = 1.0f;
    
    mineLSCs_writeResults(logFile, minSupportThreshold, confidence);
  }
  
  public void mineLSCs_writeResults(String logFile, int minSupportThreshold, double confidence) throws IOException {
    
    mineLSCs(logFile, minSupportThreshold, confidence);
    
    String targetFilePrefix = logFile;
    
    writeToFile(getTree().toDot(getShortenedNames()), targetFilePrefix+".dot");
    writeToFile(getCoverageTreeGlobal(), targetFilePrefix+"_cov.dot");
    for (int scenarioNum=0; scenarioNum<getScenarios().size(); scenarioNum++) {
      SScenario s =getScenarios().get(scenarioNum);
      writeToFile(getCoverageTreeFor(s), targetFilePrefix+"_cov_"+(scenarioNum+1)+".dot");
    }
    
    StringBuilder found_lscs = new StringBuilder();
    for (int i=0; i<lscs.size(); i++) {
      
      LSC l = lscs.get(i);
      found_lscs.append(l.toString());
      found_lscs.append("\n");
    }
    writeToFile(found_lscs.toString(), targetFilePrefix+"_all_lscs.txt");
    
  }
  
  public void mineLSCs(XLog xlog, int minSupportThreshold, double confidence, String targetFilePrefix) throws IOException {
    System.out.println("log contains "+xlog.size()+" traces");
    setSLog(new SLog(xlog));
    
    tree = new MineBranchingTree(getSLog());
    
    String stat = tree.getStatistics().toString();
    System.out.println("tree statistics: "+stat);
    
    Set<short[]> supportedWords = mineSupportedWords(tree, minSupportThreshold);
    System.out.println("found "+supportedWords.size()+" supported words");
    /*
    for (short[] w : supportedWords) {
      for (short e : w) {
        System.out.print(e+" ");
      }
      System.out.println();
    }
    */

    int total = 0;
    Map<Integer, LinkedList<short[]>> supportedWordsClasses = new HashMap<Integer, LinkedList<short[]>>();
    int largestWordSize = 0;
    for (short[] w : supportedWords) {
      if (!supportedWordsClasses.containsKey(w.length)) {
        supportedWordsClasses.put(w.length, new LinkedList<short[]>());
      }
      supportedWordsClasses.get(w.length).add(w);
      if (largestWordSize < w.length) largestWordSize = w.length;
      total += 1; //w.length;
    }

    
    System.out.println("mining scenarios");
    scenarios = new ArrayList<SScenario>();
    
    int done = 0;
    supportedWords.clear();
    
    //for (int splitPos=1;splitPos < largestWordSize; splitPos++) {
      for (int size = largestWordSize; size > 0; size--) {
        
        //if (size <= splitPos) {
        //  supportedWordsClasses.remove(size);
        //  continue;
        //}
        if (!supportedWordsClasses.containsKey(size)) continue;
        
        LinkedList<short[]> mined = new LinkedList<short[]>();
        for (short[] cand : supportedWordsClasses.get(size)) {
  
          done++;
          if (done % 100 == 0) System.out.print(".");
          if (done % 7000 == 0) { 
            System.out.println(done*100/total);
          }
        
          for (int splitPos=1; splitPos<cand.length; splitPos++)
          {
            short[] pre = Arrays.copyOfRange(cand, 0, splitPos);
            short[] main = Arrays.copyOfRange(cand, splitPos, cand.length);
            
            //System.out.println("check "+toString(pre)+" "+toString(main));
            
            float c = tree.confidence(pre, main, false);
            if (c >= confidence) {
              SScenario s = new SScenario(pre, main);
              
              boolean s_weaker = false;
              List<SScenario> toRemove = new LinkedList<SScenario>();
              for (SScenario s2 : scenarios) {
                if (s.weakerThan(s2)) {
                  s_weaker = true;
                  break;
                }
                if (s2.weakerThan(s)) {
                  toRemove.add(s2);
                }
              }
              if (!s_weaker) {
                scenarios.removeAll(toRemove);
                scenarios.add(s);
                //mined.add(cand);
                //total -= (size-splitPos);
              }
              break;  
              // we found a scenario for this candidate word. any other scenario
              // will only contain longer pre-charts and shorter main charts
              // approximative optimization: stop exploring other scenarios
            }
          }
        }
        // Remove all words for which one scenario was mined. Any other
        // scenario from this word will only contain longer pre-charts
        // and shorter main charts
        // approximative optimization: stop exploring other scenarios
        //supportedWordsClasses.get(size).removeAll(mined);
      }
    //}

    lscs = new ArrayList<LSC>();
    
    tree.clearCoverageMarking();
    for (int scenarioNum = 0; scenarioNum < scenarios.size(); scenarioNum++) {
      SScenario s = scenarios.get(scenarioNum);
      float conf = tree.confidence(s.pre, s.main, true);
      
      LSC l = slog.toLSC(s, minSupportThreshold, conf, true);
      lscs.add(scenarioNum, l);
    }
    System.out.println("reduced to "+scenarios.size()+" scenarios");
    System.out.println("tree statistics: "+stat);
    

  }
  
  public Map<Short, String> getShortenedNames() {
    
    Map<Short, String> shortenedNames = new HashMap<Short, String>();
    for (short i=0; i<getSLog().originalNames.length; i++) {
      LSCEvent e = getSLog().toLSCEvent(i, true);
      shortenedNames.put(i, e.getMethod());
    }
    
    return shortenedNames;
  }
  
  public String getCoverageTreeGlobal() {
    
    tree.clearCoverageMarking();
    for (int scenarioNum = 0; scenarioNum < scenarios.size(); scenarioNum++) {
      SScenario s = scenarios.get(scenarioNum);
      tree.confidence(s.pre, s.main, true);
    }
    return tree.toDot(getShortenedNames());
  }
  
  public String getCoverageTreeFor(SScenario s) {
    
    tree.clearCoverageMarking();
    tree.confidence(s.pre, s.main, true);
    return tree.toDot(getShortenedNames());
  }
  
  public static String toMSCRenderer(String name, LSC l) {
    StringBuilder sb = new StringBuilder();
    
    sb.append("# LSC\n");
    sb.append("msc {\n");
    
    List<String> components = new LinkedList<String>();
    
    for (LSCEvent e  : l.getPreChart()) {
      if (!components.contains(e.getCaller())) components.add(e.getCaller());
      if (!components.contains(e.getCallee())) components.add(e.getCallee());
    }
    for (LSCEvent e  : l.getMainChart()) {
      if (!components.contains(e.getCaller())) components.add(e.getCaller());
      if (!components.contains(e.getCallee())) components.add(e.getCallee());
    }
    
    sb.append("  width=\""+(components.size()*128)+"\";\n");

    // list components:
    //  a,b,c;
    sb.append("  ");
    Iterator<String> comp = components.iterator();
    while (comp.hasNext()) {
      sb.append(comp.next());
      if (comp.hasNext())
        sb.append(",");
      else
        sb.append(";\n");
    }
    
    // list pre-chart events
    for (LSCEvent e  : l.getPreChart()) {
      sb.append("  "+e.getCaller()+"=>"+e.getCallee()+" [ label = \""+e.getMethod()+"\", linecolor = \"blue\" ];\n");
    }
    
    // separate pre- and main-chart
    sb.append("  ---;\n");
    
    // list main-chart events
    for (LSCEvent e  : l.getMainChart()) {
      sb.append("  "+e.getCaller()+"=>"+e.getCallee()+" [ label = \""+e.getMethod()+"\", linecolor = \"red\" ];\n");
    }

    // add name of chart
    sb.append("  "+components.get(0)+" note "+components.get(0)+" [label=\""+name+"\"];\n");
    
    sb.append("}\n");

    return sb.toString();
  }
  
  public MineBranchingTree getTree() {
    return tree;
  }
  
  public ArrayList<LSC> getLSCs() {
    return lscs;
  }
  
  public ArrayList<SScenario> getScenarios() {
    return scenarios;
  }
  
  private void removeRedundantScenarios(List<SScenario> scenarios) {
    
    int step = scenarios.size()/1000+1;
    int done = 0;
    
    Set<SScenario> redundant = new HashSet<SScenario>();
    for (int i=0; i<scenarios.size(); i++) {
      SScenario s = scenarios.get(i);
      
      if (redundant.contains(s)) continue;

      
      for (int j=i+1; j<scenarios.size(); j++) {
        SScenario s2 = scenarios.get(j);
        if (redundant.contains(s2)) continue;
        
        if (s.weakerThan(s2)) {
          redundant.add(s);
        }
        if (s2.weakerThan(s)) {
          redundant.add(s2);
        }
        
        done++;
        if (done % step == 0) System.out.print(".");
        if (done % 70*step == 0) { 
          System.out.println(redundant.size()*100/scenarios.size());
        }
        
        /*
        if (s.weakerThan_main(s2) || s.weakerThan_pre(s2)) {
          //System.out.println(s+" is weaker than "+s2);
          redundant.add(s);
          break;
        }
        if (s2.weakerThan_main(s) || s2.weakerThan_pre(s)) {
          //System.out.println(s+" is weaker than "+s2);
          redundant.add(s2);
        }
        */
      }
    }
    
    scenarios.removeAll(redundant);
  }


  
  public static String toString(short w[]) {
    String ret = "[";
    for (short c : w) ret += c+" ";
    return ret + "]";
  }
  
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
  
  private SLog getSLog() {
    return slog;
  }
  
  private Set<short[]> mineSupportedWords(MineBranchingTree tree, int minSupThreshold) {
    
    List<Short> singleEvents = new LinkedList<Short>();
    Set<short[]> supportedWords = new HashSet<short[]>();
    int maxEventId = getSLog().originalNames.length;
    for (int e=0; e<maxEventId; e++) {
      List<SLogTreeNode[]> occ = tree.countOccurrences(new short[] { (short)e });
      System.out.println("found "+e+" "+occ.size()+" times");
      if (occ.size() >= minSupThreshold) {
        singleEvents.add((short)e);
      }
    }
    
    for (short e : singleEvents) {
      System.out.println("mining for "+e);
      mineSupportedWords(tree, minSupThreshold, new short[] { e }, singleEvents, singleEvents, supportedWords);
      System.out.println(supportedWords.size());
    }
    
    return supportedWords;
  }
  
  private int dot_count = 0;
  
  private void mineSupportedWords(MineBranchingTree tree, int minSupThreshold, short word[], List<Short> ev, List<Short> preferedSucc, Set<short[]> words) {
    
    words.add(word);
    //System.out.println(toString(word));
    
    //if (word.length >= 5)
    //  return;
    
    dot_count++;
    if (dot_count % 100 == 0) System.out.print(".");
    if (dot_count > 8000) { System.out.println("."); dot_count = 0; }
    
    for (short e : preferedSucc) {
      
      int count = 0;
      for (short f : word) {
        if (e == f) {
          count++;
        }
      }
      if (count > 0) continue;
      
      short[] nextWord = Arrays.copyOf(word, word.length+1);
      nextWord[word.length] = e;
      
      List<SLogTreeNode[]> occ = tree.countOccurrences(nextWord); 
      
      if (occ.size() >= minSupThreshold) {
        List<Short> preferedSuccNext = getPreferredSuccessors(ev, occ);
        mineSupportedWords(tree, minSupThreshold, nextWord, ev, preferedSuccNext, words);
      }
    }
  }
  
  private List<Short> getPreferredSuccessors(List<Short> allEvents, List<SLogTreeNode[]> occ) {
    HashSet<Short> allSuccessors = new HashSet<Short>();
    HashSet<Short> jointSuccessors = new HashSet<Short>(allEvents);
    
    for (SLogTreeNode[] occurrence : occ) {
      if (occurrence[occurrence.length-1].post != null) {
        
        HashSet<Short> succ = new HashSet<Short>();
        for (SLogTreeNode n : occurrence[occurrence.length-1].post) {
          succ.add(n.id);
        }
        
        allSuccessors.addAll(succ);
        // keep only those events that are successors of all events
        jointSuccessors.retainAll(succ);
      }
    }
    
    // only if the successors of all occurrences are identical and
    // allowed to build a word, use them as successors 
    if (allSuccessors.equals(jointSuccessors)) 
      return new LinkedList<Short>(jointSuccessors);
    else
    // otherwise the word may continue with any event
      return allEvents;
  }
  


  public static void main(String args[]) throws Exception {
    if (args.length != 1) {
      System.out.println("error, wrong number of arguments");
      System.out.println("usage: java "+MineBranchingLSC.class.getCanonicalName()+" <logfile.xes>");
      return;
    }
    
    String srcFile = args[0];
    MineBranchingLSC miner = new MineBranchingLSC();
    miner.mineLSCs_writeResults(srcFile);
  }
  
  public static void writeToFile(String text, String fileName) throws IOException {
    
    System.out.print("Writing "+fileName+"... ");

    // Create file 
    FileWriter fstream = new FileWriter(fileName);
    BufferedWriter out = new BufferedWriter(fstream);
    out.write(text);

    //Close the output stream
    out.close();
    
    System.out.println(" done.");
  }
}
