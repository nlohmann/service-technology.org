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

import java.io.IOException;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;

import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.Oclet;
import hub.top.editor.eclipse.FileIOHelper;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.greta.run.Activator;
import hub.top.greta.run.actions.ActionHelper;
import hub.top.greta.verification.BuildBP;
import hub.top.greta.verification.IOUtil;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.petrinet.unfold.DNodeSys_PetriNet;
import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeSet;
import hub.top.uma.DNodeSys;
import hub.top.uma.DNodeSys_PtNet;
import hub.top.uma.InvalidModelException;
import hub.top.uma.synthesis.NetSynthesis.Diagnostic_Implements;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.gmf.runtime.emf.core.GMFEditingDomainFactory;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

/**
 * Synthesize a labeled Petri net from an {@link Oclet} specification.
 * 
 * @author Dirk Fahland
 */
public class SynthesizeLabeledNetAction implements IWorkbenchWindowActionDelegate {

  public static final String ID = "hub.top.GRETA.run.SynthesizeNetLabeled";
  
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
      
    final BuildBP build;
    try {
      build = new BuildBP(BuildBP.init(adaptiveSystem), selectedFile);
    } catch (InvalidModelException e) {
      ActionHelper.showMessageDialogue(MessageDialog.ERROR, 
          "Syntheszed Petri net", 
          "Failed to synthesize Petri net. "+e.getMessage());
      return;
    }
    
