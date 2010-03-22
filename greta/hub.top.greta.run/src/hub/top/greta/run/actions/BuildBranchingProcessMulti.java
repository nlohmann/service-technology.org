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
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.Oclet;
import hub.top.editor.eclipse.FileIOHelper;
import hub.top.editor.eclipse.ResourceHelper;
import hub.top.editor.ptnetLoLA.Node;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.greta.oclets.canonical.DNodeBP;
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
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

public class BuildBranchingProcessMulti implements IWorkbenchWindowActionDelegate {

	public static final String ID = "hub.top.GRETA.run.buildBranchingProcess_multi";
	
	private IWorkbenchWindow workbenchWindow;

	private AdaptiveSystem adaptiveSystem;
	
	// fields for tracking the selection in the explorer 
	private IFile 	selectedFile = null;
	private URI 	selectedURI = null;
	
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
		adaptiveSystem = ActionHelper.getAdaptiveSystem(selectedURI);
		
		// if there was no system at the given URI, check the current editor
		if (adaptiveSystem == null) {
			IEditorPart editor = workbenchWindow.getActivePage().getActiveEditor();
			adaptiveSystem = ActionHelper.getAdaptiveSystem(editor);
			
			// if this editor has input file, remember it
			if (editor.getEditorInput() != null
				&& editor.getEditorInput() instanceof IFileEditorInput)
			{
				selectedFile = ((IFileEditorInput)editor.getEditorInput()).getFile();
			}
		}
		
		if (adaptiveSystem == null)
			return;
		
    String fileName = selectedFile.getName();
    int paramStart = fileName.indexOf('(')+1;
    int paramEnd = fileName.indexOf(')');
    
    if (paramStart == 0 || paramEnd == -1)
      return; // this system cannot be instantiated
    
    final String parameter = fileName.substring(paramStart,paramEnd);
    System.out.println("instantiating "+fileName+", parameter: "+parameter);
    
    IInputValidator intValidator = new IInputValidator() {
    
      public String isValid(String newText) {
        if (newText == null || newText.length() == 0)
          return " ";
        try {
          int val = Integer.parseInt(newText);
          if (val < Integer.MIN_VALUE) return "Number out of bounds.";
          if (val > Integer.MAX_VALUE) return "Number out of bounds.";
          return null;
        } catch (NumberFormatException e) {
          return "Please enter an integer number.";
        }
      }
    };
    
    InputDialog minDiag = new InputDialog(null,
        "Instantiate parameter '"+parameter+"' in "+fileName,
        "minimal value for '"+parameter+"'", "1", intValidator);
    if (minDiag.open() == InputDialog.CANCEL)
      return;
    
    InputDialog maxDiag = new InputDialog(null, 
        "Instantiate parameter '"+parameter+"' in "+fileName,
        "maximal value for '"+parameter+"'", "1", intValidator);
    if (maxDiag.open() == InputDialog.CANCEL)
      return;
    
    final int paramRange_min = Integer.parseInt(minDiag.getValue());
    final int paramRange_max = Integer.parseInt(maxDiag.getValue());
    
    Job bpBatchJob = new Job("construct branching processes "+paramRange_min+".."+paramRange_max)
    {
      @Override
      protected IStatus run(IProgressMonitor monitor) {
        
        monitor.beginTask("constructing", (paramRange_max-paramRange_min));

        final StringBuilder sb = new StringBuilder();
        sb.append("#;events;conditions;arcs;cut-offs;time(ms);");

        int min=1;
        for (int max=paramRange_min; max <= paramRange_max; max++) {
          
          long start = System.currentTimeMillis();
        
          AdaptiveSystem inst = InstantiateSystem.instantiateSystem(adaptiveSystem, parameter, min, max);
          
          final BuildBP build = new BuildBP(inst, selectedFile);
          boolean interrupted = false;//!build.run(monitor, System.out);
          DNodeBP bp = build.getBranchingProcess();
          bp.getStatistics();
          
          long end = System.currentTimeMillis();

          sb.append(max+";"+bp.statistic_eventNum+";"+bp.statistic_condNum+";"+bp.statistic_arcNum+";"+bp.statistic_cutOffNum+";"+(end-start)+";");
          
          int numPlaces = 0;
          int numTransition = 0;
          int numArcs = 0;
          
          for (Oclet o : inst.getOclets()) {
            for (hub.top.adaptiveSystem.Node n : o.getDoNet().getNodes()) {
              if (n instanceof Event)
                numTransition++;
              else
                numPlaces++;
              numArcs += n.getPreSet().size();
            }
          }
          
          sb.append(numPlaces+";"+numTransition+";"+numArcs+";\n");
          
          //build.minimize(monitor, System.out);
          //build.writeBPtoFile(monitor, System.out, "_bp2_"+paramRange_min+"-"+paramRange_max);
            
          monitor.worked(1);
            
          if (interrupted)
            break;
        }
        
        String targetPathStr = selectedFile.getFullPath().removeFileExtension().toString();
        IPath targetPath = new Path(targetPathStr+"_bp.csv");
        FileIOHelper.writeFile(targetPath, sb.toString());

        monitor.done();
        return Status.OK_STATUS;
      }
    };
    
    bpBatchJob.setUser(true);
    bpBatchJob.schedule();

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
