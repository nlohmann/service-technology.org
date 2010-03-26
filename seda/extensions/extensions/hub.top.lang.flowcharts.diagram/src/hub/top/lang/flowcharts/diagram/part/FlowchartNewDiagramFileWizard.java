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

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.commands.operations.OperationHistoryFactory;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.ResourceSet;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.gmf.runtime.common.core.command.CommandResult;
import org.eclipse.gmf.runtime.diagram.core.services.ViewService;
import org.eclipse.gmf.runtime.diagram.core.services.view.CreateDiagramViewOperation;
import org.eclipse.gmf.runtime.emf.commands.core.command.AbstractTransactionalCommand;
import org.eclipse.gmf.runtime.emf.core.util.EObjectAdapter;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.osgi.util.NLS;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.dialogs.WizardNewFileCreationPage;

/**
 * @generated
 */
public class FlowchartNewDiagramFileWizard extends Wizard {

  /**
   * @generated
   */
  private WizardNewFileCreationPage myFileCreationPage;

  /**
   * @generated
   */
  private ModelElementSelectionPage diagramRootElementSelectionPage;

  /**
   * @generated
   */
  private TransactionalEditingDomain myEditingDomain;

  /**
   * @generated
   */
  public FlowchartNewDiagramFileWizard(URI domainModelURI, EObject diagramRoot,
      TransactionalEditingDomain editingDomain) {
    assert domainModelURI != null : "Domain model uri must be specified"; //$NON-NLS-1$
    assert diagramRoot != null : "Doagram root element must be specified"; //$NON-NLS-1$
    assert editingDomain != null : "Editing domain must be specified"; //$NON-NLS-1$

    myFileCreationPage = new WizardNewFileCreationPage(
        Messages.FlowchartNewDiagramFileWizard_CreationPageName,
        StructuredSelection.EMPTY);
    myFileCreationPage
        .setTitle(Messages.FlowchartNewDiagramFileWizard_CreationPageTitle);
    myFileCreationPage.setDescription(NLS.bind(
        Messages.FlowchartNewDiagramFileWizard_CreationPageDescription,
        WorkflowDiagramEditPart.MODEL_ID));
    IPath filePath;
    String fileName = domainModelURI.trimFileExtension().lastSegment();
    if (domainModelURI.isPlatformResource()) {
      filePath = new Path(domainModelURI.trimSegments(1).toPlatformString(true));
    } else if (domainModelURI.isFile()) {
      filePath = new Path(domainModelURI.trimSegments(1).toFileString());
    } else {
      // TODO : use some default path
      throw new IllegalArgumentException("Unsupported URI: " + domainModelURI); //$NON-NLS-1$
    }
    myFileCreationPage.setContainerFullPath(filePath);
    myFileCreationPage.setFileName(FlowchartDiagramEditorUtil
        .getUniqueFileName(filePath, fileName, "flow_diagram")); //$NON-NLS-1$

    diagramRootElementSelectionPage = new DiagramRootElementSelectionPage(
        Messages.FlowchartNewDiagramFileWizard_RootSelectionPageName);
    diagramRootElementSelectionPage
        .setTitle(Messages.FlowchartNewDiagramFileWizard_RootSelectionPageTitle);
    diagramRootElementSelectionPage
        .setDescription(Messages.FlowchartNewDiagramFileWizard_RootSelectionPageDescription);
    diagramRootElementSelectionPage.setModelElement(diagramRoot);

    myEditingDomain = editingDomain;
  }

  /**
   * @generated
   */
  public void addPages() {
    addPage(myFileCreationPage);
    addPage(diagramRootElementSelectionPage);
  }

  /**
   * @generated
   */
  public boolean performFinish() {
    List affectedFiles = new LinkedList();
    IFile diagramFile = myFileCreationPage.createNewFile();
    FlowchartDiagramEditorUtil.setCharset(diagramFile);
    affectedFiles.add(diagramFile);
    URI diagramModelURI = URI.createPlatformResourceURI(diagramFile
        .getFullPath().toString(), true);
    ResourceSet resourceSet = myEditingDomain.getResourceSet();
    final Resource diagramResource = resourceSet
        .createResource(diagramModelURI);
    AbstractTransactionalCommand command = new AbstractTransactionalCommand(
        myEditingDomain,
        Messages.FlowchartNewDiagramFileWizard_InitDiagramCommand,
        affectedFiles) {

      protected CommandResult doExecuteWithResult(IProgressMonitor monitor,
          IAdaptable info) throws ExecutionException {
        int diagramVID = FlowchartVisualIDRegistry
            .getDiagramVisualID(diagramRootElementSelectionPage
                .getModelElement());
        if (diagramVID != WorkflowDiagramEditPart.VISUAL_ID) {
          return CommandResult
              .newErrorCommandResult(Messages.FlowchartNewDiagramFileWizard_IncorrectRootError);
        }
        Diagram diagram = ViewService.createDiagram(
            diagramRootElementSelectionPage.getModelElement(),
            WorkflowDiagramEditPart.MODEL_ID,
            FlowchartDiagramEditorPlugin.DIAGRAM_PREFERENCES_HINT);
        diagramResource.getContents().add(diagram);
        return CommandResult.newOKCommandResult();
      }
    };
    try {
      OperationHistoryFactory.getOperationHistory().execute(command,
          new NullProgressMonitor(), null);
      diagramResource.save(FlowchartDiagramEditorUtil.getSaveOptions());
      FlowchartDiagramEditorUtil.openDiagram(diagramResource);
    } catch (ExecutionException e) {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "Unable to create model and diagram", e); //$NON-NLS-1$
    } catch (IOException ex) {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "Save operation failed for: " + diagramModelURI, ex); //$NON-NLS-1$
    } catch (PartInitException ex) {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "Unable to open editor", ex); //$NON-NLS-1$
    }
    return true;
  }

  /**
   * @generated
   */
  private static class DiagramRootElementSelectionPage extends
      ModelElementSelectionPage {

    /**
     * @generated
     */
    protected DiagramRootElementSelectionPage(String pageName) {
      super(pageName);
    }

    /**
     * @generated
     */
    protected String getSelectionTitle() {
      return Messages.FlowchartNewDiagramFileWizard_RootSelectionPageSelectionTitle;
    }

    /**
     * @generated
     */
    protected boolean validatePage() {
      if (selectedModelElement == null) {
        setErrorMessage(Messages.FlowchartNewDiagramFileWizard_RootSelectionPageNoSelectionMessage);
        return false;
      }
      boolean result = ViewService.getInstance().provides(
          new CreateDiagramViewOperation(new EObjectAdapter(
              selectedModelElement), WorkflowDiagramEditPart.MODEL_ID,
              FlowchartDiagramEditorPlugin.DIAGRAM_PREFERENCES_HINT));
      setErrorMessage(result ? null
          : Messages.FlowchartNewDiagramFileWizard_RootSelectionPageInvalidSelectionMessage);
      return result;
    }
  }
}
