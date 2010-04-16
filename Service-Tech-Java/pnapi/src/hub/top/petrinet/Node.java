/*****************************************************************************\
 * Copyright (c) 2010. All rights reserved. Dirk Fahland. AGPL3.0
 * 
 * ServiceTechnology.org - Petri Net API/Java
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

package hub.top.petrinet;

/**
 * 
 * A node of a {@link PetriNet}. Represents all joint properties of {@link Place}s
 * and {@link Transition}s.
 * 
 * @author Dirk Fahland
 */
public class Node {

  private String name;
  
  public boolean tau;
  
  /**
   * Create a new node.
   * 
   * @param name
   */
  public Node (String name) {
    this.setName(name);
  }

  /**
   * Update the name of the node. Must not be called by the user. Please use
   * {@link PetriNet#setName(Node, String)} to change the name of a node.
   * 
   * @param name
   */
  public void setName(String name) {
    this.name = name;
  }

  /**
   * @return the node's name
   */
  public String getName() {
    return name;
  }

  /**
   * @return iff node is invisble
   */
  public boolean isTau() {
    return tau;
  }

  /**
   * set node invisible
   * @param tau
   */
  public void setTau(boolean tau) {
    this.tau = tau;
  }


}
