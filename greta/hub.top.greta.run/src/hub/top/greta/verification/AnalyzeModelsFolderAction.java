/*****************************************************************************\
 * Copyright (c) 2008, 2009. All rights reserved. Dirk Fahland. EPL1.0/AGPL3.0
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

package hub.top.greta.verification;

import java.util.LinkedList;

import hub.top.editor.eclipse.FileIOHelper;
import hub.top.greta.oclets.canonical.DNodeBP;
import hub.top.greta.oclets.canonical.InvalidModelException;
import hub.top.greta.run.Activator;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.emf.common.util.URI;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

public class AnalyzeModelsFolderAction implements IWorkbenchWindowActionDelegate {

  public static final String ID = "hub.top.GRETA.run.analyzeModelsFolder";

  private IWorkbenchWindow workbenchWindow;

  // fields for tracking the selection in the explorer 
  private IFolder   selectedFolder = null;
  private URI       selectedURI = null;

  public void dispose() {
  }
  
  public void init(IWorkbenchWindow window) {
    workbenchWindow = window;
  }
  
  private String  filterExtFilter = "lola";
  
  public void run(IAction action) {
    if (!action.getId().equals(ID))
      return;

    // scan all files in the folder and take those that match our filter criteria
    final LinkedList<IFile> modelFiles = new LinkedList<IFile>();
    try {
      for (IResource r : selectedFolder.members()) {
        if (r instanceof IFile) {
          IFile f = (IFile)r;
          if (f.getFileExtension().equals(filterExtFilter))
            modelFiles.add(f);
        }
      }
    } catch (CoreException e) {
      Activator.getPluginHelper().logError("Could not read "+selectedFolder.getFullPath().toString(), e);
      MessageDialog.openError(workbenchWindow.getShell(), "Analyze Models",
          "Could not read "+selectedFolder.getFullPath().toString()+". See the error log for details.");
    }
    
    // now run a verification job in all collected files
    Job bpBuildJob = new Job("Batch Analysis Job") {
      @Override
      protected IStatus run(IProgressMonitor monitor) {
        
        boolean result = true;
        
        long totalStartTime = System.currentTimeMillis();
        long totalAnalysisTime = 0;
        long totalParseTime = 0;
        long totalPrepareTime = 0;
        
        int numUnsafe = 0;
        int numDeadlock = 0;
        int numUnsound = 0;
        
        StringBuilder log = new StringBuilder();
        
        monitor.beginTask("Analyzing models...", modelFiles.size());
        for (IFile modelFile : modelFiles) {

          monitor.setTaskName("Analyzing "+modelFile.getName());
          
          String processName = modelFile.getName().substring(
              modelFile.getName().lastIndexOf('_')+1,
              modelFile.getName().lastIndexOf('.'));
          
          log.append(processName+";");

          try {
            monitor.subTask("parsing input");
            BuildBP_LoLA buildBP = new BuildBP_LoLA(modelFile);
            buildBP.setWriteBPtoFile(false);
            buildBP.setPrintStatistics(false);
            
            // now run the analysis
            result &= buildBP.run(monitor, System.out);
            monitor.worked(1);
            
            DNodeBP bp = buildBP.getBranchingProcess();
            boolean hasDeadlock = bp.hasDeadCondition();
            boolean isUnsafe = !bp.isSafe();
            
            if (hasDeadlock) {
              numDeadlock++;
              log.append("udl");
            }
            if (isUnsafe) {
              numUnsafe++;
              log.append("uls");
            }
            if (hasDeadlock || isUnsafe) {
              numUnsound++;
            } else {
              log.append("s");
            }
            log.append(";");
            
            totalAnalysisTime += buildBP.getAnalysisTime();
            totalParseTime += buildBP.getParseTime();
            totalPrepareTime += buildBP.getPrepareTime();

            log.append(buildBP.getAnalysisTime()+";"+buildBP.getPrepareTime()+";");
            
            log.append(buildBP.getBranchingProcess().getStatistics());

          } catch (InvalidModelException e) {
            System.out.println("Erroneous input model '"+modelFile.getName()+"'. "+e.getMessage());
            
            if (e.getReason() == InvalidModelException.EMPTY_PRESET) {
              numUnsafe++;
              numUnsound++;
              log.append("uls;");
            }
          }
          
          log.append("\n");
          
          // check if the analysis was canceled
          if (!result || monitor.isCanceled())
            break;
        }
        monitor.done();

        int totalProc = modelFiles.size();
        
        System.out.println("------------------------------------------");
        System.out.println("Number of processes: "+totalProc);
        System.out.println("Number of sound: "+(totalProc-numUnsound));
        System.out.println("Number of unsound: "+(numUnsound)+", deadlocking: "+numDeadlock+", unsafe: "+numUnsafe);
        System.out.println("Total parse time: "+totalParseTime+"ms");
        System.out.println("Total prepare time: "+totalPrepareTime+"ms");
        System.out.println("Total analysis time: "+totalAnalysisTime+"ms");
        long totalEndTime = System.currentTimeMillis();
        System.out.println("Total run time: "+(totalEndTime-totalStartTime)+"ms");
        
        double avg = ((totalEndTime-totalStartTime)-totalParseTime) / modelFiles.size();
        System.out.println("Run time per process (after parsing): "+avg+"ms");
        
        FileIOHelper.writeFile(selectedFolder.getFullPath().removeTrailingSeparator()
            .addFileExtension("csv"), log.toString());
        
        if (result)
          return Status.OK_STATUS;
        else
          return Status.CANCEL_STATUS;
      }
    };
      
    bpBuildJob.setUser(true);
    bpBuildJob.schedule();
  }

  public void selectionChanged(IAction action, ISelection selection) {
    selectedURI = null;
    selectedFolder = null;
    action.setEnabled(false);
    if (selection instanceof IStructuredSelection == false
        || selection.isEmpty()) {
      return;
    }
    
    // store selection and URI, re-enable action
    try {
      selectedFolder = (IFolder) ((IStructuredSelection) selection).getFirstElement();
      selectedURI = URI.createPlatformResourceURI(selectedFolder.getFullPath()
            .toString(), true);
        action.setEnabled(true);
    } catch (ClassCastException e) {
      // just catch, do nothing
    }
  }
}
