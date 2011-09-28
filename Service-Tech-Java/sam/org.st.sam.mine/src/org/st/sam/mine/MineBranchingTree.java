package org.st.sam.mine;

import java.util.Arrays;
import com.google.gwt.dev.util.collect.HashMap;
import com.google.gwt.dev.util.collect.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.st.sam.log.SLog;
import org.st.sam.log.SLogTreeNode;
import org.st.sam.log.SScenario;

public class MineBranchingTree extends org.st.sam.log.SLogTree {

  public MineBranchingTree(SLog log, boolean mergeTraces) {
    super(log, mergeTraces);
  }
  
  public void continuesWith(SLogTreeNode n, short word[], List<SLogTreeNode[]> occurrences, List<SLogTreeNode[]> partialOccurrences, Set<Short> violators, Set<Short> stuck_at) {
    Set<Short> visible = new HashSet<Short>();
    for (short e : word) visible.add(e);
    
    SLogTreeNode[] occurrence = new SLogTreeNode[word.length]; 
    continuesWith(n, word, 0, visible, occurrence, occurrences, partialOccurrences, violators, stuck_at);
  }
  
  public void continuesWith(SLogTreeNode n, short word[], int pos, Set<Short> visible, SLogTreeNode[] occurrence, List<SLogTreeNode[]> occurrences, List<SLogTreeNode[]> partialOccurrences, Set<Short> violators, Set<Short> stuck_at) {
    // no more letters to check: word found
    if (n.id == word[pos] && pos == word.length-1) {
      occurrence[pos] = n;
      occurrences.add(Arrays.copyOf(occurrence, occurrence.length));
      return;
    }

    // this node is labeled with the current letter or with an invisible letter
    if (n.id == word[pos] || !visible.contains(n.id)) {
      // find the successors that continues the word
      for (SLogTreeNode n2 : n.post) {
        // if the current node is labeled with the current event, move to the next event
        // otherwise search for the current event at the successors
        int nextLetter = (n.id == word[pos]) ? 1 : 0;
        if (n.id == word[pos]) occurrence[pos] = n;
        continuesWith(n2, word, pos+nextLetter, visible, occurrence, occurrences, partialOccurrences, violators, stuck_at);
      }
    } else if (visible.contains(n.id)) {
      // a visible letter, occurring in wrong order -> no continuation
      if (violators != null) {
        if (pos > 0)
          registerViolators(word, occurrence[pos-1], violators);
        else
          registerViolators(word, n, violators);
      }
      
      if (stuck_at != null) {
        stuck_at.add(word[pos]);
      }
      
      return;
    }

    if ((n.post == null || n.post.length == 0) && partialOccurrences != null) {
      // partial occurrence ends at a leaf, remember this partial occurrence
      partialOccurrences.add(Arrays.copyOf(occurrence, pos));
    }
  }
  
  private void registerViolators(short[] word, SLogTreeNode endNode, Set<Short> violators) {
    for (SLogTreeNode v : endNode.post) {
      // remember all successor ids that occur in the word, i.e. prevented an occurrence 
      for (short w : word) {
        if (w == v.id) {
          violators.add(v.id);
          break;
        }
      }
    }
  }
        
  public boolean endsWith(SLogTreeNode n, short word[], int pos, Set<Short> visible, SLogTreeNode[] occurrence) {
    // no more letters to check: word found
    if (n.id == word[pos] && pos == 0) {
      occurrence[pos] = n;
      return true;
    }

    // this node is labeled with the current letter or with an invisible letter
    if (n.id == word[pos] || !visible.contains(n.id)) {
      // find the predecessors that continues the word
      SLogTreeNode n2 = n.pre;
      if (n2 == null) {
        // reached root of the tree: word not found
        return false;
      }
      
      // if the current node is labeled with the current event, move to the next event
      // otherwise seach for the current event at the successors
      int nextLetter = (n.id == word[pos]) ? -1 : 0;
      if (n.id == word[pos]) occurrence[pos] = n;
      return endsWith(n2, word, pos+nextLetter, visible, occurrence);
    }
    // this node not labeled with the current letter/no successor continues the word
    return false;
    
  }
  
