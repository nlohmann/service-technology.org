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
      c.triggers = new LinkedList<short[]>();
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
  
  public MineLSC(Configuration config) {
    this(config, null);
  }

  public MineLSC(Configuration config, SLogTree supportedWords) {
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
    setSLog(new SLog(xlog));
    
    boolean mergeTraces = (config.mode == Configuration.MODE_BRANCHING) ? true : false;
    
    long time_start_candiate_words = System.currentTimeMillis();
    
    tree = new MineBranchingTree(getSLog(), mergeTraces);
    
    String stat = tree.getStatistics().toString();
    System.out.println("tree statistics: "+stat);
    
    List<short[]> supportedWords = getSupportedWords(tree, minSupportThreshold);
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
    discoverScenariosFromWords(supportedWords, confidence, scenarios);
      
    long time_end_scenario_discovery = System.currentTimeMillis();
    
    time_scenario_discovery = time_end_scenario_discovery-time_start_scenario_discovery;
   
    System.out.println("reduced to "+lscs.size()+" scenarios");
    System.out.println("tree statistics: "+stat);
  }
  
  private void discoverScenariosFromWords(List<short[]> supportedWords, double confidence, ArrayList<SScenario> scenarios) {
    
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

    
    int done = 0;
    supportedWords.clear();
    
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
              
              SScenario s = new SScenario(pre, main_cand);
              
              LSC l_test = slog.toLSC(s, 0, 0);
              if (!l_test.isConnected()) {
                if (showDebug(cand, null)) {
                  System.out.println("check "+toString(pre)+" "+toString(main_cand)+" is not connected");
                }

                Set<Integer> unConnected = l_test.getUnConnectedEvents();
                if (main_cand.length > unConnected.size())
                {
                  // skip unconnected events
                  short[] newMain = new short[main_cand.length-unConnected.size()];
                  int i=0,j=0;
                  while (j<newMain.length) {
                    if (!unConnected.contains(i+pre.length)) {
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
                
                SimpleArrayList<SLogTreeNode[]> occ = tree.countOccurrences(cand, null, null);
                int total_occurrences = getTotalOccurrences(occ);
                
                LSC l = slog.toLSC(s, total_occurrences, c);
                
                if (l.isConnected()) {
                  if (showDebug(cand, null)) {
                    System.out.println("  is connected");
                  }
                  boolean s_weaker = false;
                  List<SScenario> toRemove = new LinkedList<SScenario>();
                  for (SScenario s2 : scenarios) {
                    if (s.weakerThan(s2) || implies(s2, s)
                        && l.getSupport() <= s2l.get(s2).getSupport() 
                        //&& l.getConfidence() <= s2l.get(s2).getConfidence()
                        )
                    {
                      if (showDebug(cand, null)) {
                        System.out.println("  subsubmed by "+s2);
                      }
                      s_weaker = true;
                      break;
                    }
                    if (s2.weakerThan(s) || implies(s, s2)
                        && l.getSupport() >= s2l.get(s2).getSupport() 
                        //&& l.getConfidence() >= s2l.get(s2).getConfidence()
                        )
                    {
                      if (showDebug(cand, null)) {
                        System.out.println("  subsubmes "+s2);
                      }
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
                  } else {
                    
                  }
                  break;  
                  // we found a scenario for this candidate word. any other scenario
                  // will only contain longer pre-charts and Shorter main charts
                  // approximative optimization: stop exploring other scenarios
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
      
    for (SScenario s : scenarios) {
      System.out.println(s);
    }
    
    supportedWordsClasses.clear();
  }
  
  public boolean implies(SScenario s1, SScenario s2) {
    if (s1.implies(s2) && tree.support(s1) >= tree.support(s2))
      return true;
    else
      return false;
  }
  
  public int getTotalOccurrences(SimpleArrayList<SLogTreeNode[]> occ) {
    
    if (OPTIONS_WEIGHTED_OCCURRENCE) {
      int total_occurrences = 0;
      for (SLogTreeNode[] o : occ) {
        // total number of occurrences = number of different occurrences * number of traces having this occurrence until the end of the word
        total_occurrences += tree.nodeCount.get(o[o.length-1]);
      }
      return total_occurrences;
    } else {
      return occ.size();
    }
  }
  
  public Map<Short, String> getShortenedNames() {
    
    Map<Short, String> ShortenedNames = new HashMap<Short, String>();
    for (Short i=0; i<getSLog().originalNames.length; i++) {
      LSCEvent e = getSLog().toLSCEvent(i);
      ShortenedNames.put(i, SAMOutput.shortenNames(e.getMethod()));
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
    
    log_new(log_msw, "");
    
    List<Short> singleEvents = new LinkedList<Short>();
    
    supportedWords = new SLogTree();
    int maxEventId = getSLog().originalNames.length;
    for (int e=0; e<maxEventId; e++) {
      
      //if (e < 11 || (e >= 16 && e <=26)) continue; 
      
      SimpleArrayList<SLogTreeNode[]> occ = tree.countOccurrences(new short[] { (short)e }, null, null);

      int total_occurrences = getTotalOccurrences(occ);

      System.out.println("found "+e+" "+occ.size()+" times amounting to "+total_occurrences+" occurrences in the log // "+getSLog().originalNames[e]);
      if (total_occurrences >= minSupThreshold) {
        singleEvents.add((short)e);
      }
    }
    
    // copy all events to an array
    short[] allEvents = toArray(singleEvents);

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
        mineSupportedWords_optimized(tree, minSupThreshold, new short[] { e }, remainingEvents, remainingEvents, supportedWords, false);
      else
        mineSupportedWords(tree, minSupThreshold, new short[] { e }, singleEvents, supportedWords);
      System.out.println(supportedWords.nodes.size());
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
  
  private HashMap<SLogTreeNode, Set<Short>> checkSuccessors = new HashMap<SLogTreeNode, Set<Short>>();
  private HashMap<Short,HashSet<Short>> retryMap = new HashMap<Short, HashSet<Short>>(); 
  
  private void mineSupportedWords_optimized(MineBranchingTree tree, int minSupThreshold, final short word[], final short[] ev, final short[] preferedSucc, SLogTree words, boolean check) {

    short[] newPreferedSucc = preferedSucc;
    SLogTreeNode leaf = words.contains(word);
    if (leaf != null && checkSuccessors.containsKey(leaf)) {
      Set<Short> checkedSuccessors = checkSuccessors.get(leaf);
      // remove all successors to explore that have already been explored
      newPreferedSucc = removeFrom(newPreferedSucc, checkedSuccessors);
      // if there is none left, we are done
      if (newPreferedSucc.length == 0) {
        if (showDebug(word, preferedSucc)) {
          log(log_msw, "already seen "+toString(word)+" "+checkSuccessors.get(leaf)+"\n");
        }
        return;
      }
    }
    
    if (largestWord_checked == null) largestWord_checked = word;
    else if (lessThan(largestWord_checked, word)) largestWord_checked = word;
    
    SLogTreeNode n = words.addTrace(word);
    if (!checkSuccessors.containsKey(n)) checkSuccessors.put(n, new HashSet<Short>());
    for (int i=0; i<newPreferedSucc.length; i++)
      checkSuccessors.get(n).add(newPreferedSucc[i]);
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
      
      int total_occurrences = getTotalOccurrences(occ);
      
      if (total_occurrences >= minSupThreshold) {
      
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
          log(log_msw, " IS IN "+total_occurrences+" prefered succ: "+preferedSuccNext+" violating succ: "+violatingSuccessors+"\n");
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
              
              if (!retryMap.containsKey(ignore)) retryMap.put(ignore, new HashSet<Short>());
              if (ignoreSuccessor != -1) {
                retryMap.get(ignore).add(ignoreSuccessor); // possible successor
              } else {
                // worst case: we don't know anything: try all
                for (short _event_avail : ev_avail)
                  retryMap.get(ignore).add(_event_avail);  
              }
            }
          }
        }
        // during recursion, we found that the current new letter becomes a violator
        // but we already collected all letters that could come after the current letter
        // extend 'word' with these new successors (skipping over the current letter)
        if (retryMap.containsKey(e)) {
          short[] preferedSuccNext_skip_over_e = toArray(retryMap.get(e));
          retryMap.remove(e); // we have handled these successors, remove from the map
          
          Arrays.sort(preferedSuccNext_skip_over_e);
          
          if (showDebug(word, preferedSuccNext_skip_over_e)) {
            log(log_msw, "RETRY 1b "+toString(word) +" WITH "+preferedSuccNext_skip_over_e+"\n");
          }
          mineSupportedWords_optimized(tree, minSupThreshold, word, ev_avail, preferedSuccNext_skip_over_e, words, true);
        }
        
      } else {
        
        if (showDebug(nextWord, preferedSucc)) {
          log(log_msw, " IS OUT "+total_occurrences+" violators: "+violators+" stuck: "+stuck_at+"\n");
        }
        
        if (newPreferedSucc != ev) {
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
            for (int i=0;i<stuck_at.length;i++) {
              if (stuck_at[i]) violators[i] = true;
            }
          }
          for (short ignore=0; ignore<violators.length; ignore++) {
            // only consider the events that are violating some occurrence
            if (!violators[ignore]) continue;
            
            
            int v_count = 0;
            for (short v : word) {
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
  }
  
  private void mineSupportedWords(MineBranchingTree tree, int minSupThreshold, final short word[], List<Short> ev, SLogTree words) {

    words.addTrace(word);
    
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
      
      short[] nextWord = Arrays.copyOf(word, word.length+1);
      nextWord[word.length] = e;
      
      SimpleArrayList<SLogTreeNode[]> occ = (config.skipEvents_invisible)
          ? tree.countOccurrences(nextWord, null, null) 
          : tree.countOccurrences(nextWord, visibleEvents, null, null);
      
      int total_occurrences = getTotalOccurrences(occ);
      
      if (total_occurrences >= minSupThreshold) {
      
        mineSupportedWords(tree, minSupThreshold, nextWord, ev, words);

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
  private short[] removeFrom(short[] list, Set<Short> elements) {
    
    int new_length = 0;
    for (int i=0; i<list.length; i++) {
      if (!elements.contains(list[i])) new_length++;
    }
    
    short[] reduced_list = new short[new_length];
    int i_new = 0;
    for (int i_old=0; i_old<list.length; i_old++) {
      if (!elements.contains(list[i_old])) reduced_list[i_new++] = list[i_old];
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
    short[] to_check = new short [] {1,4,5,14,15,30,31,41,42,43,44,45,46};
    
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
