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

import hub.top.lang.flowcharts.diagram.edit.parts.WorkflowDiagramEditPart;

import org.eclipse.core.resources.IFile;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.common.util.WrappedException;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.ResourceSet;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.gmf.runtime.emf.core.GMFEditingDomainFactory;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.osgi.util.NLS;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IObjectActionDelegate;
import org.eclipse.ui.IWorkbenchPart;

/**
 * @generated
 */
public class FlowchartInitDiagramFileAction implements IObjectActionDelegate {

  /**
   * @generated
   */
  private IWorkbenchPart targetPart;

  /**
   * @generated
   */
  private URI domainModelURI;

  /**
   * @generated
   */
  public void setActivePart(IAction action, IWorkbenchPart targetPart) {
    this.targetPart = targetPart;
  }

  /**
   * @generated
   */
  public void selectionChanged(IAction action, ISelection selection) {
    domainModelURI = null;
    action.setEnabled(false);
    if (selection instanceof IStructuredSelection == false
        || selection.isEmpty()) {
      return;
    }
    IFile file = (IFile) ((IStructuredSelection) selection).getFirstElement();
    domainModelURI = URI.createPlatformResourceURI(file.getFullPath()
        .toString(), true);
    action.setEnabled(true);
  }

  /**
   * @generated
   */
  private Shell getShell() {
    return targetPart.getSite().getShell();
  }

  /**
   * @generated
   */
  public void run(IAction action) {
    TransactionalEditingDomain editingDomain = GMFEditingDomainFactory.INSTANCE
        .createEditingDomain();
    ResourceSet resourceSet = editingDomain.getResourceSet();
    EObject diagramRoot = null;
    try {
      Resource resource = resourceSet.getResource(domainModelURI, true);
      diagramRoot = (EObject) resource.getContents().get(0);
    } catch (WrappedException ex) {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "Unable to load resource: " + domainModelURI, ex); //$NON-NLS-1$
    }
    if (diagramRoot == null) {
      MessageDialog
          .openError(
              getShell(),
              Messages.FlowchartInitDiagramFileAction_InitDiagramFileResourceErrorDialogTitle,
              Messages.FlowchartInitDiagramFileAction_InitDiagramFileResourceErrorDialogMessage);
      return;
    }
    Wizard wizard = new FlowchartNewDiagramFileWizard(domainModelURI,
        diagramRoot, editingDomain);
    wizard.setWindowTitle(NLS.bind(
        Messages.FlowchartInitDiagramFileAction_InitDiagramFileWizardTitle,
        WorkflowDiagramEditPart.MODEL_ID));
    FlowchartDiagramEditorUtil.runWizard(getShell(), wizard, "InitDiagramFile"); //$NON-NLS-1$
  }
}
