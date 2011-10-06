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

import hub.top.editor.ptnetLoLA.Arc;
import hub.top.editor.ptnetLoLA.Node;
import hub.top.editor.ptnetLoLA.PNAPI;
import hub.top.editor.ptnetLoLA.Place;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.editor.ptnetLoLA.Transition;
import hub.top.uma.DNodeBP;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.util.URI;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

public class CompareBranchingPTnet implements IWorkbenchWindowActionDelegate {

  public static final String ID = "hub.top.GRETA.run.compareBranchingProcess_ptnet";
  
  private IWorkbenchWindow workbenchWindow;

  // fields for tracking the selection in the explorer 
  private URI   selectedURI1 = null;
  private URI   selectedURI2 = null;
  
  public void dispose() {
    // TODO Auto-generated method stub

  }

  public void init(IWorkbenchWindow window) {
    workbenchWindow = window;
  }
  
  private static boolean comparableNames(String name1, String name2) {
    int end1 = name1.indexOf('[');
    int end2 = name2.indexOf('[');
    return name1.substring(0, end1).equals(name2.substring(0, end2));
  }
  
  private void compare(URI uri1, URI uri2) {
    // seek system to check from a given URI
    PtNet net1 = ActionHelper.getPtNet(uri1);
    PtNet net2 = ActionHelper.getPtNet(uri2);
    
    HashSet<Node> cutNodes1 = new HashSet<Node>();
    HashSet<Node> cutNodes2 = new HashSet<Node>();
    
    LinkedList<Node> min1 = new LinkedList<Node>();
    for (Place p : net1.getPlaces()) {
      if (p.getPreSet().size() == 0) min1.addLast(p);
    }
    
    HashMap<Node, Node> match = new HashMap<Node, Node>();
    HashMap<Node, Node> matchInv = new HashMap<Node, Node>();
    for (Node n : min1) {
      for (Place p2 : net2.getPlaces()) {
        if (p2.getPreSet().size() > 0) continue;
        if (comparableNames(n.getName(), p2.getName())) {
          match.put(n, p2);
          matchInv.put(p2, n);
        }
      }
    }
    
    HashMap<Node, HashSet<Node>> misMatch = new HashMap<Node, HashSet<Node>>();
    HashMap<Node, HashSet<Node>> multiMatch = new HashMap<Node, HashSet<Node>>();
    LinkedList<Node> notFound = new LinkedList<Node>();
    
    LinkedList<Node> queue = new LinkedList<Node>(match.keySet());
    while (!queue.isEmpty()) {
      Node n = queue.removeFirst();
      Node n2 = match.get(n);
      
      for (Node post : n.getPostSet()) {
        
        boolean allPre = true;
        for (Node postPre : post.getPreSet()) {
          if (!match.containsKey(postPre)) {
            allPre = false; break;
          }
        }
        // cannot match post yet because not all predecessors have been
        // matched
        if (!allPre) continue;
        
        boolean found = false;
        for (Node post2 : n2.getPostSet()) {
          if (comparableNames(post.getName(), post2.getName())) {
            
            if (post.getPreSet().size() != post2.getPreSet().size())
              continue;
            
            allPre = true;
            for (Node postPre : post.getPreSet()) {
              if (!post2.getPreSet().contains(match.get(postPre))) {
                allPre = false;
                break;
              }
            }
            if (!allPre)
              continue;
            
            if (!match.containsKey(post) && !match.containsValue(post2)) {
              // visiting for the first time, add to mapping
              match.put(post, post2);
              matchInv.put(post2, post);
              queue.addLast(post);
            } else if (match.containsKey(post) && !match.containsValue(post2)) {
              // visiting again: improper matching, store mismatches
              if (!multiMatch.containsKey(post2)) multiMatch.put(post2, new HashSet<Node>());
              multiMatch.get(post2).add(post);
              multiMatch.get(post2).add(matchInv.get(post2));
              
            } else if (match.containsKey(post) && match.get(post) == post2) {
              // visiting again with proper matching: do nothing

            } else if (match.containsKey(post) && match.get(post) != post2) {
              // visiting again: improper matching, store mismatches
              if (!misMatch.containsKey(post)) misMatch.put(post, new HashSet<Node>());
              misMatch.get(post).add(post2);
              misMatch.get(post).add(match.get(post));
            }
            found = true;
            break;
          }
        }
        if (!found) notFound.add(post);
      }
    }
    
    for (Node n : notFound) {
      if (n.getName().startsWith("CUT("))
        cutNodes1.add(n);
    }
    
    LinkedList<Node> notFound2 = new LinkedList<Node>();
    for (Place p : net2.getPlaces()) {
      if (!match.containsValue(p)) {
        notFound2.add(p);
        if (p.getName().startsWith("CUT(")) cutNodes2.add(p);
      }
    }
    for (Transition t : net2.getTransitions()) {
      if (!match.containsValue(t)) {
        notFound2.add(t);
        if (t.getName().startsWith("CUT(")) cutNodes2.add(t);
      }
    }
    
    System.out.println("N1: "+uri1);
    System.out.println("N2: "+uri2);
    System.out.println("number of matches: "+match.size());
    System.out.println("number of mismatches: "+misMatch.size());
    System.out.println("number of multi-matches: "+multiMatch.size());
    System.out.println("nodes of N1 not found: "+notFound.size()+" of "+(net1.getPlaces().size()+net1.getTransitions().size()));
    System.out.println("cut-off nodes: "+cutNodes1.size());
    System.out.println("nodes of N2 not found: "+notFound2.size()+" of "+(net2.getPlaces().size()+net2.getTransitions().size()));
    System.out.println("cut-off nodes: "+cutNodes2.size());
    
    if (misMatch.size() == 0 && multiMatch.size() == 0 && notFound.size() == 0 && notFound2.size() == 0)
      return;
    
    queue.clear();
    
    PtNet newNet = PtnetLoLAFactory.eINSTANCE.createPtNet();
    HashMap<Node, Node> copyMap = new HashMap<Node, Node>();

    
    for (Node notFoundNode : notFound) {
      queue.add(notFoundNode);
      if (notFoundNode instanceof Transition) {
        Transition t = PtnetLoLAFactory.eINSTANCE.createTransition();
        t.setName(notFoundNode.getName());
        copyMap.put(notFoundNode, t);
        newNet.getTransitions().add(t);
      } else {
        Place p = PtnetLoLAFactory.eINSTANCE.createPlace();
        p.setName(notFoundNode.getName());
        copyMap.put(notFoundNode, p);
        newNet.getPlaces().add(p);
      }
    }
    
    while (!queue.isEmpty()) {
      Node n = queue.removeFirst();
      Node other = copyMap.get(n);
      for (Node pre : n.getPreSet()) {
        Node preOther;
        
        if (copyMap.containsKey(pre)) {
          preOther = copyMap.get(pre);
        } else {
          if (pre instanceof Transition) {
            preOther = PtnetLoLAFactory.eINSTANCE.createTransition();
            newNet.getTransitions().add((Transition)preOther);
          } else {
            preOther = PtnetLoLAFactory.eINSTANCE.createPlace();
            newNet.getPlaces().add((Place)preOther);
          }
          copyMap.put(pre, preOther);
          preOther.setName(pre.getName());
          queue.addLast(pre);
        }
        
        if (preOther instanceof Transition) {
          Arc a = PtnetLoLAFactory.eINSTANCE.createArcToPlace();
          a.setSource(preOther);
          a.setTarget(other);
          newNet.getArcs().add(a);
        } else {
          Arc a = PtnetLoLAFactory.eINSTANCE.createArcToTransition();
          a.setSource(preOther);
          a.setTarget(other);
          newNet.getArcs().add(a);
        }
      }
    }
    
    String fileName = uri1.trimFileExtension().lastSegment();
    fileName += "_VS_"+uri2.trimFileExtension().lastSegment();
    
    URI targetURI = uri1.trimFileExtension().trimSegments(1).appendSegment(fileName).appendFileExtension("dot"); 
    IPath targetPath = new Path(targetURI.toPlatformString(true));
    ActionHelper.writeFile (targetPath, PNAPI.toDot(newNet));
  }
  
  public void run(IAction action) {
    if (!action.getId().equals(ID))
      return;
    if (selectedURI1 == null) return;
    if (selectedURI2 == null) return;
    
    compare(selectedURI1, selectedURI2);
    compare(selectedURI2, selectedURI1);
  }

  public void selectionChanged(IAction action, ISelection selection) {
      selectedURI1 = null;
      selectedURI2 = null;
      action.setEnabled(false);
      if (selection instanceof IStructuredSelection == false
          || selection.isEmpty()) {
        return;
      }
      try {
        LinkedList<IFile> selectedFiles = new LinkedList<IFile>();
        Iterator it = ((IStructuredSelection) selection).iterator();
        while (it.hasNext()) {
          Object o = it.next();
          if (o instanceof IFile) {
            selectedFiles.add((IFile)o);
          }
        }
        
        if (selectedFiles.size() != 2) {
          return;
        }

        selectedURI1 = URI.createPlatformResourceURI(selectedFiles.get(0)
            .getFullPath().toString(), true);
        selectedURI2 = URI.createPlatformResourceURI(selectedFiles.get(1)
            .getFullPath().toString(), true);

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
