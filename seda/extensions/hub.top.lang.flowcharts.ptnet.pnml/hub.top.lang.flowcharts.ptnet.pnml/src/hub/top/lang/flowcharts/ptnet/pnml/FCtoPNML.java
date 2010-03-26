/*****************************************************************************\
 * Copyright (c) 2009 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 * 
 * ServiceTechnolog.org - FlowChart Editors
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
 * The Original Code is this file as it was released on March 09, 2009.
 * The Initial Developer of the Original Code are
 * 		Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2009
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the EPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the EPL, the GPL or the LGPL.
\*****************************************************************************/

package hub.top.lang.flowcharts.ptnet.pnml;

import hub.top.lang.flowcharts.DiagramArc;
import hub.top.lang.flowcharts.DiagramNode;
import hub.top.lang.flowcharts.Workflow;
import hub.top.lang.flowcharts.WorkflowDiagram;

import org.eclipse.emf.ecore.EObject;

import fr.lip6.move.pnml.framework.general.PnmlExport;
import fr.lip6.move.pnml.framework.utils.exception.InvalidIDException;
import fr.lip6.move.pnml.framework.utils.exception.VoidRepositoryException;
import fr.lip6.move.pnml.ptnet.hlapi.ArcHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.NameHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.NodeGraphicsHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.NodeHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PNTypeHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PTMarkingHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PageHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PetriNetDocHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PetriNetHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PlaceHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PositionHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.RefPlaceHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.TransitionHLAPI;

public class FCtoPNML {

	// this is one workflow diagram of the model
	WorkflowDiagram wfDiagram = null;
	// this is the complete workflow model
	Workflow completeWorkflow = null;
	
	public FCtoPNML(EObject modelObject) {
		
		if (modelObject instanceof hub.top.lang.flowcharts.WorkflowDiagram) {
			wfDiagram = (WorkflowDiagram)modelObject;
			completeWorkflow = (Workflow)wfDiagram.eContainer();
		}
	}
	
