/*****************************************************************************\
 * Copyright (c) 2008 Manja Wolf, Dirk Fahland. EPL1.0/GPL3.0/LGPL3.0
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
 * The Original Code is this file as it was released on November 26, 2008.
 * The Initial Developer of the Original Code are
 *      Manja Wolf, and
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

package hub.top.adaptiveSystem.diagram.edit.parts;

import hub.top.adaptiveSystem.AdaptiveSystemPackage;
import hub.top.adaptiveSystem.Oclet;

import java.util.LinkedList;
import java.util.List;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.GridData;
import org.eclipse.draw2d.GridLayout;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.draw2d.StackLayout;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.impl.EAttributeImpl;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.editpolicies.LayoutEditPolicy;
import org.eclipse.gef.editpolicies.NonResizableEditPolicy;
import org.eclipse.gmf.runtime.diagram.ui.editparts.IGraphicalEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editparts.ShapeNodeEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.CreationEditPolicy;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.DragDropEditPolicy;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.EditPolicyRoles;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.XYLayoutEditPolicy;
import org.eclipse.gmf.runtime.draw2d.ui.figures.ConstrainedToolbarLayout;
import org.eclipse.gmf.runtime.draw2d.ui.figures.WrappingLabel;
import org.eclipse.gmf.runtime.gef.ui.figures.DefaultSizeNodeFigure;
import org.eclipse.gmf.runtime.gef.ui.figures.NodeFigure;
import org.eclipse.gmf.runtime.notation.Node;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.widgets.Display;

/**
 * @generated
 */
public class OcletEditPart extends ShapeNodeEditPart {

	/**
	 * @generated
	 */
	public static final int VISUAL_ID = 1002;

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
	public OcletEditPart(View view) {
		super(view);
	}

	/**
	 * @generated
	 */
	protected void createDefaultEditPolicies() {
		installEditPolicy(EditPolicyRoles.CREATION_ROLE,
				new CreationEditPolicy());
		super.createDefaultEditPolicies();
		installEditPolicy(
				EditPolicyRoles.SEMANTIC_ROLE,
				new hub.top.adaptiveSystem.diagram.edit.policies.OcletItemSemanticEditPolicy());
		installEditPolicy(EditPolicyRoles.DRAG_DROP_ROLE,
				new DragDropEditPolicy());
		installEditPolicy(
				EditPolicyRoles.CANONICAL_ROLE,
				new hub.top.adaptiveSystem.diagram.edit.policies.OcletCanonicalEditPolicy());
		installEditPolicy(EditPolicy.LAYOUT_ROLE, createLayoutEditPolicy());
		// XXX need an SCR to runtime to have another abstract superclass that would let children add reasonable editpolicies
		// removeEditPolicy(org.eclipse.gmf.runtime.diagram.ui.editpolicies.EditPolicyRoles.CONNECTION_HANDLES_ROLE);

	}

	/**
	 * create an XY layout policy for this oclet
	 * 
	 * @generated NOT
	 * @author Dirk Fahland
	 */
	protected LayoutEditPolicy createLayoutEditPolicy() {
		LayoutEditPolicy lep = new XYLayoutEditPolicy() {
			
			private List<EditPolicy> childEditPolicies = new LinkedList<EditPolicy>();

			protected EditPolicy createChildEditPolicy(EditPart child) {
				EditPolicy result = child
						.getEditPolicy(EditPolicy.PRIMARY_DRAG_ROLE);
				if (result == null) {
					result = new NonResizableEditPolicy();
				}
				childEditPolicies.add(result);
				
				return result;
			}
		};
		return lep;
	}
	
	/**
	 * @generated
	 */
	protected IFigure createNodeShape() {
		OcletDescriptor figure = new OcletDescriptor();
		return primaryShape = figure;
	}

	/**
	 * @generated
	 */
	public OcletDescriptor getPrimaryShape() {
		return (OcletDescriptor) primaryShape;
	}

	/**
	 * @generated
	 */
	protected boolean addFixedChild(EditPart childEditPart) {
		if (childEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.OcletNameEditPart) {
			((hub.top.adaptiveSystem.diagram.edit.parts.OcletNameEditPart) childEditPart)
					.setLabel(getPrimaryShape().getFigureOcletName());
			return true;
		}
		if (childEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.OcletQuantorOrientationEditPart) {
			((hub.top.adaptiveSystem.diagram.edit.parts.OcletQuantorOrientationEditPart) childEditPart)
					.setLabel(getPrimaryShape().getFigureOcletQuantor());
			return true;
		}
		return false;
	}

	/**
	 * @generated
	 */
	protected boolean removeFixedChild(EditPart childEditPart) {

		return false;
	}

	/**
	 * @generated
	 */
	protected void addChildVisual(EditPart childEditPart, int index) {
		if (addFixedChild(childEditPart)) {
			return;
		}
		super.addChildVisual(childEditPart, -1);
	}

