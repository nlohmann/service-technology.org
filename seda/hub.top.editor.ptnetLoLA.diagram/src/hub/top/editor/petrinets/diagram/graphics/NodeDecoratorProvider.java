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
 * The Original Code is this file as it was released on April 07, 2009.
 * The Initial Developer of the Original Code is
 * 		IBM Corporation
 * 
 * Portions created by the Initial Developer are Copyright (c) 2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Dirk Fahland
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

package hub.top.editor.petrinets.diagram.graphics;

import org.eclipse.core.runtime.Assert;
import org.eclipse.gmf.runtime.common.core.service.AbstractProvider;
import org.eclipse.gmf.runtime.common.core.service.IOperation;
import org.eclipse.gmf.runtime.diagram.ui.services.decorator.CreateDecoratorsOperation;
import org.eclipse.gmf.runtime.diagram.ui.services.decorator.IDecoratorProvider;
import org.eclipse.gmf.runtime.diagram.ui.services.decorator.IDecoratorTarget;
import org.eclipse.gmf.runtime.notation.Node;


/**
 * Decorator provider for {@link NodeDecorator}
 */
public class NodeDecoratorProvider
	extends AbstractProvider
	implements IDecoratorProvider {

	public static final String NODE_DECORATOR = "Node_Decorator"; //$NON-NLS-1$
	
	public void createDecorators(IDecoratorTarget decoratorTarget) {
		Node node = NodeDecorator.getDecoratorTargetNode(decoratorTarget);
		if (node != null) {
			decoratorTarget.installDecorator(NODE_DECORATOR, new NodeDecorator(decoratorTarget));
		}
	}

	/* (non-Javadoc)
	 * @see org.eclipse.gmf.runtime.common.core.internal.service.IProvider#provides(org.eclipse.gmf.runtime.common.core.service.IOperation)
	 */
	public boolean provides(IOperation operation) {
		Assert.isNotNull(operation);

		if (!(operation instanceof CreateDecoratorsOperation))
			return false;

		IDecoratorTarget decoratorTarget = 
			((CreateDecoratorsOperation) operation).getDecoratorTarget();
		return NodeDecorator.getDecoratorTargetNode(decoratorTarget) != null;
	}
}
