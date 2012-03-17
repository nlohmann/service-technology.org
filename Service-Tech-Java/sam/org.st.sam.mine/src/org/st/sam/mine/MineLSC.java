package org.st.sam.mine;


import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import com.google.gwt.dev.util.collect.HashMap;
import com.google.gwt.dev.util.collect.HashSet;

import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
//import java.util.TreeSet;

import lscminer.datastructure.LSC;
import lscminer.datastructure.LSCEvent;

import org.deckfour.xes.model.XLog;
import org.st.sam.log.SLog;
import org.st.sam.log.SLogTree;
import org.st.sam.log.SLogTree.SupportedWord;
import org.st.sam.log.SLogTreeNode;
import org.st.sam.log.SScenario;
import org.st.sam.log.XESImport;
import org.st.sam.mine.collect.SimpleArrayList;
import org.st.sam.util.SAMOutput;

public class MineLSC {
  
  public static class Configuration {
    public static final int MODE_BRANCHING = 0;
    public static final int MODE_LINEAR = 1;

    public int mode = MODE_BRANCHING;
    public boolean optimizedSearch = true;
    public boolean skipEvents_invisible = true;
    public boolean allowEventRepetitions = false;
    public List<short[]> triggers = null;
    public List<short[]> effects = null;
    public boolean discoverScenarios = true;
    
    public boolean removeSubsumed = true;
    
    public static Configuration mineBranching() {
      Configuration c = new Configuration();
      c.mode = MODE_BRANCHING;
      return c;
    }
    
    public static Configuration mineLinear() {
      Configuration c = new Configuration();
      c.mode = MODE_LINEAR;
      return c;
    }
    
    public static Configuration mineEffect(short[] trigger) {
      Configuration c = new Configuration();
      c.mode = MODE_BRANCHING;
      c.triggers = new ArrayList<short[]>();
      c.triggers.add(trigger);
      return c;
    }
  }
  
  public boolean OPTIONS_WEIGHTED_OCCURRENCE = true;
  
  private ArrayList<LSC> lscs;
  private HashMap<LSC, SScenario> originalScenarios;
  private Map<SScenario, LSC> s2l;
  
  private MineBranchingTree tree;
  
  private Configuration config;
  
  public  long time_candiate_words;
  
  public  long time_scenario_discovery;
  
  public  long number_of_candidate_words;
  
  public  long total_number_of_scenarios = 0;
  
  public MineLSC(Configuration config) {
    this(config, null);
  }

  public MineLSC(Configuration config, SimpleArrayList<SupportedWord> supportedWords) {
    this.config = config;
    setSupportedWords(supportedWords);
  }
  
  public void mineLSCs(String logfile) throws IOException {
    int minSupportThreshold = 10;
    float confidence = 1.0f;
    
    mineLSCs(logfile, minSupportThreshold, confidence);
  }
  
  public void mineLSCs(String logFile, int minSupportThreshold, double confidence) throws IOException {
    mineLSCs(XESImport.readXLog(logFile), minSupportThreshold, confidence, logFile);
  }
  
  public void mineLSCs_writeResults(String logFile) throws IOException {
    int minSupportThreshold = 10;
    float confidence = 1.0f;
    
    mineLSCs_writeResults(logFile, minSupportThreshold, confidence);
  }
  
  public void mineLSCs_writeResults(String logFile, int minSupportThreshold, double confidence) throws IOException {
    
    mineLSCs(logFile, minSupportThreshold, confidence);
    sortLSCs(getLSCs());
    
    String targetFilePrefix = logFile;
    
    SAMOutput.writeToFile(getTree().toDot(getShortenedNames()), targetFilePrefix+".dot");
    SAMOutput.writeToFile(getCoverageTreeGlobal(), targetFilePrefix+"_cov.dot");
    for (int lscNum=0; lscNum<getLSCs().size(); lscNum++) {
      LSC l = getLSCs().get(lscNum);
      SAMOutput.writeToFile(getCoverageTreeFor(originalScenarios.get(l)), targetFilePrefix+"_cov_"+(lscNum+1)+".dot");
    }
    
    StringBuilder found_lscs = new StringBuilder();
    for (int i=0; i<lscs.size(); i++) {
      
      LSC l = lscs.get(i);
      found_lscs.append(l.toString());
      found_lscs.append("\n");
    }
    SAMOutput.writeToFile(found_lscs.toString(), targetFilePrefix+"_all_lscs.txt");
    
  }
  
