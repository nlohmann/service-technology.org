/*****************************************************************************\
 * Copyright (c) 2008, 2009 Dirk Fahland. All rights reserved. EPL1.0/AGPL3.0
 * 
 * ServiceTechnolog.org - Greta
 *                       (Graphical Runtime Environment for Adaptive Processes) 
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
 * The Original Code is this file as it was released on December 04, 2008.
 * The Initial Developer of the Original Code are
 *    Manja Wolf
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

package hub.top.greta.run.actions;

import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.editor.eclipse.FileIOHelper;
import hub.top.editor.eclipse.ResourceHelper;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.greta.oclets.canonical.DNodeBP;
import hub.top.greta.oclets.canonical.InvalidModelException;
import hub.top.greta.run.Activator;
import hub.top.greta.synthesis.DNode2PtNet;
import hub.top.greta.verification.BuildBP;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.gef.EditPart;
import org.eclipse.gmf.runtime.emf.core.GMFEditingDomainFactory;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

public class BuildBranchingPTnet implements IWorkbenchWindowActionDelegate {

  public static final String ID = "hub.top.GRETA.run.buildBranchingProcess_ptnet";
  
  private IWorkbenchWindow workbenchWindow;

  private PtNet net;
  
  // fields for tracking the selection in the explorer 
  private IFile   selectedFile = null;
  private URI   selectedURI = null;
  
  public void dispose() {
    // TODO Auto-generated method stub

  }

  public void init(IWorkbenchWindow window) {
    workbenchWindow = window;
  }
  

  public void run(IAction action) {
    if (!action.getId().equals(ID))
      return;
    
    // seek system to check from a given URI
    net = ActionHelper.getPtNet(selectedURI);
    
    /*
    // if there was no system at the given URI, check the current editor
    if (net == null) {
      IEditorPart editor = workbenchWindow.getActivePage().getActiveEditor();
      net = ActionHelper.getAdaptiveSystem(editor);
      
      // if this editor has input file, remember it
      if (editor.getEditorInput() != null
        && editor.getEditorInput() instanceof IFileEditorInput)
      {
        selectedFile = ((IFileEditorInput)editor.getEditorInput()).getFile();
      }
    }
    */
    if (net == null)
      return;
      
    //final DNodeBP bp = BuildBP.init(net);
    final BuildBP build;
    
    try {
      build = new BuildBP(net, selectedFile);
    } catch (InvalidModelException e) {
      Activator.getPluginHelper().logError("Could not load model for verification.", e);
      return;
    }
    
    Job bpBuildJob = new Job("constructing branching process") 
    {
      @Override
      protected IStatus run(IProgressMonitor monitor) {
        
        monitor.beginTask("constructing branching process", IProgressMonitor.UNKNOWN);
        
        // build branching process
        boolean interrupted = !build.run(monitor, System.out);
        build.printStatistics(System.out);
        build.analyze(monitor, System.out);
        
        //build.minimize(monitor, System.out);
        build.writeBPtoFile(monitor, System.out, "_bp2");
        
        PtNet net = DNode2PtNet.process(build.getBranchingProcess());
        
        String modelName = selectedFile.getFullPath().removeFileExtension().lastSegment();
        IPath targetPath = selectedFile.getFullPath().removeLastSegments(1).append(modelName+"_bp2").addFileExtension("ptnet");
        TransactionalEditingDomain editing = GMFEditingDomainFactory.INSTANCE.createEditingDomain();
        FileIOHelper.writeEObjectToResource(net, editing, targetPath);

        monitor.done();
        
        /*
        if (interrupted)
          return Status.CANCEL_STATUS;
        else*/
          return Status.OK_STATUS;
        
      }
    };
      
    bpBuildJob.setUser(true);
    bpBuildJob.schedule();
  }

  public void selectionChanged(IAction action, ISelection selection) {
      selectedURI = null;
      selectedFile = null;
      action.setEnabled(false);
      if (selection instanceof IStructuredSelection == false
          || selection.isEmpty()) {
        return;
      }
      try {
        Object o = ((IStructuredSelection) selection).getFirstElement();
        if (o instanceof IFile) {
          selectedFile = (IFile) ((IStructuredSelection) selection).getFirstElement();
          selectedURI = URI.createPlatformResourceURI(selectedFile.getFullPath()
              .toString(), true);
          
        } else if (o instanceof EditPart) {
          EObject e = (EObject)((EditPart)o).getModel();
          selectedURI = e.eResource().getURI();
          selectedFile = ResourceHelper.uriToFile(selectedURI);
          
        } else {
          return;
        }
        
        action.setEnabled(true);
      } catch (ClassCastException e) {
        // just catch, do nothing
      } catch (NullPointerException e) {
        // just catch, do nothing
      }
  }

  private void writeDotFile (DNodeBP bp, IFile inputFile, String suffix) {

    String targetPathStr = inputFile.getFullPath().removeFileExtension().toString();
    IPath targetPath = new Path(targetPathStr+suffix+".dot");

    ActionHelper.writeFile (targetPath, bp.toDot());
  }
}
