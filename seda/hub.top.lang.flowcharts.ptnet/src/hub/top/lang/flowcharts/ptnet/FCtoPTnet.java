/*****************************************************************************\
 * Copyright (c) 2009 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 * 
 * ServiceTechnolog.org - FlowChart Editors
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
 * The Original Code is this file as it was released on March 09, 2009.
 * The Initial Developer of the Original Code are
 * 		Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2009
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the EPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the EPL, the GPL or the LGPL.
\*****************************************************************************/

package hub.top.lang.flowcharts.ptnet;

import hub.top.editor.ptnetLoLA.ArcToPlaceExt;
import hub.top.editor.ptnetLoLA.ArcToTransitionExt;
import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.editor.ptnetLoLA.TransitionExt;
import hub.top.lang.flowcharts.ActivityNode;
import hub.top.lang.flowcharts.DiagramArc;
import hub.top.lang.flowcharts.DiagramNode;
import hub.top.lang.flowcharts.Endnode;
import hub.top.lang.flowcharts.Event;
import hub.top.lang.flowcharts.Flow;
import hub.top.lang.flowcharts.MergeNode;
import hub.top.lang.flowcharts.ResourceNode;
import hub.top.lang.flowcharts.SimpleActivity;
import hub.top.lang.flowcharts.SplitNode;
import hub.top.lang.flowcharts.StartNode;
import hub.top.lang.flowcharts.Subprocess;
import hub.top.lang.flowcharts.Workflow;
import hub.top.lang.flowcharts.WorkflowDiagram;
import hub.top.lang.flowcharts.ptnet.Translator.NodePair;

import java.util.Hashtable;

import org.eclipse.emf.ecore.EObject;

public class FCtoPTnet {

	// this is one workflow diagram of the model
	WorkflowDiagram wfDiagram = null;
	// this is the complete workflow model
	Workflow completeWorkflow = null;
	// this maps the workflow nodes to their PT-net expressions
	Hashtable<DiagramNode, NodePair> nodesToNets;
	// remember the resource nodes by name for later reuse
	Hashtable<String, NodePair> translatedResources;
	// the PT-Net
	PtNet net;
	
	public Hashtable<String, NodePair> getTranslatedResources() {
		return translatedResources;
	}


	public void setTranslatedResources(
			Hashtable<String, NodePair> translatedResources) {
		this.translatedResources = translatedResources;
	}


	public PtNet getNet() {
		return net;
	}


	public void setNet(PtNet net) {
		this.net = net;
	}


	/**
	 * returns the Mapping of DiagramNodes to their start- and endplaces in the PNML-Expression
	 */
	public Hashtable<DiagramNode, NodePair> getTable(){
		return this.nodesToNets;
	}
	
	
	/**
	 * sets the Mapping
	 * @param nodesToNets
	 */
	public void setTable(Hashtable<DiagramNode, NodePair> nodesToNets) {
		this.nodesToNets = nodesToNets;
	}


	public FCtoPTnet(EObject modelObject) {
		
		if (modelObject instanceof hub.top.lang.flowcharts.WorkflowDiagram) {
			wfDiagram = (WorkflowDiagram)modelObject;
			completeWorkflow = (Workflow)wfDiagram.eContainer();
		}
	}
	