  public void mineLSCs(final XLog xlog, int minSupportThreshold, double confidence, String targetFilePrefix) throws IOException {
    System.out.println("log contains "+xlog.size()+" traces");
    
    SLog slog = new SLog(xlog);
    boolean mergeTraces = (config.mode == Configuration.MODE_BRANCHING) ? true : false;
    tree = new MineBranchingTree(slog, mergeTraces);
    
    mineLSCs(slog, tree, minSupportThreshold, confidence, targetFilePrefix);
  }
    
  public void mineLSCs(final SLog slog, final MineBranchingTree tree, int minSupportThreshold, double confidence, String targetFilePrefix) throws IOException {
    
    setSLog(slog);
    
    String stat = tree.getStatistics().toString();
    System.out.println("tree statistics: "+stat);
    
    long time_start_candiate_words = System.currentTimeMillis();
    
    SimpleArrayList<SupportedWord> supportedWords = getSupportedWords(tree, minSupportThreshold);
    System.out.println("found "+supportedWords.size()+" supported words");
    number_of_candidate_words = supportedWords.size();
    
    long time_end_candiate_words = System.currentTimeMillis();
    
    this.time_candiate_words = time_end_candiate_words-time_start_candiate_words;
    
    /*
    for (short[] w : supportedWords) {
      for (Short e : w) {
        System.out.print(e+" ");
      }
      System.out.println();
    }
    */
    
    lscs = new ArrayList<LSC>();
    originalScenarios = new HashMap<LSC, SScenario>();
    s2l = new HashMap<SScenario, LSC>();
    
    long time_start_scenario_discovery = System.currentTimeMillis();
    
    ArrayList<SScenario> scenarios = new ArrayList<SScenario>();
    if (config.discoverScenarios)
      discoverScenariosFromWords(supportedWords, confidence, scenarios);
      
    long time_end_scenario_discovery = System.currentTimeMillis();
    
    time_scenario_discovery = time_end_scenario_discovery-time_start_scenario_discovery;
   
    System.out.println("reduced to "+lscs.size()+" scenarios");
    System.out.println("tree statistics: "+stat);
  }
  
