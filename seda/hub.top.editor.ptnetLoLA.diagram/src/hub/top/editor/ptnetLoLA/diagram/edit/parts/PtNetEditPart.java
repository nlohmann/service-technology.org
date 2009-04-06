/*****************************************************************************\
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
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
 * The Original Code is this file as it was released on July 30, 2008.
 * The Initial Developer of the Original Code is
 * 		Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2008
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

package hub.top.editor.ptnetLoLA.diagram.edit.parts;

import hub.top.editor.petrinets.diagram.graphics.LineBorderNet;
import hub.top.editor.ptnetLoLA.NodeType;
import hub.top.editor.ptnetLoLA.Place;

import java.util.Iterator;

import org.eclipse.draw2d.FreeformLayer;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayeredPane;
import org.eclipse.draw2d.LayoutListener;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.draw2d.StackLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.editparts.LayerManager;
import org.eclipse.gmf.runtime.diagram.ui.editparts.DiagramEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editparts.GraphicalEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.EditPolicyRoles;
import org.eclipse.gmf.runtime.gef.ui.figures.DefaultSizeNodeFigure;
import org.eclipse.gmf.runtime.gef.ui.figures.NodeFigure;
import org.eclipse.gmf.runtime.notation.Node;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.swt.graphics.Color;

/**
 * @generated
 */
public class PtNetEditPart extends DiagramEditPart {

	/**
	 * @generated
	 */
	public final static String MODEL_ID = "PtnetLoLA"; //$NON-NLS-1$

	/**
	 * @generated
	 */
	public static final int VISUAL_ID = 1000;

	/**
	 * @generated
	 */
	public PtNetEditPart(View view) {
		super(view);
	}
	
	/**
	 * @generated
	 */
	protected void createDefaultEditPolicies() {
		super.createDefaultEditPolicies();
		//installEditPolicy(EditPolicyRoles.CREATION_ROLE,
		//		new hub.top.editor.ptnetLoLA.diagram.edit.policies.PtNetCreationEditPolicy());
		installEditPolicy(
				EditPolicyRoles.SEMANTIC_ROLE,
				new hub.top.editor.ptnetLoLA.diagram.edit.policies.PtNetItemSemanticEditPolicy());
		installEditPolicy(
				EditPolicyRoles.CANONICAL_ROLE,
				new hub.top.editor.ptnetLoLA.diagram.edit.policies.PtNetCanonicalEditPolicy());
		// removeEditPolicy(org.eclipse.gmf.runtime.diagram.ui.editpolicies.EditPolicyRoles.POPUPBAR_ROLE);
	}

	public static final String BACKGROUND_LAYERS = "hub.top.editor.petrinets.layers.background";
	
	@Override
	public void activate() {

		// create a new background layer for this diagram
		LayerManager man = (LayerManager)getRoot();
		LayeredPane printable = (LayeredPane)man.getLayer(PRINTABLE_LAYERS);
		FreeformLayer backgroundLayer = new FreeformLayer();
		// push background to the back of all printable layers
		printable.add(backgroundLayer,BACKGROUND_LAYERS, 0);
		
		NodeFigure borderFig = createBorderFigure();

		// add figure to the this diagram, and draw it
		getFigure().add(borderFig, 0);	
		borderFig.setParent(getFigure());
		borderFig.repaint();
		// add figure to the background layer, must be executed last for
		// proper positioning
		backgroundLayer.add(borderFig);		
		
		// add a layout-listener that is called whenever a node is
		// repositioned in this diagram, the layout-listener calculates
		// the outermost nodes of the diagram and repositions the
		// bordering rectangle around it
		getFigure().addLayoutListener(new LayoutListener.Stub() {
      @Override
      public void postLayout(IFigure hostFigure) {
      	PtNetEditPart net = PtNetEditPart.this;
      	Iterator it = net.getChildren().iterator();
      	
      	boolean needsBorder = false;
      	
      	// determine outermost bounds of the net by iterating over all
      	// child edit parts of this net
      	Rectangle netBounds = null;
      	while (it.hasNext()) {
      		EditPart part = (EditPart)it.next();
      		if (part instanceof PlaceEditPart || part instanceof TransitionEditPart) {
      			// consider edit parts of places and transitions only
      			
      			boolean center = false;
      			if (part instanceof PlaceEditPart) {
      				Place p = (Place)((Node)part.getModel()).getElement();
      				if (   p.getType() == NodeType.INPUT
      						|| p.getType() == NodeType.OUTPUT
      						|| p.getType() == NodeType.INOUT )
      				{
      					// in case the place is an interface place, find the
      					// center of the shape of the node (interface nodes are
      					// positioned directly on the border)
      					center = true;
      					// we only need a border if there is an interface node
      					needsBorder = true;
      				}
      			}
      			
      			// retrieve geometry of the nodes edit part
      			GraphicalEditPart gep = (GraphicalEditPart)part;
      			Rectangle nodeBounds = new Rectangle(gep.getFigure().getBounds());
      			
      			// calculate its contribution to the bounds of the net
      			if (center)
      				nodeBounds = new Rectangle(nodeBounds.getCenter(), new Dimension(0,0));
      			else {
      				nodeBounds.expand(20, 20);
      			}
      			
      			// and extend the existing bounds accordingly
      			if (netBounds == null)
      				netBounds = nodeBounds;
      			else
      				netBounds = netBounds.union(nodeBounds);
      		}
      	}

      	// check whether to hide or to show the border 
      	if (needsBorder && netBounds != null) {
      		// draw the border rectangle
      		
        	if (netBounds.equals(PtNetEditPart.this.getBorderFigure().getBounds())) {
        		// do not redraw if nothing has changed
        		return;
        	}

      		getBorderPrimaryShape().repositionAndShow(netBounds);
      	} else {
      		// hide the border rectangle
      		getBorderPrimaryShape().hide();
      	}
      }
});
		
		super.activate();
	}
	
	
	protected NodeFigure borderFigure;
	
