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
import hub.top.editor.ptnetLoLA.Node;
import hub.top.editor.ptnetLoLA.PNAPI;
import hub.top.editor.ptnetLoLA.Place;
import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.editor.ptnetLoLA.TransitionExt;
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

import java.util.HashMap;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.List;
import java.util.Vector;

public abstract class Translator {
	DiagramNode  node;
	NodePair     pair;
	PtNet        net;
	Hashtable<DiagramNode, NodePair> table;
	int      placeNum;
	int      transitionNum;
	
	Translator   parentTranslator = null;
	
	// mapping to keep track of the merge results
	HashMap<Place, Place>  mergeMap;
	
	public Translator() {
	  mergedPlaces = new HashSet<Place>();
	  mergeMap = new HashMap<Place, Place>();
  }
	
  // remember the resource nodes by name for later reuse
  Hashtable<String, NodePair> translatedResources;
  
  public Hashtable<String, NodePair> getTranslatedResources() {
    return translatedResources;
  }

  public void setTranslatedResources(
      Hashtable<String, NodePair> translatedResources) {
    this.translatedResources = translatedResources;
  }
	
	public int getPlaceNum() {
		return placeNum;
	}

	public void setPlaceNum(int pNum) {
		this.placeNum = pNum;
	}

	public int getTransitionNum() {
		return transitionNum;
	}

	public void setTransitionNum(int tNum) {
		this.transitionNum = tNum;
	}

	public NodePair getPair() {
		return pair;
	}

	public PtNet getNet() {
		return net;
	}

	public void setNet(PtNet net) {
		this.net = net;
	}

	public void setPair(NodePair pair) {
		this.pair = pair;
	}

	public Hashtable<DiagramNode, NodePair> getTable() {
		return table;
	}

	public void setTable(Hashtable<DiagramNode, NodePair> table) {
		this.table = table;
	}

	public void setNode(DiagramNode node){
		this.node= node;
	}
	
	public DiagramNode getNode(){
		return(this.node);
	}
	
	public void translate() {
    removeAllMergedPlaces();
	}
	
	public void translateChildNode(DiagramNode node) {
    Translator trans;

    if (node instanceof StartNode) {
      if (node.getLabel() != null && node.getLabel().equals("then"))
        trans = new MidNodeTranslator();
      else
        trans = new StartNodeTranslator();
    }
    else if (node instanceof Endnode)        trans = new EndNodeTranslator();
    else if (node instanceof SimpleActivity) trans = new SimpleActTranslator();
    else if (node instanceof Event)          trans = new EventTranslator();
    else if (node instanceof ResourceNode)   trans = new ResourceTranslator();
    else if (node instanceof SplitNode)      trans = new SplitTranslator();
    else if (node instanceof MergeNode)      trans = new MergeTranslator();
    else if (node instanceof Subprocess)     trans = new SubprocessTranslator();
    else if (node instanceof Flow)           trans = new FlowTranslator();
    else throw new NullPointerException("Error while translating Node " + node.getLabel() + ". Node has no known or yet implemented Type.");

    trans.setNet(this.getNet());
    trans.setNode(node);
    trans.setTable(this.getTable());
    trans.setPlaceNum(this.getPlaceNum());
    trans.setTransitionNum(this.getTransitionNum());
    trans.setTranslatedResources(this.getTranslatedResources());
    trans.parentTranslator = this;
    trans.mergeMap = this.mergeMap;
    
    trans.translate();
    
    this.setTable(trans.getTable());
    this.setNet(trans.getNet());
    this.setTranslatedResources(trans.getTranslatedResources());
    this.setPlaceNum(trans.getPlaceNum());
    this.setTransitionNum(trans.getTransitionNum());
	}
	
  /**
   * Translate a {@link DiagramArc} to a Petri net to connect the arc's source
   * and the arc's target {@link DiagramNode} also in the resulting Petri net.
   * The method identifies the corresponding Petri net patterns of the src and
   * the tgt node and their interface places. It then picks the right source and
   * target places in the respective interface based on each node's {@link NodePair}
   * that was built by {@link Translator} in {@link #translateChildNode(DiagramNode)}
   * and all {@link Translator}s that inherit from this class.
   * 
   * @param arc
   */
	public void translateChildArc(DiagramArc arc) {
	  
	  DiagramNode src = arc.getSrc();
	  DiagramNode tgt = arc.getTarget();
	  
	  //System.out.println("translate arc "+ arc.getSrc().getLabel() +" -> "+arc.getTarget().getLabel());
	  
    if (src == null || tgt == null)
      throw new NullPointerException("Error in while translating arc: arc has no source- or targetNode in Diagram!");
    
    translateChildArc(src, tgt, arc);
	}

