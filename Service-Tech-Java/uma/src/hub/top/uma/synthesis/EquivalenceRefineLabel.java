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

import java.util.Collection;
import java.util.HashMap;

import com.google.gwt.dev.util.collect.HashSet;

/**
 * Implementation of {@link IEquivalentNodesRefine} to partition an equivalence class
 * of nodes into finer equivalence classes. Two nodes are equivalent iff they have
 * the same label.
 * 
 * @author dfahland
 */
public class EquivalenceRefineLabel implements IEquivalentNodesRefine {

  public static final EquivalenceRefineLabel instance = new EquivalenceRefineLabel();
  
  /*
   * (non-Javadoc)
   * @see hub.top.uma.synthesis.IEquivalentConditions#splitIntoEquivalenceClasses(java.util.Collection)
   */
  public Collection<HashSet<DNode>> splitIntoEquivalenceClasses(HashSet<DNode> nodes) {
    
    HashMap<Short, HashSet<DNode>> labelClasses = new HashMap<Short, HashSet<DNode>>();
    for (DNode d : nodes) {
      if (!labelClasses.containsKey(d.id))
        labelClasses.put(d.id, new HashSet<DNode>());
      labelClasses.get(d.id).add(d);
    }
    
    if (labelClasses.keySet().size() > 1) {
      // there are several nodes with different labels in the same equivalence class
      // we cannot fold them to the same node, so split the class
      System.out.println("splitting equivalence class "+nodes+" by label");
    }
    
    return labelClasses.values();
  }
}
