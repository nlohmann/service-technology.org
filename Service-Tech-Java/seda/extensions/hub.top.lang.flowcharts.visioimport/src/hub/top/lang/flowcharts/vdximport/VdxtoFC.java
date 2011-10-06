/*****************************************************************************\
 * Copyright (c) 2009 Dirk Fahland. All rights reserved. EPL1.0/AGPL3.0
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

package hub.top.lang.flowcharts.vdximport;

import hub.top.lang.flowcharts.ActivityNode;
import hub.top.lang.flowcharts.DiagramArc;
import hub.top.lang.flowcharts.DiagramNode;
import hub.top.lang.flowcharts.Flow;
import hub.top.lang.flowcharts.ResourceNode;
import hub.top.lang.flowcharts.Workflow;
import hub.top.lang.flowcharts.WorkflowDiagram;
import hub.top.lang.flowcharts.impl.FlowChartFactoryImpl;

import java.math.BigInteger;

import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.BasicEMap;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.EMap;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.ecore.util.FeatureMap;
import org.eclipse.emf.ecore.util.FeatureMapUtil;

import com.microsoft.schemas.visio._2003.vdx.ConnectType;
import com.microsoft.schemas.visio._2003.vdx.ConnectionType;
import com.microsoft.schemas.visio._2003.vdx.ConnectsType;
import com.microsoft.schemas.visio._2003.vdx.DocumentRoot;
import com.microsoft.schemas.visio._2003.vdx.MasterType;
import com.microsoft.schemas.visio._2003.vdx.PageType;
import com.microsoft.schemas.visio._2003.vdx.ShapeType;
import com.microsoft.schemas.visio._2003.vdx.ShapesType;
import com.microsoft.schemas.visio._2003.vdx.TextType;
import com.microsoft.schemas.visio._2003.vdx.XFormType;

/**
 * Model transformation class to transform a visio XML document into a
 * model according the hub.top.lang.flowcharts meta-model. The class is split into
 * two parts:
 *   the first part is concerned with parsing the visio document to extract
 *   the graph structure
 *   
 *   the second part translates the parsed graph structure into an explicit
 *   graph-model
 * 
 * 
 * @author Dirk Fahland
 *
 */
public class VdxtoFC {
	
	public static boolean __debug = false;

	private DocumentRoot vdx_document;		// root node of the document to be converted
	
	private EList<ShapeType> vdx_nodes;			// list of nodes in the document
	private EList<ShapeType> vdx_links;			// list of links in the document
	private EMap<BigInteger, ShapeType> vdx_idMap;		// table mapping ids of shapes to shape objects (for resolving references)
	private EMap<ShapeType, ShapeType> vdx_linkSource;	// assigns each link its source node
	private EMap<ShapeType, ShapeType> vdx_linkTarget;	// assigns each link its target node
  private EMap<ShapeType, Integer> vdx_linkSource_ConnectionType;  // number of the ConnectionType at the source node
  private EMap<ShapeType, Integer> vdx_linkTarget_ConnectionType;  // number of the ConnectionType at the target node

	private EMap<BigInteger, BigInteger> __debug_linkSourceID;	// assigns each link its source node
	private EMap<BigInteger, BigInteger> __debug_linkTargetID;	// assigns each link its target node

	
	private EList<ShapeType> vdx_Flows;	// list of flow nodes in the document
	
	private EMap<ShapeType, DiagramNode> translate_vdxShapeToFCNode;	// map to relate visio shapes to the corresponding nodes of the TF-EQ diagram


	public VdxtoFC (DocumentRoot doc) {
		vdx_document = doc;

		vdx_nodes = new BasicEList<ShapeType>();
		vdx_links = new BasicEList<ShapeType>();
		vdx_idMap = new BasicEMap<BigInteger, ShapeType>();
		vdx_linkSource = new BasicEMap<ShapeType, ShapeType>();
		vdx_linkTarget = new BasicEMap<ShapeType, ShapeType>();
		vdx_linkSource_ConnectionType = new BasicEMap<ShapeType, Integer>(); // number of the ConnectionType at the source node
		vdx_linkTarget_ConnectionType = new BasicEMap<ShapeType, Integer>(); // number of the ConnectionType at the source node
		
		vdx_Flows = new BasicEList<ShapeType>();
		
		translate_vdxShapeToFCNode = new BasicEMap<ShapeType,DiagramNode>();
		
		if (__debug) {
			__debug_linkSourceID = new BasicEMap<BigInteger, BigInteger>();
			__debug_linkTargetID = new BasicEMap<BigInteger, BigInteger>();
		}
	}
	
