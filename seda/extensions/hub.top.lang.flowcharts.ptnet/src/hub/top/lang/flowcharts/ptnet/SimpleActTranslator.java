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
import hub.top.lang.flowcharts.SimpleActivity;

public class SimpleActTranslator extends ActivityTranslator {

	
	
	public SimpleActivity getNode(){
		if (!(super.getNode() instanceof SimpleActivity))
			throw new NullPointerException("Error. Instantiated SimpleActTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return((SimpleActivity)super.getNode());
	}
	
	
	public void translate() {
		SimpleActivity act = this.getNode();

			PlaceExt pS = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); pS.setName("start_" + super.createLabel(act.getLabel(), 'p'));
			this.net.getPlaces().add(pS);
			
			TransitionExt t0 = PtnetLoLAFactory.eINSTANCE.createTransitionExt(); t0.setName(super.createLabel(act.getLabel(), 't'));
			if (act.isOptional()) t0.setProbability(act.getProbability());
			else t0.setProbability(1);
			t0.setCost(act.getCost());
			t0.setMinTime(act.getMinTime());
			t0.setMaxTime(act.getMaxTime());
			this.net.getTransitions().add(t0);
				
			PlaceExt pE = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); pE.setName("end_" + super.createLabel(act.getLabel(), 'p'));
			this.net.getPlaces().add(pE);
				
			ArcToTransitionExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
			a1.setSource(pS); a1.setTarget(t0);
			this.net.getArcs().add(a1);
			
			ArcToPlaceExt a2 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
			a2.setSource(t0); a2.setTarget(pE);
			this.net.getArcs().add(a2);
			
			
			this.setPair(new NodePair(pS, pE));
			
			if (act.getIncoming().size() > 1){
				this.getPair().removeStart(pS);
				super.addInterfaceIn(pS);
			}
			else if (act.getIncoming().size() > 0) this.getPair().setOtherEnd(pS, act.getIncoming().get(0).getSrc());
				
			if (act.getOutgoing().size() > 1){
				this.getPair().removeEnd(pE);
				super.addInterfaceOut(pE);
			}
			else if (act.getOutgoing().size() > 0) this.getPair().setOtherEnd(pE, act.getOutgoing().get(0).getTarget());
				
			super.getTable().put(act, this.getPair());
			
			super.translate();
	}

}
