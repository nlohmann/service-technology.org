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
import hub.top.editor.ptnetLoLA.Transition;
import hub.top.editor.ptnetLoLA.TransitionExt;
import hub.top.lang.flowcharts.DiagramNode;
import hub.top.lang.flowcharts.Flow;

public class FlowTranslator extends Translator implements IBlockTranslator {

	public Flow getNode(){
		if (!(super.getNode() instanceof Flow))
			throw new NullPointerException("Error. Instantiated FlowTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return((Flow)super.getNode());
	}
	
	public void translate() {
		Flow fl = this.getNode();
		// startplace of the Flow
		PlaceExt pS = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); 
		pS.setName("START_" + super.createLabel(fl.getLabel(), 'p'));
		this.net.getPlaces().add(pS);
		
		TransitionExt tS = PtnetLoLAFactory.eINSTANCE.createTransitionExt(); 
		tS.setName("synchronizeStart_" + super.createLabel(fl.getLabel(), 't'));
		this.net.getTransitions().add(tS);
		this.setBlockEntry(tS);
		
		ArcToTransitionExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
		a1.setSource(pS); a1.setTarget(tS);
		this.net.getArcs().add(a1);
		
		// endplace of the Flow
		TransitionExt tE = PtnetLoLAFactory.eINSTANCE.createTransitionExt(); 
		tE.setName("synchronizeEnd_" + super.createLabel(fl.getLabel(), 't'));
		this.net.getTransitions().add(tE);
    this.setBlockExit(tE);
		
		PlaceExt pE = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); 
		pE.setName("end" + super.createLabel(fl.getLabel(), 'p'));
		this.net.getPlaces().add(pE);
		
		ArcToPlaceExt a2 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
		a2.setSource(tE); a2.setTarget(pE);
		this.net.getArcs().add(a2);
		
		this.setPair(new NodePair(pS, pE));
			
		if (fl.getIncoming().size() > 1){
			this.getPair().removeStart(pS);
			super.addInterfaceIn(pS);
		}
		else if (fl.getIncoming().size() > 0) this.getPair().setOtherEnd(pS, fl.getIncoming().get(0).getSrc());
			
		if (fl.getOutgoing().size() > 1){
			this.getPair().removeEnd(pE);
			super.addInterfaceOut(pE);
		}
		else if (fl.getOutgoing().size() > 0) this.getPair().setOtherEnd(pE, fl.getOutgoing().get(0).getTarget());
			
		super.getTable().put(fl, this.getPair() );
			
    // TODO: refactor to have a general recursion pattern only once
		// translate the Flow as if it was a page
		// translate all nodes
		for (DiagramNode node : fl.getChildNodes()){
		  this.translateChildNode(node);
		}
	}

  // implementations of IBlockTranslator
  private Transition tStart;
  private Transition tEnd;

  public Transition getBlockEntry() {
    return tStart;
  }

  public Transition getBlockExit() {
    return tEnd;
  }

  public void setBlockEntry(Transition start) {
    tStart = start;
  }

  public void setBlockExit(Transition end) {
    tEnd = end;
  }

}