	// -------------------------------------------------------------------
	//  Visio-Diagram related members and methods
	// -------------------------------------------------------------------

	/**
	 * clear all internal tables and maps to prepare the translation of the
	 * next page
	 */
	private void translate_clearForNextPage () {
		vdx_nodes.clear();
		vdx_links.clear();
		vdx_idMap.clear();
		vdx_linkSource.clear();
		vdx_linkTarget.clear();
		vdx_Flows.clear();
		
		translate_vdxShapeToFCNode.clear();
		
		if (__debug) {
			__debug_linkSourceID.clear();
			__debug_linkTargetID.clear();
		}
	}
	
	// master type strings of nodes
	public static final String TYPE_ACTIVITY = "Process";
	public static final String TYPE_SUBPROCESS = "Predefined process";
	public static final String TYPE_START_STOP = "Terminator";
	
	public static final String TYPE_OFF_PAGE_REFERENCE = "Off-page reference"; // reference linking to another page
	public static final String TYPE_ON_PAGE_REFERENCE = "On-page reference"; // short-cuts on a page
	
	public static final String TYPE_ACTIVITY_PREPARATION = "Preparation";
	
	public static final String TYPE_DATA_TRANSIENT = "Data";
	public static final String TYPE_DATA_TRANSIENT2 = "Direct data";
	public static final String TYPE_DATA_DOCUMENT = "Document";
	public static final String TYPE_DATA_STORAGE = "Stored data";
	public static final String TYPE_DATA_STORAGE2 = "Internal storage";
	
	public static final String TYPE_PHYSICAL_ITEM = "Sequential data";
	
	
	public static final String TYPE_DECISION_MERGE = "Decision";
	public static final String TYPE_DECISION_MERGE2 = "45 degree single"; // bogus decision/merge
	public static final String TYPE_FLOW = "Parallel mode";
	
	public static final String TYPE_EVENT = "Event";

	// nodes that shall not be used (will be disregarded in the translation)
	public static final String TYPE_CONTROL_TRANSFER = "Control transfer"; // not to be used
	public static final String TYPE_DISPLAY = "Display"; 		// not to be used
	public static final String TYPE_LOOP_LIMIT = "Loop limit";	// not to be used
		
	// link types
	public static final String TYPE_LINK = "Dynamic connector";
	public static final String TYPE_LINK2 = "Line-curve connector";

	// other types
	public static final String TYPE_ANNOTATION = "Annotation";
	
	// list all node types
	public static final String[] vdx_nodeTypes = 
		{	TYPE_ACTIVITY, TYPE_SUBPROCESS, TYPE_START_STOP,
			TYPE_ACTIVITY_PREPARATION, TYPE_EVENT,
			
			TYPE_DECISION_MERGE, TYPE_DECISION_MERGE2, TYPE_FLOW,
			
			TYPE_OFF_PAGE_REFERENCE, TYPE_ON_PAGE_REFERENCE,
			
			TYPE_DATA_TRANSIENT, TYPE_DATA_TRANSIENT2,
			TYPE_DATA_DOCUMENT, TYPE_DATA_STORAGE, TYPE_DATA_STORAGE2,
			TYPE_PHYSICAL_ITEM
		};

	// list all link types
	public static final String[] vdx_linkTypes = 
		{	TYPE_LINK, TYPE_LINK2	};
	
	/**
	 * @param shape
	 * @oaram typeName name of the type
	 * @return true iff the current visio shape has the given type
	 */
	public boolean vdx_hasType (ShapeType shape, String typeName) {
		MasterType type = vdx_getTypeForID(shape.getMaster());
		
		if (type == null || type.getNameU() == null)
			return false;
		
		if (type.getNameU().startsWith(typeName))
			return true;
		else
			return false;
	}
	
