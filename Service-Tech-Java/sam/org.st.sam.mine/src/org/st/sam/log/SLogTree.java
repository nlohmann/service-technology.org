package org.st.sam.log;

import com.google.gwt.dev.util.collect.HashMap;
import com.google.gwt.dev.util.collect.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.st.sam.mine.collect.SimpleArrayList;

public class SLogTree {
  
  public List<SLogTreeNode> nodes = new LinkedList<SLogTreeNode>();
  public List<SLogTreeNode> roots = new LinkedList<SLogTreeNode>();
  public Map<SLogTreeNode, Integer> nodeCount = new HashMap<SLogTreeNode, Integer>();
  
  private final boolean mergeTraces;
  
  private Set<SLogTreeNode> leafs = new HashSet<SLogTreeNode>();
  
  protected SLog slog;
  
  public SLogTree() {
    this.mergeTraces = true;
  }
  
  public SLogTree(SLog log, boolean mergeTraces) {
    this.mergeTraces = mergeTraces;
    this.slog = log;
    buildTree(log);
  }
  
  public SLogTreeNode contains(short[] trace) {
    SLogTreeNode current = null;
    for (int e=0; e<trace.length; e++) {
      
      short event = trace[e];
      
      // is this the first event of the trace?
      if (current == null) {
        for (SLogTreeNode r : roots) {
          if (r.id == event) {
            // found it, remember
            current = r;
            break;
          }
        }
        if (current == null) {
          // no root event for 'event'
          return null;
        }
        
      // this is a successor event, 'current' represents the predecessor event
      // in the tree
      } else {
        // see if a successor of 'current' represents 'event'
        boolean inTree = false;
        for (int e2 = 0; e2 < current.post.length; e2++) {
          if (current.post[e2].id == event) {
            // yes, remember it
            current = current.post[e2];
            inTree = true;
            break;
          }
        }
        if (!inTree) {
          return null;
        }
      }
    }
    return current;
  }
  
  public short[] getWord(SLogTreeNode n) {
    LinkedList<Short> path = new LinkedList<Short>();
    while (n != null) {
      path.addFirst(n.id);
      n = n.pre;
    }
    short[] result = new short[path.size()];
    for (int i=0; i<path.size(); i++) {
      result[i] = path.get(i);
    }
    return result;
  }
  
  public static class SupportedWord {
    public short[] word;
    public int support;
    public SupportedWord(short[] word, int support) {
      this.word = word;
      this.support = support;
    }
  }
  
  public SimpleArrayList<SupportedWord> getAllWords() {
    SimpleArrayList<SupportedWord> words = new SimpleArrayList<SupportedWord>();
    for (SLogTreeNode r : roots) {
      getAllWords(r, words);
    }
    return words;
  } 
  
  public void getAllWords(SLogTreeNode n, SimpleArrayList<SupportedWord> words) {
    words.add(new SupportedWord(getWord(n), n.occurrences));
    for (SLogTreeNode s : n.post) {
      getAllWords(s, words);
    }
  }
  
  public SLogTreeNode addTrace(short[] trace) {
    SLogTreeNode current = null;
    for (int e=0; e<trace.length; e++) {
      
      short event = trace[e];
      
      // is this the first event of the trace?
      if (current == null) {
        // yes, see if there is a root event in the tree
        if (mergeTraces) {
          for (SLogTreeNode r : roots) {
            if (r.id == event) {
              // found it, remember
              current = r;
              break;
            }
          }
        }
        if (current == null) {
          // no root event for 'event', create a new event
          current = new SLogTreeNode(event, null);
          roots.add(current);
          nodes.add(current);
          nodeCount.put(current, 0);
        }
        
      // this is a successor event, 'current' represents the predecessor event
      // in the tree
      } else {
        // see if a successor of 'current' represents 'event'
        boolean inTree = false;
        if (mergeTraces) {
          for (int e2 = 0; e2 < current.post.length; e2++) {
            if (current.post[e2].id == event) {
              // yes, remember it
              current = current.post[e2];
              inTree = true;
              break;
            }
          }
        }
        if (!inTree) {
          // new event
          SLogTreeNode newNode = new SLogTreeNode(event, current);
          current.append(newNode);
          nodes.add(newNode);
          current = newNode;
          nodeCount.put(current, 0);
        }
      }
      // the current node has been visited once more
      nodeCount.put(current, nodeCount.get(current)+1);
    }
    leafs.add(current);
    return current;
  }
  
