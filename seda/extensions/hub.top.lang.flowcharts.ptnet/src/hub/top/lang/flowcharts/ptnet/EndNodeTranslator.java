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

import hub.top.editor.ptnetLoLA.ArcToTransitionExt;
import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.lang.flowcharts.Endnode;
import hub.top.lang.flowcharts.WorkflowDiagram;

import java.util.List;
import java.util.Vector;

public class EndNodeTranslator extends Translator {
	public Endnode getNode(){
		if (!(super.getNode() instanceof Endnode))
			throw new NullPointerException("Error. Instantiated EndNodeTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return ((Endnode)super.getNode());
	}
	
	public void translate() {
		Endnode st = this.getNode();
		// translating an EndNode
		List<PlaceExt> startNodes = new Vector<PlaceExt>();
		List<PlaceExt> endNode = new Vector<PlaceExt>();
		
    boolean isAtTopLevel = false;
    //if (st.eContainer() != null && st.eContainer() instanceof WorkflowDiagram)
    if (parentTranslator == null || parentTranslator.node == null)
      isAtTopLevel = true;

    
		PlaceExt pE = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); pE.setName(super.createLabel(st.getLabel(), 'p'));
		this.net.getPlaces().add(pE);
		
		endNode.add(pE);
		this.pair = new NodePair(startNodes, endNode);
			
		// more than one incoming arc --> need to insert an InterfaceIn (on an EndNode there cannot be outgoing arcs)
		if (st.getIncoming().size() > 1){
			addInterfaceIn(pE);	
		}
		else if (st.getIncoming().size() > 0) {
			startNodes.add(pE);
			this.pair.setOtherEnd(pE, st.getIncoming().get(0).getSrc());
		}
		
    if (st.getOutgoing().size() > 0){
      System.err.println("Warning: end node "+st.getLabel()+" ["+(this.node != null ? this.node.getLabel() : "root" ) +"] has an outgoing arc");
    }
			
		super.getTable().put(st, this.getPair());
		
    if (!isAtTopLevel && parentTranslator instanceof IBlockTranslator
        && node.getOutgoing().isEmpty())
    {
      // insert arcs between begin of surrounding block-structure and EndNode
      ArcToTransitionExt arc = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
      arc.setSource(pE);
      arc.setTarget(((IBlockTranslator)parentTranslator).getBlockExit());
      this.net.getArcs().add(arc);
    }
	}
}
