package org.st.sam.log;

import java.io.IOException;
import java.util.Map;
import java.util.Set;

import org.deckfour.xes.model.XLog;

import com.google.gwt.dev.util.collect.HashSet;

public class Log_ChoiceConsistency {
  
  public Log_ChoiceConsistency(String logFile) throws IOException {
    loadLog(logFile);
  }

  public Log_ChoiceConsistency(SLogTree tree) throws IOException {
    setSLog(tree.slog);
    setTree(tree);
  }
  
  // ------------ resulting measures -------------------
  public double min_consistency_low[];
  public double min_consistency_high[];
  
  public int min_support_low[];
  public int min_support_high[];
  
  public Map<Short,Double>[] continuation_consistency;
  public Map<Short,Integer>[] continuation_support;
  

  // -------------------- log handling ---------------------------------------
  
  private void loadLog(String logFile) throws IOException {
    loadXLog(logFile);
    XLog xlog = getXLog();
    setSLog(new SLog(xlog));
    setTree(new SLogTree(getSLog(), true));
  }
  
  private XLog xlog;
  private SLog slog;
  private SLogTree tree;
  
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
  
  public SLogTree getTree() {
    return tree;
  }


  public void setTree(SLogTree tree) {
    this.tree = tree;
  }

  public void getChoiceConsistency() {
    
    int[] choiceMin = new int[slog.originalNames.length];
    int[] choiceMax = new int[slog.originalNames.length];
    double[] choiceAverage = new double[slog.originalNames.length];
    int[] choiceCount = new int[slog.originalNames.length];
    Map<Short,Integer>[] all_continuations = new Map[slog.originalNames.length];
    Set<Short>[] continuations_intersect = new Set[slog.originalNames.length];
    
    continuation_support = new Map[slog.originalNames.length];
    
    for (short e=0; e<slog.originalNames.length; e++) {
      choiceMin[e] = Integer.MAX_VALUE;
      choiceMax[e] = Integer.MIN_VALUE;
      choiceAverage[e] = 0;
      choiceCount[e] = 0;
      
      all_continuations[e] = null;
      continuation_support[e] = null;
      continuations_intersect[e] = null;
    }
    
    for (SLogTreeNode n : getTree().nodes) {
      if (n.post == null) continue;
      
      Map<Short,Integer> n_successors_confidence = new com.google.gwt.dev.util.collect.HashMap<Short,Integer>();
      Map<Short,Integer> n_successors_support = new com.google.gwt.dev.util.collect.HashMap<Short,Integer>();
      for (SLogTreeNode succ : n.post) {
        // this choice for 'n2.id' is available as often as n occurs
        n_successors_confidence.put(succ.id, getTree().nodeCount.get(n));
        n_successors_support.put(succ.id, getTree().nodeCount.get(succ));
      }
      
      choiceMin[n.id] = (n_successors_confidence.size() < choiceMin[n.id]) ? n_successors_confidence.size() : choiceMin[n.id];
      choiceMax[n.id] = (n_successors_confidence.size() > choiceMax[n.id]) ? n_successors_confidence.size() : choiceMax[n.id];
      choiceCount[n.id] += tree.nodeCount.get(n);
      choiceAverage[n.id] = (n_successors_confidence.size()*tree.nodeCount.get(n));
      
      if (continuations_intersect[n.id] == null) continuations_intersect[n.id] = new HashSet<Short>(n_successors_confidence.keySet());
      else continuations_intersect[n.id].retainAll(n_successors_confidence.keySet());
      
      if (all_continuations[n.id] == null) {
        all_continuations[n.id] = n_successors_confidence;
        continuation_support[n.id] = n_successors_support;
      } else {
        for (short f : n_successors_confidence.keySet()) {
          if (!all_continuations[n.id].containsKey(f)) all_continuations[n.id].put(f, 0);
          all_continuations[n.id].put(f, all_continuations[n.id].get(f) + n_successors_confidence.get(f));
          
          if (!continuation_support[n.id].containsKey(f)) continuation_support[n.id].put(f, 0);
          continuation_support[n.id].put(f, continuation_support[n.id].get(f) + n_successors_support.get(f));
        }
      }
    }

    int maxBranch = 0;
    

    continuation_consistency = new Map[slog.originalNames.length];
    
    for (short e=0; e<slog.originalNames.length; e++) {
      if (all_continuations[e] == null || all_continuations[e].keySet().size() <= 1) {
        
      } else {
        choiceAverage[e] = choiceAverage[e]/choiceCount[e];
        continuation_consistency[e] = new com.google.gwt.dev.util.collect.HashMap<Short, Double>();
        
        double e_minConsistency = Double.MAX_VALUE;
        
        
        //System.out.println("choice event: "+e+" ("+slog.originalNames[e]+")");
        
        for (short f : all_continuations[e].keySet()) {
          
          double consistency = (double)all_continuations[e].get(f) / (double)choiceCount[e];
          continuation_consistency[e].put(f, consistency);
          
          if (consistency < e_minConsistency) {
            e_minConsistency = consistency;
          }
        }
        
        if (all_continuations[e].size() > maxBranch) maxBranch = all_continuations[e].size();
        
        System.out.println("event "+e+"  ("+slog.originalNames[e]+") requires");
        System.out.println("confidence: "+continuation_consistency[e]);
        System.out.println("support: "+continuation_support[e]);
        System.out.println("guarantees "+ continuations_intersect[e]);
      }
      
    }
    

    min_consistency_low = new double[maxBranch];
    min_consistency_high = new double[maxBranch];
    min_support_low = new int[maxBranch];
    min_support_high = new int[maxBranch];
    
    for (int b=0; b<maxBranch; b++) {
      min_consistency_low[b] = Double.MAX_VALUE;
      min_consistency_high[b] = Double.MIN_VALUE;
      
      min_support_low[b] = Integer.MAX_VALUE;
      min_support_high[b] = Integer.MIN_VALUE;
    }
    
    for (short e=0; e<slog.originalNames.length; e++) {
      if (continuation_consistency[e] == null) continue;
      
      Set<Short> seen = new HashSet<Short>();
      
      int _b = 0;
      while (seen.size() < continuation_consistency[e].size()) {
        
        short largest_remaining = -1;
        double largest_remaining_cons = Double.MIN_VALUE;
        int largest_remaining_supp = Integer.MIN_VALUE;
        
        for (short f : continuation_consistency[e].keySet()) {
          if (seen.contains(f)) continue;
          
          double cons = continuation_consistency[e].get(f);
          int supp = continuation_support[e].get(f);
          if (   largest_remaining_cons < cons
              || (largest_remaining_cons ==  cons && largest_remaining_supp < supp))
          {
            largest_remaining_cons = cons;
            largest_remaining = f;
            largest_remaining_supp = supp;
          }
        }
        
        if (min_consistency_low[_b] >= largest_remaining_cons) {
          min_consistency_low[_b] = largest_remaining_cons;
          
          if (min_support_low[_b] > continuation_support[e].get(largest_remaining)) {
            min_support_low[_b] = continuation_support[e].get(largest_remaining);
          }
        }
        if (min_consistency_high[_b] <= largest_remaining_cons) {
          min_consistency_high[_b] = largest_remaining_cons;
          
          if (min_support_high[_b] < continuation_support[e].get(largest_remaining)) {
            min_support_high[_b] = continuation_support[e].get(largest_remaining);
          } 
        }
   
        seen.add(largest_remaining);
        _b++;
      }
    }
    
    for (int b=0; b<maxBranch; b++) {
      min_consistency_low[b] = (double)((int)(min_consistency_low[b]*100))/100;
      min_consistency_high[b] = (double)((int)(min_consistency_high[b]*100))/100;
    }
    
    System.out.println("consistency profile: all = "+toString(min_consistency_low)+" one = "+toString(min_consistency_high));
    System.out.println("support profile: all = "+toString(min_support_low)+" one = "+toString(min_support_high));
  }

  public static String toString(double []d) {
    if (d == null) return "null";
    String ret = "";
    for (double _d : d) {
      if (ret.length() == 0) ret += "[";
      else ret += ", ";
      ret += _d;
    }
    return ret+"]";
  }
  
  public static String toString(int []d) {
    if (d == null) return "null";
    String ret = "";
    for (int _d : d) {
      if (ret.length() == 0) ret += "[";
      else ret += ", ";
      ret += _d;
    }
    return ret+"]";
  }
  
  public static void main(String[] args) throws IOException {
    
    if (args.length != 1) {
      System.out.println("error. wrong number of arguments. required:");
      System.out.println("   <logfile.xes.gz>");
      return;
    }
    
    Log_ChoiceConsistency consistency = new Log_ChoiceConsistency(args[0]);
    consistency.getChoiceConsistency();
    //System.out.println(consistency.getChoiceConsistency());
  }
}
