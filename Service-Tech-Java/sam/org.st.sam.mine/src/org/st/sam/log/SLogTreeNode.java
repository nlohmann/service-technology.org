package org.st.sam.log;

import java.util.Arrays;

public class SLogTreeNode {
  
  public static int globalIDgen = 0;
  
  public final SLogTreeNode pre;
  public SLogTreeNode post[];
  
  public final short id;
  public final int globalID;

  public SLogTreeNode(short id, SLogTreeNode pre) {
    this.id = id;
    this.pre = pre;
    this.post = new SLogTreeNode[0];
    this.globalID = globalIDgen++;
  }
  
  public void append(SLogTreeNode other) {
    for (int i=0; i<post.length; i++) {
      if (post[i] == other) return;
    }
    post = Arrays.copyOf(post, post.length+1);
    post[post.length-1] = other;
  }
}
