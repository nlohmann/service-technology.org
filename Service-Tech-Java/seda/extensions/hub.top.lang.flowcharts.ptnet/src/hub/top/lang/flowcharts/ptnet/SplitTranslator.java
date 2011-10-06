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
import hub.top.lang.flowcharts.SplitNode;

public class SplitTranslator extends Translator {

	public SplitNode getNode(){
		if (!(super.getNode() instanceof SplitNode))
			throw new NullPointerException("Error. Instantiated SplitTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return((SplitNode)super.getNode());
	}
	
	public void translate() {
		SplitNode node = this.getNode();
		
		PlaceExt pS = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); pS.setName("start_" + super.createLabel(node.getLabel(), 'p'));
		this.net.getPlaces().add(pS);
		List<PlaceExt> start = new Vector<PlaceExt>();
		List<PlaceExt> ends = new Vector<PlaceExt>();
		Hashtable<PlaceExt, DiagramNode> otherEnds = new Hashtable<PlaceExt, DiagramNode>();
		start.add(pS);
		
		// add a transition and an EndPlace for every outgoing Edge
		for(DiagramArc arc: node.getOutgoing()){
			
			TransitionExt t = PtnetLoLAFactory.eINSTANCE.createTransitionExt();
			t.setName(super.createLabel(arc.getLabel(), 't'));
			
			double p = 0;	// probability of path (if not set, all paths have the same probability)
			if (arc.getProbability() != 1) p = arc.getProbability();
			else p = 1 / node.getOutgoing().size();
			t.setProbability(p);
			
			this.net.getTransitions().add(t);
			
			ArcToTransitionExt a = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
			a.setSource(pS); a.setTarget(t);
			this.net.getArcs().add(a);
			
			PlaceExt pE = PtnetLoLAFactory.eINSTANCE.createPlaceExt();
			pE.setName("to_" + super.createLabel(arc.getTarget().getLabel(), 'p'));
			this.net.getPlaces().add(pE);

			ArcToPlaceExt a2 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
			a2.setSource(t); a2.setTarget(pE);
			this.net.getArcs().add(a2);
			
			ends.add(pE);
			otherEnds.put(pE, arc.getTarget());
		}
		
		this.setPair(new NodePair(start, ends));
		this.getPair().setOtherEnds(otherEnds);
			
		if (node.getIncoming().size() > 1){
			this.getPair().removeStart(pS);
			super.addInterfaceIn(pS);
		}
		else if (node.getIncoming().size() > 0) this.getPair().setOtherEnd(pS, node.getIncoming().get(0).getSrc());
			
		super.getTable().put(node, this.getPair());
	}
}