  private void discoverScenariosFromWords(SimpleArrayList<SupportedWord> supportedWords, double confidence, ArrayList<SScenario> scenarios) {
    
    int total = 0;
    Map<Integer, SimpleArrayList<SupportedWord>> supportedWordsClasses = new HashMap<Integer, SimpleArrayList<SupportedWord>>();
    int largestWordSize = 0;
    for (SupportedWord w : supportedWords) {
      if (!supportedWordsClasses.containsKey(w.word.length)) {
        supportedWordsClasses.put(w.word.length, new SimpleArrayList<SupportedWord>());
      }
      supportedWordsClasses.get(w.word.length).add(w);
      if (largestWordSize < w.word.length) largestWordSize = w.word.length;
      total += 1; //w.length;
    }

    
    System.out.println("mining scenarios");

    
    int done = 0;
    
    SimpleArrayList<SScenario> toRemove = new SimpleArrayList<SScenario>();
    
    //for (int splitPos=1;splitPos < largestWordSize; splitPos++) {
      for (int size = largestWordSize; size > 0; size--) {
        
        //if (size <= splitPos) {
        //  supportedWordsClasses.remove(size);
        //  continue;
        //}
        if (!supportedWordsClasses.containsKey(size)) continue;
        
        //LinkedList<short[]> mined = new LinkedList<short[]>();
        for (SupportedWord _cand : supportedWordsClasses.get(size)) {
          
          short[] cand = _cand.word;
  
          done++;
          if (done % 100 == 0) System.out.print(".");
          if (done % 7000 == 0) { 
            System.out.println(done*100/total+"% (found "+scenarios.size()+" so far)");
          }
        
          for (int splitPos=1; splitPos<cand.length; splitPos++)
          {
            short[] pre = Arrays.copyOfRange(cand, 0, splitPos);
            short[] main = Arrays.copyOfRange(cand, splitPos, cand.length);
            
            // trigger/effect mining: only check scenarios
            // that have the matching trigger of effect
            if (config.triggers != null) {
              boolean not_looking_for_this = true;
              for (short[] trigger : config.triggers)
                if (Arrays.equals(pre, trigger)) {
                  not_looking_for_this = false;
                  break;
                }
              if (not_looking_for_this) continue;
            }
            if (config.effects != null) {
              boolean not_looking_for_this = true;
              for (short[] effect : config.effects)
                if (Arrays.equals(main, effect)) {
                  not_looking_for_this = false;
                  break;
                }
              if (not_looking_for_this) continue;
            }
            
            LinkedList<short[]> mainCand_queue = new LinkedList<short[]>();
            mainCand_queue.add(main);
            
            while (!mainCand_queue.isEmpty()) {
              
              short[] main_cand = mainCand_queue.removeFirst();
              
              boolean skip = false;
              
              SScenario s = new SScenario(pre, main_cand, slog.originalNames.length);
              
              LSC l_test = slog.toLSC(s, 0, 0);
              if (!l_test.isConnected()) {
                if (showDebug(cand, null)) {
                  System.out.println("check "+toString(pre)+" "+toString(main_cand)+" is not connected");
                }

                boolean[] unConnected = l_test.getUnConnectedEvents();
                int numUnconnected = 0;
                for (int i=0; i<unConnected.length; i++) if (unConnected[i]) numUnconnected++;
                
                if (main_cand.length > numUnconnected)
                {
                  // skip unconnected events
                  short[] newMain = new short[main_cand.length-numUnconnected];
                  int i=0,j=0;
                  while (j<newMain.length) {
                    if (!unConnected[i+pre.length]) {
                      newMain[j] = main_cand[i];
                      j++;
                    }
                    i++;
                  }
                  mainCand_queue.addLast(newMain);
                  if (showDebug(cand, null)) {
                    System.out.println("check sub-chart "+toString(pre)+" "+toString(newMain));
                  }

                }
                
                continue;
              }
              
              double c = skip ? 0 : tree.confidence(s, false);
              
              if (showDebug(cand, null)) {
                System.out.println("check "+toString(pre)+" "+toString(main_cand)+" has confidence "+c);
              }

              
              if (c >= confidence) {
                
                if (showDebug(cand, null)) {
                  System.out.println(s+" satisfies confidence");
                }
                
                total_number_of_scenarios++;
                
                //if (_cand.support == -1)
                {
                  SimpleArrayList<SLogTreeNode[]> occ = tree.countOccurrences(cand, null, null);
                  _cand.support = tree.getTotalOccurrences(occ, OPTIONS_WEIGHTED_OCCURRENCE);
                }
                
                LSC l = slog.toLSC(s, _cand.support, c);
                s.support = _cand.support;
                
                if (l.isConnected()) {
                  if (showDebug(cand, null)) {
                    System.out.println("  is connected");
                  }
                  
                  boolean s_weaker = false;
                  toRemove.quickClear();
                  
                  if (config.removeSubsumed) {
                    for (SScenario s2 : scenarios) {
                      if (subsumes(s2, s)  
                          //&& l.getConfidence() <= s2l.get(s2).getConfidence()
                          )
                      {
                        if (showDebug(cand, null)) {
                          System.out.println("  subsubmed by "+s2);
                        }
                        s_weaker = true;
                        break;
                      }
                    }
                  }
                  
                  if (!s_weaker) {
                    int insertAt = -1;
                    
                    if (config.removeSubsumed) {
                      for (int s2_index=0; s2_index<scenarios.size(); s2_index++) { 
                        SScenario s2 = scenarios.get(s2_index);
                        if (subsumes(s, s2)
                            //&& l.getConfidence() >= s2l.get(s2).getConfidence()
                            )
                        {
                          if (showDebug(cand, null)) {
                            System.out.println("  subsubmes "+s2);
                          }
                          toRemove.add(s2);
                          if (insertAt != -1) insertAt =  s2_index;
                        }
                      }
                    }

                    //System.out.println("removing all "+toRemove);
                    for (SScenario s_toRemove : toRemove) {
                      
                      if (showDebug(s_toRemove.word, null)) {
                        System.out.println(s_toRemove+"  subsubmed by "+s);
                      }
                      
                      scenarios.remove(s_toRemove);
                      originalScenarios.remove(s2l.get(s_toRemove));
                      lscs.remove(s2l.get(s_toRemove));
                      s2l.remove(s_toRemove);
                    }
  
                    if (insertAt != -1) // insert at the position where it subsumed the first scenario
                      scenarios.add(insertAt, s);
                    else
                      scenarios.add(s);
                    lscs.add(l);
                    originalScenarios.put(l, s);
                    s2l.put(s, l);
                    
                    //mined.add(cand);
                    //total -= (size-splitPos);
                  } else { // s is weaker
                  }
                  break;  
                  // we found a scenario for this candidate word. any other scenario
                  // will only contain longer pre-charts and Shorter main charts

                } else {
                  
                  if (showDebug(cand, null)) {
                    System.out.println("  is not connected");
                  }
                  
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
      
    if (config.removeSubsumed) {
      System.out.println("\nRemoving remaining implied scenarios ("+scenarios.size()+")...");  
      boolean changed = true;
      while(changed) {
        changed = false;
  
        toRemove.quickClear();
        
        for (SScenario s : scenarios) {
          for (SScenario s2 : scenarios) {
            if (s == s2) continue;
            if (implies(s, s2)) {
              System.out.println(s+"  implies  "+s2);
              toRemove.add(s2);
            }
          }
          if (toRemove.size() > 0) {
            break;
          }
        }
        
        //System.out.println("removing all "+toRemove);
        for (SScenario s_toRemove : toRemove) {
          scenarios.remove(s_toRemove);
          originalScenarios.remove(s2l.get(s_toRemove));
          lscs.remove(s2l.get(s_toRemove));
          s2l.remove(s_toRemove);
          changed = true;
        }
      }
    }
      
    for (SScenario s : scenarios) {
      System.out.println(s);
    }
    
    supportedWordsClasses.clear();
  }
  
  public boolean implies(SScenario s1, SScenario s2) {
    //if (s1.implies(s2) && s2l.get(s1).getSupport() >= s2l.get(s2).getSupport())
    
    assert s1.support > 0;
    assert s2.support > 0;
    
    if (s1.support >= s2.support && s1.implies(s2)
        )
      return true;
    else
      return false;
  }
  
  public boolean subsumes(SScenario s1, SScenario s2) {
    //if (s1.implies(s2) && s2l.get(s1).getSupport() >= s2l.get(s2).getSupport())
    
    assert s1.support > 0;
    assert s2.support > 0;
    
    if (s1.support >= s2.support && s1.subsumes(s2))
      return true;
    else
      return false;
  }
  

  
  public Map<Short, String> getShortenedNames() {
    
    Map<Short, String> ShortenedNames = new HashMap<Short, String>();
    for (Short i=0; i<getSLog().originalNames.length; i++) {
      String[] e = getSLog().toLSCEventNames(i);
      ShortenedNames.put(i, SAMOutput.shortenNames(e[SLog.LSC_METHOD]));
    }
    
    return ShortenedNames;
  }
  
  public String getCoverageTree_current() {
    return tree.toDot(getShortenedNames());
  }
  
  
  public String getCoverageTreeGlobal() {
    
    tree.clearCoverageMarking();
    for (SScenario s : originalScenarios.values()) {
      tree.confidence(s, true);
    }
    return tree.toDot(getShortenedNames());
  }
  
  public String getCoverageTreeFor(SScenario s) {
    
    tree.clearCoverageMarking();
    double c = tree.confidence(s, true);
    System.out.println(s+" has confidence "+c);
    return tree.toDot(getShortenedNames());
  }
  
  public MineBranchingTree getTree() {
    return tree;
  }
  
  public ArrayList<LSC> getLSCs() {
    return lscs;
  }
  
  
  public static void sortLSCs(ArrayList<LSC> lscs) {
    
    Comparator<LSC> comp = new Comparator<LSC>() {
      @Override
      public int compare(LSC o1, LSC o2) {
        return o1.toString().compareTo(o2.toString());
      }
    };
    
    Collections.sort(lscs, comp);
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
  
  private SLog slog;
  
  private void setSLog(SLog slog) {
    this.slog = slog;
  }
  
  public SLog getSLog() {
    return slog;
  }
  
  private SLogTree supportedWordsTree = null;
  private SimpleArrayList<SupportedWord> supportedWords = null;
  
  private void setSupportedWords(SimpleArrayList<SupportedWord> supportedWords) {
    this.supportedWords = supportedWords; 
  }
  
  public SimpleArrayList<SupportedWord> getSupportedWords() {
    return supportedWords;
  }
  
  private SimpleArrayList<SupportedWord> getSupportedWords(MineBranchingTree tree, int minSupThreshold) {
    if (supportedWords == null) {
      mineSupportedWords(tree, minSupThreshold);
      supportedWords = supportedWordsTree.getAllWords();
    }
    return supportedWords; 
  }
  
  private void mineSupportedWords(MineBranchingTree tree, int minSupThreshold) {
    
    log_new(log_msw, "");
    
    List<Short> singleEvents = new ArrayList<Short>();
    
    supportedWordsTree = new SLogTree();
    int maxEventId = getSLog().originalNames.length;
    for (int e=0; e<maxEventId; e++) {
      
      //if (e < 11 || (e >= 16 && e <=26)) continue; 
      
      SimpleArrayList<SLogTreeNode[]> occ = tree.countOccurrences(new short[] { (short)e }, null, null);

      int total_occurrences = tree.getTotalOccurrences(occ, OPTIONS_WEIGHTED_OCCURRENCE);

      System.out.println("found "+e+" "+occ.size()+" times amounting to "+total_occurrences+" occurrences in the log // "+getSLog().originalNames[e]);
      if (total_occurrences >= minSupThreshold) {
        singleEvents.add((short)e);
      }
    }
    
    // copy all events to an array
    short[] allEvents = toArray(singleEvents);

    if (config.effects == null) { // no effect mining
      for (short e : singleEvents) {
        
        if (config.triggers != null) {
          boolean isTriggerEvent = false;
          for (short[] trig : config.triggers) {
            if (trig[0] == e) isTriggerEvent = true;
          }
          // when mining triggers, skip words that do not start like a trigger
          if (!isTriggerEvent) continue;
        }
        
        System.out.println("mining for "+e);
        short remainingEvents[] = removeFrom(allEvents, e);
        
        if (config.optimizedSearch)
          mineSupportedWords_optimized(tree, minSupThreshold, new short[] { e }, remainingEvents, remainingEvents, supportedWordsTree, false);
        else
          mineSupportedWords(tree, minSupThreshold, new short[] { e }, singleEvents, supportedWordsTree, false);
        System.out.println(supportedWordsTree.nodes.size());
      }
    } else { // effect mining
      
      for (short[] effect : config.effects) {
        System.out.println("mining for "+toString(effect));
        // search backwards
        mineSupportedWords(tree, minSupThreshold, effect, singleEvents, supportedWordsTree, true);
      }
      
    }
    

  }
  
  public static Comparator< Short > short_comp = new Comparator<Short>() {

    @Override
    public int compare(Short o1, Short o2) {
      if (o1 < o2) return -1;
      if (o1 > o2) return 1;
      return 0;
    }
  };
  
  private int dot_count = 0;
  
  private short[] largestWord_checked = null;
  
  private HashMap<SLogTreeNode,boolean[]> checkSuccessors = new HashMap<SLogTreeNode,boolean[]>();
  private HashMap<Short,boolean[]> retryMap = new HashMap<Short, boolean[]>(); 
  
  private SLogTreeNode mineSupportedWords_optimized(MineBranchingTree tree, int minSupThreshold, final short word[], final short[] ev, final short[] preferedSucc, SLogTree words, boolean check) {

    short[] newPreferedSucc = preferedSucc;
    SLogTreeNode leaf = words.contains(word);
    if (leaf != null && checkSuccessors.containsKey(leaf)) {
      
      boolean[] checkedSuccessors2 = checkSuccessors.get(leaf);
      
      //Set<Short> checkedSuccessors = new HashSet<Short>();
      //for (short b=0; b<checkedSuccessors2.length; b++) if (checkedSuccessors2[b]) checkedSuccessors.add(b);
      
      // remove all successors to explore that have already been explored
      newPreferedSucc = removeFrom(newPreferedSucc, checkedSuccessors2);
      
      // if there is none left, we are done
      if (newPreferedSucc.length == 0) {
        if (showDebug(word, preferedSucc)) {
          log(log_msw, "already seen "+toString(word)+" "+checkSuccessors.get(leaf)+"\n");
        }
        return null;
      }
    }
    
    if (largestWord_checked == null) largestWord_checked = word;
    else if (lessThan(largestWord_checked, word)) largestWord_checked = word;
    
    SLogTreeNode n = words.addTrace(word);
    if (!checkSuccessors.containsKey(n)) checkSuccessors.put(n, new boolean[slog.originalNames.length]);
    for (int i=0; i<newPreferedSucc.length; i++)
      checkSuccessors.get(n)[newPreferedSucc[i]] = true;
    //System.out.println(toString(word));
    if (showDebug(word, preferedSucc)) {
      log(log_msw, "adding "+toString(word)+" "+checkSuccessors.get(n)+"\n");
    }
    
    //if (word.length >= 8)
    //  return;
    
    dot_count++;
    if (dot_count % 100 == 0) System.out.print(".");
    if (dot_count > 8000) { System.out.println(". "+words.nodes.size()+" "+toString(largestWord_checked)); dot_count = 0; }
    
    boolean[] violators = new boolean[slog.originalNames.length];
    boolean[] stuck_at = null;
    
    if (showDebug(word, preferedSucc)) {
      log(log_msw, "--- extending "+toString(word)+" with "+newPreferedSucc+"\n");
    }    
    
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
        if (!config.allowEventRepetitions || count > 2) continue;
        if (e == word[0]) continue; // never repeat first pre-chart event
      }
      
      short[] nextWord = Arrays.copyOf(word, word.length+1);
      nextWord[word.length] = e;
      
      if (showDebug(nextWord, preferedSucc)) {
        log(log_msw, "next word: "+toString(nextWord));
      }
      
      boolean[] stuck_here = new boolean[slog.originalNames.length];
      SimpleArrayList<SLogTreeNode[]> occ = tree.countOccurrences(nextWord, violators, stuck_here);
      if (stuck_at == null) {
        stuck_at = stuck_here;
      } else {
        //stuck_at.retainAll(stuck_here);
        for (int i=0; i<stuck_at.length;i++) {
          stuck_at[i] = stuck_at[i] && stuck_here[i];
        }
      }
      
      int occurrences_nextWord = tree.getTotalOccurrences(occ, OPTIONS_WEIGHTED_OCCURRENCE);
      
      if (occurrences_nextWord >= minSupThreshold) {
      
        // compute list of available events for continuation in the recursion
        short ev_avail[];
        if (config.allowEventRepetitions) {
          // re-use list of available events
          ev_avail = ev;
        } else {
          // event id 'e' no longer available for continuation,
          // copy list and remove 'e' from list
          ev_avail = removeFrom(ev, e);
        }
        
        //List<Short> preferedSuccNext = ev;
        Set<Short> violatingSuccessors = new HashSet<Short>();
        final short[] preferedSuccNext = getPreferredSuccessors(ev_avail, occ, violatingSuccessors);

        if (showDebug(nextWord, preferedSucc)) {
          log(log_msw, " IS IN "+occurrences_nextWord+" prefered succ: "+preferedSuccNext+" violating succ: "+violatingSuccessors+"\n");
        }
        
        SLogTreeNode node_nextWord = mineSupportedWords_optimized(tree, minSupThreshold, nextWord, ev_avail, preferedSuccNext, words, false);
        if (node_nextWord != null) node_nextWord.occurrences = occurrences_nextWord;
        
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
            short ignoreSuccessor = (j<newWord.length) ? nextWord[j+1] : -1; // remember the letter that immediately follows the violator
            while (j<newWord.length) {
              newWord[j] = nextWord[j+1];
              j++;
            }
            if (lessThan(newWord, largestWord_checked))
            {
              if (showDebug(newWord, preferedSucc)) {
                log(log_msw, "RETRY 1 "+toString(newWord) +" WITHOUT "+ignore+"\n");
              }
              mineSupportedWords_optimized(tree, minSupThreshold, newWord, ev_avail, preferedSuccNext, words, true);
            } else {
              
              if (!retryMap.containsKey(ignore)) retryMap.put(ignore, new boolean[slog.originalNames.length]);
              if (ignoreSuccessor != -1) {
                retryMap.get(ignore)[ignoreSuccessor] = true; // possible successor
              } else {
                // worst case: we don't know anything: try all
                for (short _event_avail : ev_avail)
                  retryMap.get(ignore)[_event_avail] = true;  
              }
            }
          }
        }
        // during recursion, we found that the current new letter becomes a violator
        // but we already collected all letters that could come after the current letter
        // extend 'word' with these new successors (skipping over the current letter)
        if (retryMap.containsKey(e)) {
          
          boolean[] retry = retryMap.get(e);
          int size = 0;
          for (int i=0; i<retry.length; i++) {
            if (retry[i] && i != e) size++; // all events but e
          }
          
          short[] preferedSuccNext_skip_over_e = new short[size];
          int pos=0;
          for (short i=0; i<retry.length; i++) {
            if (retry[i] && i != e) preferedSuccNext_skip_over_e[pos++] = i;
          }
          
          retryMap.remove(e); // we have handled these successors, remove from the map
          
          if (showDebug(word, preferedSuccNext_skip_over_e)) {
            log(log_msw, "RETRY 1b "+toString(word) +" WITH "+preferedSuccNext_skip_over_e+"\n");
          }
          mineSupportedWords_optimized(tree, minSupThreshold, word, ev_avail, preferedSuccNext_skip_over_e, words, true);
        }
        
      } else {
        
        if (showDebug(nextWord, preferedSucc)) {
          log(log_msw, " IS OUT "+occurrences_nextWord+" violators: "+violators+" stuck: "+stuck_at+"\n");
        }
        
        if (!Arrays.equals(newPreferedSucc,ev)) {
          if (showDebug(nextWord, preferedSucc)) {
            log(log_msw, "RETRY 2 "+toString(word) +" WITH ALL SUCCESSORS\n");
          }
          // the preferred successors turned out to be a bad choice, so retry with all successors
          mineSupportedWords_optimized(tree, minSupThreshold, word, ev, ev, words, false);
        }
      }
    } // for all successor events
        
        if (word.length > 1) {
          if (stuck_at != null) {
            // violators.addAll(stuck_at);
            for (int i=0;i<stuck_at.length;i++) stuck_at[i] = stuck_at[i] || violators[i];
          }
          for (short ignore=0; ignore<violators.length; ignore++) {
            // only consider the events that are violating some occurrence
            if (!violators[ignore]) continue;
            
            short[] newWord = removeFrom(word, ignore);
            short[] ev_red = removeFrom(ev, ignore);

            if (lessThan(newWord, largestWord_checked))
            {
              if (showDebug(newWord, preferedSucc)) {
                log(log_msw, "RETRY 3 "+toString(newWord) +" WITHOUT "+ignore+" from "+toString(word)+"\n");
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
      log(log_msw, "finish "+toString(word)+" succ: "+preferedSucc+"\n");
    }
    
    return n;
  }
  
  private void mineSupportedWords(MineBranchingTree tree, int minSupThreshold, final short word[], List<Short> ev, SLogTree words, boolean prepend_events) {

    words.addTrace(word);
    
    //if (prepend_events && word.length > 8) return;
    
    dot_count++;
    if (dot_count % 100 == 0) System.out.print(".");
    if (dot_count > 8000) { System.out.println(". "+words.nodes.size()+" "+toString(largestWord_checked)); dot_count = 0; }
    
    
    boolean visibleEvents[];
    if (config.skipEvents_invisible == false) {
      visibleEvents = new boolean[slog.originalNames.length];
      for (int e=0; e<word.length;e++) visibleEvents[word[e]] = true;
    } else {
      visibleEvents = null;
    }
    
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
      
      short[] nextWord;
      if (!prepend_events) {
        nextWord = Arrays.copyOf(word, word.length+1);
        nextWord[word.length] = e;
      } else {
        nextWord = new short[word.length+1];
        nextWord[0] = e;
        for (int i=0; i<word.length; i++) {
          nextWord[i+1] = word[i];
        }
      }
      
      
      SimpleArrayList<SLogTreeNode[]> occ = (config.skipEvents_invisible)
          ? tree.countOccurrences(nextWord, null, null) 
          : tree.countOccurrences(nextWord, visibleEvents, null, null);
      
      int total_occurrences = tree.getTotalOccurrences(occ, OPTIONS_WEIGHTED_OCCURRENCE);
      
      if (total_occurrences >= minSupThreshold) {
      
        mineSupportedWords(tree, minSupThreshold, nextWord, ev, words, prepend_events);

      }
    }
  }

  /**
   * Convert collection of Short objects to an array of shorts.
   * @param c
   * @return
   */
  private short[] toArray(Collection<Short> c) {
    short a[] = new short[c.size()];
    int i=0;
    for (Short s : c) a[i++] = s;
    return a;
  }
  
  /**
   * Remove all occurrences of element from the list.
   * 
   * @param list
   * @param element
   * @return a new array containing all elements except the given element
   * 
   */
  private short[] removeFrom(short[] list, short element) {
    int new_length = 0;
    for (int i=0; i<list.length; i++) {
      if (element != list[i]) new_length++;
    }
    
    short[] reduced_list = new short[new_length];
    int i_new = 0;
    for (int i_old=0; i_old<list.length; i_old++) {
      if (element != list[i_old]) reduced_list[i_new++] = list[i_old];
    }
    
    return reduced_list;
  }
  
  /**
   * Remove element from the list.
   * 
   * @param list
   * @param element
   * @return a new array containing all elements except the given element
   * 
   */
  private short[] removeFrom(short[] list, boolean[] removeIndex) {
    
    int new_length = 0;
    for (int i=0; i<list.length; i++) {
      if (!removeIndex[list[i]]) new_length++;
    }
    
    short[] reduced_list = new short[new_length];
    int i_new = 0;
    for (int i_old=0; i_old<list.length; i_old++) {
      if (!removeIndex[list[i_old]]) reduced_list[i_new++] = list[i_old];
    }
    
    return reduced_list;
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

  private static boolean showDebug(short[] word, short[] preferedSucc) {
    
    return false;

    /*
    short[] to_check = new short [] {28, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
     
    if (word.length < to_check.length) return false;
    
    for (int i=0; i<to_check.length; i++) {
      if (word[i] != to_check[i]) return false;
    }
    return true;
    */
    
    //return (word.length >= 6 && word[0] == 35 && word[1] == 36 && word[2] == 11 && word[3] == 12 && word[4] == 39 && word[5] == 40 );
    //return (word[0] == 85);
  }
  
  private boolean contains(short[] list, short element) {
    for (int i=0; i<list.length; i++) {
      if (list[i] == element) return true;
    }
    return false;
  }
  
  private void getVisibleSuccessors(short[] visibleEvents, SLogTreeNode n, Set<Short> visibleSuccessors, Set<Short> violatingSuccessors) {
    for (SLogTreeNode succ : n.post) {
      if (contains(visibleEvents, succ.id))
        visibleSuccessors.add(succ.id);
      else {
        violatingSuccessors.add(succ.id);
        getVisibleSuccessors(visibleEvents, succ, visibleSuccessors, violatingSuccessors);
      }
    }
  }
  
  private short[] getPreferredSuccessors(short[] allowedEvents, SimpleArrayList<SLogTreeNode[]> occ, Set<Short> violatingSuccessors) {
    HashSet<Short> allSuccessors = new HashSet<Short>();
    HashSet<Short> jointSuccessors = new HashSet<Short>();
    for (int i=0; i<allowedEvents.length; i++) jointSuccessors.add(allowedEvents[i]);
    
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
      short[] result = new short[jointSuccessors.size()];
      int i=0;
      for (Short s : jointSuccessors) {
        result[i++] = s;
      }
      Arrays.sort(result);
      return result;
    } else {
      // otherwise the word may continue with any event
      return allowedEvents;
    }
  }
  
  private static String tempDir = System.getProperty("java.io.tmpdir");
  private static String log_msw = tempDir+"/org.st.sam.mine_mineSupportedWords.log";

  public synchronized static void log_new(String logfile, String line) {
    try {
      writeFile(logfile, line, false);
    } catch (IOException e) {
      System.err.println(e);
    }
  }
  
  public synchronized static void log(String logfile, String line) {
    try {
      writeFile(logfile, line, true);
    } catch (IOException e) {
      System.err.println(e);
    }
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
}
