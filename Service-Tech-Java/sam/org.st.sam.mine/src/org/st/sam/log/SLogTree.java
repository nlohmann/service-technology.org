package org.st.sam.log;

import java.util.LinkedList;
import java.util.List;

public class SLogTree {
  
  public List<SLogTreeNode> nodes = new LinkedList<SLogTreeNode>();
  public List<SLogTreeNode> roots = new LinkedList<SLogTreeNode>();
  
  public SLogTree(SLog log) {
    buildTree(log);
  }
  
  private void buildTree(SLog log) {
    for (int t=0; t<log.traces.length; t++) {
      
      SLogTreeNode current = null;
      for (int e=0; e<log.traces[t].length; e++) {
        
        short event = log.traces[t][e];
        
        // is this the first event of the trace?
        if (current == null) {
          // yes, see if there is a root event in the tree
          for (SLogTreeNode r : roots) {
            if (r.id == event) {
              // found it, remember
              current = r;
              break;
            }
          }
          if (current == null) {
            // no root event for 'event', create a new event
            current = new SLogTreeNode(event, null);
            roots.add(current);
            nodes.add(current);
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
            // new event
            SLogTreeNode newNode = new SLogTreeNode(event, current);
            current.append(newNode);
            nodes.add(newNode);
            current = newNode;
          }
        }
      }
    }
  }
  
  public String toDot() {
    StringBuilder sb = new StringBuilder();
    
    sb.append("digraph D {\n");
    for (SLogTreeNode n : nodes) {
      sb.append(n.globalID + " [ label=\""+n.id+"\" ];\n");
      for (SLogTreeNode n2 : n.post) {
        sb.append(n.globalID + " -> "+n2.globalID+";\n");
      }
    }
    sb.append("}\n");
    
    return sb.toString();
  }
}
