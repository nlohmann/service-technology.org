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
import hub.top.lang.flowcharts.ActivityNode;
import hub.top.lang.flowcharts.DiagramArc;
import hub.top.lang.flowcharts.DiagramNode;
import hub.top.lang.flowcharts.Subprocess;
import hub.top.lang.flowcharts.WorkflowDiagram;

public class SubprocessTranslator extends Translator implements IBlockTranslator  {
	
	public Subprocess getNode(){
		if (!(super.getNode() instanceof Subprocess))
			throw new NullPointerException("Error. Instantiated SubprocessTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return((Subprocess)super.getNode());
	}
	
	public void translate() {
		Subprocess sub = this.getNode();
		
		// Start
		PlaceExt pS = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); pS.setName("start_" + super.createLabel(sub.getLabel(), 'p'));
		this.net.getPlaces().add(pS);
		
		// End
		PlaceExt pE = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); pE.setName("end_" + super.createLabel(sub.getLabel(), 'p'));
		this.net.getPlaces().add(pE);
		
		//WorkflowDiagram subProc = sub.getSubprocess();
		
		// weighted subprocess --> subprocess will be executed several times parallel
		if (sub.getWeight() > 1){
			// a transition to synchronize the begin of all instances
			TransitionExt t1 = PtnetLoLAFactory.eINSTANCE.createTransitionExt();
			t1.setName("weightedSub_" + super.createLabel(sub.getLabel(), 't'));
			this.getNet().getTransitions().add(t1);
			
			ArcToTransitionExt aS = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
			aS.setSource(pS); aS.setTarget(t1);
			this.getNet().getArcs().add(aS);
			
			// a transition to synchronize the end of all instances
			TransitionExt t2 = PtnetLoLAFactory.eINSTANCE.createTransitionExt();
			t2.setName("weightedSub_end_" + super.createLabel(null, 't'));
			this.getNet().getTransitions().add(t2);
			
			ArcToPlaceExt aE = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
			aE.setSource(t2); aE.setTarget(pE);
			this.getNet().getArcs().add(aE);
			
			for (int i=1; i<=sub.getWeight(); i++){
				// a startplace for each instance
				PlaceExt p1 = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); 
				p1.setName("inst"+ i + "_" + super.createLabel(null, 'p'));
				this.getNet().getPlaces().add(p1);
				
				ArcToPlaceExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
				a1.setSource(t1); a1.setTarget(p1);
				this.getNet().getArcs().add(a1);
				
				// an endplace for each instance
				PlaceExt p2 = PtnetLoLAFactory.eINSTANCE.createPlaceExt();
				p2.setName("inst"+ i + "_" + super.createLabel(null, 'p'));
				this.getNet().getPlaces().add(p2);
				
				ArcToTransitionExt a2 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
				a2.setSource(p2); a2.setTarget(t2);
				this.getNet().getArcs().add(a2);
				
				this.createProcess(p1, p2);
			}
		}
		else this.createProcess(pS, pE);		
		
		this.setPair(new NodePair(pS, pE));		
			
		// more than one incoming arc?
		if (sub.getIncoming().size() > 1){
			this.getPair().removeStart(pS);
			super.addInterfaceIn(pS);
		}
		else if (sub.getIncoming().size() > 0) this.getPair().setOtherEnd(pS, sub.getIncoming().get(0).getSrc());
			
		// more than one outgoing arc?
		if (sub.getOutgoing().size() > 1){
			this.getPair().removeEnd(pE);
			super.addInterfaceOut(pE);
		}
		else if (sub.getOutgoing().size() > 0) this.getPair().setOtherEnd(pE, sub.getOutgoing().get(0).getTarget());
			
		super.getTable().put(sub, this.getPair());
		
		super.translate();
	}
	
	private void createProcess(PlaceExt pS, PlaceExt pE){
		
		Subprocess sub = this.getNode();
		WorkflowDiagram subProc = sub.getSubprocess();
		// no subprocess or subprocess empty --> insert dummy
		if (subProc == null || subProc.getDiagramNodes().isEmpty()){
			TransitionExt t = PtnetLoLAFactory.eINSTANCE.createTransitionExt(); 
			t.setName(super.createLabel(sub.getLabel(), 't'));
			
			if (sub.isOptional()) t.setProbability(sub.getProbability());
			else t.setProbability(1);
			
			t.setCost(sub.getCost());
			t.setMinTime(sub.getMinTime());
			t.setMaxTime(sub.getMaxTime());
			this.net.getTransitions().add(t);
			
			ArcToTransitionExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
			a1.setSource(pS); a1.setTarget(t);
			this.net.getArcs().add(a1);
			
			ArcToPlaceExt a2 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
			a2.setSource(t); a2.setTarget(pE);
			this.net.getArcs().add(a2);
		}
		// translate subprocess
		else {
			// Start
			TransitionExt tS = PtnetLoLAFactory.eINSTANCE.createTransitionExt(); 
			tS.setName("start_" + super.createLabel(subProc.getName(), 't'));
			if (sub.isOptional()) tS.setProbability(sub.getProbability());
			
			// cost and time
			int c = 0;
			int mint = 0;
			int maxt = 0;
			for (DiagramNode a : subProc.getDiagramNodes()){
				if (a instanceof ActivityNode){
					c += ((ActivityNode) a).getCost();
					mint += ((ActivityNode) a).getMinTime();
					maxt += ((ActivityNode) a).getMaxTime();
				}
			}
			this.net.getTransitions().add(tS);
      this.setBlockEntry(tS);
			
			ArcToTransitionExt aS = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
			aS.setSource(pS); aS.setTarget(tS);
			this.net.getArcs().add(aS);
			
			// End with restcost and -time
			TransitionExt tE = PtnetLoLAFactory.eINSTANCE.createTransitionExt(); 
			tE.setName("end_" + super.createLabel(subProc.getName(), 't'));
			c = sub.getCost()-c;
			if (c < 0) c = 0;
			mint = sub.getMinTime()-mint;
			if (mint < 0) mint = 0;
			maxt = sub.getMaxTime()-maxt;
			if (maxt < 0) maxt = 0;
			tE.setCost(c);
			tE.setMinTime(mint);
			tE.setMaxTime(maxt);
			this.net.getTransitions().add(tE);
			this.setBlockExit(tE);
			
			ArcToPlaceExt aE = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
			aE.setSource(tE); aE.setTarget(pE);
			this.net.getArcs().add(aE);
			
			for (DiagramNode node : subProc.getDiagramNodes()){
			  this.translateChildNode(node);
			}
			
			// translate all arcs of Subprocess
  		for (DiagramArc arc : subProc.getDiagramArcs()) {
  			this.translateChildArc(arc);
  		}
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
