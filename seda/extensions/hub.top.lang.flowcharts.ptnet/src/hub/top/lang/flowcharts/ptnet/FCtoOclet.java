/*****************************************************************************\
 * Copyright (c) 2009 Dirk Fahland. EPL1.0/AGPL3.0
 * All rights reserved.
 * 
 * ServiceTechnolog.org - Modeling Languages
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
 * The Original Code is this file as it was released on December 12, 2009.
 * The Initial Developer of the Original Code are
 *    Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2009
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

package hub.top.lang.flowcharts.ptnet;

import hub.top.lang.flowcharts.DiagramArc;
import hub.top.lang.flowcharts.DiagramNode;
import hub.top.lang.flowcharts.StartNode;

import org.eclipse.emf.ecore.EObject;

/**
 * TODO: refactor to a separate plugin
 * 
 * @author Dirk Fahland
 */
public class FCtoOclet extends FCtoPTnet {

  public FCtoOclet(EObject modelObject) {
    super(modelObject);
  }

  @Override
  protected void translateAllChildArcs() {
    for (DiagramArc arc : wfDiagram.getDiagramArcs()) {
      // translate all arcs of wfDiagram
     
      DiagramNode src = arc.getSrc();
      DiagramNode tgt = arc.getTarget();
     
      if (src == null || tgt == null) throw new NullPointerException("Error in FCtoPTnet while translating arc: arc has no source- or targetNode in Diagram!");
     
      if ( (src instanceof StartNode && src.getLabel() != null && src.getLabel().equals("then"))
        || (tgt instanceof StartNode && tgt.getLabel() != null && tgt.getLabel().equals("then")))
         continue;
               
      this.translateChildArc(src, tgt, arc);
    }
     
    for (DiagramArc arc : wfDiagram.getDiagramArcs()) {
       // translate all arcs of wfDiagram
     
      DiagramNode src = arc.getSrc();
      DiagramNode mid = arc.getTarget();
     
      if (src == null || mid == null) throw new NullPointerException("Error in FCtoPTnet while translating arc: arc has no source- or targetNode in Diagram!");
     
      if (!(mid instanceof StartNode && mid.getLabel() != null && mid.getLabel().equals("then")))
         continue;
       
      for (DiagramArc arc2 : wfDiagram.getDiagramArcs()) {
        if (arc == arc2) continue;
        if (arc2.getSrc() == mid) {
          DiagramNode tgt = arc2.getTarget();
          this.translateChildArc(src, tgt, arc2);
        }
      }
    }
  }
  
}