	/**
	 * @param shape
	 * @return true iff the current visio shape represents a diagram node
	 */
	public boolean vdx_isNode (ShapeType shape) {
		MasterType type = vdx_getTypeForID(shape.getMaster());
		
		if (type == null) return false;
		String shapeTypeName = type.getNameU(); 
		if (shapeTypeName == null) return false;
		
		for (int i=0; i<vdx_nodeTypes.length; i++) {
			// check if the shape type name begins with one of the known
			// node types
			if (shapeTypeName.startsWith(vdx_nodeTypes[i]))
				return true;
		}
		return false;
	}

	/**
	 * @param shape
	 * @return true iff the current visio shape represents a diagram link
	 */
	public boolean vdx_isLink (ShapeType shape) {
		MasterType type = vdx_getTypeForID(shape.getMaster());
		
		if (type == null) return false;
		String shapeTypeName = type.getNameU(); 
		if (shapeTypeName == null) return false;
		
		for (int i=0; i<vdx_linkTypes.length; i++) {
			// check if the shape type name begins with one of the known
			// node types
			if (shapeTypeName.startsWith(vdx_linkTypes[i]))
				return true;
		}
		return false;
	}
	
	/**
	 * scans the given page of the visio document for all (known) nodes and links
	 * and fills the appropriate tables, after execution the lists
	 * {@code nodes}, {@code links}, {@code idMap}, {@code linkSource}, and
	 * {@code linkTarget} are filled and describe the graph that is stored
	 * in the vdx document    
	 * @param page to be parsed for nodes and links
	 */
	public void vdx_fillNodeAndLinkTables (PageType page) {
		// fill the node and link tables
		for (ShapesType shapes : page.getShapes()) {
			for (ShapeType shape : shapes.getShape()) {
				if (vdx_isNode(shape))	{
					vdx_nodes.add(shape);
					vdx_idMap.put(shape.getID(), shape);
					
					// remember flows for later translation
					if (vdx_hasType(shape, TYPE_FLOW))
						vdx_Flows.add(shape);
					
					if (__debug) {
						System.err.println("storing node "+shape.getID()+ " "+vdx_getText(shape)+"@"+vdx_shapeParentPage(shape).getName() );
					}
				}
				else if (vdx_isLink(shape)) {
					vdx_links.add(shape);
					vdx_idMap.put(shape.getID(), shape);
				}
			}
		}

		for (ConnectsType connects : page.getConnects()) {
			for (ConnectType conn : connects.getConnect()) {
				if (conn.getFromCell().equals("BeginX")) { // the starting point of a link
					BigInteger linkId = conn.getFromSheet();
					BigInteger sourceId = conn.getToSheet();
					//String sourceConnector = conn.getToCell();
					vdx_linkSource.put(vdx_idMap.get(linkId), vdx_idMap.get(sourceId));
					if (__debug)
						__debug_linkSourceID.put(linkId, sourceId);
					
					// store the number of the ConnectionType of the source node of this link
					// where this connection is attached to
					try {
  					String sCT = conn.getToCell();
  					sCT = sCT.substring(sCT.indexOf(".X")+2);  // the target cell is stored as "Connections.X____number____" 
  					vdx_linkSource_ConnectionType.put(vdx_idMap.get(linkId), new Integer(sCT));
					} catch (Exception e) {
					  // none, just in case we were wrong with the "Connections.X"-thing
					}
				}
				else if (conn.getFromCell().equals("EndX")) { // the end point of a link
					BigInteger linkId = conn.getFromSheet();
					BigInteger targetId = conn.getToSheet();
					//String sourceConnector = conn.getToCell();
					vdx_linkTarget.put(vdx_idMap.get(linkId), vdx_idMap.get(targetId));
					if (__debug)
						__debug_linkTargetID.put(linkId, targetId);
					
          // store the number of the ConnectionType of the target node of this link
          // where this connection is attached to
          try {
            String sCT = conn.getToCell();
            sCT = sCT.substring(sCT.indexOf(".X")+2);  // the target cell is stored as "Connections.X____number____" 
            vdx_linkTarget_ConnectionType.put(vdx_idMap.get(linkId), new Integer(sCT));
          } catch (Exception e) {
            // none, just in case we were wrong with the "Connections.X"-thing
          }
				}
			}
		}
		
		if (__debug) {
			for (ShapeType link : vdx_links) {
				if (vdx_linkSource.get(link) == null)
					System.err.println("link "+link.getID()+" has no source ("+__debug_linkSourceID.get(link.getID())+")@"+vdx_shapeParentPage(link).getName());
				if (vdx_linkTarget.get(link) == null)
					System.err.println("link "+link.getID()+" has no target ("+__debug_linkTargetID.get(link.getID())+")@"+vdx_shapeParentPage(link).getName());
				/*
				if (vdx_linkSource.get(link) != null &&
					vdx_linkTarget.get(link) != null)
				{
					String src = vdx_getText(vdx_linkSource.get(link)).toString();
					String trgt = vdx_getText(vdx_linkTarget.get(link)).toString();
					System.err.println(src +" -> "+ trgt);
				}
				*/
			}
		}
	}
	/**
	 * find the master type object in the current document
	 * that has the given ID
	 * @param ID of the master object describing the type of a shape node 
	 * @return null in case no object with the given ID is found
	 */
	private MasterType vdx_getTypeForID (BigInteger ID) {
		// list of master types of the document
		EList<MasterType> masterTypes =
			vdx_document.getVisioDocument().getMasters().getMaster();
		for (MasterType t : masterTypes) {
			if (t.getID() != null && t.getID().equals(ID))
				return t;
		}
		return null;
	}
	