	private void translateFC_toPTnet () {
		if (wfDiagram == null)
			return;
		
				
		this.nodesToNets = new Hashtable<DiagramNode, NodePair>();
		this.translatedResources = new Hashtable<String, NodePair>();
		Integer placeCount = 1;
		Integer transCount = 1;
		
		for (DiagramNode node : wfDiagram.getDiagramNodes()) {
			// translate all nodes of wfDiagram
			Translator trans;
  			
  			if (node instanceof StartNode)				trans = new StartNodeTranslator();
  			else if (node instanceof Endnode) 			trans = new EndNodeTranslator();
  			else if (node instanceof SimpleActivity)	trans = new SimpleActTranslator();
  			else if (node instanceof Event) 			trans = new EventTranslator();
  			else if (node instanceof ResourceNode)		trans = new ResourceTranslator();
  			else if (node instanceof SplitNode)			trans = new SplitTranslator();
  			else if (node instanceof MergeNode)			trans = new MergeTranslator();
  			else if (node instanceof Subprocess)		trans = new SubprocessTranslator();
  			else if (node instanceof Flow)				trans = new FlowTranslator();
  			else throw new NullPointerException("Error while translating Node " + node.getLabel() + ". Node has no known or yet implemented Type.");
  			
  			trans.setNet(this.getNet());
  			trans.setNode(node);
  			trans.setTable(this.getTable());
  			trans.setPct(placeCount);
  			trans.setTct(transCount);
  			if (trans instanceof ResourceTranslator) ((ResourceTranslator) trans).setTranslatedResources(this.translatedResources);
  			if (trans instanceof SubprocessTranslator) ((SubprocessTranslator) trans).setTranslatedResources(this.translatedResources);
  			if (trans instanceof StartNodeTranslator) ((StartNodeTranslator) trans).translate(true);
  			else trans.translate();
  			this.setTable(trans.getTable());
  			this.setNet(trans.getNet());
  			if (trans instanceof ResourceTranslator) this.setTranslatedResources(((ResourceTranslator) trans).getTranslatedResources());
  			if (trans instanceof SubprocessTranslator) this.setTranslatedResources(((SubprocessTranslator) trans).getTranslatedResources());
  			
  			placeCount = trans.getPct();
  			transCount = trans.getTct();
  			if (node instanceof ActivityNode && ((ActivityNode) node).isOptional()) this.insertOpt((ActivityNode) node);
  			
		}
		
		for (DiagramArc arc : wfDiagram.getDiagramArcs()) {
			// translate all arcs of wfDiagram
			
			DiagramNode src = arc.getSrc();
  			DiagramNode tgt = arc.getTarget();
  			
  			if (src == null || tgt == null) throw new NullPointerException("Error in FCtoPTnet while translating arc: arc has no source- or targetNode in Diagram!");
  			  			
  			PlaceExt rightArcStart=null;
  			PlaceExt rightArcEnd=null;
  			
  			// Arc from ResourceNode
  			if (src instanceof ResourceNode) {
  				// Resources are always only one Place --> ArcStart is the resourcePlace
  				rightArcStart = this.getTable().get(src).getEnd().get(0);
  				// ArcEnd is the Endplace of the other Node which is marked as other Endplace of the resource
  				rightArcEnd = this.getTable().get(tgt).getOtherEndPlace(src, true);
  			}
  			// Arc to ResourceNode
  			else if (tgt instanceof ResourceNode) {
  				// ArcStart is the startPlace which is marked as other Startplace of the resource 
  				rightArcStart = this.getTable().get(src).getOtherEndPlace(tgt, false);
  				// ArcEnd is the resourcePlace
  				rightArcEnd = this.getTable().get(tgt).getStart().get(0);
  				
  				// Arc with weight-Intervall
  				if (arc.getMaxWeight() != arc.getMinWeight()) {
  					double prob = 1/(arc.getMaxWeight() - arc.getMinWeight());
  					for (int i = arc.getMinWeight(); i <= arc.getMaxWeight(); i++){
  						TransitionExt t = PtnetLoLAFactory.eINSTANCE.createTransitionExt();
  						t.setName("weightedArc" + i);
  						this.getNet().getTransitions().add(t);
  						
  						ArcToTransitionExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
  						a1.setSource(rightArcStart); a1.setTarget(t);
  						a1.setProbability(prob);
  						this.getNet().getArcs().add(a1);
  						
  						ArcToPlaceExt a2 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
  						a2.setSource(t); a2.setTarget(rightArcEnd);
  						a2.setWeight(i);
  						this.getNet().getArcs().add(a2);
  					}
  					continue;
  				}
  			}
  			// other Arc
  			else {
  				if (this.getTable().get(tgt) != null){ 
  					rightArcEnd = this.getTable().get(tgt).getOtherEndPlace(src, true);
  					// if there is no entry in the Hashtable (@see Translator.NodePair.otherEnds), there can be only one Endplace
  					if (rightArcEnd == null) rightArcEnd = this.getTable().get(src).getEnd().get(0);  	  			
  				}
  			
  				if (this.getTable().get(src) != null) {
  					rightArcStart = this.getTable().get(src).getOtherEndPlace(tgt, false);
  					if (rightArcStart == null) rightArcStart = this.getTable().get(tgt).getStart().get(0);
  				}
  			}
			if (rightArcStart == null || rightArcEnd == null){
				System.out.println("Error in FCtoPTnet.translateFC_toPTnet() while translating arc: arc has no source- or targetPlace!");
  			}
  			
  			TransitionExt t1 = PtnetLoLAFactory.eINSTANCE.createTransitionExt();
  			if (arc.getSrc().getLabel() != null && arc.getTarget().getLabel() != null )
  				t1.setName(createLabel(arc.getSrc().getLabel()+"To"+arc.getTarget().getLabel()));
  			else t1.setName("arc_t" + transCount);
  			this.getNet().getTransitions().add(t1);
  			
  			ArcToTransitionExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
  			a1.setSource(rightArcStart); a1.setTarget(t1);
  			this.getNet().getArcs().add(a1);
  			
  			ArcToPlaceExt a2 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
  			a2.setSource(t1); a2.setTarget(rightArcEnd);
  			this.getNet().getArcs().add(a2);
		}
		
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
			//				first Startplace	outgoing arc		  target = transition outgoing arc target = Startplace
			pS = (PlaceExt) np.getStart().get(0).getOutgoing().get(0).getTarget().getOutgoing().get(0).getTarget();
		}
		else pS = np.getStart().get(0);
		