	/**
	 * @return newly created figure of the border around an open net
	 */
	protected NodeFigure createBorderFigure() {
		DefaultSizeNodeFigure figure = new DefaultSizeNodeFigure(getMapMode()
				.DPtoLP(0), getMapMode().DPtoLP(0));
		
		figure.setBounds(new Rectangle(0,0,0,0));
		figure.setOpaque(false);
		figure.setFocusTraversable(true);
		figure.setLayoutManager(new StackLayout());
		figure.setVisible(true);
		figure.setValid(true);

		createBorderPrimaryShape(figure);

		return borderFigure = figure;
	}
	
	/**
	 * @return current figure of the border around an open net
	 */
	protected NodeFigure getBorderFigure () {
		return borderFigure;
	}

	/**
	 * primary shape of the border figure, accessible for manipulation
	 * of the node graphics
	 */
	protected IFigure borderPrimaryShape;
	
	/**
	 * create a new primary shape of the border figure, can be accessed
	 * at any time via <code>{@link PtNetEditPart#getBorderPrimaryShape()}</code>
	 * 
	 * @param host Figure that will contain this shape
	 * @return the newly created shape
	 */
	protected IFigure createBorderPrimaryShape(NodeFigure host) {
		NetBorderFigure figure = new NetBorderFigure(host);
		host.add(figure);
		return borderPrimaryShape = figure;
	}

	/**
	 * @return shape of the border figure
	 */
	public NetBorderFigure getBorderPrimaryShape() {
		return (NetBorderFigure) borderPrimaryShape;
	}
	
	/**
	 * dashed rectangle to be repositioned according to the contents of
	 * this diagram
	 * 
	 * @author Dirk Fahland
	 */
	public class NetBorderFigure extends RectangleFigure {
		
		private final NodeFigure fHostFigure;

		public NetBorderFigure(NodeFigure host) {
			super();
			
			this.setFill(false);
			this.setPreferredSize(new Dimension(getMapMode().DPtoLP(25), getMapMode()
					.DPtoLP(25)));
			this.setFocusTraversable(true);
			this.setOpaque(false);
			
			
			this.setBorder(new LineBorderNet());
			
			fHostFigure = host;
		}

		private boolean myUseLocalCoordinates = false;

		protected boolean useLocalCoordinates() {
			return myUseLocalCoordinates;
		}

		protected void setUseLocalCoordinates(boolean useLocalCoordinates) {
			myUseLocalCoordinates = useLocalCoordinates;
		}
		
		public void repositionAndShow (Rectangle r) {
	  		setOutline(true);
	  		//setVisibility(true);
	  		//setVisible(true);
	  		fHostFigure.setBounds(r);
	  		fHostFigure.repaint(r.x-1, r.y-1, r.width+1, r.height+1);
		}
		
		public void hide () {
	  		Rectangle oldR = fHostFigure.getBounds();
	  		
	  		setOutline(false);
	  		//setVisibility(false);
	  		//setVisible(false);
	  		setSize(getMapMode().DPtoLP(0), getMapMode().DPtoLP(0));
	  		fHostFigure.setBounds(new Rectangle(0,0,0,0));
	  		fHostFigure.repaint(oldR.x-1, oldR.y-1, oldR.width+1, oldR.height+1);
		}
	}
	

	static final Color THIS_FORE = new Color(null, 0, 0, 0);

}
