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

import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.resource.Resource;

import hub.sam.tef.modelcreating.ModelCreatingContext;
import hub.sam.tef.semantics.ISemanticsProvider;
import hub.top.editor.ptnetLoLA.Node;
import hub.top.editor.ptnetLoLA.Place;
import hub.top.editor.ptnetLoLA.Transition;

public class LolaModelCreatingContext extends ModelCreatingContext {

	public LolaModelCreatingContext(EPackage[] packages,
			ISemanticsProvider semanticsProvider, Resource resource, String text) {
		super(packages, semanticsProvider, resource, text);
	}

	@Override
	public Object evaluateActionStatement(String methodName,
			EList<Object> methodParameters) {
		
		// methods for filtering during pretty printing
		if (methodName.startsWith("ppFilter_"))
			return null;
		
		if (methodName.equals("addArcsToNet"))
		{
			// resolve types from generic parameters and call the method
			Transition t = (Transition)methodParameters.get(0);
			return LolaStatementEvaluation.addArcsToNet(this, t);
		}
		else if (methodName.equals("placeSetTokenNumber"))
		{
			// resolve types from generic parameters and call the method
			Place p = (Place)methodParameters.get(0);
			Integer token = (Integer)methodParameters.get(1);
			return LolaStatementEvaluation.placeSetTokenNumber(this, p, token);
		}
		else if (methodName.equals("setNodeTypeForNode")) {
			Node n = (Node)methodParameters.get(0);
			return LolaStatementEvaluation.setNodeTypeForNode(this, n);
		}
		else if (methodName.equals("setNodeTypeInternal")) {
			return LolaStatementEvaluation.setNodeTypeInternal(this);
		}
		else if (methodName.equals("setNodeTypeInput")) {
			return LolaStatementEvaluation.setNodeTypeInput(this);
		}
		else if (methodName.equals("setNodeTypeOutput")) {
			return LolaStatementEvaluation.setNodeTypeOutput(this);
		}
		else
			return super.evaluateActionStatement(methodName, methodParameters);
	}
}