	/**
	 * turn an array of Strings into a single string and remove control
	 * characters
	 * @param text
	 * @return
	 */
	private String vdx_renderString(EList<String> text) {
		String result = "";
		for (String tBlock : text) {
			result += tBlock;
		}
		
		int pos = result.indexOf("\\n"); 
		while (pos != -1) {
			result = result.substring(0, pos)+result.substring(pos+2);
			pos = result.indexOf("\\n");
		}
		return result;
	}
	
	/**
	 * translate the text that is associated with a visio shape into
	 * a list of Strings
	 * @param shape
	 * @return
	 */
	private EList<String> vdx_getText (ShapeType shape) {
		if (shape == null)
			return null;
		
		EList<String> texts = new BasicEList<String>();
		for (TextType txt : shape.getText()) {
			texts.addAll(vdx_getTextType_texts(txt));
		}
		return texts;
	}
	
	/**
	 * collect all text associated to a Visio XML element and store it
	 * in a list of Strings, this method traverses all child XML elements
	 * of the given element to collect the text
	 * @param o a Visio XML element containing text (is checked for correct type)
	 * @return
	 */
	private EList<String> vdx_getTextType_texts (EObject o) {
		EList<String> texts = new BasicEList<String>();
		if (o instanceof TextType)
		{
			TextType txt = (TextType) o;
			// a Visio-XML TextType-node may contain a string
			// that are stored as mixed children of the node
			FeatureMap mixed = txt.getMixed();
			for (int i=0; i<mixed.size(); i++)
			{
				// check whether the child element is actually text
				EStructuralFeature entry = mixed.getEStructuralFeature(i);
				if (FeatureMapUtil.isText(entry)) {
					// to extract the string in the XML, access the object
					// that is stored in the feature map instead of the
					// feature entry
					String textContents = mixed.getValue(i).toString().replaceAll("\n", "\\\\n");
					texts.add(textContents);
				}
			}
		}
		return texts;
	}
	
	/**
	 * @param shape
	 * @return page that holds the given shape
	 */
	private PageType vdx_shapeParentPage(ShapeType shape) {
		return (PageType)shape.eContainer().eContainer();
	}
	
	/**
	 * Reformat name to ensure compatibility with URI conventions etc.  
	 * @param name
	 * @return
	 */
	private static String safeName(String name) {
	  name = name.replace('/', '_');
	  name = name.replace(' ', '_');
	  name = name.replace('.', '_');
	  name = name.replace(',', '_');
	  return name;
	}
	
	// -------------------------------------------------------------------
	//  transformation related members and methods
	// -------------------------------------------------------------------
	
