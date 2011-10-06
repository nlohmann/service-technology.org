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

import java.util.Hashtable;
import java.util.List;
import java.util.Vector;


import hub.top.editor.ptnetLoLA.ArcToPlaceExt;
import hub.top.editor.ptnetLoLA.ArcToTransitionExt;
import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.editor.ptnetLoLA.TransitionExt;
import hub.top.lang.flowcharts.DiagramArc;
import hub.top.lang.flowcharts.DiagramNode;
import hub.top.lang.flowcharts.MergeNode;

public class MergeTranslator extends Translator {
	
	public MergeNode getNode(){
		if (!(super.getNode() instanceof MergeNode))
			throw new NullPointerException("Error. Instantiated MergeTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return((MergeNode)super.getNode());
	}
	
	public void translate(){
		MergeNode node = this.getNode();
		
		// translate EndNode first
		PlaceExt pE = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); 
		pE.setName("end" + super.createLabel(node.getLabel(), 'p'));
		this.net.getPlaces().add(pE);
		
		List<PlaceExt> starts = new Vector<PlaceExt>();
		List<PlaceExt> end = new Vector<PlaceExt>();
		Hashtable<PlaceExt, DiagramNode> otherEnds = new Hashtable<PlaceExt, DiagramNode>();
		end.add(pE);
			
		// add a startPlace and a Transition with Arc to the EndPlace for each incoming Edge
		for (DiagramArc arc: node.getIncoming()){
			PlaceExt pIn = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); 
			if (node.getLabel() != null && arc.getSrc().getLabel() != null)
				pIn.setName(super.createLabel(node.getLabel()+"_start_"+arc.getSrc().getLabel(), 'p'));
			else pIn.setName(super.createLabel(null, 'p'));
			this.net.getPlaces().add(pIn);
			
			TransitionExt t = PtnetLoLAFactory.eINSTANCE.createTransitionExt();
			t.setName(super.createLabel(arc.getLabel(), 't'));
			this.net.getTransitions().add(t);
			
			ArcToTransitionExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
			a1.setSource(pIn); a1.setTarget(t);
			this.net.getArcs().add(a1);

			ArcToPlaceExt a2 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
			a2.setSource(t); a2.setTarget(pE);
			this.net.getArcs().add(a2);

			starts.add(pIn);
			otherEnds.put(pIn, arc.getSrc());
		}
			
		this.setPair(new NodePair(starts, end));
		this.getPair().setOtherEnds(otherEnds);
		if (node.getOutgoing().size() > 1){
			this.getPair().removeEnd(pE);
			super.addInterfaceOut(pE);
		}
		else if (node.getOutgoing().size() > 0) this.getPair().setOtherEnd(pE, node.getOutgoing().get(0).getTarget());
		super.getTable().put(node, this.getPair());
			
	}
}
