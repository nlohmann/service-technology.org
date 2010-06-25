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

import java.util.HashSet;
import java.util.List;

/**
 * 
 * A node of a {@link PetriNet}. Represents all joint properties of {@link Place}s
 * and {@link Transition}s.
 * 
 * @author Dirk Fahland
 */
public abstract class Node {
  
  private PetriNet  net;

  private String    name;
  public boolean    tau;
  
  public final int id;
  
  // the roles this node is associated to
  private HashSet<String> roles;
  
  // incoming arcs
  private HashSet<Arc> incoming;

  // outgoing arcs
  private HashSet<Arc> outgoing;

  
  /**
   * Create a new node.
   * 
   * @param name
   */
  public Node (PetriNet net, String name) {
    this.net = net;
    this.setName(name);
    this.roles = new HashSet<String>();
    this.id = net.getNextNodeID();
    
    this.incoming = new HashSet<Arc>();
    this.outgoing = new HashSet<Arc>();
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
   * @return this node's unique id
   */
  protected int getID() {
    return id;
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
  
  /**
   * set role of this node (and register the role at the node's net)
   * @param role
   */
  public void addRole(String role) {
    this.roles.add(role);
    this.net.addRole(role);
  }
  
  /**
   * @return the roles of this node
   */
  public HashSet<String> getRoles() {
    return this.roles;
  }

  /**
   * @return pre-nodes of this node
   */
  public abstract List<? extends Node> getPreSet();
  
  /**
   * @return post-nodes of this node
   */
  public abstract List<? extends Node> getPostSet();
  
  /**
   * Register arc as incoming or outgoing arc of this node, depending on
   * whether this node is source or target of the arc.
   *  
   * @param a
   */
  public void attachArc(Arc a) {
    if (a.getSource() == this) {
      outgoing.add(a); return;
    }
    if (a.getTarget() == this) {
      incoming.add(a); return;
    }
  }
  
  /**
   * Remove arc from incoming or outgoing arcs depending on whether this node
   * is source or target of the arc.
   *  
   * @param a
   */
  public void detachArc(Arc a) {
    if (a.getSource() == this) {
      outgoing.remove(a); return;
    }
    if (a.getTarget() == this) {
      incoming.remove(a); return;
    }
  }

  /**
   * @return incoming arcs of this node
   */
  public HashSet<Arc> getIncoming() {
    return incoming;
  }
  
  /**
   * @return outgoing arcs of this node
   */
  public HashSet<Arc> getOutgoing() {
    return outgoing;
  }

  /**
   * @return a unique identifier for this node
   */
  public String getUniqueIdentifier() {
    return "n"+id+"_"+getName();
  }
  
  /*
   * (non-Javadoc)
   * @see java.lang.Object#toString()
   */
  public String toString() {
    return getUniqueIdentifier();
  }
}
