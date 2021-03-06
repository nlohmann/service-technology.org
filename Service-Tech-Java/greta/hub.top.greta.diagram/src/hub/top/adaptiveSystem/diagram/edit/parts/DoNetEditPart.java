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

import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.AdaptiveSystemPackage;
import hub.top.adaptiveSystem.ArcToCondition;
import hub.top.adaptiveSystem.ArcToEvent;
import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.DoNet;
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.Oclet;
import hub.top.adaptiveSystem.gmf.AutoResizePredictRequest;
import hub.top.adaptiveSystem.gmf.OcletResizableEditPolicy;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayoutListener;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.draw2d.StackLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.emf.common.command.CommandStack;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.Request;
import org.eclipse.gef.RequestConstants;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gef.editpolicies.LayoutEditPolicy;
import org.eclipse.gef.editpolicies.NonResizableEditPolicy;
import org.eclipse.gef.requests.CreateRequest;
import org.eclipse.gef.requests.GroupRequest;
import org.eclipse.gmf.runtime.diagram.ui.commands.CommandProxy;
import org.eclipse.gmf.runtime.diagram.ui.commands.ICommandProxy;
import org.eclipse.gmf.runtime.diagram.ui.editparts.IGraphicalEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editparts.ShapeNodeEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.EditPolicyRoles;
import org.eclipse.gmf.runtime.draw2d.ui.figures.ConstrainedToolbarLayout;
import org.eclipse.gmf.runtime.emf.commands.core.command.CompositeTransactionalCommand;
import org.eclipse.gmf.runtime.gef.ui.figures.DefaultSizeNodeFigure;
import org.eclipse.gmf.runtime.gef.ui.figures.NodeFigure;
import org.eclipse.gmf.runtime.notation.Node;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;

/**
 * @generated
 */
public class DoNetEditPart extends ShapeNodeEditPart {

	/**
	 * @generated
	 */
	public static final int VISUAL_ID = 2006;

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
	public DoNetEditPart(View view) {
		super(view);
	}
	
	/**
	 * @generated NOT
	 */
	protected void createDefaultEditPolicies() {
		super.createDefaultEditPolicies();
		installEditPolicy(
				EditPolicyRoles.SEMANTIC_ROLE,
				new hub.top.adaptiveSystem.diagram.edit.policies.DoNetItemSemanticEditPolicy());
		installEditPolicy(EditPolicy.LAYOUT_ROLE, createLayoutEditPolicy());
		// XXX need an SCR to runtime to have another abstract superclass that would let children add reasonable editpolicies
		// removeEditPolicy(org.eclipse.gmf.runtime.diagram.ui.editpolicies.EditPolicyRoles.CONNECTION_HANDLES_ROLE);
		
		//// <Dirk.F> install nice edit policy
		installEditPolicy(EditPolicy.PRIMARY_DRAG_ROLE,
				new OcletResizableEditPolicy (
						this, PositionConstants.NONE, SWT.VERTICAL | SWT.BOTTOM));
		//// </Dirk.F>

	}

