/*****************************************************************************\
 * Copyright (c) 2008, 2009, 2010. Dirk Fahland. AGPL3.0
 * All rights reserved. 
 * 
 * ServiceTechnology.org - Uma, an Unfolding-based Model Analyzer
 * 
 * This program and the accompanying materials are made available under
 * the terms of the GNU Affero General Public License Version 3 or later,
 * which accompanies this distribution, and is available at 
 * http://www.gnu.org/licenses/agpl.txt
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 * 
\*****************************************************************************/

package hub.top.uma.synthesis;

import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.Uma;

import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;

import com.google.gwt.dev.util.collect.HashSet;

/**
 * Implementation of {@link IEquivalentConditions} to partition an equivalence class
 * of nodes into finer equivalence classes. Two nodes are equivalent iff they have
 * equivalent successor sets.
 * 
 * @author dfahland
 */
public class SuccessorEquivalence implements IEquivalentConditions {
  
  private DNodeBP bp;
  
  /**
   * The branching process constructor within which we want to refine equivalence
   * classes of nodes based on their successors. The equivalence criterion decides
   * equivalence of successors using {@link DNodeBP#getElementary_ccPair()}.
   * 
   * @param bp
   */
  public SuccessorEquivalence(DNodeBP bp) {
    this.bp = bp;
  }
  
  /*
   * (non-Javadoc)
   * @see hub.top.uma.synthesis.IEquivalentConditions#splitIntoEquivalenceClasses(java.util.Collection)
   */
  public Collection<HashSet<DNode>> splitIntoEquivalenceClasses(
      HashSet<DNode> nodes)
  {
    // do not split sets of events
    if (!nodes.isEmpty() && nodes.iterator().next().isEvent) {
      HashSet<HashSet<DNode>> result = new HashSet<HashSet<DNode>>();
      result.add(nodes);
      return result;
    }
    
    HashMap<DNode[], HashSet<DNode>> succ = new HashMap<DNode[], HashSet<DNode>>();
    
    Uma.out.print("splitting: "+nodes);
    
    // partition the set of nodes in this equivalence class based on their
    // successors: two nodes go into the same partition if their sets of successors
    // are equivalent
    for (DNode d : nodes) {
      // compute the set of successors of 'd' (i.e. their canonical representatives)
      DNode s[] = getSuccessorEquivalence(d);
      
      boolean found = false;
      for (DNode[] s2 : succ.keySet()) {  // get all successor sets stored in 'succ'
        if (Arrays.equals(s, s2)) {       // 's' is equivalent to one of those sets
          succ.get(s2).add(d);            // node 'd' has the same successors as 's2'
          found = true;
        }
      }
      if (!found) {
        // the successors of 'd' have not been stored yet: new partition
        succ.put(s, new HashSet<DNode>());
        succ.get(s).add(d);
      }
    }
    
    Uma.out.print(" --> ");
    for (HashSet<DNode> part : succ.values()) {
      Uma.out.print(part+" | ");
    }
    Uma.out.println();
    
    return succ.values(); // each partition becomes a separate equivalence class
  }
  
  /**
   * @param d
   * @return a vector of the the canonical representatives of the
   *         successors of 'd'
   */
  private DNode[] getSuccessorEquivalence(DNode d) {
    DNode[] s = new DNode[d.post.length];
    for (int i=0; i<d.post.length; i++) {
      s[i] = bp.getElementary_ccPair().get(d.post[i]);
    }
    return s;
  }
}


