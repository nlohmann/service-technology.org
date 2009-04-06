/*****************************************************************************\
 * Copyright (c) 2009 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
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
 * The Original Code is this file as it was released on March 08, 2009.
 * The Initial Developer of the Original Code is
 * 		Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2009
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

package hub.top.editor.eclipse.ui;

import hub.top.editor.resource.Activator;

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
import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IObjectActionDelegate;
import org.eclipse.ui.IWorkbenchPart;

/**
 * Action class to invoke a model conversion wizard. The wizard is
 * implemented in {@link ConvertFileWizard}. To use this action, create a
 * corresponding handler in plugin.xml
 * 
 * <code>
 *    <extension point="org.eclipse.ui.popupMenus">
 *      <objectContribution
 *          id="my.id" nameFilter="*.ext"
 *          objectClass="org.eclipse.core.resources.IFile">
 *        <action
 *             label="Convert Model..."
 *             class="my.package.ConvertDiagramFileAction"
 *             menubarPath="additions"
 *             enablesFor="1"
 *             id="my.action.id">
 *        </action>
 *      </objectContribution>
 *    </extension>
 *  </code>
 * 
 * @author Dirk Fahland
 */
public class ConvertDiagramFileAction implements IObjectActionDelegate {

  private IWorkbenchPart 	targetPart;
  private URI 				domainModelURI;

  public void setActivePart(IAction action, IWorkbenchPart targetPart) {
    this.targetPart = targetPart;
  }
	
  /**
   * Track selection changes in the workbench to have a proper selection
   * for conversion.  
   */
  public void selectionChanged(IAction action, ISelection selection) {
    domainModelURI = null;
    action.setEnabled(false);
    if (   selection instanceof IStructuredSelection == false
        || selection.isEmpty()) {
      return;
    }
    IFile file = (IFile) ((IStructuredSelection) selection).getFirstElement();
    domainModelURI = URI.createPlatformResourceURI(file.getFullPath().toString(), true);
    action.setEnabled(true);
  }

  private Shell getShell() {
    return targetPart.getSite().getShell();
  }

  /**
   * Executes this action by passing the selected file's root object
   * to a {@link ConvertFileWizard} which invokes a model conversion
   * routine and writes the result into a workspace file.
   * 
   * @param action
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
    	Activator.getPluginHelper().logError(
          "Unable to load resource: " + domainModelURI, ex);
    }
    if (diagramRoot == null) {
      MessageDialog
          .openError(
              getShell(),
              "Convert Model",
              "Could not convert model, no model found in " + domainModelURI);
      return;
    }

    ConvertFileWizard wizard = createConvertFileWizard(domainModelURI,
    		diagramRoot, editingDomain);
    WizardDialog dialog = new WizardDialog(getShell(), wizard);
    dialog.create();
    dialog.getShell().setSize(Math.max(500, dialog.getShell().getSize().x), 500);
    dialog.open();
  }
  
  /**
   * Create a new wizard object to guide the user through the conversion
   * process. Override this method to invoke your own wizard.
   * 
   * @param root
   * @param editingDomain
   * @return
   */
  public ConvertFileWizard createConvertFileWizard (URI srcURI, EObject root, TransactionalEditingDomain editingDomain) {
	  ConvertFileWizard wizard = new ConvertFileWizard(srcURI, root, editingDomain, "txt");
	  wizard.setWindowTitle("Convert Model");
	  return wizard;
  }
}
