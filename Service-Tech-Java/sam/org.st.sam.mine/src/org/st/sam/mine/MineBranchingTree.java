package org.st.sam.mine;

import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.st.sam.log.SLog;
import org.st.sam.log.SLogTreeNode;

public class MineBranchingTree extends org.st.sam.log.SLogTree {

  public MineBranchingTree(SLog log) {
    super(log);
  }
  
  public void continuesWith(SLogTreeNode n, short word[], List<SLogTreeNode[]> occurrences) {
    Set<Short> visible = new HashSet<Short>();
    for (short e : word) visible.add(e);
    
    SLogTreeNode[] occurrence = new SLogTreeNode[word.length]; 
    continuesWith(n, word, 0, visible, occurrence, occurrences);
  }
  
  public void continuesWith(SLogTreeNode n, short word[], int pos, Set<Short> visible, SLogTreeNode[] occurrence, List<SLogTreeNode[]> occurrences) {
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
        continuesWith(n2, word, pos+nextLetter, visible, occurrence, occurrences);
      }
    }
    // this node not labeled with the current letter/no successor continues the word
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
  
  public float confidence(short[] pre, short[] main, boolean markTree) {
    int preMatch = 0;
    int mainMatch_pos = 0;
    int mainMatch_neg = 0;
    
    Set<Short> visible = new HashSet<Short>();
    for (short e : pre) visible.add(e);
    for (short e : main) visible.add(e);
    
    for (SLogTreeNode n : nodes) {
      if (n.id == pre[pre.length-1]) {
        SLogTreeNode pre_occurrence[] = new SLogTreeNode[pre.length];
        if (endsWith(n, pre, pre.length-1, visible, pre_occurrence)) {
          preMatch++;
          
          if (markTree) {
            for (SLogTreeNode n3 : pre_occurrence) {
              if (!preChartCoverage.containsKey(n3)) preChartCoverage.put(n3, 0);
              preChartCoverage.put(n3, preChartCoverage.get(n3)+1);
            }
          }
          
          boolean positive = false;
          for (SLogTreeNode n2 : n.post) {
            SLogTreeNode occurrence[] = new SLogTreeNode[main.length];
            LinkedList<SLogTreeNode[]> occurrences = new LinkedList<SLogTreeNode[]>();
            continuesWith(n2, main, 0, visible, occurrence, occurrences);
            
            if (occurrences.size() > 0) {
              positive = true;
              if (markTree) {
                for (SLogTreeNode[] witness : occurrences) {
                  for (SLogTreeNode n3 : witness) {
                    if (!mainChartCoverage.containsKey(n3)) mainChartCoverage.put(n3, 0);
                    mainChartCoverage.put(n3, mainChartCoverage.get(n3)+1);
                  }
                }
              }
            }
          }
          
          if (positive)
            mainMatch_pos++;
          else
            mainMatch_neg++;
        }
      }
    }
    return (float)(mainMatch_pos - mainMatch_neg)/(float)preMatch;
  }
  
  private Map<SLogTreeNode, Integer> preChartCoverage = new HashMap<SLogTreeNode, Integer>();
  private Map<SLogTreeNode, Integer> mainChartCoverage = new HashMap<SLogTreeNode, Integer>();
  
  public LinkedList<SLogTreeNode[]> countOccurrences(short word[]) {
    LinkedList<SLogTreeNode[]> occurrences = new LinkedList<SLogTreeNode[]>();
    
    for (SLogTreeNode n : nodes) {
      if (n.id == word[0]) {
        
        //System.out.println("  at "+n.globalID);
        continuesWith(n, word, occurrences);
        //System.out.println("  "+count);
      }
    }
    return occurrences;
  }
  
  @Override
  public String toDot() {
    StringBuilder sb = new StringBuilder();
    
    sb.append("digraph D {\n");
    for (SLogTreeNode n : nodes) {
      String nodeLabel = ""+n.id;
      String fillString = "";
      if (mainChartCoverage.containsKey(n) && !preChartCoverage.containsKey(n)) {
        nodeLabel += "\\n"+"|"+mainChartCoverage.get(n);
        fillString = "color=red style=filled";
      }
      if (!mainChartCoverage.containsKey(n) && preChartCoverage.containsKey(n)) {
        nodeLabel += "\\n"+preChartCoverage.get(n)+"|";
        fillString = "color=lightblue style=filled";
      }
      if (mainChartCoverage.containsKey(n) && preChartCoverage.containsKey(n)) {
        nodeLabel += "\\n"+preChartCoverage.get(n)+"|"+mainChartCoverage.get(n);
        fillString = "color=violet style=filled";
      }
      
      sb.append(n.globalID + " [ label=\""+nodeLabel+"\" "+fillString+" ];\n");
      for (SLogTreeNode n2 : n.post) {
        sb.append(n.globalID + " -> "+n2.globalID+";\n");
      }
    }
    sb.append("}\n");
    
    return sb.toString();
  }
  
}