	/**
	 * Create an arc from src to tgt. The reason for this arc is the given
	 * {@link DiagramArc} arc, but this arc itself does not connect src and tgt directly.
	 * Use for instance in {@link FCtoOclet}.
	 * 
   * The method identifies the corresponding Petri net patterns of the src and
   * the tgt node and their interface places. It then picks the right source and
   * target places in the respective interface based on each node's {@link NodePair}
   * that was built by {@link Translator} in {@link #translateChildNode(DiagramNode)}
   * and all {@link Translator}s that inherit from this class.
	 *  
	 * @param src
	 * @param tgt
	 * @param arc
	 */
  public void translateChildArc(DiagramNode src, DiagramNode tgt, DiagramArc arc) {
	  
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
        return;
      }
    }
    // other Arc
    else
    {
      if (this.getTable().get(tgt) != null) { 
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
      System.err.println("Could not translate arc "+src.getLabel()+" -> "+tgt.getLabel()+" ["+(this.node != null ? this.node.getLabel() : "root" )+"]");
      System.err.println("Reason: arc has no source- or targetPlace");
      //throw new NullPointerException("Error while translating arc: !");
      return;
    }
    
    Place pStart = getMergeResult(rightArcStart);
    Place pEnd = getMergeResult(rightArcEnd);
    
    // merge the source and the target places of the arc
    Place pMerged = PNAPI.mergePlaces(net, pStart, pEnd);
    mergedPlaces.add(pStart);
    mergedPlaces.add(pEnd);
    mergeMap.put(rightArcStart, pMerged);
    mergeMap.put(rightArcEnd, pMerged);
	}
	
  /**
   * @param p
   * @return the place into which the given place was merged, returns p if
   * p was not merged yet 
   */
  private Place getMergeResult(Place p) {
    if (mergeMap.get(p) == null) return p;
    else return mergeMap.get(p);
  }
  
  /**
   * The set of merged places, built up by calling {@link #mergedPlaces}.
   * Call {@link #removeAllMergedPlaces()} to remove all places in this set
   * from the net.
   */
  private HashSet<Place> mergedPlaces;
  
  /**
   * Remove all places in {@link #mergedPlaces} from the net. Specifically
   * remove all references to each place by clearing the marking and adjacent
   * arcs.
   */
  private void removeAllMergedPlaces() {
    for (Place p : this.mergedPlaces) {
      PNAPI.removePlace(this.getNet(), p);
    }
  }
	
	/**
	 * creates an XML-conform label from a String 
	 * replaces therefore special characters by their HTML notation
	 * if label is empty there will be created an alternative name
	 * @param label - the diagramNode's label, x - 't' for transitions, 'p' for places
	 * @return the PNML Label
	 */
	protected String createLabel (String label, Character x){
		if (label == null || label.length() == 0){
			if (x == 'p') {
				label = "p"+this.getPlaceNum();
				this.setPlaceNum(this.getPlaceNum()+1);
			}
			else if (x == 't') {
				label = "t" + this.getTransitionNum();
				this.setTransitionNum(this.getTransitionNum()+1);
			}
			return label;
		}
		
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
	
	/**
	 * nodes that have more than one incoming arc need an InterfaceIn
	 * @param p - the actual startPlace
	 */
	protected void addInterfaceIn(PlaceExt pStart){
		TransitionExt t = PtnetLoLAFactory.eINSTANCE.createTransitionExt();	t.setName("InterfaceIn_" + createLabel(null, 't'));
		this.net.getTransitions().add(t);
		
		ArcToPlaceExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
		a1.setSource(t); a1.setTarget(pStart);
		this.net.getArcs().add(a1);
		
		for (DiagramArc arc: this.node.getIncoming()){
			PlaceExt pIn = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); 
			if (node.getLabel() != null && arc.getSrc().getLabel() != null)
				pIn.setName(createLabel(node.getLabel()+"_In_"+arc.getSrc().getLabel(),'p'));
			else pIn.setName(createLabel(null, 'p'));
			this.net.getPlaces().add(pIn);	
			
			ArcToTransitionExt a = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
			a.setSource(pIn); a.setTarget(t);
			this.net.getArcs().add(a);
			
			this.pair.addStart(pIn);				
			this.pair.setOtherEnd(pIn, arc.getSrc());
		}
	}
	
	/**
	 * nodes that have more than one outgoing arc need an InterfaceOut
	 * @param p - the actual endPlace
	 */
	protected void addInterfaceOut(PlaceExt pEnd){
		TransitionExt t = PtnetLoLAFactory.eINSTANCE.createTransitionExt();	t.setName("InterfaceOut_" + createLabel(null, 't'));
		this.net.getTransitions().add(t);
		
		ArcToTransitionExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
		a1.setSource(pEnd); a1.setTarget(t);
		this.net.getArcs().add(a1);
			
		for (DiagramArc arc: this.node.getOutgoing()){
			PlaceExt pOut = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); 
			if (node.getLabel() != null && arc.getTarget().getLabel() != null)
				pOut.setName(this.createLabel(node.getLabel()+"_Out_"+arc.getTarget().getLabel(), 'p'));
			else pOut.setName(createLabel(null, 'p'));
			this.net.getPlaces().add(pOut);
			
			ArcToPlaceExt a = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
			a.setSource(t); a.setTarget(pOut);
			this.net.getArcs().add(a);
			
			this.pair.addEnd(pOut);
			this.pair.setOtherEnd(pOut, arc.getTarget());
		}
	}
	
	protected class NodePair{
		List<PlaceExt> startPlaces;
		List<PlaceExt> endPlaces;
		// for each Node the other end of the according arc
		Hashtable<PlaceExt, DiagramNode> otherEnds;
		
		/**
		 * 
		 * @return the Hashtable including all Places and the DiagramNodes at the other ends of the according arcs 
		 */
		public Hashtable<PlaceExt, DiagramNode> getOtherEnds() {
			return otherEnds;
		}

		public void setOtherEnds(Hashtable<PlaceExt, DiagramNode> otherEnds) {
			this.otherEnds = otherEnds;
		}

		/**
		 * set the other end of an arc according to a place in the Pair
		 * @param place - the place
		 * @param end - the other end of the according arc
		 */
		public void setOtherEnd (PlaceExt place, DiagramNode end){
			if (!(this.startPlaces.contains(place) || this.endPlaces.contains(place))){
				throw new NullPointerException("Error in NodePair.setOtherEnd()! Trying to add a place which is not in lists (startPlaces, endPlaces).");
			}
			else 
				if (this.otherEnds == null) {
					this.otherEnds = new Hashtable<PlaceExt, DiagramNode>();
				}
				this.otherEnds.put(place, end);
		}
		
		/**
		 * @param end - the source or target of the arc
		 * @param start - true, if searching for a startPlace, false, if searching for an endPlace
		 * @return the place according to the arc which source or target is given
		 */
		public PlaceExt getOtherEndPlace (DiagramNode end, Boolean start){
			if (this.getOtherEnds() == null){
				return null;
			}
			if (!this.getOtherEnds().containsValue(end)){
				// System.out.println("NodePair.getOtherEndPlace()! The " + end.getClass().getSimpleName() + " " + end.getLabel() + " is not in the List.");
				return null;
			}
			else 
			{
				for (PlaceExt p : this.getOtherEnds().keySet()){
					if (this.getOtherEnds().get(p) == end) {
						if ((start && this.startPlaces.contains(p)) || (!start && this.endPlaces.contains(p))) return p;
					}
				}
			}
			return null;
		}
		
		public NodePair(PlaceExt start, PlaceExt end){
			this.startPlaces = new Vector<PlaceExt>();
			this.startPlaces.add(start);
			this.endPlaces = new Vector<PlaceExt>();			
			this.endPlaces.add(end);
		}
				
		public NodePair (List<PlaceExt> start, List<PlaceExt> end){
			this.startPlaces = start;
			this.endPlaces = end;
		}
		

		public boolean removeStart (PlaceExt node){
			return(this.startPlaces.remove(node));
		}
		
		public boolean removeEnd (PlaceExt node){
			return(this.endPlaces.remove(node));
		}
		
		public List<PlaceExt> getStart(){
			return this.startPlaces;
		}
		
		public List<PlaceExt> getEnd(){
			return this.endPlaces;
		}
		
		public void addStart(PlaceExt start){
			this.startPlaces.add(start);
		}
		
		public void addEnd (PlaceExt end){
			this.endPlaces.add(end);
		}

		public void setStartPlaces(List<PlaceExt> startPlaces) {
			this.startPlaces = startPlaces;
		}

		public void setEndPlaces(List<PlaceExt> endPlaces) {
			this.endPlaces = endPlaces;
		}
	}
}
