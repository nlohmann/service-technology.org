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
 * A {@link PetriNet} transition.
 * 
 * @author Dirk Fahland
 */
public class Transition extends Node {

  public Transition(PetriNet net, String name) {
    super(net, name);
  }

  @Override
  public List<Place> getPreSet() {
    LinkedList<Place> preSet = new LinkedList<Place>();
    for (Arc a : getIncoming())
      preSet.add((Place)a.getSource()); 
    return preSet;
  }
  
  @Override
  public List<Place> getPostSet() {
    LinkedList<Place> postSet = new LinkedList<Place>();
    for (Arc a : getOutgoing())
      postSet.add((Place)a.getTarget());
    return postSet;
  }
  
  /*
   * (non-Javadoc)
   * @see hub.top.petrinet.Node#getUniqueIdentifier()
   */
  @Override
  public String getUniqueIdentifier() {
    return "t"+getID()+"_"+getName();
  }
}