	public Workflow translateToFC () {
		
		// create root objects of the TF-EQ model
		Workflow flowChartModel = FlowChartFactoryImpl.eINSTANCE.createWorkflow();
		
		for (PageType page : vdx_document.getVisioDocument().getPages().getPage()) {
			translate_clearForNextPage();
			
			// traverse the Visio model to gather all required information
			vdx_fillNodeAndLinkTables(page);	
			
			WorkflowDiagram wf = FlowChartFactoryImpl.eINSTANCE.createWorkflowDiagram();

			// and translate Visio model to TF-EQ model
			translateToFC_vdxDiagram(page, wf);
			wf.setName(safeName(page.getName()));

			flowChartModel.getProcesses().add(wf);
		}
		
		return flowChartModel;
	}
	
	/**
	 * translate a single node of a Visio XML diagram into a TF-EQ diagram node
	 * and update the translation map to remember the mapping from shape to node
	 * 
	 * @param n visio shape to trasnlate
	 * @return corresponding TF-EQ shape
	 */
	private DiagramNode translateToFC_vdxNode (ShapeType n) {
		MasterType type = vdx_getTypeForID(n.getMaster());
		if (type == null) return null;

		DiagramNode wfDiagramNode = null;
		
		// translate the current visio diagram node to a flowchart diagram node
		// determine the node type from the visio node's shape type and
		// the adjacent edges (if necessary)
		String shapeTypeName = type.getNameU(); 
		if (shapeTypeName == null) {
			// do nothing (actually, this code should be unreachable)
			assert(false);
			
		} else if (shapeTypeName.startsWith(TYPE_ACTIVITY)
				|| shapeTypeName.startsWith(TYPE_ACTIVITY_PREPARATION)) {
			wfDiagramNode = FlowChartFactoryImpl.eINSTANCE.createSimpleActivity();
			
		} else if (shapeTypeName.startsWith(TYPE_SUBPROCESS)) {
			wfDiagramNode = FlowChartFactoryImpl.eINSTANCE.createSubprocess();
			
		} else if (shapeTypeName.startsWith(TYPE_START_STOP)
				|| shapeTypeName.startsWith(TYPE_OFF_PAGE_REFERENCE)) {
			// distinguish control-flow start and stop-nodes
			// translate off-page reference as start/end node
			
			if (vdx_linkSource.containsValue(n))
				// node is source of a link: start
				wfDiagramNode = FlowChartFactoryImpl.eINSTANCE.createStartNode();
			
			else if (vdx_linkTarget.containsValue(n))
				// node is target of a link: end
				wfDiagramNode = FlowChartFactoryImpl.eINSTANCE.createEndnode();
			
			// otherwise this node makes no sense....
			
		} else if (shapeTypeName.startsWith(TYPE_ON_PAGE_REFERENCE)) {
			// TODO: find corresponding on-page reference and connect links
			
			
		} else if (shapeTypeName.startsWith(TYPE_DATA_TRANSIENT)
				|| shapeTypeName.startsWith(TYPE_DATA_TRANSIENT2)) {
			wfDiagramNode = FlowChartFactoryImpl.eINSTANCE.createTransientResource();

		} else if (shapeTypeName.startsWith(TYPE_DATA_DOCUMENT)) {
			wfDiagramNode = FlowChartFactoryImpl.eINSTANCE.createDocument();
			
		} else if (shapeTypeName.startsWith(TYPE_DATA_STORAGE)
				|| shapeTypeName.startsWith(TYPE_DATA_STORAGE2)) {
			wfDiagramNode = FlowChartFactoryImpl.eINSTANCE.createPersistentResource();

		} else if (shapeTypeName.startsWith(TYPE_PHYSICAL_ITEM)) {
			// not translated at the moment
			
		} else if (shapeTypeName.startsWith(TYPE_DECISION_MERGE)
				|| shapeTypeName.startsWith(TYPE_DECISION_MERGE2)) {
			
			// count the number of incoming and outgoing edges of this node
			int inEdges = 0;
			int outEdges = 0;
			for (ShapeType link : vdx_links) {
				if (vdx_linkTarget.get(link) == n) inEdges++;
				if (vdx_linkSource.get(link) == n) outEdges++;
			}
			// determine type of node by numbers of edges
			if (inEdges == 1 && outEdges > 1)
				wfDiagramNode = FlowChartFactoryImpl.eINSTANCE.createSplitNode();
			else if (inEdges > 1 && outEdges == 1)
				wfDiagramNode = FlowChartFactoryImpl.eINSTANCE.createMergeNode();
			else
				// does not fit into the typology, is an error, translate as activity
				wfDiagramNode = FlowChartFactoryImpl.eINSTANCE.createSimpleActivity();
			
		} else if (shapeTypeName.startsWith(TYPE_FLOW)) {
			wfDiagramNode = FlowChartFactoryImpl.eINSTANCE.createFlow();
			
		} else if (shapeTypeName.startsWith(TYPE_EVENT)) {
			wfDiagramNode = FlowChartFactoryImpl.eINSTANCE.createEvent();
			
		}
		
		if (wfDiagramNode != null) {
			wfDiagramNode.setLabel(vdx_renderString(vdx_getText(n)));
			// update translation map for later use when updating links and
			// assigning parent/child relations
			translate_vdxShapeToFCNode.put(n, wfDiagramNode);
		}
		
		return wfDiagramNode;
	}

