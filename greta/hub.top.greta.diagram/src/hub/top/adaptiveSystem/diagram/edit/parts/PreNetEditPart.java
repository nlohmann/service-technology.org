package hub.top.adaptiveSystem.diagram.edit.parts;

import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.AdaptiveSystemPackage;
import hub.top.adaptiveSystem.ArcToCondition;
import hub.top.adaptiveSystem.ArcToEvent;
import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.Oclet;
import hub.top.adaptiveSystem.PreNet;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
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
import org.eclipse.swt.graphics.Color;

/**
 * @generated
 */
public class PreNetEditPart extends ShapeNodeEditPart {

	/**
	 * @generated
	 */
	public static final int VISUAL_ID = 2003;

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
	public PreNetEditPart(View view) {
		super(view);
	}

	/**
	 * @generated
	 */
	protected void createDefaultEditPolicies() {
		super.createDefaultEditPolicies();
		installEditPolicy(
				EditPolicyRoles.SEMANTIC_ROLE,
				new hub.top.adaptiveSystem.diagram.edit.policies.PreNetItemSemanticEditPolicy());
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
	 * @generated
	 */
	protected IFigure createNodeShape() {
		PreNetDescriptor figure = new PreNetDescriptor();
		return primaryShape = figure;
	}

	/**
	 * @generated
	 */
	public PreNetDescriptor getPrimaryShape() {
		return (PreNetDescriptor) primaryShape;
	}

	/**
	 * @generated
	 */
	protected boolean addFixedChild(EditPart childEditPart) {
		if (childEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.PreNetCompartmentEditPart) {
			IFigure pane = getPrimaryShape().getFigurePreNetBody();
			setupContentPane(pane); // FIXME each comparment should handle his content pane in his own way 
			pane
					.add(((hub.top.adaptiveSystem.diagram.edit.parts.PreNetCompartmentEditPart) childEditPart)
							.getFigure());
			return true;
		}
		return false;
	}

	/**
	 * @generated
	 */
	protected boolean removeFixedChild(EditPart childEditPart) {

		if (childEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.PreNetCompartmentEditPart) {
			IFigure pane = getPrimaryShape().getFigurePreNetBody();
			setupContentPane(pane); // FIXME each comparment should handle his content pane in his own way 
			pane
					.remove(((hub.top.adaptiveSystem.diagram.edit.parts.PreNetCompartmentEditPart) childEditPart)
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

		if (editPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.PreNetCompartmentEditPart) {
			return getPrimaryShape().getFigurePreNetBody();
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

		if (notification.getNotifier() instanceof PreNet) {
			if (//creating new nodes in PreNet
			(notification.getNewValue() != null
					&& (notification.getNewValue() instanceof Condition || notification
							.getNewValue() instanceof Event) && notification
					.getOldValue() == null)
					||
					//deleted nodes in PreNet
					(notification.getOldValue() != null
							&& (notification.getOldValue() instanceof Condition || notification
									.getOldValue() instanceof Event) && notification
							.getNewValue() == null)) {
				//after creating a new node or deleting a node in preNet, the wellformedness of oclets should be checked
				//System.out.println("a new node in preNet instanciated.");
				resetCheckOclets();
			} else
			//if a new arcToCondition is set to a node of a PreNet or an arcToCondition is deleted
			//than wellformednes of oclet should be checked
			if (notification.getNewValue() != null
					&& notification.getNewValue() instanceof ArcToCondition
					|| notification.getOldValue() != null
					&& notification.getOldValue() instanceof ArcToCondition
					&& notification.getNewValue() == null) {
				//System.out.println("a new arcToCondition in preNet instanciated or an arcToCondition is deleted.");
				resetCheckOclets();
			} else
			//if a new arcToEvent is set to a node of a PreNet or an arcToCondition is deleted
			//than wellformednes of oclet should be checked
			if (notification.getNewValue() != null
					&& notification.getNewValue() instanceof ArcToEvent
					|| notification.getOldValue() != null
					&& notification.getOldValue() instanceof ArcToEvent
					&& notification.getNewValue() == null) {
				//System.out.println("a new arcToCondition in preNet instanciated or an arcToEvent is deleted.");
				resetCheckOclets();
			}
		}
		super.handleNotificationEvent(notification);
	}

	/** 
	 * @author Manja Wolf
	 */
	public void resetCheckOclets() {
		AdaptiveSystem adaptiveSystem = (AdaptiveSystem) ((PreNet) ((Node) this
				.getModel()).getElement()).eContainer().eContainer();
		Oclet oclet = (Oclet) ((PreNet) ((Node) this.getModel()).getElement())
				.eContainer();

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
	 * @param arc
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
	public class PreNetDescriptor extends RectangleFigure {

		/**
		 * @generated
		 */
		private RectangleFigure fFigurePreNetBody;

		/**
		 * @generated
		 */
		public PreNetDescriptor() {
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

			RectangleFigure preNetBackground0 = new RectangleFigure();
			preNetBackground0.setBackgroundColor(PRENETBACKGROUND0_BACK);

			this.add(preNetBackground0);

			fFigurePreNetBody = new RectangleFigure();
			fFigurePreNetBody.setFill(false);
			fFigurePreNetBody.setPreferredSize(new Dimension(getMapMode()
					.DPtoLP(200), getMapMode().DPtoLP(90)));

			this.add(fFigurePreNetBody);

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
		public RectangleFigure getFigurePreNetBody() {
			return fFigurePreNetBody;
		}

	}

	/**
	 * @generated
	 */
	static final Color PRENETBACKGROUND0_BACK = new Color(null, 240, 240, 240);

}
