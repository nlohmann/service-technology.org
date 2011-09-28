package org.st.sam.mine;


import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import com.google.gwt.dev.util.collect.HashMap;
import com.google.gwt.dev.util.collect.HashSet;
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
import org.st.sam.util.LSCOutput;

public class MineLSC {
  
  private ArrayList<LSC> lscs;
  private HashMap<LSC, SScenario> originalScenarios;
  
  private MineBranchingTree tree;
  
  public static final int MODE_BRANCHING = 0;
  public static final int MODE_LINEAR = 1;
  
  private int mode = MODE_BRANCHING;
  
  public MineLSC(int mode) {
    this(mode, null);
  }

  public MineLSC(int mode, SLogTree supportedWords) {
    this.mode = mode;
    setSupportedWords(supportedWords);
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
    
  }
  
  public void mineLSCs(XLog xlog, int minSupportThreshold, double confidence, String targetFilePrefix) throws IOException {
    System.out.println("log contains "+xlog.size()+" traces");
    setSLog(new SLog(xlog));
    
    boolean mergeTraces = (mode == MODE_BRANCHING) ? true : false;
    
    tree = new MineBranchingTree(getSLog(), mergeTraces);
    
    String stat = tree.getStatistics().toString();
    System.out.println("tree statistics: "+stat);
    
    List<short[]> supportedWords = getSupportedWords(tree, minSupportThreshold);
    System.out.println("found "+supportedWords.size()+" supported words");
    
    /*
    for (short[] w : supportedWords) {
      for (Short e : w) {
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
    ArrayList<SScenario> scenarios = new ArrayList<SScenario>();
    
    int done = 0;
    supportedWords.clear();
    
    lscs = new ArrayList<LSC>();
    originalScenarios = new HashMap<LSC, SScenario>();
    Map<SScenario, LSC> s2l = new HashMap<SScenario, LSC>();

    
    //for (int splitPos=1;splitPos < largestWordSize; splitPos++) {
      for (int size = largestWordSize; size > 0; size--) {
        
        //if (size <= splitPos) {
        //  supportedWordsClasses.remove(size);
        //  continue;
        //}
        if (!supportedWordsClasses.containsKey(size)) continue;
        
        //LinkedList<short[]> mined = new LinkedList<short[]>();
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
            
            LinkedList<short[]> mainCand_queue = new LinkedList<short[]>();
            mainCand_queue.add(main);
            
            while (!mainCand_queue.isEmpty()) {
              
              short[] main_cand = mainCand_queue.removeFirst();
              
              boolean skip = false;
              
              SScenario s = new SScenario(pre, main_cand);
              LSC l_test = slog.toLSC(s, 0, 0);
              if (!l_test.isConnected()) continue;
              
              //System.out.println("check "+toString(pre)+" "+toString(main));
              double c = skip ? 0 : tree.confidence(pre, main_cand, false);
              
              if (c >= confidence) {
                
                //System.out.println(s+" satisfies confidence");
                
                List<SLogTreeNode[]> occ = tree.countOccurrences(cand, null, null);
                int total_occurrences = 0;
                for (SLogTreeNode[] o : occ) {
                  // total number of occurrences = number of different occurrences * number of traces having this occurrence until the end of the word
                  total_occurrences += tree.nodeCount.get(o[o.length-1]);
                }
                
                LSC l = slog.toLSC(s, total_occurrences, c);
                
                if (l.isConnected()) {
                  boolean s_weaker = false;
                  List<SScenario> toRemove = new LinkedList<SScenario>();
                  for (SScenario s2 : scenarios) {
                    if (s.weakerThan(s2)
                        //&& l.getSupport() <= s2l.get(s2).getSupport() 
                        //&& l.getConfidence() <= s2l.get(s2).getConfidence()
                        )
                    {
                      //System.out.println("subsubmed by "+s2);
                      s_weaker = true;
                      break;
                    }
                    if (s2.weakerThan(s)
                        //&& l.getSupport() >= s2l.get(s2).getSupport() 
                        //&& l.getConfidence() >= s2l.get(s2).getConfidence()
                        )
                    {
                      //System.out.println("subsumes "+s2);
                      toRemove.add(s2);
                    }
                  }
                  if (!s_weaker) {
                    //System.out.println("removing all "+toRemove);
                    scenarios.removeAll(toRemove);
                    for (SScenario s_toRemove : toRemove) {
                      originalScenarios.remove(s2l.get(s_toRemove));
                      lscs.remove(s2l.get(s_toRemove));
                      s2l.remove(s_toRemove);
                    }
  
                    scenarios.add(s);
                    lscs.add(l);
                    originalScenarios.put(l, s);
                    s2l.put(s, l);
                    
                    //mined.add(cand);
                    //total -= (size-splitPos);
                  }
                  break;  
                  // we found a scenario for this candidate word. any other scenario
                  // will only contain longer pre-charts and Shorter main charts
                  // approximative optimization: stop exploring other scenarios
                } else {
                  //System.out.println("is not connected");
                }
              }
            }
          }
        }
        // Remove all words for which one scenario was mined. Any other
        // scenario from this word will only contain longer pre-charts
        // and Shorter main charts
        // approximative optimization: stop exploring other scenarios
        //supportedWordsClasses.get(size).removeAll(mined);
      }
    //}
      
    for (SScenario s : scenarios) {
      System.out.println(s);
    }
    
    supportedWordsClasses.clear();
    
    System.out.println("reduced to "+lscs.size()+" scenarios");
    System.out.println("tree statistics: "+stat);
    

  }
  
  public Map<Short, String> getShortenedNames() {
    
    Map<Short, String> ShortenedNames = new HashMap<Short, String>();
    for (Short i=0; i<getSLog().originalNames.length; i++) {
      LSCEvent e = getSLog().toLSCEvent(i);
      ShortenedNames.put(i, LSCOutput.shortenNames(e.getMethod()));
    }
    
    return ShortenedNames;
  }
  
  public String getCoverageTreeGlobal() {
    
    tree.clearCoverageMarking();
    for (SScenario s : originalScenarios.values()) {
      tree.confidence(s.pre, s.main, true);
    }
    return tree.toDot(getShortenedNames());
  }
  
  public String getCoverageTreeFor(SScenario s) {
    
    tree.clearCoverageMarking();
    double c = tree.confidence(s.pre, s.main, true);
    System.out.println(s+" has confidence "+c);
    return tree.toDot(getShortenedNames());
  }
  
  public MineBranchingTree getTree() {
    return tree;
  }
  
  public ArrayList<LSC> getLSCs() {
    return lscs;
  }
  
  public Map<LSC, SScenario> getScenarios() {
    return originalScenarios;
  }
  
  public static String toString(Short w[]) {
    if (w == null) return "null";
    String ret = "[";
    for (Short c : w) ret += c+" ";
    return ret + "]";
  }
  
  public static String toString(short w[]) {
    if (w == null) return "null";
    String ret = "[";
    for (Short c : w) ret += c+" ";
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
  
  public SLog getSLog() {
    return slog;
  }
  
  private SLogTree supportedWords = null;
  
  private void setSupportedWords(SLogTree supportedWords) {
    this.supportedWords = supportedWords; 
  }
  
  public SLogTree getSupportedWords() {
    return supportedWords;
  }
  
  private List<short[]> getSupportedWords(MineBranchingTree tree, int minSupThreshold) {
    if (supportedWords == null) {
      mineSupportedWords(tree, minSupThreshold);
    }
    return supportedWords.getAllWords(); 
  }
  
  private void mineSupportedWords(MineBranchingTree tree, int minSupThreshold) {
    
    List<Short> singleEvents = new LinkedList<Short>();
    
    supportedWords = new SLogTree();
    int maxEventId = getSLog().originalNames.length;
    for (int e=0; e<maxEventId; e++) {
      List<SLogTreeNode[]> occ = tree.countOccurrences(new short[] { (short)e }, null, null);

      int total_occurrences = 0;
      for (SLogTreeNode[] o : occ) {
        // total number of occurrences = number of different occurrences * number of traces having this occurrence until the end of the word
        total_occurrences += tree.nodeCount.get(o[o.length-1]);
        
/*        if (o[o.length-1].post.length == 1 && o[o.length-1].post[0].id == e) {
          total_occurrences = 0;
          break;
        }*/
      }

