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

package hub.top.scenario;

import java.util.LinkedList;

public class OcletSpecification {

  private Oclet initialRun;
  private LinkedList<Oclet> oclets;
  
  public OcletSpecification() {
    initialRun = null;
    oclets = new LinkedList<Oclet>();
  }
  
  public void addOclet(Oclet o) {
    oclets.add(o);
  }
  
  public LinkedList<Oclet> getOclets() {
    return oclets;
  }
  
  public void setInitialRun(Oclet o) {
    initialRun = o;
  }
  
  public Oclet getInitialRun() {
    return initialRun;
  }
}
