/*****************************************************************************\
 * Copyright (c) 2008, 2009. All rights reserved. Dirk Fahland. EPL1.0/AGPL3.0
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
 * The Original Code is this file as it was released on June 6, 2009.
 * The Initial Developer of the Original Code are
 *    Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2008, 2009
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

package hub.top.editor.eclipse.ui;

import hub.top.editor.eclipse.FileIOHelper;
import hub.top.editor.eclipse.ResourceHelper;
import hub.top.editor.resource.Activator;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.common.util.WrappedException;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.ResourceSet;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.gmf.runtime.emf.core.GMFEditingDomainFactory;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IObjectActionDelegate;
import org.eclipse.ui.IWorkbenchPart;

/**
 * Eclipse UI workbench action for consistently renaming diagram and
 * model files in a single step. The action works on selected GMF diagram
 * files (it checks whether the selected file contains a {@link Diagram}
 * object. In this case, the underlying EMF model file is identified, a
 * rename dialog is presented and model and diagram file are renamed together.
 * All references from the diagram file to the model file are updated
 * accordingly.
 * 
 * @author Dirk Fahland
 */
public class RenameDiagramFileAction  implements IObjectActionDelegate {

  private IWorkbenchPart  targetPart;
  private URI             diagramURI;

  public void setActivePart(IAction action, IWorkbenchPart targetPart) {
    this.targetPart = targetPart;
  }
  
  /**
   * Track selection changes in the workbench to have a proper selection
   * for conversion.  
   */
  public void selectionChanged(IAction action, ISelection selection) {
    diagramURI = null;
    action.setEnabled(false);
    if (   selection instanceof IStructuredSelection == false
        || selection.isEmpty()) {
      return;
    }
    IFile file = (IFile) ((IStructuredSelection) selection).getFirstElement();
    diagramURI = URI.createPlatformResourceURI(file.getFullPath().toString(), true);
    action.setEnabled(true);
  }

  private Shell getShell() {
    return targetPart.getSite().getShell();
  }
  
  /**
   * Retrieve EMF model object from the given URI
   * 
   * @param editingDomain
   * @param uri
   * @return
   */
  protected Object getModelObject(TransactionalEditingDomain editingDomain, URI uri) {
    ResourceSet resourceSet = editingDomain.getResourceSet();
    EObject diagramRoot = null;
    try {
      Resource resource = resourceSet.getResource(uri, true);
      diagramRoot = (EObject) resource.getContents().get(0);
    } catch (WrappedException ex) {
      Activator.getPluginHelper().logError(
          "Unable to load resource: " + uri, ex);
    }
    return diagramRoot;
  }
  
  /**
   * Run the rename operation
   */
  public void run(IAction action) {
    TransactionalEditingDomain editingDomain = GMFEditingDomainFactory.INSTANCE
          .createEditingDomain();

    // try to retrieve diagram object
    Object diagramRoot = getModelObject(editingDomain, diagramURI);
    
    if (diagramRoot == null || !(diagramRoot instanceof Diagram)) {
      MessageDialog
          .openError(
              getShell(),
              "Rename Diagram",
              "Could not rename diagram, no diagram model found in " + diagramURI);
      return;
    }
    
    // try to retrieve underlying model object
    Diagram diag = (Diagram)diagramRoot;
    URI modelURI = null;
    try {
      EObject modelRoot = diag.getElement();
      modelURI = modelRoot.eResource().getURI();
    } catch (NullPointerException e) {
      MessageDialog
        .openError(
          getShell(),
          "Rename Diagram",
          "Could not rename diagram, no model file found for " + diagramURI);
      return;
    }
    
    // both, diagram and model, are available
    // remember current name of the model
    String modelName = modelURI.trimFileExtension().lastSegment();
    
    // open dialog for the user to enter the new name
    // a validator to prevent the user from providing an illegal target file name 
    IInputValidator stringValidator = new IInputValidator() {
      public String isValid(String newText) {
        if (newText == null || newText.length() == 0)
          return "The target model name must not be empty.";
        // TODO: add check for existing files
        return null;
      }
    };
    
    InputDialog newNameDiag = new InputDialog(getShell(),
        "Rename Diagram and Model", "New Name:", modelName, stringValidator);
    if (newNameDiag.open() == InputDialog.CANCEL)
      return;
    
    String newName = newNameDiag.getValue();

    // new name is set, set target file names: newName.<ext>
    String modelExt = modelURI.fileExtension();
    URI targetModelURI = modelURI.trimFileExtension().trimSegments(1).appendSegment(newName).appendFileExtension(modelExt);
    String diagExt = diagramURI.fileExtension();
    URI targetDiagURI = diagramURI.trimFileExtension().trimSegments(1).appendSegment(newName).appendFileExtension(diagExt);
    
    // move diagram file to new location
    IFile srcDiagFile = ResourceHelper.uriToFile(diagramURI);
    IPath trgtDiagPath = new Path(targetDiagURI.toPlatformString(true));
    try {
      srcDiagFile.move(trgtDiagPath, false, true, new NullProgressMonitor());
    } catch (CoreException e) {
      MessageDialog
        .openError(
          getShell(),
          "Rename Diagram",
          "Could not move diagram from " + diagramURI + " to " + targetDiagURI+"\n"+e.getMessage());
      Activator.getPluginHelper().logError("Could not move diagram from " + diagramURI + " to " + targetDiagURI, e);
      return;
    }

    // move model file to new location
    IFile srcModelFile = ResourceHelper.uriToFile(modelURI);
    IPath trgtModelPath = new Path(targetModelURI.toPlatformString(true));
    try {
      srcModelFile.move(trgtModelPath, false, true, new NullProgressMonitor());
    } catch (CoreException e) {
      MessageDialog
        .openError(
          getShell(),
          "Rename Diagram",
          "Could not move model from " + modelURI + " to " + targetModelURI +"\n"+e.getMessage());
      Activator.getPluginHelper().logError("Could not move model from " + modelURI + " to " + targetModelURI, e);
      return;
    }

    // update references in model file by replacing the reference strings
    String diagString = FileIOHelper.readFile(trgtDiagPath);
    String newDiagString = diagString.replaceAll(
        "href=\""+modelName+"."+modelExt+"#",
        "href=\""+newName+"."+modelExt+"#");
    FileIOHelper.writeFile(trgtDiagPath, newDiagString);
    
    
  }
}