		if (np.getEnd().size() > 1){
			//			first Endplace	incoming arc			source = transition incoming arc	source = Endplace
			pE = (PlaceExt) np.getEnd().get(0).getIncoming().get(0).getSource().getIncoming().get(0).getSource();
		}
		else pE = np.getEnd().get(0);
		
		TransitionExt t = PtnetLoLAFactory.eINSTANCE.createTransitionExt(); t.setName(createLabel("no_execution"));
		t.setProbability(1-n.getProbability());
		this.getNet().getTransitions().add(t);
		
		ArcToTransitionExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
		a1.setSource(pS); a1.setTarget(t); 
		this.getNet().getArcs().add(a1);
		
		ArcToPlaceExt a2 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
		a2.setSource(t); a2.setTarget(pE);
		this.getNet().getArcs().add(a2);
	}
	
	/**
	 * creates an XML-conform label from a String 
	 * replaces therefore special characters by their HTML notation
	 * @param label - the diagramNode's label
	 * @return the PNML Label
	 */
	protected String createLabel (String label){
		if (label == null || label.length() == 0) return "";
		label = label.replaceAll("&", "und");
		label = label.replaceAll("<", "&lt;");
		label = label.replaceAll(">", "&gt;");
		label = label.replaceAll("'", "&apos;");
		label = label.replaceAll("\"", "&quot;");
		label = label.replaceAll("ß", "ss");
		label = label.replaceAll("Ä", "Ae");
		label = label.replaceAll("ä", "ae");
		label = label.replaceAll("Ö", "Oe");
		label = label.replaceAll("ö", "oe");
		label = label.replaceAll("Ü", "Ue");
		label = label.replaceAll("ü", "ue");
		return label;
	}
	
	public PtNet getModel() {
	  PtNet net = PtnetLoLAFactory.eINSTANCE.createPtNet();
	  this.setNet(net);
	  translateFC_toPTnet();
	  	  
	  return this.getNet();
	}
}
