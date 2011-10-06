/*****************************************************************************\
 * Copyright (c) 2009 Konstanze Swist, Dirk Fahland. EPL1.0/AGPL3.0
 * All rights reserved.
 * 
 * ServiceTechnolog.org - Modeling Languages
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License v1.0, which accompanies this
 * distribution, and is available at http://www.eclipse.org/legal/epl-v10.html
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 * 
 * The Original Code is this file as it was released on December 12, 2009.
 * The Initial Developer of the Original Code are
 *    Konstanze Swist
 *    Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2009
 * the Initial Developer. All Rights Reserved.
 * 
 * Contributor(s):
 * 
 * Alternatively, the contents of this file may be used under the terms of
 * the GNU Affero General Public License Version 3 or later (the "GPL") in
 * which case the provisions of the AGPL are applicable instead of those above.
 * If you wish to allow use of your version of this file only under the terms
 * of the AGPL and not to allow others to use your version of this file under
 * the terms of the EPL, indicate your decision by deleting the provisions
 * above and replace them with the notice and other provisions required by the 
 * AGPL. If you do not delete the provisions above, a recipient may use your
 * version of this file under the terms of any one of the EPL or the AGPL.
\*****************************************************************************/

package hub.top.lang.flowcharts.ptnet;

import hub.top.editor.ptnetLoLA.ArcToPlaceExt;
import hub.top.editor.ptnetLoLA.ArcToTransitionExt;
import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.editor.ptnetLoLA.TransitionExt;
import hub.top.lang.flowcharts.ActivityNode;
import hub.top.lang.flowcharts.DiagramNode;

public abstract class ActivityTranslator extends Translator {

  private ActivityNode activity;
  
  @Override
  public void setNode(DiagramNode node) {
    super.setNode(node);
    if (node instanceof ActivityNode)
      activity = (ActivityNode)node;
    else
      throw new NullPointerException("Error. Instantiated ActivityTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
  }
  
  @Override
  public void translate() {
    if (activity.isOptional())
      insertOpt(activity);
    
    super.translate();
  }
  
  /**
   * inserts a path where the activity n is not executed 
   * @param n an optional ActivityNode
   * @throws VoidRepositoryException 
   * @throws InvalidIDException 
   */
  private void insertOpt(ActivityNode n) {
    NodePair np = this.getTable().get(n);
    PlaceExt pS;
    PlaceExt pE;
    
    // if there are more than one Startplaces for n get the place after the interfaceIn
    if (np.getStart().size() > 1){
      //        first Startplace  outgoing arc      target = transition outgoing arc target = Startplace
      pS = (PlaceExt) np.getStart().get(0).getOutgoing().get(0).getTarget().getOutgoing().get(0).getTarget();
    }
    else pS = np.getStart().get(0);
    
    if (np.getEnd().size() > 1){
      //      first Endplace  incoming arc      source = transition incoming arc  source = Endplace
      pE = (PlaceExt) np.getEnd().get(0).getIncoming().get(0).getSource().getIncoming().get(0).getSource();
    }
    else pE = np.getEnd().get(0);
    
    TransitionExt t = PtnetLoLAFactory.eINSTANCE.createTransitionExt(); t.setName(super.createLabel("no_execution", 't'));
    // set probability of the "no execution" path
    t.setProbability(1-n.getProbability());
    this.getNet().getTransitions().add(t);
    
    ArcToTransitionExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
    a1.setSource(pS); a1.setTarget(t); 
    this.getNet().getArcs().add(a1);
    
    ArcToPlaceExt a2 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
    a2.setSource(t); a2.setTarget(pE);
    this.getNet().getArcs().add(a2);
  }
}
