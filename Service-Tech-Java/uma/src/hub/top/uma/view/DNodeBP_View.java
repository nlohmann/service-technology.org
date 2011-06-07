/*****************************************************************************\
 * Copyright (c) 2008, 2009, 2010. Dirk Fahland. AGPL3.0
 * All rights reserved. 
 * 
 * ServiceTechnology.org - Uma, an Unfolding-based Model Analyzer
 * 
 * This program and the accompanying materials are made available under
 * the terms of the GNU Affero General Public License Version 3 or later,
 * which accompanies this distribution, and is available at 
 * http://www.gnu.org/licenses/agpl.txt
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 * 
\*****************************************************************************/

package hub.top.uma.view;

import java.util.HashSet;
import java.util.LinkedList;

import hub.top.petrinet.unfold.DNodeSys_PetriNet;
import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeRefold;
import hub.top.uma.DNodeSet.DNodeSetElement;
import hub.top.uma.DNodeSys;
import hub.top.uma.Options;

/**
 * Adapted algorithm to construct an unfolding of a system with respect to a
 * set of traces.
 * 
 * @author dfahland
 */
public class DNodeBP_View extends DNodeRefold {

  public DNodeBP_View(DNodeSys system) {
    this(system, 1);
  }
  
  public DNodeBP_View(DNodeSys system, int bound) {
    super(system, new Options(system));
    
    getOptions().configure_buildOnly();
    getOptions().configure_setBound(bound);
    
    getOptions().configure_simplifyNet();
  }

  @Override
  protected EnablingInfo getAllEnabledEvents(DNodeSetElement fireableEvents) {
    DNodeSetElement filtered = new DNodeSetElement();
    for (DNode e : fireableEvents) {
      if (e.id == currentEventID) {
        filtered.add(e);
      }
    }
    
    EnablingInfo info = super.getAllEnabledEvents(filtered);
    
    SyncInfo locs[] = info.locations.get(currentEventID);
    LinkedList<SyncInfo> filteredLocs = new LinkedList<SyncInfo>();
    if (locs != null) {
      for (int i = 0; i<locs.length; i++) {
        if (locs[i] == null) break;
        
        boolean allInTrace = true;
        for (int j = 0; j<locs[i].loc.length; j++) {
          if (!currentTraceConditions.contains(locs[i].loc[j])) {
            allInTrace = false;
            break;
          }
        }
        if (allInTrace)
          filteredLocs.add(locs[i]);
      }
    }
    
    if (filteredLocs.size() > 0) {
      SyncInfo[] filteredLocs2 = filteredLocs.toArray(new SyncInfo[filteredLocs.size()]);
      info.locations.put(currentEventID, filteredLocs2);
    } else {
      info.locations.remove(currentEventID);
    }
    
    return info;
  }
  
  @Override
  protected DNode[] fireEnabledEvent(SyncInfo fireEvents) {
    DNode[] postCond = super.fireEnabledEvent(fireEvents);
    if (postCond == null) return null;
    
    for (DNode b : currentPrimeCut) {
      currentTraceConditions.add(b);
      DNode b2 = equivalentNode().get(b);
      if (b2 != null) currentTraceConditions.add(b2);
    }
    
    if (postCond.length > 0) {
      DNode e = postCond[0].pre[0];
      for (DNode b : e.pre)
        currentCut.remove(b);
      for (DNode b : postCond) {
        currentCut.add(b);
        DNode b2 = equivalentNode().get(b);
        if (b2 != null) currentCut.add(b2);
      }
    }
    
    for (DNode b : currentCut)
      b._isNew = true;
    
    return postCond;
  }
  
  private HashSet<DNode> currentTraceConditions;
  private HashSet<DNode> currentCut;
  private short currentEventID = -1;

  
  public void extendByTrace(String[] trace) {
    
    currentTraceConditions = new HashSet<DNode>();
    currentTraceConditions.addAll(this.bp.initialConditions);
    currentCut = new HashSet<DNode>();
    currentCut.addAll(this.bp.initialConditions);
    
    short[] idtrace = new short[trace.length];
    for (int i=0; i < idtrace.length; i++) {
      idtrace[i] = dNodeAS.nameToID.get(trace[i]);
    }
    
    for (int i=0; i<idtrace.length; i++) {
      currentEventID = idtrace[i];
      int fired = step();
      //System.out.print(trace[i]+"("+fired+") ");
    }
    //System.out.println();
  }
  
  public void extendByTraces(LinkedList<String[]> traces) {
    
    for (String[] trace : traces) {
      extendByTrace(trace); 
    }
  }
  
}
