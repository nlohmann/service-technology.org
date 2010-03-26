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

package hub.top.editor.ptnetlola.pnml;

import java.util.LinkedList;
import java.util.List;

import org.eclipse.emf.common.util.BasicEMap;
import org.eclipse.emf.common.util.EMap;

import fr.lip6.move.pnml.ptnet.Arc;
import fr.lip6.move.pnml.ptnet.Page;
import fr.lip6.move.pnml.ptnet.Place;
import fr.lip6.move.pnml.ptnet.PlaceNode;
import fr.lip6.move.pnml.ptnet.PnObject;
import fr.lip6.move.pnml.ptnet.RefPlace;
import fr.lip6.move.pnml.ptnet.RefTransition;
import fr.lip6.move.pnml.ptnet.TransitionNode;
import fr.lip6.move.pnml.ptnet.hlapi.PetriNetDocHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PetriNetHLAPI;
import hub.top.editor.ptnetLoLA.Node;
import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.editor.ptnetLoLA.TransitionExt;
  
public class PNMLtoPT {

  private List<PtNet> ptnets;
  
  private PtNet currentNet;
  private EMap<PnObject, Node> translationMap;
  
  public PNMLtoPT() {
    ptnets = new LinkedList<PtNet>();
    currentNet = null;
  }
  
  public void process(PetriNetDocHLAPI root) {
    
    for (PetriNetHLAPI net : root.getNetsHLAPI()) {
      processNets(net);
    }
  }
  
  protected void processNets(PetriNetHLAPI net) {
    currentNet = PtnetLoLAFactory.eINSTANCE.createPtNet();
    ptnets.add(currentNet);
    translationMap = new BasicEMap<PnObject, Node>();
    
    for (Page p : net.getPages()) {
      processPageNodes(p);
      processPageArcs(p);
    }
  }
  
  protected void processPageNodes(Page page) {
    for (PnObject o : page.getObjects()) {
      if (o instanceof PlaceNode)
        processPlaceNode((PlaceNode)o);
      else if (o instanceof TransitionNode)
        processTransitionNode((TransitionNode)o);
      else if (o instanceof Page)
        processPageNodes((Page)o);
    }
  }
  
  protected void processPageArcs(Page page) {
    for (PnObject o : page.getObjects()) {
      if (o instanceof Arc)
        processArc((Arc)o);
      else if (o instanceof Page)
        processPageArcs((Page)o);
    }
  }

  
  protected void processPlaceNode(PlaceNode p) {
    if (p instanceof fr.lip6.move.pnml.ptnet.Place) {
      PlaceExt pNew = PtnetLoLAFactory.eINSTANCE.createPlaceExt();
      pNew.setName(processName(p));
      currentNet.getPlaces().add(pNew);
      
      translationMap.put(p, pNew);
    }
  }
  
  protected void processTransitionNode(TransitionNode t) {
    if (t instanceof fr.lip6.move.pnml.ptnet.Transition) {
      TransitionExt tNew = PtnetLoLAFactory.eINSTANCE.createTransitionExt();
      tNew.setName(processName(t));
      currentNet.getTransitions().add(tNew);
      
      translationMap.put(t, tNew);
    }
  }
  
  protected void processArc(Arc a) {
    fr.lip6.move.pnml.ptnet.Node src = a.getSource();
    fr.lip6.move.pnml.ptnet.Node tgt = a.getTarget();
    
    if (src instanceof RefPlace) {
      RefPlace ref = (RefPlace)src;
      src = ref.getRef();
    }
    
    if (tgt instanceof RefPlace) {
      RefPlace ref = (RefPlace)tgt;
      tgt = ref.getRef();
    }

    if (src instanceof RefTransition) {
      RefTransition ref = (RefTransition)src;
      src = ref.getRef();
    }
    
    if (tgt instanceof RefTransition) {
      RefTransition ref = (RefTransition)tgt;
      tgt = ref.getRef();
    }
    
    hub.top.editor.ptnetLoLA.Arc newArc = null;
    
    if (src instanceof Place)
      newArc = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
    else
      newArc = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
    
    Node newSrc = translationMap.get(src);
    Node newTgt = translationMap.get(tgt);
    
    if (newSrc == null) System.out.println(processName(src)+"->"+processName(tgt)+" not translated (src)");
    if (newTgt == null) System.out.println(processName(src)+"->"+processName(tgt)+" not translated (tgt)");
    
    //System.out.println(processName(src)+"->"+processName(tgt) + " ===> "+ newSrc.getName()+"->"+newTgt.getName());
    
    newArc.setSource(newSrc);
    newArc.setTarget(newTgt);
    
    currentNet.getArcs().add(newArc);
  }
  
  protected String processName(fr.lip6.move.pnml.ptnet.Node node) {
    String label = null;
    if (node.getName() != null && node.getName().getText().length() > 0)
      label = node.getName().getText()+"_"+node.getId();
    else
      label = node.getId();
    
    return label.replace(' ', '_');
  }

  
  public List<PtNet> getNets() {
    return ptnets;
  }
}
