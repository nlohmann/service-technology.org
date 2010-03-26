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

import java.lang.reflect.InvocationTargetException;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.actions.WorkspaceModifyOperation;

/**
 * @generated
 */
public class FlowchartCreationWizard extends Wizard implements INewWizard {

  /**
   * @generated
   */
  private IWorkbench workbench;

  /**
   * @generated
   */
  protected IStructuredSelection selection;

  /**
   * @generated
   */
  protected FlowchartCreationWizardPage diagramModelFilePage;

  /**
   * @generated
   */
  protected FlowchartCreationWizardPage domainModelFilePage;

  /**
   * @generated
   */
  protected Resource diagram;

  /**
   * @generated
   */
  private boolean openNewlyCreatedDiagramEditor = true;

  /**
   * @generated
   */
  public IWorkbench getWorkbench() {
    return workbench;
  }

  /**
   * @generated
   */
  public IStructuredSelection getSelection() {
    return selection;
  }

  /**
   * @generated
   */
  public final Resource getDiagram() {
    return diagram;
  }

  /**
   * @generated
   */
  public final boolean isOpenNewlyCreatedDiagramEditor() {
    return openNewlyCreatedDiagramEditor;
  }

  /**
   * @generated
   */
  public void setOpenNewlyCreatedDiagramEditor(
      boolean openNewlyCreatedDiagramEditor) {
    this.openNewlyCreatedDiagramEditor = openNewlyCreatedDiagramEditor;
  }

  /**
   * @generated
   */
  public void init(IWorkbench workbench, IStructuredSelection selection) {
    this.workbench = workbench;
    this.selection = selection;
    setWindowTitle(Messages.FlowchartCreationWizardTitle);
    setDefaultPageImageDescriptor(FlowchartDiagramEditorPlugin
        .getBundledImageDescriptor("icons/wizban/NewFlowWizard.gif")); //$NON-NLS-1$
    setNeedsProgressMonitor(true);
  }

  /**
   * @generated
   */
  public void addPages() {
    diagramModelFilePage = new FlowchartCreationWizardPage(
        "DiagramModelFile", getSelection(), "flow_diagram"); //$NON-NLS-1$ //$NON-NLS-2$
    diagramModelFilePage
        .setTitle(Messages.FlowchartCreationWizard_DiagramModelFilePageTitle);
    diagramModelFilePage
        .setDescription(Messages.FlowchartCreationWizard_DiagramModelFilePageDescription);
    addPage(diagramModelFilePage);

    domainModelFilePage = new FlowchartCreationWizardPage(
        "DomainModelFile", getSelection(), "flow") { //$NON-NLS-1$ //$NON-NLS-2$

      public void setVisible(boolean visible) {
        if (visible) {
          String fileName = diagramModelFilePage.getFileName();
          fileName = fileName.substring(0, fileName.length()
              - ".flow_diagram".length()); //$NON-NLS-1$
          setFileName(FlowchartDiagramEditorUtil.getUniqueFileName(
              getContainerFullPath(), fileName, "flow")); //$NON-NLS-1$
        }
        super.setVisible(visible);
      }
    };
    domainModelFilePage
        .setTitle(Messages.FlowchartCreationWizard_DomainModelFilePageTitle);
    domainModelFilePage
        .setDescription(Messages.FlowchartCreationWizard_DomainModelFilePageDescription);
    addPage(domainModelFilePage);
  }

  /**
   * @generated
   */
  public boolean performFinish() {
    IRunnableWithProgress op = new WorkspaceModifyOperation(null) {

      protected void execute(IProgressMonitor monitor) throws CoreException,
          InterruptedException {
        diagram = FlowchartDiagramEditorUtil.createDiagram(diagramModelFilePage
            .getURI(), domainModelFilePage.getURI(), monitor);
        if (isOpenNewlyCreatedDiagramEditor() && diagram != null) {
          try {
            FlowchartDiagramEditorUtil.openDiagram(diagram);
          } catch (PartInitException e) {
            ErrorDialog.openError(getContainer().getShell(),
                Messages.FlowchartCreationWizardOpenEditorError, null, e
                    .getStatus());
          }
        }
      }
    };
    try {
      getContainer().run(false, true, op);
    } catch (InterruptedException e) {
      return false;
    } catch (InvocationTargetException e) {
      if (e.getTargetException() instanceof CoreException) {
        ErrorDialog.openError(getContainer().getShell(),
            Messages.FlowchartCreationWizardCreationError, null,
            ((CoreException) e.getTargetException()).getStatus());
      } else {
        FlowchartDiagramEditorPlugin.getInstance().logError(
            "Error creating diagram", e.getTargetException()); //$NON-NLS-1$
      }
      return false;
    }
    return diagram != null;
  }
}
