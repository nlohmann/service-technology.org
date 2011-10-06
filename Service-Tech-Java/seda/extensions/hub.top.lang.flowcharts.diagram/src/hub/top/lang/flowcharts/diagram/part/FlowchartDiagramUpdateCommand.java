/*
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
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
 * The Original Code is this file as it was released on July 30, 2008.
 * The Initial Developer of the Original Code is
 *  		Dirk Fahland
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
 */
package hub.top.lang.flowcharts.diagram.part;

import java.util.Iterator;
import java.util.List;

import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.commands.IHandler;
import org.eclipse.core.commands.IHandlerListener;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.gef.EditPart;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.CanonicalEditPolicy;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.PlatformUI;

/**
 * @generated
 */
public class FlowchartDiagramUpdateCommand implements IHandler {

  /**
   * @generated
   */
  public void addHandlerListener(IHandlerListener handlerListener) {
  }

  /**
   * @generated
   */
  public void dispose() {
  }

  /**
   * @generated
   */
  public Object execute(ExecutionEvent event) throws ExecutionException {
    ISelection selection = PlatformUI.getWorkbench().getActiveWorkbenchWindow()
        .getSelectionService().getSelection();
    if (selection instanceof IStructuredSelection) {
      IStructuredSelection structuredSelection = (IStructuredSelection) selection;
      if (structuredSelection.size() != 1) {
        return null;
      }
      if (structuredSelection.getFirstElement() instanceof EditPart
          && ((EditPart) structuredSelection.getFirstElement()).getModel() instanceof View) {
        EObject modelElement = ((View) ((EditPart) structuredSelection
            .getFirstElement()).getModel()).getElement();
        List editPolicies = CanonicalEditPolicy
            .getRegisteredEditPolicies(modelElement);
        for (Iterator it = editPolicies.iterator(); it.hasNext();) {
          CanonicalEditPolicy nextEditPolicy = (CanonicalEditPolicy) it.next();
          nextEditPolicy.refresh();
        }

      }
    }
    return null;
  }

  /**
   * @generated
   */
  public boolean isEnabled() {
    return true;
  }

  /**
   * @generated
   */
  public boolean isHandled() {
    return true;
  }

  /**
   * @generated
   */
  public void removeHandlerListener(IHandlerListener handlerListener) {
  }

}
