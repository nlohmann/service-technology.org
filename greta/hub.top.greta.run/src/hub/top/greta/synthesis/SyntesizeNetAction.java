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

package hub.top.greta.synthesis;

import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.Oclet;
import hub.top.editor.eclipse.FileIOHelper;
import hub.top.editor.ptnetLoLA.Arc;
import hub.top.editor.ptnetLoLA.Node;
import hub.top.editor.ptnetLoLA.Place;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.editor.ptnetLoLA.PtnetLoLAPackage;
import hub.top.editor.ptnetLoLA.Transition;
import hub.top.greta.oclets.canonical.DNode;
import hub.top.greta.oclets.canonical.DNodeBP;
import hub.top.greta.oclets.canonical.DNodeSet;
import hub.top.greta.oclets.canonical.DNodeSys;
import hub.top.greta.run.actions.ActionHelper;

import java.util.HashMap;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.gmf.runtime.emf.core.GMFEditingDomainFactory;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

/**
 * Synthesize a P/T net from an {@link Oclet} specification.
 * 
 * @author Dirk Fahland
 */
public class SyntesizeNetAction implements IWorkbenchWindowActionDelegate {

  public static final String ID = "hub.top.GRETA.run.convertToNet";
  
  private IWorkbenchWindow workbenchWindow;

  private AdaptiveSystem adaptiveSystem;
  
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
      
    final DNodeBP bp = new DNodeBP(adaptiveSystem);
    Job bpBuildJob = new Job("Folding to P/T net") 
    {
      @Override
      protected IStatus run(IProgressMonitor monitor) {
        
        monitor.beginTask("Folding to P/T net", IProgressMonitor.UNKNOWN);
        
        int steps = 0;
        int current_steps = 0;
        int num = 0;
        
        boolean interrupted = false;
        long tStart = System.currentTimeMillis();
        while ((current_steps = bp.step()) > 0) {
          steps += current_steps;

          monitor.subTask("explored "+steps+" events");
          
          if (monitor.isCanceled()) {
            interrupted = true;
            break;
          }
        }
        long tEnd = System.currentTimeMillis();
        
        if (!interrupted && selectedFile != null) {
          monitor.subTask("generating P/T net");

          try {
            bp.minimize();
            bp.relax();
          } catch (NullPointerException e) {
            
          }
          
          DNodeSys dAS = bp.getSystem();
          DNodeSet dBP = bp.getBranchingProcess();
          
          HashMap<DNode, DNode> equiv = bp.getElementary_ccPair();
          
          HashMap<DNode, Node> d2n = new HashMap<DNode, Node>();
          
          PtnetLoLAFactory fact = PtnetLoLAPackage.eINSTANCE.getPtnetLoLAFactory();
          PtNet net = fact.createPtNet();
          
          for (DNode b: dBP.getAllConditions()) {
            if (!b.isCutOff || equiv.get(b) == b) {
              Place p = fact.createPlace();
              p.setName(dAS.properNames[b.id]+"_"+b.globalId);
              net.getPlaces().add(p);
              
              if (b.pre == null || b.pre.length == 0)
                p.setToken(1);
              
              d2n.put(b, p);
            }
          }
          
          // now map each condition that has an equivalent counterpart to
          // the place that represents this counterpart
          for (DNode b: dBP.getAllConditions()) {
            if (b.isCutOff && !b.isAnti) {
              d2n.put(b, d2n.get(equiv.get(b)));
            }
          }

          for (DNode e : dBP.getAllEvents()) {
            
            if (!e.isCutOff || equiv.get(e) == e) {
              Transition t = fact.createTransition();
              t.setName(dAS.properNames[e.id]+"_"+e.globalId);
              net.getTransitions().add(t);
              d2n.put(e, t);
            }
          }
          
          for (DNode e : dBP.getAllEvents()) {
            
            if (e.isAnti)
              continue;
            
            Transition t = null;
            if (!e.isCutOff)
              t = (Transition) d2n.get(e);
            else {
              DNode e2 = equiv.get(e);
              if (e2 == null) {
                t = fact.createTransition();
                t.setName(dAS.properNames[e.id]+"_"+e.globalId);
                net.getTransitions().add(t);
                d2n.put(e, t);
              } else {
                t = (Transition)d2n.get(e2);
              }
            }
            
            for (DNode b : e.pre) {
              Place p = (Place)d2n.get(b);
              if (t.getPreSet().contains(p))
                continue;
              
              Arc a = fact.createArcToTransition();
              a.setSource(p);
              a.setTarget(t);
              
              net.getArcs().add(a);
            }
            
            for (DNode b : e.post) {
              Place p = (Place)d2n.get(b);
              if (t.getPostSet().contains(p))
                continue;

              Arc a = fact.createArcToPlace();
              a.setSource(t);
              a.setTarget(p);
              
              net.getArcs().add(a);
            }
          }
          
          
          IPath targetPath = selectedFile.getFullPath().removeFileExtension().addFileExtension("ptnet");
          
          TransactionalEditingDomain editing = GMFEditingDomainFactory.INSTANCE.createEditingDomain();
          FileIOHelper.writeEObjectToResource(net, editing, targetPath);
        }
        monitor.done();
        
        if (interrupted)
          return Status.CANCEL_STATUS;
        else
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
      // store selection and URI, re-enable action
      selectedFile = (IFile) ((IStructuredSelection) selection).getFirstElement();
      selectedURI = URI.createPlatformResourceURI(selectedFile.getFullPath()
          .toString(), true);
      action.setEnabled(true);
    } catch (ClassCastException e) {
      // just catch, do nothing
    }
  }
}