	/**
	 * move node into its parent flow according to positions of the shapes
	 * on the visio drawing (a flow always spacially contains its child nodes)
	 *  
	 * @param node
	 */
	private void translateToFC_assignNodeToParentFlow (ShapeType vdxNode) {
		DiagramNode wfNode = translate_vdxShapeToFCNode.get(vdxNode);
		if (wfNode == null)
			// node was not translated to a diagram node
			return;
		
		// get and calculate position of node (upper left corner)
		// coordinates are given by (center - offset)
		XFormType layoutDataN = vdxNode.getXForm().get(0);
		Double x = 
			(Double.valueOf(layoutDataN.getPinX().get(0).getValue())
			 -Double.valueOf(layoutDataN.getLocPinX().get(0).getValue()));
		Double y =
            (Double.valueOf(layoutDataN.getPinY().get(0).getValue())
			 -Double.valueOf(layoutDataN.getLocPinY().get(0).getValue()));
	
		// now seek for the flow node that is geometrically the closest shape
		Flow wfInnerMostFlow = null;
		Double inner_diffX = Double.MAX_VALUE;
		Double inner_diffY = Double.MAX_VALUE;
		
		for (ShapeType vdxFlow : vdx_Flows) {
			// get geometry of each flow shape (upper left corner and size)
			XFormType layoutDataF = vdxFlow.getXForm().get(0);
			Double xF = 
				(Double.valueOf(layoutDataF.getPinX().get(0).getValue())
				 -Double.valueOf(layoutDataF.getLocPinX().get(0).getValue()));
			Double yF =
			    (Double.valueOf(layoutDataF.getPinY().get(0).getValue())
				 -Double.valueOf(layoutDataF.getLocPinY().get(0).getValue()));
			Double wF = Double.valueOf(layoutDataF.getWidth().get(0).getValue());
			Double hF = Double.valueOf(layoutDataF.getHeight().get(0).getValue());

			Flow wfFlow = (Flow)translate_vdxShapeToFCNode.get(vdxFlow);
			// if the upper left corner of the node is positioned within the
			// geometry of the flow shape 
			if (xF < x && x < xF+wF && yF < y && y < yF+hF) {
				// check if this flow shapes is closer than the previous flow shapes
				if (inner_diffX > x-xF && inner_diffY > y-yF) {
					// remember the closest
					inner_diffX = x-xF;
					inner_diffY = y-yF;
					wfInnerMostFlow = wfFlow;
				}
			}
		}
		// in case we found one, add node to the flow in the diagram
		if (wfInnerMostFlow != null) {
			wfInnerMostFlow.getChildNodes().add(wfNode);
		}
	}
	
