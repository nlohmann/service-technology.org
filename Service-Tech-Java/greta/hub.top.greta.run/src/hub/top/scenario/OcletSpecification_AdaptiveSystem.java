/*****************************************************************************\
 * Copyright (c) 2008, 2009, 2010. Dirk Fahland. EPL1.0/AGPL3.0
 * All rights reserved. 
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

package hub.top.scenario;

import java.util.HashMap;

import hub.top.adaptiveSystem.AdaptiveSystem;

public class OcletSpecification_AdaptiveSystem {

  public static OcletSpecification toOcletSpecification(AdaptiveSystem as) {
    OcletSpecification os = new OcletSpecification();
    
    // copy all oclets
    for (hub.top.adaptiveSystem.Oclet o : as.getOclets()) {
      hub.top.scenario.Oclet oNew = new  hub.top.scenario.Oclet(
          o.getName(), 
          o.getOrientation() == hub.top.adaptiveSystem.Orientation.ANTI);
      
      os.addOclet(oNew);
      
      HashMap<hub.top.adaptiveSystem.Condition, hub.top.petrinet.Place> placeMap = new HashMap<hub.top.adaptiveSystem.Condition, hub.top.petrinet.Place>();
      
      // copy the conditions of the adaptive system oclet to the new oclet
      // ... for the history
      for (hub.top.adaptiveSystem.Node n : o.getPreNet().getNodes()) {
        if (n instanceof hub.top.adaptiveSystem.Event) continue;
        
        hub.top.petrinet.Place pNew = oNew.addPlace(n.getName(), true);
        placeMap.put((hub.top.adaptiveSystem.Condition)n, pNew);
      }
      // ... and for the contribution
      for (hub.top.adaptiveSystem.Node n : o.getDoNet().getNodes()) {
        if (n instanceof hub.top.adaptiveSystem.Event) continue;
        
        hub.top.petrinet.Place pNew = oNew.addPlace(n.getName(), false);
        placeMap.put((hub.top.adaptiveSystem.Condition)n, pNew);
      }

      // copy the events of the adaptive system oclet to the new oclet
      // ... for the history
      for (hub.top.adaptiveSystem.Node n : o.getPreNet().getNodes()) {
        if (!(n instanceof hub.top.adaptiveSystem.Event)) continue;
        
        hub.top.petrinet.Transition tNew = oNew.addTransition(n.getName(), true);
        for (hub.top.adaptiveSystem.Condition p : ((hub.top.adaptiveSystem.Event)n).getPreConditions()) {
          oNew.addArc(placeMap.get(p), tNew);
        }
        for (hub.top.adaptiveSystem.Condition p : ((hub.top.adaptiveSystem.Event)n).getPostConditions()) {
          oNew.addArc(tNew, placeMap.get(p));
        }
      }
      // ... and for the contribution
      for (hub.top.adaptiveSystem.Node n : o.getDoNet().getNodes()) {
        if (!(n instanceof hub.top.adaptiveSystem.Event)) continue;
        
        hub.top.petrinet.Transition tNew = oNew.addTransition(n.getName(), false);
        for (hub.top.adaptiveSystem.Condition p : ((hub.top.adaptiveSystem.Event)n).getPreConditions()) {
          oNew.addArc(placeMap.get(p), tNew);
        }
        for (hub.top.adaptiveSystem.Condition p : ((hub.top.adaptiveSystem.Event)n).getPostConditions()) {
          oNew.addArc(tNew, placeMap.get(p));
        }
      }
    }
    
    // copy the initial run
    hub.top.scenario.Oclet oInit = new  hub.top.scenario.Oclet("init", false);
    os.setInitialRun(oInit);
    
    HashMap<hub.top.adaptiveSystem.Condition, hub.top.petrinet.Place> placeMap = new HashMap<hub.top.adaptiveSystem.Condition, hub.top.petrinet.Place>();
    
    // copy the conditions of the adaptive process to the initial oclet
    for (hub.top.adaptiveSystem.Node n : as.getAdaptiveProcess().getNodes()) {
      if (n instanceof hub.top.adaptiveSystem.Event) continue;
      
      hub.top.petrinet.Place pNew = oInit.addPlace(n.getName(), false);
      placeMap.put((hub.top.adaptiveSystem.Condition)n, pNew);
      pNew.setTokens(((hub.top.adaptiveSystem.Condition)n).getToken());
    }

    // copy the events of the adaptive process to the intial oclet
    for (hub.top.adaptiveSystem.Node n : as.getAdaptiveProcess().getNodes()) {
      if (!(n instanceof hub.top.adaptiveSystem.Event)) continue;
      
      hub.top.petrinet.Transition tNew = oInit.addTransition(n.getName(), false);
      for (hub.top.adaptiveSystem.Condition p : ((hub.top.adaptiveSystem.Event)n).getPreConditions()) {
        oInit.addArc(placeMap.get(p), tNew);
      }
      for (hub.top.adaptiveSystem.Condition p : ((hub.top.adaptiveSystem.Event)n).getPostConditions()) {
        oInit.addArc(tNew, placeMap.get(p));
      }
    }
    
    return os;
  }
}
