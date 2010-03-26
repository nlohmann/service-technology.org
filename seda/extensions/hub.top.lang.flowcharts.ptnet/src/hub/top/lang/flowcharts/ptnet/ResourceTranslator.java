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


import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.lang.flowcharts.ResourceNode;

import java.util.Hashtable;

public class ResourceTranslator extends Translator {

	// remember the resource nodes by name for later reuse
	Hashtable<String, NodePair> translatedResources;
	
	public Hashtable<String, NodePair> getTranslatedResources() {
		return translatedResources;
	}

	public void setTranslatedResources(
			Hashtable<String, NodePair> translatedResources) {
		this.translatedResources = translatedResources;
	}

	public ResourceNode getNode(){
		if (!(super.getNode() instanceof ResourceNode))
			throw new NullPointerException("Error. Instantiated ResourceTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return((ResourceNode)super.getNode());
	}
	
	public void translate() {
		ResourceNode res = this.getNode();
		// resource is already translated --> use the same NodePair
		if (this.getTranslatedResources().containsKey(res.getLabel())){
			this.getTable().put(res, this.getTranslatedResources().get(res.getLabel()));
		}
		else {
			// translating a resource
			PlaceExt pS = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); pS.setName(super.createLabel(res.getLabel(), 'p'));
			//if (res.getIncoming().isEmpty()) pS.setToken(1);
			this.net.getPlaces().add(pS);
			this.setPair(new NodePair(pS, pS));
			
//			if (res.getIncoming().size() > 1){
//				this.getPair().removeStart(pS);
//				super.addInterfaceIn(pS);
//			}
//			else if (res.getIncoming().size() > 0) this.getPair().setOtherEnd(pS, res.getIncoming().get(0).getSrc());
//			
//			if (res.getOutgoing().size() > 1){
//				this.getPair().removeEnd(pS);
//				super.addInterfaceOut(pS);
//			}
//			else if (res.getOutgoing().size() > 0) this.getPair().setOtherEnd(pS, res.getOutgoing().get(0).getTarget());
			
			super.getTable().put(res, this.getPair());
			this.getTranslatedResources().put(res.getLabel(), this.getPair());
		}
	}
	
}
