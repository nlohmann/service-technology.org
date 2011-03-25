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

import com.google.gwt.dev.util.collect.HashSet;

/**
 * Partition a set of equivalent DNodes based on a finer equivalence criterion.
 * 
 * @author dfahland
 */
public interface IEquivalentNodesRefine {

  /**
   * Partition the set of nodes into several sets of equivalent nodes.
   * 
   * @param nodes
   * @return the partition into the finer equivalence classes; if all nodes are
   *         equivalent according to the finer equivalence criterion, then the
   *         returned collection must contain only one set that equals the
   *         original set of 'nodes'
   */
  public Collection<HashSet<DNode>> splitIntoEquivalenceClasses(
      HashSet<DNode> nodes);

}