  private void buildTree(SLog log) {
    for (int t=0; t<log.traces.length; t++) {
      addTrace(log.traces[t]);
    }
  }
  
  public static class TreeStatistics {
    public double maxOutDegree;
    public double averageOutDegree;
    public int depth;
    public int width;
    public int alphabetSize;
    public int size;
    
    @Override
    public String toString() {
      return "size: "+size+" out(max): "+maxOutDegree+" out(avg): "+averageOutDegree+" depth: "+depth+" width: "+width;
    }
  }
  
  /**
   * @return depth of the tree
   */
  private int getDepth() {
    LinkedList<SLogTreeNode> stack = new LinkedList<SLogTreeNode>();
    LinkedList<Integer> depths = new LinkedList<Integer>();
    for (SLogTreeNode n : roots) {
      stack.add(n);
      depths.add(0);
    }

    int maxDepth = 0;
    while (!stack.isEmpty()) {
      SLogTreeNode n = stack.pop();
      int d = depths.pop();
      maxDepth = (maxDepth < d) ? d : maxDepth;
      
      for (SLogTreeNode n2 : n.post) {
        stack.push(n2);
        depths.push(d+1);
      }
    }
    return maxDepth;
  }
  
  /**
   * @return width of the tree (= maximum number of nodes on the same level of three)
   */
  private int getWidth() {
    LinkedList<SLogTreeNode> queue_currentLevel = new LinkedList<SLogTreeNode>();
    boolean isLeafRow = false;
    int maxWidth = 0;
    
    // add roots to first level of the tree
    for (SLogTreeNode n : roots) queue_currentLevel.addLast(n);
    // if there are still successors
    while (!isLeafRow) {
      isLeafRow = true;
      
      int width = 0;
      LinkedList<SLogTreeNode> queue_nextLevel = new LinkedList<SLogTreeNode>();
      // iterate over the next level of the tree
      while (!queue_currentLevel.isEmpty()) {
        SLogTreeNode n = queue_currentLevel.removeFirst();
        // count width
        width++;
        // all successors are nodes of the next level, store them in q2
        for (SLogTreeNode n2 : n.post) {
          isLeafRow = false;
          queue_nextLevel.addLast(n2);
        }
      } // finished computing next level and current width
      
      if (maxWidth < width) maxWidth = width;
      queue_currentLevel = queue_nextLevel;
    }
    return maxWidth;
  }
  
  public TreeStatistics getStatistics() {
    
    TreeStatistics s = new TreeStatistics();
    
    HashSet<Short> alphabet = new HashSet<Short>();
    
    // get average and maximum out degree
    int arcs = 0;
    int maxArcs = 0;
    for (SLogTreeNode n : nodes) {
      arcs += n.post.length;
      maxArcs = (maxArcs < n.post.length) ? n.post.length : maxArcs;
      alphabet.add(n.id);
    }
    
    s.size = nodes.size();
    s.alphabetSize = alphabet.size();
    s.averageOutDegree = (double)arcs / (double)nodes.size();
    s.maxOutDegree = maxArcs;
    s.depth = getDepth();
    s.width = getWidth();
    
    return s;
  }
  
  public String toDot() {
    return toDot(new HashMap<Short, String>());
  }
  
  public String toDot(Map<Short, String> names) {
    StringBuilder sb = new StringBuilder();
    
    sb.append("digraph coverage_tree {\n");
    for (SLogTreeNode n : nodes) {
      if (!names.containsKey(n.id)) names.put(n.id, Short.toString(n.id));
      
      String label = n.id+" ("+nodeCount.get(n)+")";
      sb.append(n.globalID + " [ label=\""+label+"\" ];\n");
      for (SLogTreeNode n2 : n.post) {
        sb.append(n.globalID + " -> "+n2.globalID+";\n");
      }
    }
    sb.append("}\n");
    
    return sb.toString();
  }
}
