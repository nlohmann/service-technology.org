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
 * Represents an arc of a {@link PetriNet}.
 * 
 * @author Dirk Fahland
 */
public class Arc {

  private Node source;
  private Node target;

  /**
   * Create a new arc with arc weight 1
   * @param src
   * @param tgt
   */
  public Arc(Node src, Node tgt) {
    source = src;
    target = tgt;
  }

  /**
   * @return source node of this arc
   */
  public Node getSource() {
    return source;
  }

  /**
   * @return target node of this arc
   */
  public Node getTarget() {
    return target;
  }
  
  @Override
  public String toString() {
    return getSource()+" -> "+getTarget();
  }
}