  public void clearCoverageMarking() {
    preChartCoverage.clear();
    preChartCoverage_fail.clear();
    mainChartCoverage.clear();
    mainChartCoverage_partial.clear();
  }
  
  public int support(short[] word) {
    List<SLogTreeNode[]> occ = countOccurrences(word, null, null);
    return occ.size();
  }
  
  public int support(SScenario s) {
    short[] word = new short[s.pre.length+s.main.length];
    for (int e=0; e<s.pre.length; e++) {
      word[e] = s.pre[e];
    }
    for (int e=0; e<s.main.length; e++) {
      word[e+s.pre.length] = s.main[e];
    }
    return support(word);
  }
  
  public double confidence(short[] pre, short[] main, boolean markTree) {
    int preMatch = 0;
    int mainMatch_pos = 0;
    int mainMatch_neg = 0;
    int mainMatch_weak_neg = 0;
    
    Set<Short> visible = new HashSet<Short>();
    for (short e : pre) visible.add(e);
    for (short e : main) visible.add(e);
    
    for (SLogTreeNode n : nodes) {
      if (n.id == pre[pre.length-1]) {
        SLogTreeNode pre_occurrence[] = new SLogTreeNode[pre.length];
        if (endsWith(n, pre, pre.length-1, visible, pre_occurrence)) {
          
          //preMatch += nodeCount.get(n);
          preMatch++;
          
          if (markTree) {
            for (SLogTreeNode n3 : pre_occurrence) {
              if (!preChartCoverage.containsKey(n3)) preChartCoverage.put(n3, 0);
              preChartCoverage.put(n3, preChartCoverage.get(n3)+nodeCount.get(n3));
            }
          }
          
          boolean positive = false;
          boolean weakNegative = false;
          for (SLogTreeNode n2 : n.post) {
            SLogTreeNode occurrence[] = new SLogTreeNode[main.length];
            
            // to store complete occurrences of the main-chart
            LinkedList<SLogTreeNode[]> occurrences = new LinkedList<SLogTreeNode[]>();
            // to store partial occurrences of the main-chart that could be completed
            // (when a trace was extended at a leaf)
            LinkedList<SLogTreeNode[]> partialOccurrences = new LinkedList<SLogTreeNode[]>();
            
            // get continuations with main-chart
            continuesWith(n2, main, 0, visible, occurrence, occurrences, partialOccurrences, null, null);
            
            if (occurrences.size() > 0) {
              positive = true;
              if (markTree) {
                for (SLogTreeNode[] witness : occurrences) {
                  for (SLogTreeNode n3 : witness) {
                    if (!mainChartCoverage.containsKey(n3)) mainChartCoverage.put(n3, 0);
                    mainChartCoverage.put(n3, mainChartCoverage.get(n3)+nodeCount.get(n3));
                  }
                }
              }
              
              //int total_occurrences = 0;
              //for (SLogTreeNode[] o : occurrences) {
              //  // total number of occurrences = number of different occurrences * number of traces having this occurrence until the end of the word
              //  total_occurrences += nodeCount.get(o[o.length-1]);
              //}
              //mainMatch_pos += total_occurrences;
              mainMatch_pos++;
              
            } /*
              else if (partialOccurrences.size() > 0) {
              weakNegative = true;
              if (markTree) {
                for (SLogTreeNode[] witness : partialOccurrences) {
                  for (SLogTreeNode n3 : witness) {
                    if (!mainChartCoverage_partial.containsKey(n3)) mainChartCoverage_partial.put(n3, 0);
                    mainChartCoverage_partial.put(n3, mainChartCoverage_partial.get(n3)+nodeCount.get(n3));
                  }
                }
              }
              
              int total_occurrences = 0;
              for (SLogTreeNode[] o : partialOccurrences) {
                if (o.length != 0) {
                  // total number of occurrences = number of different occurrences * number of traces having this occurrence until the end of the word
                  total_occurrences += nodeCount.get(o[o.length-1]);
                } else {
                  total_occurrences += nodeCount.get(n);
                }
              }
              mainMatch_weak_neg += total_occurrences;
              
            } */ else {
              // pre-chart not followed by main-chart. mark this specifically
              if (markTree) {
                for (SLogTreeNode n3 : pre_occurrence) {
                  if (!preChartCoverage_fail.containsKey(n3)) preChartCoverage_fail.put(n3, 0);
                  preChartCoverage_fail.put(n3, preChartCoverage_fail.get(n3)+nodeCount.get(n3));
                }
              }
            }
          }
          
          if (!positive && !weakNegative) {
            //mainMatch_neg += nodeCount.get(n);
            mainMatch_neg++;
          }
        }
      }
    }
    //System.out.println(mainMatch_pos+" + "+mainMatch_weak_neg+" - "+mainMatch_neg+" / "+preMatch);
    //return (double)(mainMatch_pos + mainMatch_weak_neg - mainMatch_neg)/(double)preMatch;
    //System.out.println(mainMatch_pos+" + "+mainMatch_weak_neg+" / "+preMatch);
    
    return (double)(mainMatch_pos + mainMatch_weak_neg)/(double)preMatch;
  }
  
