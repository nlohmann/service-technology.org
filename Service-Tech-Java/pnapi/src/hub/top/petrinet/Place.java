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

import java.util.LinkedList;
import java.util.List;

/**
 * A {@link PetriNet} place.
 * 
 * @author Dirk Fahland
 */
public class Place extends Node {
  
  private int tokens;

  /**
   * Create a new unmarked place
   * @param name
   */
  public Place(PetriNet net, String name) {
    super(net, name);
    setTokens(0);
  }

  /**
   * Mark this place with the given number of <code>tokens</code>.
   * @param tokens
   */
  public void setTokens(int tokens) {
    this.tokens = tokens;
  }

  /**
   * @return the number of tokens on this place
   */
  public int getTokens() {
    return tokens;
  }

  @Override
  public List<Transition> getPreSet() {
    LinkedList<Transition> preSet = new LinkedList<Transition>();
    for (Arc a : getIncoming())
      preSet.add((Transition)a.getSource());
    return preSet;
  }
  
  @Override
  public List<Transition> getPostSet() {
    LinkedList<Transition> postSet = new LinkedList<Transition>();
    for (Arc a : getOutgoing())
      postSet.add((Transition)a.getTarget());
    return postSet;
  }
  
  /*
   * (non-Javadoc)
   * @see hub.top.petrinet.Node#getUniqueIdentifier()
   */
  @Override
  public String getUniqueIdentifier() {
    return "p"+getID()+"_"+getName();
  }
}
