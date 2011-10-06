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
import hub.top.adaptiveSystem.AdaptiveSystemFactory;
import hub.top.adaptiveSystem.AdaptiveSystemPackage;
import hub.top.adaptiveSystem.ArcToCondition;
import hub.top.adaptiveSystem.ArcToEvent;
import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.DoNet;
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.Node;
import hub.top.adaptiveSystem.Oclet;
import hub.top.adaptiveSystem.PreNet;
import hub.top.adaptiveSystem.Temp;
import hub.top.greta.run.actions.ActionHelper;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

public class UnfoldTransitiveDependenciesAction implements
    IWorkbenchWindowActionDelegate {
  
  public static final String ID = "hub.top.GRETA.run.synthesis.UnfoldTransitiveDependenciesAction";
  
  private IWorkbenchWindow workbenchWindow;

  private AdaptiveSystem adaptiveSystem;
  private Diagram        adaptiveSystemDiagram;
  
  public void dispose() {

  }

  public void init(IWorkbenchWindow window) {
    workbenchWindow = window;
  }

  public void run(IAction action) {
    if (!action.getId().equals(ID))
      return;
    
    IEditorPart editor = workbenchWindow.getActivePage().getActiveEditor();
    adaptiveSystemDiagram = ActionHelper.getAdaptiveSystemDiagram(editor);
    if (adaptiveSystemDiagram == null) return;
    
    adaptiveSystem = (AdaptiveSystem)adaptiveSystemDiagram.getElement();
    if (adaptiveSystem == null) return;
      
    if (!(editor.getEditorInput() instanceof IFileEditorInput))
      return;
    
    IFile in = ((IFileEditorInput)editor.getEditorInput()).getFile();

    EcoreUtil.Copier asCopier = new EcoreUtil.Copier();
    AdaptiveSystem as = (AdaptiveSystem)asCopier.copy(adaptiveSystem);
    asCopier.copyReferences();

    Map<String, HashSet<String> > preType = new HashMap<String, HashSet<String> >();
    Map<String, HashSet<String> > postType = new HashMap<String, HashSet<String> >(); 
    java.util.ArrayList<Event> events = new ArrayList<Event>();
    
    for (Oclet o : as.getOclets()) {
      for (Node n : o.getPreNet().getNodes()) {
        if (n instanceof Event) events.add((Event)n);
      }
      for (Node n : o.getDoNet().getNodes()) {
        if (n instanceof Event) events.add((Event)n);
        // initialize map for pre and post names of event names
        if (preType.get(n.getName()) == null) {
          preType.put(n.getName(), new HashSet<String>());
        }
        if (postType.get(n.getName()) == null) {
          postType.put(n.getName(), new HashSet<String>());
        }
      }
    }
    
    boolean dSucc[][] = new boolean[events.size()][events.size()];
    boolean succ[][] = new boolean[events.size()][events.size()];
    for (int i=0; i<events.size(); i++) {
      Event e = events.get(i);
      for (Condition b : e.getPostConditions()) {
        for (Event e2 : b.getPostEvents()) {
          int j = events.lastIndexOf(e2);
          dSucc[i][j] = true;
          succ[i][j] = true;
        }
        // add name of post-condition to post names of this event's name
        postType.get(e.getName()).add(b.getName());
      }
      for (Condition b : e.getPreConditions()) {
        preType.get(e.getName()).add(b.getName());
      }
    }
    
    for (int i=0; i<succ.length; i++) {
      for (int j=i+1; j<succ.length; j++) {
        if (succ[i][j])
          System.out.println(events.get(i)+"->"+events.get(j));
      }
    }
    
    System.out.println("unfold dependencies:");
    
    int added = 0;
    do {
      added = 0;
      for (int i=0; i<succ.length; i++) {
        for (int j=i+1; j<succ.length; j++) {
          for (int k=j+1; k<succ.length; k++) {
            if (succ[i][j] && succ[j][k]) {
              if (!succ[i][k]) {
                succ[i][k] = true;
                added++;
              }
            }
          }
        }
      }
    } while (added > 0);
    
    AdaptiveSystemFactory fact = AdaptiveSystemPackage.eINSTANCE.getAdaptiveSystemFactory();
    for (int i=0; i<succ.length; i++) {
      for (int j=i+1; j<succ.length; j++) {
        if (succ[i][j] && !dSucc[i][j]) {
          Event ei = events.get(i), ej = events.get(j);
          System.out.println("new dependency "+ei.getName()+"->"+ej.getName());
          Condition b = fact.createCondition();
          b.setName(ei.getName()+"_"+ej.getName());
          
          // add name of new post-condition to post names of ei's name
          postType.get(ei.getName()).add(b.getName());
          preType.get(ej.getName()).add(b.getName());
          
          ArcToCondition arc1 = fact.createArcToCondition();
          arc1.setSource(ei); arc1.setDestination(b);
          ArcToEvent arc2 = fact.createArcToEvent();
          arc2.setSource(b); arc2.setDestination(ej);
          
          if (ei.eContainer() instanceof DoNet) {
            DoNet net = (DoNet)ei.eContainer();
            net.getNodes().add(b);
            net.getArcs().add(arc1);
            net.getArcs().add(arc2);
            b.setTemp(Temp.COLD);
            
          } else {
            PreNet net = (PreNet)ei.eContainer();
            net.getNodes().add(b);
            net.getArcs().add(arc1);
            net.getArcs().add(arc2);
            if (ej instanceof DoNet) {
              b.setToken(1);
              b.setTemp(Temp.WITHOUT);
            }
          }
        }
      }
    }
    
    for (int i=0; i<succ.length; i++) {
      Event ei = events.get(i);
      if (!(ei.eContainer() instanceof DoNet)) continue;
      DoNet net = (DoNet) ei.eContainer();
      
      for (String postName : postType.get(ei.getName())) {
        boolean exists = false;
        for (Condition b : ei.getPostConditions()) {
          if (b.getName().equals(postName)) {
            exists = true; break; 
          }
        }
        if (!exists) {
          Condition b = fact.createCondition();
          b.setName(postName);
          b.setTemp(Temp.COLD);
          
          ArcToCondition arc1 = fact.createArcToCondition();
          arc1.setSource(ei); arc1.setDestination(b);
          
          net.getNodes().add(b);
          net.getArcs().add(arc1);
        }
      }
      
      for (String preName : preType.get(ei.getName())) {
        boolean exists = false;
        for (Condition b : ei.getPreConditions()) {
          if (b.getName().equals(preName)) {
            exists = true; break; 
          }
        }
        if (!exists) {
          Condition b = fact.createCondition();
          b.setName(preName);
          b.setTemp(Temp.WITHOUT);
          b.setToken(1);
          
          ArcToEvent arc1 = fact.createArcToEvent();
          arc1.setSource(b); arc1.setDestination(ei);

          PreNet preNet = ((Oclet)net.eContainer()).getPreNet();
          preNet.getNodes().add(b);
          preNet.getArcs().add(arc1);
        }
      }

    }
    
    /*
    EcoreUtil.Copier diagramCopier = new EcoreUtil.Copier();
    Diagram dCopy = (Diagram)diagramCopier.copy(adaptiveSystemDiagram);
    diagramCopier.copyReferences();
     */
    
    String modelName = in.getFullPath().removeFileExtension().lastSegment();
    URI modelTargetURI = URI.createFileURI(in.getFullPath().removeLastSegments(1).append(modelName+"_unf").addFileExtension("adaptivesystem").toString());
    URI diagramTargetURI = URI.createFileURI(in.getFullPath().removeLastSegments(1).append(modelName+"_unf").addFileExtension("adaptivesystem_diagram").toString());
    System.out.println("write to "+diagramTargetURI);
    ActionHelper.writeEcoreResourceToFile(workbenchWindow, modelTargetURI, as);
    //ActionHelper.writeEcoreResourceToFile(workbenchWindow, diagramTargetURI, dCopy);

  }


  public void selectionChanged(IAction action, ISelection selection) {
    // TODO Auto-generated method stub

  }

}
