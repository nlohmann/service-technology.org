/*****************************************************************************\
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 * 
 * ServiceTechnolog.org - PetriNet Editor Framework
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
 * The Original Code is this file as it was released on July 30, 2008.
 * The Initial Developer of the Original Code is
 * 		Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2008
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

package hub.top.editor.lola.text.modelcreating;

import hub.sam.tef.modelcreating.IModelCreatingContext;
import hub.top.editor.ptnetLoLA.Node;
import hub.top.editor.ptnetLoLA.NodeType;
import hub.top.editor.ptnetLoLA.Place;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.Transition;

public class LolaStatementEvaluation {

	/**
	 * insert all adjacent arcs of the given transition into the net 
	 * @param context context where the method is evaluated
	 * @param t transition with adjacent arcs
	 * @return null
	 */
	public static Object addArcsToNet(IModelCreatingContext context, Transition t) {
		// the net that contains this transition
		PtNet net = (PtNet) t.eContainer();	
		net.getArcs().addAll(t.getIncoming());
		net.getArcs().addAll(t.getOutgoing());
		return null;
	}
	
	/**
	 * set initial token number of the given place 
	 * @param context context where the method is evaluated
	 * @param p place
	 * @param token new initial token number of place p
	 * @return null
	 */
	public static Object placeSetTokenNumber(IModelCreatingContext context, Place p, Integer token) {
		p.setToken(token.intValue());
		return null;
	}
	
	private static NodeType currentNodeType = NodeType.INTERNAL;
	public static Object setNodeType(IModelCreatingContext context, NodeType nodeType) {
		currentNodeType = nodeType;
		return null;
	}
	
	public static Object setNodeTypeInternal(IModelCreatingContext context) {
		return setNodeType(context, NodeType.INTERNAL);
	}
	
	public static Object setNodeTypeInput(IModelCreatingContext context) {
		return setNodeType(context, NodeType.INPUT);
	}

	public static Object setNodeTypeOutput(IModelCreatingContext context) {
		return setNodeType(context, NodeType.OUTPUT);
	}
	
	public static Object setNodeTypeForNode(IModelCreatingContext context, Node n) {
		n.setType(currentNodeType);
		return null;
	}
	
	public static Object ppFilter_matchesNodeType(Node n) {
		if (n.getType().equals(currentNodeType))
			return n;
		else
			return null;
	}
}
