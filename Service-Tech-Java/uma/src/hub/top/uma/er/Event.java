/*****************************************************************************\
 * Copyright (c) 2011-2012 All rights reserved. Dirk Fahland. AGPL3.0
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

package hub.top.uma.er;

import java.util.LinkedList;

import com.google.gwt.dev.util.collect.HashSet;

import hub.top.uma.DNode;

/**
 * Event of an event structure.
 *
 * @author dfahland
 */
public class Event extends DNode {
  
  public boolean originalModelEvent = false;
  
  public int level = -1;

  public Event(short id, int preSize) {
    super(id, preSize);
    this.isEvent = true;
  }
  
  public Event(short id, DNode pre) {
    super(id, pre);
    this.isEvent = true;
  }
  
  public Event(short id, DNode pre[]) {
    super(id, pre);
    this.isEvent = true;
  }
  
  public boolean hasPred(Event e) {
    for (int i=0;i<pre.length;i++) {
      if (pre[i] == e) return true;
      if (pre[i].id > e.id) return false;
    }
    return false;
  }
  
}
