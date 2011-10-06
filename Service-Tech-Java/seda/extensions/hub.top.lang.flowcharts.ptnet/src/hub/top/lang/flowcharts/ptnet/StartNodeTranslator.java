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
import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.lang.flowcharts.StartNode;
import hub.top.lang.flowcharts.WorkflowDiagram;

public class StartNodeTranslator extends Translator {

	public StartNode getNode(){
		if (!(super.getNode() instanceof StartNode)){
			throw new NullPointerException("Error. Instantiated StartNodeTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		}
		return ((StartNode)super.getNode());
	}
	
	public void translate() {
		StartNode st = this.getNode();

		boolean isAtTopLevel = false;
		//if (st.eContainer() != null && st.eContainer() instanceof WorkflowDiagram)
		if (parentTranslator == null || parentTranslator.node == null)
		  isAtTopLevel = true;
		
		// translating a StartNode
		PlaceExt pS = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); pS.setName(super.createLabel(st.getLabel(), 'p'));
		pS.setIsStart(isAtTopLevel);
		this.net.getPlaces().add(pS);
		// activate StartNode, if this is the start of the workflow
		if (isAtTopLevel) pS.setToken(1);
		
		this.setPair(new NodePair(pS, pS));
		
		if (st.getIncoming().size() > 0){
		  System.err.println("Warning: start node "+st.getLabel()+" ["+(this.node != null ? this.node.getLabel() : "root" ) +"] has an incoming arc");
		}
		
		if (st.getOutgoing().size() > 1){
			this.getPair().removeEnd(pS);
			super.addInterfaceOut(pS);
		}
		else if (st.getOutgoing().size() > 0) this.getPair().setOtherEnd(pS, st.getOutgoing().get(0).getTarget());
		
		this.getTable().put(st, this.getPair());
		
		if (!isAtTopLevel && parentTranslator instanceof IBlockTranslator
		    && node.getIncoming().isEmpty())
		{
		  // insert arcs between begin of surrounding block-structure and StartNode
		  ArcToPlaceExt arc = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
		  arc.setSource(((IBlockTranslator)parentTranslator).getBlockEntry());
		  arc.setTarget(pS);
		  this.net.getArcs().add(arc);
		}
	}
}
