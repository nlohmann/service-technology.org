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

import fr.lip6.move.pnml.framework.utils.exception.InvalidIDException;
import fr.lip6.move.pnml.framework.utils.exception.VoidRepositoryException;

import fr.lip6.move.pnml.ptnet.hlapi.NameHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PNTypeHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PageHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PetriNetDocHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PetriNetHLAPI;
import hub.top.lang.flowcharts.DiagramNode;
import hub.top.lang.flowcharts.Workflow;
import hub.top.lang.flowcharts.WorkflowDiagram;
import hub.top.lang.flowcharts.ptnet.pnml.Translator.NodePair;

import java.util.Hashtable;

import org.eclipse.emf.ecore.EObject;

public class FCtoPNML {

	// this is one workflow diagram of the model
	WorkflowDiagram wfDiagram = null;
	// this is the complete workflow model
	Workflow completeWorkflow = null;	
	// mapping from DiagramNodes to the start- and endPlace in their PNML-Expression
	Hashtable<DiagramNode, NodePair> nodesToNets;
	// ID - count for places, transitions and arcs
	int placeCount = 0;
	int transCount = 0;
	int arcCount = 0;
	
	// returns the index of the last inserted Place
	public int getPlaceCount() {
		return placeCount;
	}

	// set index of the last inserted Place
	public void setPlaceCount(int placeCount) {
		this.placeCount = placeCount;
	}

	// returns the index of the last inserted Transition
	public int getTransCount() {
		return transCount;
	}

	// set index of the last inserted Transition
	public void setTransCount(int transCount) {
		this.transCount = transCount;
	}

	// returns the index of the last inserted Arc
	public int getArcCount() {
		return arcCount;
	}

	// set index of the last inserted Arc
	public void setArcCount(int arcCount) {
		this.arcCount = arcCount;
	}

	public FCtoPNML(EObject modelObject) {
		
		if (modelObject instanceof hub.top.lang.flowcharts.WorkflowDiagram) 
		{
			wfDiagram = (WorkflowDiagram)modelObject;
			completeWorkflow = (Workflow)wfDiagram.eContainer();
		}
	}
	
	/**
	 * returns the Mapping of DiagramNodes to their start- and endplaces in the PNML-Expression
	 */
	public Hashtable<DiagramNode, NodePair> getTable(){
		return this.nodesToNets;
	}
	
	/**
	 * translates the WorkflowDiagram wfDiagram to PNML
	 * @param doc 
	 */
	private void translateFC_toPNML (PetriNetDocHLAPI doc) 
	{
		if (wfDiagram == null)
			return;
		PageHLAPI page = null;
		try {
			this.nodesToNets = new Hashtable<DiagramNode, NodePair>();	
			// create PNML net
			PetriNetHLAPI net = new PetriNetHLAPI("net0", PNTypeHLAPI.COREMODEL,new NameHLAPI(wfDiagram.getName()), doc);
			// create toppage: the 
  			page = new PageHLAPI("toppage", net.getNameHLAPI(), null, net);     //use of "null" is authorized but not encouraged 
  		/*	NodeGraphicsHLAPI _g = new NodeGraphicsHLAPI((NodeHLAPI)null);
  			_g.setPositionHLAPI(new PositionHLAPI(100, 100));
  			page.setNodegraphicsHLAPI(_g);  
  		*/	
  			// translate the actual DiagramPage
  			PageTranslator tr = new PageTranslator();
  			tr.setDiagram(wfDiagram);
  			tr.setPage(page);
  			tr.setTable(this.nodesToNets);
  			tr.setCounts(this.getPlaceCount(), this.getTransCount(), this.getArcCount());
  			tr.translate();
  			this.setPlaceCount(tr.getPlaceCount());
			this.setTransCount(tr.getTransCount());
			this.setArcCount(tr.getArcCount());
			this.nodesToNets = tr.getTable();
			  				
    	} catch (VoidRepositoryException ex) {
    		Activator.getPluginHelper().logError("Error creating PNML document from "+wfDiagram.eResource()+".", ex);
    	} catch (InvalidIDException ex) {
    		Activator.getPluginHelper().logError("Error creating PNML document from "+wfDiagram.eResource()+".", ex);
    	}
	}
	
	/**
	 * @return the PNML net representation of the flowchart
	 */
	public PetriNetDocHLAPI getPNMLDoc () {
	  PetriNetDocHLAPI doc = translateToPNMLDoc();

	  try {
	    // finalize PNML-Framework
      fr.lip6.move.pnml.framework.utils.ModelRepository.getInstance().destroyCurrentWorkspace();
    } catch (VoidRepositoryException ex) {
      Activator.getPluginHelper().logError("Error creating PNML document from "+wfDiagram.eResource()+".", ex);
    }
    
    return doc;
	}
	
	/**
	 * Initializes the PNML framework model repository and translates the
	 * flowchart into a PNML document. PNML repository must be cleaned up
	 * afterwards.
	 * 
	 * @return the PNML net representation of the flowchart
	 */
	private PetriNetDocHLAPI translateToPNMLDoc () {
	  try {
	    
      // initialize PNML-Framework
      fr.lip6.move.pnml.framework.utils.ModelRepository.getInstance().createDocumentWorkspace("void");
      PetriNetDocHLAPI doc = new PetriNetDocHLAPI();
      // invoke translation, fill PetriNet document with contents
      translateFC_toPNML(doc);
      return doc;
      
	  } catch (InvalidIDException ex) {
      Activator.getPluginHelper().logError("Error creating PNML document from "+wfDiagram.eResource()+".", ex);
    }
	  return null;
	}
		
	/**
	 * @return a serialized PNML representation of the flowchart
	 */
	public String getText() {

		String result = "";

		try	{

		  PetriNetDocHLAPI doc = translateToPNMLDoc();
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
		}
		return result;
	}
}