      System.out.println("found "+e+" "+occ.size()+" times amounting to "+total_occurrences+" occurrences in the log");
      if (total_occurrences >= minSupThreshold) {
        singleEvents.add((short)e);
      }
    }
    
    for (short e : singleEvents) {
      System.out.println("mining for "+e);
      List<Short> remainingEvents = new LinkedList<Short>(singleEvents);
      remainingEvents.remove((Short)e);
      mineSupportedWords_optimized(tree, minSupThreshold, new short[] { e }, remainingEvents, remainingEvents, supportedWords, false);
      //mineSupportedWords(tree, minSupThreshold, new short[] { e }, singleEvents, supportedWords);
      System.out.println(supportedWords.nodes.size());
    }
  }
  
  private int dot_count = 0;
  
  private short[] largestWord_checked = null;
  
  private HashMap<SLogTreeNode, Set<Short>> checkSuccessors = new HashMap<SLogTreeNode, Set<Short>>();
  
  private void mineSupportedWords_optimized(MineBranchingTree tree, int minSupThreshold, final short word[], List<Short> ev, List<Short> preferedSucc, SLogTree words, boolean check) {

    List<Short> newPreferedSucc = new LinkedList<Short>(preferedSucc);
    SLogTreeNode leaf = words.contains(word);
    if (leaf != null && checkSuccessors.containsKey(leaf)) {
      Set<Short> checkedSuccessors = checkSuccessors.get(leaf);
      // remove all successors to explore that have already been explored
      newPreferedSucc.removeAll(checkedSuccessors);
      // if there is none left, we are done
      if (newPreferedSucc.isEmpty()) {
        if (showDebug(word, preferedSucc)) {
          System.out.println("already seen "+toString(word)+" "+checkSuccessors.get(leaf));
        }
        return;
      }
    }
    
    if (largestWord_checked == null) largestWord_checked = word;
    else if (lessThan(largestWord_checked, word)) largestWord_checked = word;
    
    SLogTreeNode n = words.addTrace(word);
    if (!checkSuccessors.containsKey(n)) checkSuccessors.put(n, new HashSet<Short>());
    checkSuccessors.get(n).addAll(newPreferedSucc);
    //System.out.println(toString(word));
    if (showDebug(word, preferedSucc)) {
      System.out.println("adding "+toString(word)+" "+checkSuccessors.get(n));
    }
    
    //if (word.length >= 8)
    //  return;
    
    dot_count++;
    if (dot_count % 100 == 0) System.out.print(".");
    if (dot_count > 8000) { System.out.println(". "+words.nodes.size()+" "+toString(largestWord_checked)); dot_count = 0; }
    
    Set<Short> violators = new HashSet<Short>();
    Set<Short> stuck_at = null;
    
    for (Short e : newPreferedSucc) {
      
      // count how often successor e already occurs in the word w
      int count = 0;
      for (Short f : word) {
        if (e == f) {
          count++;
        }
      }
      // it does: skip this successor (each event occurs only once)
      if (count > 0) {
        //mineSupportedWords_subwords(tree, minSupThreshold, word, ev, words, e);
        continue;
      }
      
      short[] nextWord = Arrays.copyOf(word, word.length+1);
      nextWord[word.length] = e;
      
      if (showDebug(nextWord, preferedSucc)) {
        System.out.print(toString(nextWord));
      }
      
      Set<Short> stuck_here = new HashSet<Short>();
      List<SLogTreeNode[]> occ = tree.countOccurrences(nextWord, violators, stuck_here);
      if (stuck_at == null) stuck_at = stuck_here;
      else stuck_at.retainAll(stuck_here);
      
      int total_occurrences = 0;
      for (SLogTreeNode[] o : occ) {
        // total number of occurrences = number of different occurrences * number of traces having this occurrence until the end of the word
        total_occurrences += tree.nodeCount.get(o[o.length-1]);
      }
      
      if (total_occurrences >= minSupThreshold) {
      
        List<Short> ev_avail = new LinkedList<Short>(ev);
        ev_avail.remove((Short)e); // event id 'e' no longer available for continuation
        
        //List<Short> preferedSuccNext = ev;
        Set<Short> violatingSuccessors = new HashSet<Short>();
        List<Short> preferedSuccNext = getPreferredSuccessors(ev_avail, occ, violatingSuccessors);

        if (showDebug(nextWord, preferedSucc)) {
          System.out.println("IS IN "+total_occurrences+" "+preferedSuccNext);
        }
        
        mineSupportedWords_optimized(tree, minSupThreshold, nextWord, ev_avail, preferedSuccNext, words, false);
        if (nextWord.length > 1) {
          for (Short ignore : violatingSuccessors) {
            
            int v_count = 0;
            for (Short v : nextWord) {
              if (v == ignore) v_count++;
            }
            if (v_count == 0) continue;
            
            short[] newWord = new short[nextWord.length-1];
            int j=0;
            while (nextWord[j] != ignore) {
              newWord[j] = nextWord[j];
              j++;
            }
            while (j<newWord.length) {
              newWord[j] = nextWord[j+1];
              j++;
            }
            if (lessThan(newWord, largestWord_checked))
            {
              if (showDebug(newWord, preferedSucc)) {
                System.out.println("RETRY 1 "+toString(newWord) +" WITHOUT "+ignore);
              }
              mineSupportedWords_optimized(tree, minSupThreshold, newWord, ev_avail, preferedSuccNext, words, true);
            }
          }
        }
      } else {
        
        if (showDebug(nextWord, preferedSucc)) {
          System.out.println("IS OUT "+total_occurrences+" violators: "+violators+" stuck: "+stuck_at);
        }
        
        if (newPreferedSucc != ev) {
          if (showDebug(nextWord, preferedSucc)) {
            System.out.println("RETRY 2 "+toString(word) +" WITH ALL SUCCESSORS");
          }
          // the preferred successors turned out to be a bad choice, so retry with all successors
          mineSupportedWords_optimized(tree, minSupThreshold, word, ev, ev, words, false);
        }
      }
    }
        
        if (word.length > 1) {
          if (stuck_at != null) violators.addAll(stuck_at);
          for (Short ignore : violators) {
            
            int v_count = 0;
            for (Short v : word) {
              if (v == ignore) v_count++;
            }
            if (v_count == 0) continue;
            
            short[] newWord = new short[word.length-1];
            int j=0;
            while (word[j] != ignore) {
              newWord[j] = word[j];
              j++;
            }
            while (j<newWord.length) {
              newWord[j] = word[j+1];
              j++;
            }
            
            LinkedList<Short> ev_red = new LinkedList<Short>(ev);
            ev_red.remove((Short)ignore);

            if (lessThan(newWord, largestWord_checked))
            {
              if (showDebug(newWord, preferedSucc)) {
                System.out.println("RETRY 3 "+toString(newWord) +" WITHOUT "+ignore+" from "+toString(word));
              }

              mineSupportedWords_optimized(tree, minSupThreshold, newWord, ev_red, ev_red, words, true);
            }
          }
        }
        
        // it could be that one letter in the middle of the word prevents extensions of the found
        // word to exceed the threshold
        //if (stuck_at.size() > 0)
        //  mineSupportedWords_subwords(tree, minSupThreshold, word, ev, words);

        //mineSupportedWords_subwords(tree, minSupThreshold, nextWord, ev, words);

    if (showDebug(word, preferedSucc)) {
      System.out.println("#");
    }
  }
  
  private void mineSupportedWords(MineBranchingTree tree, int minSupThreshold, final short word[], List<Short> ev, SLogTree words) {

    words.addTrace(word);
    
    if (word.length >= 5)
      return;
    
    dot_count++;
    if (dot_count % 100 == 0) System.out.print(".");
    if (dot_count > 8000) { System.out.println(". "+words.nodes.size()+" "+toString(largestWord_checked)); dot_count = 0; }
    
    
    for (Short e : ev) {
      
      // count how often successor e already occurs in the word w
      int count = 0;
      for (Short f : word) {
        if (e == f) {
          count++;
        }
      }
      // it does: skip this successor (each event occurs only once)
      if (count > 0) {
        //mineSupportedWords_subwords(tree, minSupThreshold, word, ev, words, e);
        continue;
      }
      
      short[] nextWord = Arrays.copyOf(word, word.length+1);
      nextWord[word.length] = e;
      
      List<SLogTreeNode[]> occ = tree.countOccurrences(nextWord, null, null);
      
      int total_occurrences = 0;
      for (SLogTreeNode[] o : occ) {
        // total number of occurrences = number of different occurrences * number of traces having this occurrence until the end of the word
        total_occurrences += tree.nodeCount.get(o[o.length-1]);
      }
      
      if (total_occurrences >= minSupThreshold) {
      
        mineSupportedWords(tree, minSupThreshold, nextWord, ev, words);

      }
    }
        
  }

  
  private static boolean lessThan(short[] word1, short[] word2) {
    int i=0;
    while (i < word1.length) {
      if (i >= word2.length) break;
      if (word1[i] > word2[i]) return false;
      if (word1[i] < word2[i]) return true;
      i++;
    }
    return (word1.length < word2.length);
  }

  private static boolean showDebug(short[] word, List<Short> preferedSucc) {
    return false;
    //return (word.length == 12 && word[0] == 0 && word[1] == 1 && word[11] == 11 );
    //return (word[0] == 85);
  }
  
  private void getVisibleSuccessors(List<Short> visibleEvents, SLogTreeNode n, Set<Short> visibleSuccessors, Set<Short> violatingSuccessors) {
    for (SLogTreeNode succ : n.post) {
      if (visibleEvents.contains(succ.id))
        visibleSuccessors.add(succ.id);
      else {
        violatingSuccessors.add(succ.id);
        getVisibleSuccessors(visibleEvents, succ, visibleSuccessors, violatingSuccessors);
      }
    }
  }
  
  private List<Short> getPreferredSuccessors(List<Short> allowedEvents, List<SLogTreeNode[]> occ, Set<Short> violatingSuccessors) {
    HashSet<Short> allSuccessors = new HashSet<Short>();
    HashSet<Short> jointSuccessors = new HashSet<Short>(allowedEvents);
    
    for (SLogTreeNode[] occurrence : occ) {
      
//      int occCount = tree.nodeCount.get(occurrence[occurrence.length-1]);
//      for (SLogTreeNode n : occurrence[occurrence.length-1].post) {
//        if (tree.nodeCount.get(n) < occCount) return allowedEvents;
//      }
      
      HashSet<Short> succ = new HashSet<Short>();
      getVisibleSuccessors(allowedEvents, occurrence[occurrence.length-1], succ, violatingSuccessors);
      
      allSuccessors.addAll(succ);
      // keep only those events that are successors of all events
      jointSuccessors.retainAll(succ);
    }
    
    // only if the successors of all occurrences are identical and
    // allowed to build a word, use them as successors 
    if (allSuccessors.equals(jointSuccessors) && jointSuccessors.size() == 1) {
      // remove all letters that cannot be used for further extensions 
      return new LinkedList<Short>(jointSuccessors);
    } else {
      // otherwise the word may continue with any event
      return allowedEvents;
    }
  }
  



}