	private DiagramArc translateToFC_vdxLink (ShapeType link) {
		if (vdx_linkSource.get(link) != null && vdx_linkTarget.get(link) != null) {
			DiagramNode srcNode = translate_vdxShapeToFCNode.get(vdx_linkSource.get(link));
			DiagramNode tgtNode = translate_vdxShapeToFCNode.get(vdx_linkTarget.get(link));
			if (srcNode != null && tgtNode != null) {
				// set successor relation according to the link
				DiagramArc arc = FlowChartFactoryImpl.eINSTANCE.createDiagramArc();
				arc.setSrc(srcNode);
				arc.setTarget(tgtNode);
				arc.setLabel(vdx_renderString(vdx_getText(link)));
				
				// see, if arc is optional, retrieve from geometry:
				ShapeType srcShape = vdx_linkSource.get(link);
				
				Integer ct_index = vdx_linkSource_ConnectionType.get(link);
				if (ct_index != null) {
  				ConnectionType ct = srcShape.getConnection().get(ct_index.intValue()-1);
  
  				boolean xEdge = true;
          boolean yEdge = true;
  
          String wF = ct.getX().get(0).getF();
  				String hF = ct.getY().get(0).getF();
  				if (wF != null && wF.indexOf("0.") != -1) // formula for width contains a factor 0.YYY
  				  xEdge = false;
  				if (hF != null && hF.indexOf("0.") != -1) // formula for height contains a factor 0.YYY
            yEdge = false;
  				
  				if (!yEdge && xEdge) {
  				  // the arc starts at the left or right most edge of the source node, but
  				  // not at its top or bottom
  				  if (srcNode instanceof ActivityNode && tgtNode instanceof ResourceNode) {
  				    // the source is an activity, the target is a resource node:
  				    // this connection is meant to be optional
  				    arc.setOptional(true);
  				  }
  				}
				}
				
        ShapeType tgtShape = vdx_linkTarget.get(link);
        ct_index = vdx_linkTarget_ConnectionType.get(link);
        if (ct_index != null) {
          ConnectionType ct = tgtShape.getConnection().get(ct_index.intValue()-1);
  
          boolean xEdge = true;
          boolean yEdge = true;
  
          String wF = ct.getX().get(0).getF();
          String hF = ct.getY().get(0).getF();
          if (wF != null && wF.indexOf("0.") != -1) // formula for width contains a factor 0.YYY
            xEdge = false;
          if (hF != null && hF.indexOf("0.") != -1) // formula for height contains a factor 0.YYY
            yEdge = false;
          
          if (!yEdge && xEdge) {
            // the arc starts at the left or right most edge of the source node, but
            // not at its top or bottom
            if (srcNode instanceof ResourceNode && tgtNode instanceof ActivityNode) {
              // the target is an activity, the source is a resource node:
              // this connection is meant to be optional
              arc.setOptional(true);
            }
          }
        }

				return arc;
			}
		}
		return null;
	}
	
	/**
	 * translate the given page of the visio XML diagram and store the result
	 * in the given TF-EQ workflow diagram by adding new nodes and links
	 * @param page 
	 * @param wf
	 */
	private void translateToFC_vdxDiagram (PageType page, WorkflowDiagram wf) {
		// translate all shapes from visio to nodes of the tf-eq diagram
		for (ShapeType node : vdx_nodes) {
			// translate only the nodes of the current page
			if (vdx_shapeParentPage(node) != page)
				continue;
			
			DiagramNode wfDiagramNode = translateToFC_vdxNode(node);
			if (wfDiagramNode != null) {
				// add diagram node to the workflow diagram
				wf.getDiagramNodes().add(wfDiagramNode);
			}
		}
		
		// assign all nodes of the tf-eq diagram to its parent shapes,
		// the translation uses geometric data from the visio diagram
		for (ShapeType node : vdx_nodes) {
			// translate only the nodes of the current page
			if (vdx_shapeParentPage(node) != page)
				continue;

			translateToFC_assignNodeToParentFlow(node);
		}
		
		// translate all visio links to diagram links
		for (ShapeType link : vdx_links) {
			// translate only the links of the current page
			if (vdx_shapeParentPage(link) != page)
				continue;

			DiagramArc wfDiagramArc = translateToFC_vdxLink(link);
			if (wfDiagramArc != null) {
				wf.getDiagramArcs().add(wfDiagramArc);
			}
		}
	}
}