	/**
	 * @generated
	 */
	protected void removeChildVisual(EditPart childEditPart) {
		if (removeFixedChild(childEditPart)) {
			return;
		}
		super.removeChildVisual(childEditPart);
	}

	/**
	 * @generated
	 */
	protected IFigure getContentPaneFor(IGraphicalEditPart editPart) {

		return super.getContentPaneFor(editPart);
	}

	/**
	 * @generated
	 */
	protected NodeFigure createNodePlate() {
		DefaultSizeNodeFigure result = new DefaultSizeNodeFigure(getMapMode()
				.DPtoLP(200), getMapMode().DPtoLP(200));
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
	protected NodeFigure createNodeFigure() {
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
	 * @generated NOT
	 */
	protected IFigure setupContentPane(IFigure nodeShape) {
		if (nodeShape.getLayoutManager() == null) {
			ConstrainedToolbarLayout layout = new ConstrainedToolbarLayout();
			layout.setSpacing(getMapMode().DPtoLP(0));
			nodeShape.setLayoutManager(layout);
		}
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
		return getChildBySemanticHint(hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.getType(hub.top.adaptiveSystem.diagram.edit.parts.OcletNameEditPart.VISUAL_ID));
	}
	
	/**
	 * @return the pre-net edit part of this oclet
	 */
	public PreNetEditPart getPreNetEditPart() {
		return (PreNetEditPart) getChildBySemanticHint(hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.getType(hub.top.adaptiveSystem.diagram.edit.parts.PreNetEditPart.VISUAL_ID));
	}
	
	/**
	 * @return the pre-net edit part of this oclet
	 */
	public DoNetEditPart getDoNetEditPart() {
		return (DoNetEditPart) getChildBySemanticHint(hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.getType(hub.top.adaptiveSystem.diagram.edit.parts.DoNetEditPart.VISUAL_ID));
	}


	/**
	 * @author Manja Wolf
	 */
	@Override
	protected void handleNotificationEvent(Notification notification) {
		Object feature = notification.getFeature();
		if (notification.getNotifier() instanceof Oclet) {

			//if the attribute wellformed changed, then recolor oclet
			if (feature instanceof EAttributeImpl) {
				EAttributeImpl attribute = (EAttributeImpl) feature;
				if (attribute.getName().equals(
						AdaptiveSystemPackage.eINSTANCE.getOclet_WellFormed()
								.getName())) {
					this.getPrimaryShape().updateFace();
				}
			}
		}
		
		super.handleNotificationEvent(notification);
	}

	
	/**
	 * @generated
	 */
	public class OcletDescriptor extends RectangleFigure {

		/**
		 * @generated
		 */
		private WrappingLabel fFigureOcletName;
		/**
		 * @generated
		 */
		private WrappingLabel fFigureOcletQuantor;
		/**
		 * @generated
		 */
		private RectangleFigure fFigureOcletBody;
		/**
		 * @generated
		 */
		private WrappingLabel fFigureOcletOrientation;
		/**
		 * @generated NOT
		 */
		private RectangleFigure fFigureOcletHeader;

		/**
		 * @generated NOT
		 */
		public OcletDescriptor() {
			setLayoutManager(new XYLayout());
			
			this.setFill(false);
			this.setPreferredSize(new Dimension(getMapMode().DPtoLP(200),
					getMapMode().DPtoLP(200)));
			this.setMaximumSize(new Dimension(getMapMode().DPtoLP(1000),
					getMapMode().DPtoLP(1000)));
			this.setMinimumSize(new Dimension(getMapMode().DPtoLP(200),
					getMapMode().DPtoLP(100)));
			updateFace();
			createContents();
		}

		/**
		 * @generated NOT
		 * @author Manja Wolf
		 * @author Dirk Fahland
		 */
		private void createContents() {

			fFigureOcletHeader = new RectangleFigure();
			fFigureOcletHeader.setFill(false);
			fFigureOcletHeader.setOutline(false);
			fFigureOcletHeader.setForegroundColor(ColorConstants.orange);
			fFigureOcletHeader.setPreferredSize(new Dimension(getMapMode().DPtoLP(
					200), getMapMode().DPtoLP(20)));
			fFigureOcletHeader.setMaximumSize(new Dimension(getMapMode().DPtoLP(
					1000), getMapMode().DPtoLP(20)));
			fFigureOcletHeader.setMinimumSize(new Dimension(getMapMode().DPtoLP(
					200), getMapMode().DPtoLP(20)));
			fFigureOcletHeader.setLocation(
					new Point(getMapMode().DPtoLP(0),
							  getMapMode().DPtoLP(0)));
			fFigureOcletHeader.setSize(fFigureOcletHeader.getMinimumSize());

			this.add(fFigureOcletHeader);

			GridLayout layoutLabelContainer0 = new GridLayout();
			layoutLabelContainer0.numColumns = 3;
			layoutLabelContainer0.makeColumnsEqualWidth = false;
			//START: Manja Wolf
			layoutLabelContainer0.marginHeight = 2;
			layoutLabelContainer0.marginWidth = 0;
			layoutLabelContainer0.verticalSpacing = 0;
			//END: Manja Wolf
			fFigureOcletHeader.setLayoutManager(layoutLabelContainer0);

			fFigureOcletName = new WrappingLabel();
			fFigureOcletName.setText("");

			fFigureOcletName.setFont(FFIGUREOCLETNAME_FONT);

			GridData constraintFFigureOcletName = new GridData();
			constraintFFigureOcletName.verticalAlignment = SWT.CENTER;
			constraintFFigureOcletName.horizontalAlignment = GridData.BEGINNING;
			constraintFFigureOcletName.horizontalIndent = 10;
			constraintFFigureOcletName.horizontalSpan = 1;
			constraintFFigureOcletName.verticalSpan = 1;
			constraintFFigureOcletName.grabExcessHorizontalSpace = false;
			constraintFFigureOcletName.grabExcessVerticalSpace = false;
			fFigureOcletHeader.add(fFigureOcletName, constraintFFigureOcletName);

			fFigureOcletQuantor = new WrappingLabel();
			fFigureOcletQuantor.setText("");

			GridData constraintFFigureOcletQuantor = new GridData();
			constraintFFigureOcletQuantor.verticalAlignment = SWT.CENTER;
			constraintFFigureOcletQuantor.horizontalAlignment = GridData.BEGINNING;
			constraintFFigureOcletQuantor.horizontalIndent = 0;
			constraintFFigureOcletQuantor.horizontalSpan = 1;
			constraintFFigureOcletQuantor.verticalSpan = 1;
			constraintFFigureOcletQuantor.grabExcessHorizontalSpace = false;
			constraintFFigureOcletQuantor.grabExcessVerticalSpace = false;
			fFigureOcletHeader.add(fFigureOcletQuantor,
					constraintFFigureOcletQuantor);

			fFigureOcletOrientation = new WrappingLabel();
			fFigureOcletOrientation.setText("");

			GridData constraintFFigureOcletOrientation = new GridData();
			constraintFFigureOcletOrientation.verticalAlignment = SWT.CENTER;
			constraintFFigureOcletOrientation.horizontalAlignment = GridData.BEGINNING;
			constraintFFigureOcletOrientation.horizontalIndent = 0;
			constraintFFigureOcletOrientation.horizontalSpan = 1;
			constraintFFigureOcletOrientation.verticalSpan = 1;
			constraintFFigureOcletOrientation.grabExcessHorizontalSpace = false;
			constraintFFigureOcletOrientation.grabExcessVerticalSpace = false;
			fFigureOcletHeader.add(fFigureOcletOrientation,
					constraintFFigureOcletOrientation);

			fFigureOcletBody = new RectangleFigure();
			fFigureOcletBody.setFill(false);
			fFigureOcletBody.setPreferredSize(new Dimension(getMapMode()
					.DPtoLP(200), getMapMode().DPtoLP(1)));
			fFigureOcletBody.setMaximumSize(new Dimension(getMapMode().DPtoLP(
					1000), getMapMode().DPtoLP(1)));

			this.add(fFigureOcletBody);

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

		/**
		 * @generated
		 */
		public WrappingLabel getFigureOcletName() {
			return fFigureOcletName;
		}

		/**
		 * @generated
		 */
		public WrappingLabel getFigureOcletQuantor() {
			return fFigureOcletQuantor;
		}

		/**
		 * @generated
		 */
		public RectangleFigure getFigureOcletBody() {
			return fFigureOcletBody;
		}

		/**
		 * @generated
		 */
		public WrappingLabel getFigureOcletOrientation() {
			return fFigureOcletOrientation;
		}
		
		/**
		 * @author Dirk Fahland
		 */
		public RectangleFigure getFigureOcletHeader() {
			return fFigureOcletHeader;
		}

		/**
		 * @author Manja Wolf
		 */
		public void updateFace() {
			Oclet oclet = (Oclet) ((Node) OcletEditPart.this.getModel())
					.getElement();

			if (oclet.isWellFormed()) {
				this.setBorder(new LineBorder(new Color(null, 30, 204, 30),
						getMapMode().DPtoLP(2)));
			} else {
				this.setBorder(null);
			}

		}
	}

	/**
	 * @generated
	 */
	static final Font FFIGUREOCLETNAME_FONT = new Font(Display.getCurrent(),
			Display.getDefault().getSystemFont().getFontData()[0].getName(), 9,
			SWT.BOLD);

}