	private void translateFC_toPNML (PetriNetDocHLAPI doc) {
		if (wfDiagram == null)
			return;

    try {

  		for (DiagramNode node : wfDiagram.getDiagramNodes()) {
  			// do something for all nodes of wfDiagram
  		}
  		
  		for (DiagramArc arc : wfDiagram.getDiagramArcs()) {
  			// do something for all arcs of wfDiagram
  		}
		
  		NodeGraphicsHLAPI _g;   // graphics definition variable;
  		
      PetriNetHLAPI net = new PetriNetHLAPI("net0", PNTypeHLAPI.COREMODEL,new NameHLAPI("hello"), doc);
      PageHLAPI page = new PageHLAPI("toppage", new NameHLAPI("toppage"), null, net);     //use of "null" is authorized but not encouraged 
  
      _g = new NodeGraphicsHLAPI((NodeHLAPI)null);
      _g.setPositionHLAPI(new PositionHLAPI(100, 100));
      page.setNodegraphicsHLAPI(_g);
      
      
      PlaceHLAPI p1 = new PlaceHLAPI("place1"); p1.setNameHLAPI(new NameHLAPI(p1.getId()));
      p1.setInitialMarkingHLAPI(new PTMarkingHLAPI(1));
      PlaceHLAPI p2 = new PlaceHLAPI("place2"); p2.setNameHLAPI(new NameHLAPI(p2.getId()));
      PlaceHLAPI p3 = new PlaceHLAPI("place3"); p3.setNameHLAPI(new NameHLAPI(p3.getId()));
  
      TransitionHLAPI t1 = new TransitionHLAPI("transistion1"); t1.setNameHLAPI(new NameHLAPI(t1.getId()));
      TransitionHLAPI t2 = new TransitionHLAPI("transistion2"); t2.setNameHLAPI(new NameHLAPI(t2.getId()));
  
      // fill "page" with net elements
      p1.setContainerPageHLAPI(page);
      t1.setContainerPageHLAPI(page);
      p3.setContainerPageHLAPI(page);
      new ArcHLAPI("a3", p1, t1, page);
      new ArcHLAPI("a4", t1, p3, page);
      
      // add a subpage (= subnet transition) to "page"
      PageHLAPI subpage = new PageHLAPI("subpage", new NameHLAPI("subpage"), null, page); //same
      
      // position net elements on "page"
      _g = new NodeGraphicsHLAPI((NodeHLAPI)null);
      _g.setPositionHLAPI(new PositionHLAPI(100, 100));
      p1.setNodegraphicsHLAPI(_g);

      _g = new NodeGraphicsHLAPI((NodeHLAPI)null);
      _g.setPositionHLAPI(new PositionHLAPI(100, 200));
      t1.setNodegraphicsHLAPI(_g);

      _g = new NodeGraphicsHLAPI((NodeHLAPI)null);
      _g.setPositionHLAPI(new PositionHLAPI(100, 300));
      p3.setNodegraphicsHLAPI(_g);
      
      _g = new NodeGraphicsHLAPI((NodeHLAPI)null);
      _g.setPositionHLAPI(new PositionHLAPI(200, 400));
      subpage.setNodegraphicsHLAPI(_g);
      
  
      //  fill "subpage" with net elements
      // create place r1 in "subpage" that refers to place p3 in "page" 
      RefPlaceHLAPI r1 = new RefPlaceHLAPI("reftop3", p3);
    
      r1.setContainerPageHLAPI(subpage);
      p2.setContainerPageHLAPI(subpage);
      t2.setContainerPageHLAPI(subpage);
      new ArcHLAPI("a1", r1, t2, subpage);
      new ArcHLAPI("a2", t2, p2, subpage);
      
      // position net elements on "subpage"
      _g = new NodeGraphicsHLAPI((NodeHLAPI)null);
      _g.setPositionHLAPI(new PositionHLAPI(200, 100));
      r1.setNodegraphicsHLAPI(_g);

      _g = new NodeGraphicsHLAPI((NodeHLAPI)null);
      _g.setPositionHLAPI(new PositionHLAPI(400, 100));
      p2.setNodegraphicsHLAPI(_g);

      _g = new NodeGraphicsHLAPI((NodeHLAPI)null);
      _g.setPositionHLAPI(new PositionHLAPI(300, 100));
      t2.setNodegraphicsHLAPI(_g);
      
    } catch (VoidRepositoryException ex) {
      Activator.getPluginHelper().logError("Error creating PNML document from "+wfDiagram.eResource()+".", ex);
    } catch (InvalidIDException ex) {
      Activator.getPluginHelper().logError("Error creating PNML document from "+wfDiagram.eResource()+".", ex);
    }

	}
	
	public String getText() {

		String result = "";

		try	{
		  // initialize PNML-Framework
			fr.lip6.move.pnml.framework.utils.ModelRepository.getInstance().createDocumentWorkspace("void");
			PetriNetDocHLAPI doc = new PetriNetDocHLAPI();

			// invoke translation, fill PetriNet document with contents
			translateFC_toPNML(doc);

			// prepare PNML document export
			final String ns_str = " xmlns=\""+doc.getXmlns()+"\"";
			hub.top.lang.flowcharts.ptnet.pnml.PnmlExport pex = new hub.top.lang.flowcharts.ptnet.pnml.PnmlExport() {
			  @Override
			  public String postProcess(String output) {
			    // clear all name space references in the output string to make it parseable by Yasper
			    int pos = output.indexOf(ns_str);
			    while (pos != -1) {
			      output = output.substring(0, pos)+output.substring(pos+ns_str.length());
			      pos = output.indexOf(ns_str);
			    }
			    return output;
			  }
			};
			
			// write PNML code...
			// ... into result string and remove XML name space attributes
			result = pex.postProcess(doc.toPNML());
      //pex.exportObject(doc, "D:/tmp/exporttest.pnml");
			
			// finalize PNML-Framework
			fr.lip6.move.pnml.framework.utils.ModelRepository.getInstance().destroyCurrentWorkspace();
		} catch (VoidRepositoryException ex) {
      Activator.getPluginHelper().logError("Error creating PNML document from "+wfDiagram.eResource()+".", ex);
    } catch (InvalidIDException ex) {
      Activator.getPluginHelper().logError("Error creating PNML document from "+wfDiagram.eResource()+".", ex);
    }
		
		return result;
	}
}
