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

import hub.top.editor.petrinets.diagram.tool.FireTool;
import hub.top.editor.ptnetLoLA.Arc;
import hub.top.editor.ptnetLoLA.Place;
import hub.top.editor.ptnetLoLA.PtnetLoLAPackage;
import hub.top.editor.ptnetLoLA.Transition;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.draw2d.StackLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.BasicEMap;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.EMap;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.editpolicies.LayoutEditPolicy;
import org.eclipse.gef.editpolicies.NonResizableEditPolicy;
import org.eclipse.gef.requests.CreateRequest;
import org.eclipse.gef.requests.SelectionRequest;
import org.eclipse.gmf.runtime.diagram.ui.editparts.AbstractBorderedShapeEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editparts.IBorderItemEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.BorderItemSelectionEditPolicy;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.EditPolicyRoles;
import org.eclipse.gmf.runtime.diagram.ui.figures.BorderItemLocator;
import org.eclipse.gmf.runtime.gef.ui.figures.DefaultSizeNodeFigure;
import org.eclipse.gmf.runtime.gef.ui.figures.NodeFigure;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.swt.graphics.Color;

/**
 * @generated
 */
public class TransitionEditPart extends AbstractBorderedShapeEditPart {

	/**
	 * @generated
	 */
	public static final int VISUAL_ID = 2001;

	/**
	 * @generated
	 */
	protected IFigure contentPane;

	/**
	 * @generated
	 */
	protected IFigure primaryShape;

	/**
	 * @generated
	 */
	public TransitionEditPart(View view) {
		super(view);
	}

	/**
	 * @generated
	 */
	protected void createDefaultEditPolicies() {
		super.createDefaultEditPolicies();
		installEditPolicy(
				EditPolicyRoles.SEMANTIC_ROLE,
				new hub.top.editor.ptnetLoLA.diagram.edit.policies.TransitionItemSemanticEditPolicy());
		installEditPolicy(EditPolicy.LAYOUT_ROLE, createLayoutEditPolicy());
		// XXX need an SCR to runtime to have another abstract superclass that would let children add reasonable editpolicies
		// removeEditPolicy(org.eclipse.gmf.runtime.diagram.ui.editpolicies.EditPolicyRoles.CONNECTION_HANDLES_ROLE);
	}

	/**
	 * @generated
	 */
	protected LayoutEditPolicy createLayoutEditPolicy() {
		LayoutEditPolicy lep = new LayoutEditPolicy() {

			protected EditPolicy createChildEditPolicy(EditPart child) {
				if (child instanceof IBorderItemEditPart) {
					return new BorderItemSelectionEditPolicy();
				}
				EditPolicy result = child.getEditPolicy(EditPolicy.PRIMARY_DRAG_ROLE);
				if (result == null) {
					result = new NonResizableEditPolicy();
				}
				return result;
			}

			protected Command getMoveChildrenCommand(Request request) {
				return null;
			}

			protected Command getCreateCommand(CreateRequest request) {
				return null;
			}
		};
		return lep;
	}

	/**
	 * @generated
	 */
	protected IFigure createNodeShape() {
		TransitionFigure figure = new TransitionFigure();
		return primaryShape = figure;
	}

	/**
	 * @generated
	 */
	public TransitionFigure getPrimaryShape() {
		return (TransitionFigure) primaryShape;
	}

	/**
	 * @generated not
	 */
	protected void addBorderItem(IFigure borderItemContainer,
			IBorderItemEditPart borderItemEditPart) {
		if (borderItemEditPart instanceof hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionNameEditPart) {
			BorderItemLocator locator = new BorderItemLocator(getMainFigure(),
					PositionConstants.SOUTH);
			locator.setBorderItemOffset(new Dimension(-5, -5));
			borderItemContainer.add(borderItemEditPart.getFigure(), locator);
		} else {
			super.addBorderItem(borderItemContainer, borderItemEditPart);
		}
	}

	/**
	 * @generated
	 */
	protected NodeFigure createNodePlate() {
		DefaultSizeNodeFigure result = new DefaultSizeNodeFigure(getMapMode()
				.DPtoLP(25), getMapMode().DPtoLP(25));
		return result;
	}

	/**
	 * Creates figure for this edit part.
	 * 
	 * Body of this method does not depend on settings in generation model
	 * so you may safely remove <i>generated</i> tag and modify it.
	 * 
	 * @generated
	 */
	protected NodeFigure createMainFigure() {
		NodeFigure figure = createNodePlate();
		figure.setLayoutManager(new StackLayout());
		IFigure shape = createNodeShape();
		figure.add(shape);
		contentPane = setupContentPane(shape);
		return figure;
	}

	/**
	 * Default implementation treats passed figure as content pane.
	 * Respects layout one may have set for generated figure.
	 * @param nodeShape instance of generated figure class
	 * @generated
	 */
	protected IFigure setupContentPane(IFigure nodeShape) {
		return nodeShape; // use nodeShape itself as contentPane
	}