    Job bpBuildJob = new Job("Synthesizing Labeled Net") 
    {
      @Override
      protected IStatus run(IProgressMonitor monitor) {
        
        long t0 = System.currentTimeMillis();
        
        monitor.beginTask("Synthesizing Labeled Net", IProgressMonitor.UNKNOWN);

        // build branching process
        boolean interrupted = !build.run(monitor, System.out);
        
        long t1 = System.currentTimeMillis();
        System.out.println("completeted after "+(t1-t0)+"ms ");
        System.out.println(build.getBranchingProcess().getStatistics(false));
        
        if (!interrupted && selectedFile != null) {

          /*
          PtNet net = NetSynthesis.foldToNet_labeled(build.getBranchingProcess());
          IPath targetPath = selectedFile.getFullPath().removeFileExtension().addFileExtension("ptnet");
          */
          
          // build.writeBPtoFile(monitor, null, "_synbp");
          
          /*
          TransactionalEditingDomain editing = GMFEditingDomainFactory.INSTANCE.createEditingDomain();
          FileIOHelper.writeEObjectToResource(net, editing, targetPath);
          */
          
          try {
            monitor.subTask("generating labeled net");
            
            hub.top.uma.synthesis.NetSynthesis synth = new hub.top.uma.synthesis.NetSynthesis(build.getBranchingProcess());

            hub.top.petrinet.PetriNet net2 = synth.foldToNet_labeled();
            //String netFileName = selectedFile.getRawLocation().removeFileExtension().toString();
            //hub.top.petrinet.PetriNetIO.writeToFile(net2, netFileName, hub.top.petrinet.PetriNetIO.FORMAT_LOLA, 0);
            IPath targetPath = selectedFile.getFullPath().removeFileExtension().addFileExtension("lola");
            ActionHelper.writeFile(targetPath, hub.top.petrinet.PetriNetIO_Out.toLoLA(net2));
            /*
            for (Place p : net2.getPlaces()) {
              if (p.getName().startsWith("controller"))
                p.addRole("controller");
              if (p.getName().startsWith("Floor1Btn"))
                p.addRole("Floor1Btn");
              if (p.getName().startsWith("Floor2Btn"))
                p.addRole("Floor2Btn");
              if (p.getName().startsWith("Floor3Btn"))
                p.addRole("Floor3Btn");
              if (p.getName().startsWith("e.floor") || p.getName().startsWith("e.doors"))
                p.addRole("elevator");
              if (p.getName().startsWith("user"))
                p.addRole("user");
            }
            net2.spreadRolesToTransitions_union();
            net2.setRoles_unassigned();
            hub.top.petrinet.PetriNetIO.writeToFile(net2, selectedFile.getRawLocation().removeFileExtension().toString(), hub.top.petrinet.PetriNetIO.FORMAT_DOT, hub.top.petrinet.PetriNetIO.PARAM_SWIMLANE);
             */
            try {
             monitor.subTask("comparing synthesized net and specification");
             DNodeBP dbp = build.getBranchingProcess();
             Diagnostic_Implements diagnostics = hub.top.uma.synthesis.NetSynthesis.doesImplement(net2, dbp);
              
             if (diagnostics.result != hub.top.uma.synthesis.NetSynthesis.COMPARE_EQUAL) {
               IOUtil.writeDotFile(diagnostics.dbp_oclets, selectedFile, "_syn_covered1");
               IOUtil.writeDotFile(diagnostics.dbp_net, selectedFile, "_syn_covered2");
             }
              
            } catch (InvalidModelException e) {
              Activator.getPluginHelper().logError("Failed to compare net and specification.", e);
            }
            
            hub.top.petrinet.PetriNet eq_cust = computeComponent(net2, 
                new String[] { 
                "cust", "eq[cust]", "eq", 
                "car", "car[need]", "car[done]", 
                "maps", "maps[need]", "maps[done]", "map", "map req",
                "request vehicle", "granted vehicle", "jeep", 
                "support", "letter of recommendation", 
                "req missing docs", "missing docs" } );
            hub.top.petrinet.PetriNetIO_Out.writeToFile(eq_cust, selectedFile.getRawLocation().removeFileExtension().toString()+"_eq", hub.top.petrinet.PetriNetIO_Out.FORMAT_LOLA, 0);

            hub.top.petrinet.PetriNet gfz = computeComponent(net2, 
                new String[] { 
                "GFZ",
                "support", "letter of recommendation", 
                "req missing docs", "missing docs" } );
            hub.top.petrinet.PetriNetIO_Out.writeToFile(gfz, selectedFile.getRawLocation().removeFileExtension().toString()+"_gfz", hub.top.petrinet.PetriNetIO_Out.FORMAT_LOLA, 0);

            hub.top.petrinet.PetriNet partner = computeComponent(net2, 
                new String[] { 
                "partner", "partner[avail]",
                "request vehicle", "granted vehicle", "map", "map req" });
            hub.top.petrinet.PetriNetIO_Out.writeToFile(partner, selectedFile.getRawLocation().removeFileExtension().toString()+"_partner", hub.top.petrinet.PetriNetIO_Out.FORMAT_LOLA, 0);

            /*
            hub.top.petrinet.PetriNet net_controller = new hub.top.petrinet.PetriNet(net2);
            String[] keep = new String[] { "controller" };
            removeAllNodesNotIn(net_controller, keep);
            net_controller.removeParallelTransitions();
            net_controller.removeIsolatedNodes();
            hub.top.petrinet.PetriNetIO_out.writeToFile(net_controller, selectedFile.getRawLocation().removeFileExtension().toString()+"_controller", hub.top.petrinet.PetriNetIO_out.FORMAT_LOLA, 0);

            hub.top.petrinet.PetriNet net_Floor1Btn = new hub.top.petrinet.PetriNet(net2);
            keep = new String[] { "Floor1Btn" };
            removeAllNodesNotIn(net_Floor1Btn, keep);
            net_Floor1Btn.removeParallelTransitions();
            net_Floor1Btn.removeIsolatedNodes();
            hub.top.petrinet.PetriNetIO_out.writeToFile(net_Floor1Btn, selectedFile.getRawLocation().removeFileExtension().toString()+"_Floor1Btn", hub.top.petrinet.PetriNetIO_out.FORMAT_LOLA, 0);
            
            hub.top.petrinet.PetriNet net_Floor2Btn = new hub.top.petrinet.PetriNet(net2);
            keep = new String[] { "Floor2Btn" };
            removeAllNodesNotIn(net_Floor2Btn, keep);            
            net_Floor2Btn.removeParallelTransitions();
            net_Floor2Btn.removeIsolatedNodes();
            hub.top.petrinet.PetriNetIO_out.writeToFile(net_Floor2Btn, selectedFile.getRawLocation().removeFileExtension().toString()+"_Floor2Btn", hub.top.petrinet.PetriNetIO_out.FORMAT_LOLA, 0);

            hub.top.petrinet.PetriNet net_Floor3Btn = new hub.top.petrinet.PetriNet(net2);
            keep = new String[] { "Floor3Btn" };
            removeAllNodesNotIn(net_Floor3Btn, keep);            
            net_Floor3Btn.removeParallelTransitions();
            net_Floor3Btn.removeIsolatedNodes();
            hub.top.petrinet.PetriNetIO_out.writeToFile(net_Floor3Btn, selectedFile.getRawLocation().removeFileExtension().toString()+"_Floor3Btn", hub.top.petrinet.PetriNetIO_out.FORMAT_LOLA, 0);

            hub.top.petrinet.PetriNet net_elevator = new hub.top.petrinet.PetriNet(net2);
            keep = new String[] { "e.floor", "e.doors" };
            removeAllNodesNotIn(net_elevator, keep);            
            net_elevator.removeParallelTransitions();
            net_elevator.removeIsolatedNodes();
            hub.top.petrinet.PetriNetIO_out.writeToFile(net_elevator, selectedFile.getRawLocation().removeFileExtension().toString()+"_elevator", hub.top.petrinet.PetriNetIO_out.FORMAT_LOLA, 0);

            hub.top.petrinet.PetriNet net_user = new hub.top.petrinet.PetriNet(net2);
            keep = new String[] { "user" };
            removeAllNodesNotIn(net_user, keep);            
            net_user.removeParallelTransitions();
            net_user.removeIsolatedNodes();
            hub.top.petrinet.PetriNetIO_out.writeToFile(net_user, selectedFile.getRawLocation().removeFileExtension().toString()+"_user", hub.top.petrinet.PetriNetIO_out.FORMAT_LOLA, 0);
          */
          } catch (IOException e) {
            Activator.getPluginHelper().logError("Could not write Petri net plain text file", e);
          }
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
  
  public static hub.top.petrinet.PetriNet computeComponent(hub.top.petrinet.PetriNet net, String[] keep) {
    hub.top.petrinet.PetriNet net_component = new hub.top.petrinet.PetriNet(net);
    removeAllNodesNotIn(net_component, keep);            
    net_component.removeParallelTransitions();
    net_component.removeIsolatedNodes();
    return net_component;
  }
  
  public static void removeAllNodesNotIn(hub.top.petrinet.PetriNet net, String[] keep) {
    LinkedList<hub.top.petrinet.Node> toRemove = new LinkedList<hub.top.petrinet.Node>();
    for (hub.top.petrinet.Place p : net.getPlaces()) {
      boolean containsKeep = false;
      for (String s : keep) {
        if (p.getName().startsWith(s)) {
          containsKeep = true; break;
        }
      }
      if (!containsKeep) toRemove.add(p);
    }
    
    for (hub.top.petrinet.Transition t : net.getTransitions()) {
      boolean neededTransitionPre = false;
      boolean neededTransitionPost = false;
      for (hub.top.petrinet.Place p : t.getPreSet()) {
        if (!toRemove.contains(p)) {
          neededTransitionPre = true; break;
        }
      }
      for (hub.top.petrinet.Place p : t.getPostSet()) {
        if (!toRemove.contains(p)) {
          neededTransitionPost = true;
        }
      }
      if (!neededTransitionPre || !neededTransitionPost) toRemove.add(t);
    }
    
    for (hub.top.petrinet.Node n : toRemove) {
      if (n instanceof hub.top.petrinet.Place) net.removePlace((hub.top.petrinet.Place)n);
      else if (n instanceof hub.top.petrinet.Transition) net.removeTransition((hub.top.petrinet.Transition)n);
    }
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
