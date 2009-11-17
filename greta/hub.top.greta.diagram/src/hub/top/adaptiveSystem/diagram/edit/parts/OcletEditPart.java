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

import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

import org.eclipse.draw2d.Border;
import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.GridData;
import org.eclipse.draw2d.GridLayout;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayoutListener;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.draw2d.StackLayout;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.impl.EAttributeImpl;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gef.editparts.AbstractGraphicalEditPart;
import org.eclipse.gef.editpolicies.LayoutEditPolicy;
import org.eclipse.gef.editpolicies.NonResizableEditPolicy;
import org.eclipse.gmf.runtime.diagram.ui.commands.ICommandProxy;
import org.eclipse.gmf.runtime.diagram.ui.commands.SetBoundsCommand;
import org.eclipse.gmf.runtime.diagram.ui.editparts.IGraphicalEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editparts.ShapeNodeEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.CreationEditPolicy;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.DragDropEditPolicy;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.EditPolicyRoles;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.PopupBarEditPolicy;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.XYLayoutEditPolicy;
import org.eclipse.gmf.runtime.diagram.ui.l10n.DiagramUIMessages;
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
		removeEditPolicy(EditPolicyRoles.POPUPBAR_ROLE);
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
	 * @param aep
	 * @return
	 *   a {@link Rectangle} that directly encloses all children of
	 *   the given edit part
	 *   
	 * @generated NOT
	 */
	@SuppressWarnings("unchecked")
  public static Rectangle getChildrenBounds(AbstractGraphicalEditPart aep) {
    Rectangle r = new Rectangle(0,0,0,0);
    
    Iterator it = aep.getChildren().iterator();
    while (it.hasNext()) {
      Object o2 = it.next();
      if (o2 instanceof AbstractGraphicalEditPart) {
        AbstractGraphicalEditPart child = (AbstractGraphicalEditPart)o2;
        Rectangle bounds = child.getFigure().getBounds();
        
        r.union(bounds);
      }
    }
    return r;
	}
	
	/**
	 * Resize {@link OcletEditPart} to the size that it displays all event and all conditions
	 * which it contains. It does not internally resize its child edit parts {@link DoNetEditPart}
	 * and {@link PreNetEditPart}.
	 */
	public void resizeOclet() {
    
	  // update only if change made some nodes disappear
    //boolean preNetVisible = false, doNetVisible = false;
    Rectangle preNetChildBounds = null, doNetChildBounds = null;
    PreNetEditPart pne = null;
    DoNetEditPart dne = null;
    // compute bounds of the children
    if (OcletEditPart.this.getChildren().size() > 0) {

      for (Object childEP : OcletEditPart.this.getChildren()) {
        if (childEP instanceof PreNetEditPart) {
          pne = (PreNetEditPart)childEP;
          PreNetCompartmentEditPart pnce = (PreNetCompartmentEditPart)pne.getChildren().get(0);
          preNetChildBounds = getChildrenBounds(pnce);

          // update only if change made some nodes disappear
          //preNetVisible = (pne.primaryShape.getBounds().width >= preNetChildBounds.x + preNetChildBounds.width)
          //             && (pne.primaryShape.getBounds().height >= preNetChildBounds.y + preNetChildBounds.height);
        }
        if (childEP instanceof DoNetEditPart) {
          dne = (DoNetEditPart)childEP;
          DoNetCompartmentEditPart dnce = (DoNetCompartmentEditPart)dne.getChildren().get(0);
          doNetChildBounds = getChildrenBounds(dnce);
         
          // update only if change made some nodes disappear
          //doNetVisible = (dne.primaryShape.getBounds().width >= doNetChildBounds.x + doNetChildBounds.width)
          //            && (dne.primaryShape.getBounds().height >= doNetChildBounds.y + doNetChildBounds.height);
        }
      }
    }
    
    if (preNetChildBounds == null || doNetChildBounds == null)
      return;
    if (preNetChildBounds.isEmpty() || doNetChildBounds.isEmpty())
      return;
    // update only if change made some nodes disappear
    //if (preNetVisible && doNetVisible)
    //  return;
    
    // compute new bounds and position
    //int oldWidth = getSize().width;
    //int oldHeight = getSize().width;
    
    int oldX = getLocation().x;
    int oldY = getLocation().y;

    // enlarge size of preNet and doNet parts to allow for a small border around the nodes
    preNetChildBounds.width += 30 + preNetChildBounds.x;
    preNetChildBounds.height += 30 + preNetChildBounds.y;
    doNetChildBounds.width += 30 + doNetChildBounds.x;
    doNetChildBounds.height += 30 + doNetChildBounds.y;
    
    // compute new total width and height of the oclet
    int newWidth = (preNetChildBounds.width > doNetChildBounds.width) ? preNetChildBounds.width : doNetChildBounds.width;
    int newHeight = preNetChildBounds.height + doNetChildBounds.height + getPrimaryShape().getFigureOcletHeader().getBounds().height;
    int newX = oldX;
    int newY = oldY;
    
    // cap size at minimum dimensions
    if (newWidth < getPrimaryShape().getMinimumSize().width)
      newWidth = getPrimaryShape().getMinimumSize().width;
    if (newHeight < getPrimaryShape().getMinimumSize().height)
      newHeight = getPrimaryShape().getMinimumSize().height;
    
    // set coordinates for oclet
    Point     ocletLocation
      = new Point(getMapMode().DPtoLP(newX), getMapMode().DPtoLP(newY));
    Dimension ocletSize
      = new Dimension(getMapMode().DPtoLP(newWidth), getMapMode().DPtoLP(newHeight));
    
    // now compute size of preNet and doNet inside the newly positioned oclet
    
    // compute size of the border of the oclet for proper alignment of preNet and doNet
    int borderOffset = 0;
    Border border = getPrimaryShape().getBorder();
    if (border != null && border instanceof LineBorder) {
      borderOffset = ((LineBorder)border).getWidth();
    }
    
    // the preNet is positioned right below the OcletHeader
    // the width of the oclet is larger than the contents of the preNet: use it
    // set height of the preNet to show all its contents
    int preNet_x = 0;
    int preNet_y = getPrimaryShape().getFigureOcletHeader().getBounds().height;
    int preNet_width = newWidth - 2*borderOffset;
    int preNet_height = preNetChildBounds.height;
    Point     preNetLocation
      = new Point(getMapMode().DPtoLP(preNet_x), getMapMode().DPtoLP(preNet_y));
    Dimension preNetSize
      = new Dimension(getMapMode().DPtoLP(preNet_width), getMapMode().DPtoLP(preNet_height));

    // the doNet is positioned below the preNet and extends to the bottom of the oclet
    // the width of the oclet is larger than the contents of the doNet: use it
    // set height of the doNet relative to height of the oclet, preNet, and header
    int doNet_x = 0;
    int doNet_y = preNet_height + (preNet_y - borderOffset);
    int doNet_width = newWidth - 2*borderOffset;
    int doNet_height = newHeight - preNet_height - preNet_y - borderOffset;
    Point     doNetLocation
      = new Point(getMapMode().DPtoLP(doNet_x), getMapMode().DPtoLP(doNet_y));
    Dimension doNetSize
      = new Dimension(getMapMode().DPtoLP(doNet_width), getMapMode().DPtoLP(doNet_height));
    
    // and set new size and positions by SetBoundsCommand, one for each EditPart
    TransactionalEditingDomain editingDomain = OcletEditPart.this.getEditingDomain();
    
    CompoundCommand cc = new CompoundCommand(DiagramUIMessages.SetAutoSizeCommand_Label);
    SetBoundsCommand c = new SetBoundsCommand(editingDomain,
        DiagramUIMessages.SetAutoSizeCommand_Label,
        OcletEditPart.this, new Rectangle(ocletLocation,ocletSize));
    SetBoundsCommand c2 = new SetBoundsCommand(editingDomain,
        DiagramUIMessages.SetAutoSizeCommand_Label,
        pne, new Rectangle(preNetLocation,preNetSize));
    SetBoundsCommand c3 = new SetBoundsCommand(editingDomain,
        DiagramUIMessages.SetAutoSizeCommand_Label,
        dne, new Rectangle(doNetLocation,doNetSize));
    
    cc.add(new ICommandProxy(c));
    cc.add(new ICommandProxy(c2));
    cc.add(new ICommandProxy(c3));
    
    OcletEditPart.this.getViewer().getEditDomain().getCommandStack().execute(cc);
	}
	
 /**
   * Flag for notifying the oclet that the layout of its children has changed and
   * that a resize is reasonable. Reset this flag after the resize has been done.
   */
  public boolean layoutDirty = true;
  

	
	/**
   * @generated NOT
   * 
   * Install a Layout-Listener to automatically resizes the {@link OcletEditPart}
   * to display all its child elements. The resize is only performed if {@link #layoutDirty} is
   * set to <code>true</code>.
   * 
   * @author Dirk Fahland
   */
  @Override
  public void activate() {
    
    // retrieve figure
    IFigure figure = getFigure();
    IFigure parentFigure = figure.getParent();

    parentFigure.addLayoutListener(new LayoutListener.Stub() {
      
      @Override
      public void postLayout(IFigure hostFigure) {
        if (layoutDirty) {
          resizeOclet();
          layoutDirty = false;
        }
      }
    });
    layoutDirty = true;

    super.activate();
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
					500), getMapMode().DPtoLP(20)));
			fFigureOcletHeader.setMaximumSize(new Dimension(getMapMode().DPtoLP(
					1000), getMapMode().DPtoLP(20)));
			fFigureOcletHeader.setMinimumSize(new Dimension(getMapMode().DPtoLP(
					200), getMapMode().DPtoLP(20)));
			fFigureOcletHeader.setLocation(
					new Point(getMapMode().DPtoLP(0),
							  getMapMode().DPtoLP(0)));
			fFigureOcletHeader.setSize(fFigureOcletHeader.getPreferredSize());

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