	/**
	 * @generated
	 */
	protected LayoutEditPolicy createLayoutEditPolicy() {
		LayoutEditPolicy lep = new LayoutEditPolicy() {

			protected EditPolicy createChildEditPolicy(EditPart child) {
				EditPolicy result = child
						.getEditPolicy(EditPolicy.PRIMARY_DRAG_ROLE);
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
	 * Active edit part, install layout listener at parent figure
	 * for auto-resizing this edit part.
	 * 
	 * @generated NOT
	 * @author Dirk Fahland
	 */
	@Override
	public void activate() {
		
		// retrieve parent figure
		IFigure figure = getFigure();
		IFigure parentFig = figure.getParent();
		
		// add layout listener to the surrounding oclet figure
		// to handle this subnet figure
		parentFig.addLayoutListener(new LayoutListener.Stub() {
			
            @Override
            public void postLayout(IFigure hostFigure) {
              
            	// after every re-layout of the host figure, send an auto re-size
            	// request to all edit parts and execute the corresponding commands
            	// TODO: move to the host figure?
    	        Dimension newSize = hostFigure.getBounds().getSize();
    	        AutoResizePredictRequest request = new AutoResizePredictRequest(
    	        		hostFigure, newSize);

    	        if (DoNetEditPart.this.understandsRequest(request)) {
  		        	Command c = DoNetEditPart.this.getCommand(request);
  		        	DoNetEditPart.this.getViewer().getEditDomain().getCommandStack().execute(c);
    	        }
            }
		});
		super.activate();
	}


	/**
	 * @generated
	 */
	protected IFigure createNodeShape() {
		DoNetDescriptor figure = new DoNetDescriptor();
		return primaryShape = figure;
	}

	/**
	 * @generated
	 */
	public DoNetDescriptor getPrimaryShape() {
		return (DoNetDescriptor) primaryShape;
	}

	/**
	 * @generated
	 */
	protected boolean addFixedChild(EditPart childEditPart) {
		if (childEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.DoNetCompartmentEditPart) {
			IFigure pane = getPrimaryShape().getFigureDoNetBody();
			setupContentPane(pane); // FIXME each comparment should handle his content pane in his own way 
			pane
					.add(((hub.top.adaptiveSystem.diagram.edit.parts.DoNetCompartmentEditPart) childEditPart)
							.getFigure());
			return true;
		}
		return false;
	}

	/**
	 * @generated
	 */
	protected boolean removeFixedChild(EditPart childEditPart) {

		if (childEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.DoNetCompartmentEditPart) {
			IFigure pane = getPrimaryShape().getFigureDoNetBody();
			setupContentPane(pane); // FIXME each comparment should handle his content pane in his own way 
			pane
					.remove(((hub.top.adaptiveSystem.diagram.edit.parts.DoNetCompartmentEditPart) childEditPart)
							.getFigure());
			return true;
		}
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

		if (editPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.DoNetCompartmentEditPart) {
			return getPrimaryShape().getFigureDoNetBody();
		}
		return super.getContentPaneFor(editPart);
	}

	/**
	 * @generated
	 */
	protected NodeFigure createNodePlate() {
		DefaultSizeNodeFigure result = new DefaultSizeNodeFigure(getMapMode()
				.DPtoLP(200), getMapMode().DPtoLP(90));
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
	 * @generated
	 */
	protected IFigure setupContentPane(IFigure nodeShape) {
		if (nodeShape.getLayoutManager() == null) {
			ConstrainedToolbarLayout layout = new ConstrainedToolbarLayout();
			layout.setSpacing(getMapMode().DPtoLP(5));
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
	 * @author Manja Wolf
	 */
	@Override
	protected void handleNotificationEvent(Notification notification) {

		if (notification.getNotifier() instanceof DoNet) {
			if (//creating new nodes in DoNet
			(notification.getNewValue() != null
					&& (notification.getNewValue() instanceof Condition || notification
							.getNewValue() instanceof Event) && notification
					.getOldValue() == null)
					||
					//deleted nodes in DoNet
					(notification.getOldValue() != null
							&& (notification.getOldValue() instanceof Condition || notification
									.getOldValue() instanceof Event) && notification
							.getNewValue() == null)) {
				//after creating a new node or deleting a node in doNet, the wellformedness of oclets should be checked
				//System.out.println("a new node in doNet instanciated.");
				resetCheckOclets();
			} else
			//if a new arcToCondition is created and source or target in preNet, than oclets should be checked
			if (notification.getNewValue() != null
					&& notification.getNewValue() instanceof ArcToCondition) {
				ArcToCondition arc = (ArcToCondition) notification
						.getNewValue();
				if (arc.getSource().eContainer() instanceof DoNet
						|| arc.getDestination().eContainer() instanceof DoNet) {
					resetCheckOclets();
				}
			}
			//if an arcToCondition is deleted and source or target was in doNet, than oclets should be checked
			if (notification.getOldValue() != null
					&& notification.getOldValue() instanceof ArcToCondition
					&& notification.getNewValue() == null) {
				resetCheckOclets();
			}
			//if a new arcToEvent is created and source or target in doNet, than oclets should be checked
			if (notification.getNewValue() != null
					&& notification.getNewValue() instanceof ArcToEvent) {
				ArcToEvent arc = (ArcToEvent) notification.getNewValue();
				if (arc.getSource().eContainer() instanceof DoNet
						|| arc.getDestination().eContainer() instanceof DoNet) {
					resetCheckOclets();
				}
			}
			//if an arcToEvent is deleted and source or target was in doNet, than oclets should be checked
			if (notification.getOldValue() != null
					&& notification.getOldValue() instanceof ArcToEvent
					&& notification.getNewValue() == null) {
				resetCheckOclets();
			}
		}
		super.handleNotificationEvent(notification);
	}

	/** 
	 * @author Manja Wolf
	 * @param adaptiveSystem
	 */
	public void resetCheckOclets() {
		AdaptiveSystem adaptiveSystem;
		Oclet oclet;
		
		try {
      // in case of complex deletion operations, container relations and model
      // relations might no longer be valid, returning null values
      if (this.getModel() == null) return;
      if ( (((Node)this.getModel()).getElement()) == null) return;
      if ( (((Node)this.getModel()).getElement()).eContainer() == null) return;
      if ( (((Node)this.getModel()).getElement()).eContainer().eContainer() == null) return;
      adaptiveSystem = (AdaptiveSystem) ((DoNet) ((Node) this
				.getModel()).getElement()).eContainer().eContainer();
			oclet = (Oclet) ((DoNet) ((Node) this.getModel()).getElement())
				.eContainer();
		} catch (NullPointerException e) {
			// in case of complex deletion operations, container relations
			// and model relations might no longer be valid, returning
			// null values
			return;
		}

		if (adaptiveSystem.isSetWellformednessToOclets()) {
			SetCommand cmd = new SetCommand(this.getEditingDomain(),
					adaptiveSystem, AdaptiveSystemPackage.eINSTANCE
							.getAdaptiveSystem_SetWellformednessToOclets(),
					false);
			cmd.setLabel("set adaptive process attribute "
					+ AdaptiveSystemPackage.eINSTANCE
							.getAdaptiveSystem_SetWellformednessToOclets()
							.getName());
			cmd.canExecute();
			((CommandStack) ((EditingDomain) this.getEditingDomain())
					.getCommandStack()).execute(cmd);
		}
		if (oclet.isWellFormed()) {
			SetCommand cmd = new SetCommand(this.getEditingDomain(), oclet,
					AdaptiveSystemPackage.eINSTANCE.getOclet_WellFormed(),
					false);
			cmd.setLabel("set oclet attribute "
					+ AdaptiveSystemPackage.eINSTANCE.getOclet_WellFormed()
							.getName());
			cmd.canExecute();
			((CommandStack) ((EditingDomain) this.getEditingDomain())
					.getCommandStack()).execute(cmd);
		}
		((OcletEditPart) this.getParent()).getPrimaryShape().updateFace();
	}

	/**
	 * Arcs between different nets (main process and oclet or different oclets will be removed
	 * @author Manja Wolf
	 * @param arcEP
	 */
	public void removeArc(EditPart arcEP) {
		/* Create the delete request */
		GroupRequest deleteReq = new GroupRequest(RequestConstants.REQ_DELETE);

		CompoundCommand deleteCC = new CompoundCommand("Remove Arc");

		TransactionalEditingDomain editingDomain = this.getEditingDomain();

		CompositeTransactionalCommand compositeCommand = new CompositeTransactionalCommand(
				editingDomain, "Remove Arc");

		/* Send the request to the edit part */
		Command command = arcEP.getCommand(deleteReq);
		if (command != null)
			compositeCommand.compose(new CommandProxy(command));
		// deleteCC.add(editPart.getCommand(deleteReq));

		if (!compositeCommand.isEmpty()) {
			deleteCC.add(new ICommandProxy(compositeCommand));
		}

		this.getEditDomain().getCommandStack().execute(deleteCC);
	}

	/**
	 * @generated
	 */
	public class DoNetDescriptor extends RectangleFigure {

		/**
		 * @generated
		 */
		private RectangleFigure fFigureDoNetBody;

		/**
		 * @generated
		 */
		public DoNetDescriptor() {
			this.setLayoutManager(new StackLayout());
			this.setFill(false);
			this.setOutline(false);
			this.setLineStyle(Graphics.LINE_DASH);
			this.setPreferredSize(new Dimension(getMapMode().DPtoLP(200),
					getMapMode().DPtoLP(90)));
			createContents();
		}

		/**
		 * @generated
		 */
		private void createContents() {

			RectangleFigure doNetBackground0 = new RectangleFigure();
			doNetBackground0.setBackgroundColor(DONETBACKGROUND0_BACK);

			this.add(doNetBackground0);

			fFigureDoNetBody = new RectangleFigure();
			fFigureDoNetBody.setFill(false);
			fFigureDoNetBody.setPreferredSize(new Dimension(getMapMode()
					.DPtoLP(200), getMapMode().DPtoLP(90)));

			this.add(fFigureDoNetBody);

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
		public RectangleFigure getFigureDoNetBody() {
			return fFigureDoNetBody;
		}

	}

	/**
	 * @generated
	 */
	static final Color DONETBACKGROUND0_BACK = new Color(null, 240, 240, 240);

}