	/**
	 * @generated
	 */
	public IFigure getContentPane() {
		if (contentPane != null) {
			return contentPane;
		}
		return super.getContentPane();
	}

	/**
	 * @generated
	 */
	public EditPart getPrimaryChildEditPart() {
		return getChildBySemanticHint(hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
				.getType(hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionNameEditPart.VISUAL_ID));
	}

	/**
	 * @generated
	 */
	public class TransitionFigure extends RectangleFigure {

		/**
		 * @generated
		 */
		public TransitionFigure() {
			this.setLineWidth(2);
			this.setForegroundColor(THIS_FORE);
			this.setBackgroundColor(THIS_BACK);
			this.setPreferredSize(new Dimension(getMapMode().DPtoLP(25), getMapMode()
					.DPtoLP(25)));
		}

		/**
		 * @generated
		 */
		private boolean myUseLocalCoordinates = false;

		/**
		 * @generated
		 */
		protected boolean useLocalCoordinates() {
			return myUseLocalCoordinates;
		}

		/**
		 * @generated
		 */
		protected void setUseLocalCoordinates(boolean useLocalCoordinates) {
			myUseLocalCoordinates = useLocalCoordinates;
		}

	}

	/**
	 * @generated
	 */
	static final Color THIS_FORE = new Color(null, 0, 0, 0);

	/**
	 * @generated
	 */
	static final Color THIS_BACK = new Color(null, 255, 255, 255);

	/**
	 * @see org.eclipse.gef.editparts.AbstractEditPart#getTargetEditPart(org.eclipse.gef.Request)
	 * 
	 * @generated NOT
	 */
	@Override
	public EditPart getTargetEditPart(Request request) {
		// check whether request comes from a standard tool
		EditPart editPart = super.getTargetEditPart(request);
		// if none is found, do extended checking
		if (editPart == null) {
			// a transition can be fired
			if (request.getType() == FireTool.REQ_FIRE_NET_ELEMENT) {
				return this;
			}
		}
		return editPart;
	}

	/**
	 * @see org.eclipse.gmf.runtime.diagram.ui.editparts.GraphicalEditPart#performRequest(org.eclipse.gef.Request)
	 * 
	 * @generated NOT
	 */
	@Override
	public void performRequest(Request request) {

		// this request is a transition fire-request
		if (request instanceof SelectionRequest
				&& request.getType() == FireTool.REQ_FIRE_NET_ELEMENT) {
			// get the model element of this controller: a transition
			View view = (View) this.getModel();
			Transition t = (Transition) view.getElement();

			// get the structural feature we want to manipulate:
			// the number of tokens on a place
			EStructuralFeature featToken = PtnetLoLAPackage.eINSTANCE
					.getPlace_Token();
			// create a list of commands and add token consume/produce commands to this list
			EList<org.eclipse.emf.common.command.Command> cmdList = new BasicEList<org.eclipse.emf.common.command.Command>();
			boolean enabled = true;
			// the preset of the transition
			for (Arc arc : t.getIncoming()) {
				Place p = (Place) arc.getSource();
				// this places has not enough tokens, t is not enabled
				if (p.getToken() < arc.getWeight()) {
					enabled = false;
					break;
				}
			}

			if (enabled) // if t is enabled
			{
				EMap<Place, Integer> tokenMap = new BasicEMap<Place, Integer>();
				// tokens to be consumed
				for (Arc arc : t.getIncoming()) {
					Place p = (Place) arc.getSource();
					tokenMap.put(p, new Integer(p.getToken() - arc.getWeight()));
				}
				// tokens to be produced
				for (Arc arc : t.getOutgoing()) {
					Place p = (Place) arc.getTarget();
					Integer pRemoved = tokenMap.get(p);
					if (pRemoved == null)
						tokenMap.put(p, new Integer(p.getToken() + arc.getWeight()));
					else
						tokenMap.put(p, pRemoved + arc.getWeight());
				}

				// create the marking update commands
				for (java.util.Map.Entry<Place, Integer> e : tokenMap) {

					org.eclipse.emf.edit.command.SetCommand cmd = new org.eclipse.emf.edit.command.SetCommand(
							this.getEditingDomain(), e.getKey(), featToken, e.getValue());
					cmd.setLabel("set token on " + e.getKey().getName());
					cmdList.add(cmd);
				}
				// and join all token commands in a compound transition fire command
				org.eclipse.emf.common.command.CompoundCommand fireCmd = new org.eclipse.emf.common.command.CompoundCommand(
						cmdList);
				fireCmd.setLabel("fire " + t.getName());
				fireCmd.setDescription("fire " + t.getName());
				// and execute it in a transactional editing domain (for undo/redo)
				this.getEditingDomain().getCommandStack().execute(fireCmd);
			}
		} else
			super.performRequest(request);
	}
}
