/*****************************************************************************\
 * Copyright (c) 2009 Dirk Fahland. All rights reserved. EPL1.0/AGPL3.0
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

import hub.top.editor.resource.Activator;


import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
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
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IObjectActionDelegate;
import org.eclipse.ui.IWorkbenchPart;

import com.microsoft.schemas.visio._2003.vdx.DocumentRoot;
import com.microsoft.schemas.visio._2003.vdx.editor.actions.ProcessVdxDelegate;

public class ConvertFileAction_VDXtoFlow implements IObjectActionDelegate {

  public static final String ID = "hub.top.lang.flowcharts.visioimport.ConvertToFlowAction";
  
  private IWorkbenchPart  targetPart;
  private URI         domainModelURI;

  public void setActivePart(IAction action, IWorkbenchPart targetPart) {
    this.targetPart = targetPart;
  }
  
  private Shell getShell() {
    return targetPart.getSite().getShell();
  }
  
  protected Object getModelObject(TransactionalEditingDomain editingDomain, URI uri) {
    ResourceSet resourceSet = editingDomain.getResourceSet();
    EObject modelRoot = null;
    try {
      Resource resource = resourceSet.getResource(uri, true);
      modelRoot = (EObject) resource.getContents().get(0);
    } catch (WrappedException ex) {
      Activator.getPluginHelper().logError(
          "Unable to load resource: " + uri, ex);
    }
    return modelRoot;
  }

  public void run(IAction action) {
    if (!action.getId().equals(ID))
      return;
    
    final TransactionalEditingDomain editingDomain = GMFEditingDomainFactory.INSTANCE
        .createEditingDomain();
    final URI fDomainModelUri = domainModelURI;

    // conversion takes time: run in an Eclipse job
    Job convertJob = new Job("Convert Visio XML to Flowchart") {
      @Override
      protected IStatus run(IProgressMonitor monitor) {
        monitor.beginTask("reading Visio", IProgressMonitor.UNKNOWN);
        
        Object modelRoot = getModelObject(editingDomain, fDomainModelUri);
        
        if (modelRoot == null) {
          MessageDialog
              .openError(
                  getShell(),
                  "Convert Model",
                  "Could not convert model, no model found in " + domainModelURI);
          return Status.OK_STATUS;
        }
        
        if (modelRoot instanceof DocumentRoot) {
          ProcessVdxDelegate.convertVisioToFlow((DocumentRoot)modelRoot);
        }
        return Status.OK_STATUS;
      }
    };
    convertJob.setUser(true);
    convertJob.schedule();

  }

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
}