  private Map<SLogTreeNode, Integer> preChartCoverage = new HashMap<SLogTreeNode, Integer>();
  private Map<SLogTreeNode, Integer> mainChartCoverage = new HashMap<SLogTreeNode, Integer>();
  private Map<SLogTreeNode, Integer> preChartCoverage_fail = new HashMap<SLogTreeNode, Integer>();
  private Map<SLogTreeNode, Integer> mainChartCoverage_partial = new HashMap<SLogTreeNode, Integer>();
  
  public LinkedList<SLogTreeNode[]> countOccurrences(short word[], Set<Short> violators, Set<Short> stuck_at) {
    LinkedList<SLogTreeNode[]> occurrences = new LinkedList<SLogTreeNode[]>();
    
    for (SLogTreeNode n : nodes) {
      if (n.id == word[0]) {
        
        //System.out.println("  at "+n.globalID);
        continuesWith(n, word, occurrences, null, violators, stuck_at);
        //System.out.println("  "+count);
        
      }
    }
    
    return occurrences;
  }

  @Override
  public String toDot(Map<Short, String> names) {
    StringBuilder sb = new StringBuilder();
    
    sb.append("digraph D {\n");
    for (SLogTreeNode n : nodes) {
      if (!names.containsKey(n.id)) names.put(n.id, Short.toString(n.id));
      
      String nodeLabel = names.get(n.id)+" ["+n.id+"]";
      String fillColor = null;
      
      if (preChartCoverage.containsKey(n) || mainChartCoverage.containsKey(n) || mainChartCoverage_partial.containsKey(n)) {
        String l1 = preChartCoverage.containsKey(n) ? preChartCoverage.get(n).toString() : "";
        String l2 = mainChartCoverage.containsKey(n) ? mainChartCoverage.get(n).toString() : "";
        String l3 = mainChartCoverage_partial.containsKey(n) ? "+("+mainChartCoverage_partial.get(n).toString()+")" : "";
        nodeLabel += "\\n "+l1+"|"+l2+l3;
      }
      
      if (!preChartCoverage.containsKey(n)) {
        if (mainChartCoverage.containsKey(n))
          fillColor = "red";
        else if (mainChartCoverage_partial.containsKey(n))
          fillColor = "salmon";
      } else {
        if (!mainChartCoverage.containsKey(n) && !mainChartCoverage_partial.containsKey(n)) {
          if (preChartCoverage_fail.containsKey(n))
            fillColor = "orange";
          else
            fillColor = "lightblue";
        } else if (mainChartCoverage.containsKey(n) || preChartCoverage.containsKey(n)) {
          fillColor = "violet";
        }
      }

      String fillString = "";
      if (fillColor != null) fillString += "color="+fillColor+" style=filled";
      
      sb.append(n.globalID + " [ label=\""+nodeLabel+"\" "+fillString+" ];\n");
      for (SLogTreeNode n2 : n.post) {
        sb.append(n.globalID + " -> "+n2.globalID+";\n");
      }
    }
    sb.append("}\n");
    
    return sb.toString();
  }
  
}
