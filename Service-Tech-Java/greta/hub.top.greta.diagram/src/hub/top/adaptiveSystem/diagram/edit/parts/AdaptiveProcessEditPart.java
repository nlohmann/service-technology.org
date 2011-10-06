package hub.top.adaptiveSystem.diagram.edit.parts;

import hub.top.adaptiveSystem.AdaptiveProcess;
import hub.top.adaptiveSystem.AdaptiveSystemPackage;

import java.util.Collection;
import java.util.HashSet;
import java.util.Iterator;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayoutListener;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.draw2d.StackLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.Request;
import org.eclipse.gef.RequestConstants;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gef.editparts.AbstractGraphicalEditPart;
import org.eclipse.gef.editpolicies.LayoutEditPolicy;
import org.eclipse.gef.editpolicies.NonResizableEditPolicy;
import org.eclipse.gef.requests.CreateRequest;
import org.eclipse.gef.requests.GroupRequest;
import org.eclipse.gmf.runtime.diagram.ui.commands.CommandProxy;
import org.eclipse.gmf.runtime.diagram.ui.commands.ICommandProxy;
import org.eclipse.gmf.runtime.diagram.ui.commands.SetBoundsCommand;
import org.eclipse.gmf.runtime.diagram.ui.editparts.IGraphicalEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editparts.ShapeCompartmentEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editparts.ShapeNodeEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.EditPolicyRoles;
import org.eclipse.gmf.runtime.diagram.ui.l10n.DiagramUIMessages;
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
public class AdaptiveProcessEditPart extends ShapeNodeEditPart {

	/**
	 * @generated
	 */
	public static final int VISUAL_ID = 1001;

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
	public AdaptiveProcessEditPart(View view) {
		super(view);
	}

	/**
	 * @generated NOT
	 */
	protected void createDefaultEditPolicies() {
		super.createDefaultEditPolicies();
		installEditPolicy(
				EditPolicyRoles.SEMANTIC_ROLE,
				new hub.top.adaptiveSystem.diagram.edit.policies.AdaptiveProcessItemSemanticEditPolicy());
		installEditPolicy(EditPolicy.LAYOUT_ROLE, createLayoutEditPolicy());
		// XXX need an SCR to runtime to have another abstract superclass that would let children add reasonable editpolicies

		removeEditPolicy(EditPolicyRoles.CONNECTION_HANDLES_ROLE);
		removeEditPolicy(EditPolicyRoles.POPUPBAR_ROLE);  // do not show editor popup bars
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
		AdaptiveProcessDescriptor figure = new AdaptiveProcessDescriptor();
		return primaryShape = figure;
	}
	
	/**
	 * @generated NOT
	 * 
	 * Install a Layout-Listener to automatically resizes the {@link AdaptiveProcessEditPart}
	 * to display all its child elements. The resize policy extends this edit part to the left
	 * and the bottom keeping the right border and the top border fixed.
	 * 
	 * @author Dirk Fahland
	 */
	@Override
	public void activate() {
		// retrieve figure
		IFigure figure = getFigure();
		IFigure parentFigure = figure.getParent();
		
		parentFigure.addLayoutListener(new LayoutListener.Stub() {
			
		  /**
		   * layout listener for automatic resizing of this edit part to display all its children
		   */
			@SuppressWarnings("unchecked")
      @Override
			public void postLayout(IFigure hostFigure) {

				int minX = Integer.MAX_VALUE, maxX = 0;
				int minY = Integer.MAX_VALUE, maxY = 0;

				// compute bounds of the children
				if (AdaptiveProcessEditPart.this.getChildren().size() > 0) {
					AdaptiveProcessCompartmentEditPart apce =
						(AdaptiveProcessCompartmentEditPart)AdaptiveProcessEditPart.this.getChildren().get(0);
					Iterator it = apce.getChildren().iterator();
					while (it.hasNext()) {
						Object o2 = it.next();
						if (o2 instanceof AbstractGraphicalEditPart) {
							AbstractGraphicalEditPart child = (AbstractGraphicalEditPart)o2;
							Rectangle bounds = child.getFigure().getBounds();

							if (bounds.x < minX) minX = bounds.x;
							if (bounds.x+bounds.width > maxX) maxX = bounds.x+bounds.width;

							if (bounds.y < minY) minY = bounds.y;
							if (bounds.y+bounds.height > maxY) maxY = bounds.y+bounds.height;
						}
					}
				}
				
				// compute new bounds and position
				int oldWidth = getSize().width;
				int oldHeight = getSize().width;
				
				int oldX = getLocation().x;
				int oldY = getLocation().y;

				
				maxX += 30; // safe distance to border (take care of scroll bars)
				maxY += 30; // safe distance to border (take care of scroll bars)
				
				int newWidth = (maxX > oldWidth) ? maxX : oldWidth;
				int newHeight = (maxY > oldHeight) ? maxY : oldHeight;
				
				int newX = oldX - (newWidth - oldWidth);
				int newY = oldY;
				
				Point newLocation = new Point(getMapMode().DPtoLP(newX),
						getMapMode().DPtoLP(newY));
				Dimension newSize = new Dimension(getMapMode().DPtoLP(newWidth),
						getMapMode().DPtoLP(newHeight));
				
				/*
				System.out.println(
						"("+oldX+","+oldY+","+(oldX+oldWidth)+","+(oldY+oldHeight)+")"
						+" -> "
						+"("+newX+","+newY+","+(newX+newWidth)+","+(newY+newHeight)+")");
				 */	
				
				// and set new size and position by a SetBoundsCommand
				TransactionalEditingDomain editingDomain = AdaptiveProcessEditPart.this.getEditingDomain();
				
				SetBoundsCommand c = new SetBoundsCommand(editingDomain,
						DiagramUIMessages.SetAutoSizeCommand_Label,
						AdaptiveProcessEditPart.this, new Rectangle(newLocation,newSize));
				AdaptiveProcessEditPart.this.getViewer().getEditDomain().getCommandStack().
					execute(new ICommandProxy(c));
			}
		});

		super.activate();
	}

	/**
	 * @generated
	 */
	public AdaptiveProcessDescriptor getPrimaryShape() {
		return (AdaptiveProcessDescriptor) primaryShape;
	}

	/**
	 * @generated
	 */
	protected boolean addFixedChild(EditPart childEditPart) {
		if (childEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessCompartmentEditPart) {
			IFigure pane = getPrimaryShape().getFigureAdaptiveProcessContent();
			setupContentPane(pane); // FIXME each comparment should handle his content pane in his own way 
			pane
					.add(((hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessCompartmentEditPart) childEditPart)
							.getFigure());
			return true;
		}
		return false;
	}

	/**
	 * @generated
	 */
	protected boolean removeFixedChild(EditPart childEditPart) {

		if (childEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessCompartmentEditPart) {
			IFigure pane = getPrimaryShape().getFigureAdaptiveProcessContent();
			setupContentPane(pane); // FIXME each comparment should handle his content pane in his own way 
			pane
					.remove(((hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessCompartmentEditPart) childEditPart)
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

		if (editPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessCompartmentEditPart) {
			return getPrimaryShape().getFigureAdaptiveProcessContent();
		}
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

	@Override
	protected void handleNotificationEvent(Notification notification) {
		
		if (notification.getNotifier() instanceof AdaptiveProcess) {
			
			// if removing nodes from the adaptive process,
			// also remove all adjacent arcs
			if (   notification.getEventType() == Notification.REMOVE_MANY
				&& notification.getFeature().equals(AdaptiveSystemPackage.eINSTANCE.getOccurrenceNet_Nodes()))
			{
				// the set of arcs we have to remove from the model as well
				HashSet<EditPart> arcEPtoRemove = new HashSet<EditPart>();
				
				// remove all adjacent arcs as well
				if (notification.getOldValue() instanceof Collection) {
					// the nodes that are going to be removed from this edit part
					Collection toRemove = (Collection)notification.getOldValue();
					ShapeCompartmentEditPart comp = (ShapeCompartmentEditPart)this.getChildren().get(0);
					
					// iterate over all child edit parts of this adaptive process to find the
					// edit parts that represent the nodes that are about to be deleted
					for (Object child : comp.getChildren()) {
						if (child instanceof AbstractGraphicalEditPart
							&& ((EditPart) child).getModel() instanceof Node)
						{
							// we found a node ...
							AbstractGraphicalEditPart nodeEP = (AbstractGraphicalEditPart)child;
							EObject modelElement = ((Node)(nodeEP).getModel()).getElement();
							if (toRemove.contains(modelElement)) {
								// ... that will be removed
								// so remove all its outgoing arcs
								for (Object arcEP : nodeEP.getSourceConnections()) {
									if (arcEP instanceof ArcToEventEditPart ||
										arcEP instanceof ArcToConditionEditPart)
									{
										arcEPtoRemove.add((EditPart)arcEP);
									}
								}
								// and remove all its incoming arcs
								for (Object arcEP : nodeEP.getTargetConnections()) {
									if (arcEP instanceof ArcToEventEditPart ||
										arcEP instanceof ArcToConditionEditPart)
									{
										arcEPtoRemove.add((EditPart)arcEP);
									}
								}
							}
						}
					} // collected all arcs that need to be removed
					
					for (EditPart arcEP : arcEPtoRemove)
						removeArc(arcEP);
				}
			}
		}
		
		super.handleNotificationEvent(notification);
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
	public class AdaptiveProcessDescriptor extends RectangleFigure {

		/**
		 * @generated
		 */
		private RectangleFigure fFigureAdaptiveProcessContent;

		/**
		 * @generated
		 */
		public AdaptiveProcessDescriptor() {
			this.setLayoutManager(new StackLayout());
			this.setFill(false);
			this.setOutline(false);
			this.setPreferredSize(new Dimension(getMapMode().DPtoLP(200),
					getMapMode().DPtoLP(200)));
			this.setMinimumSize(new Dimension(getMapMode().DPtoLP(200),
					getMapMode().DPtoLP(200)));
			createContents();
		}

		/**
		 * @generated
		 */
		private void createContents() {

			RectangleFigure adaptiveProcessBackground0 = new RectangleFigure();
			adaptiveProcessBackground0
					.setBackgroundColor(ADAPTIVEPROCESSBACKGROUND0_BACK);
			adaptiveProcessBackground0.setPreferredSize(new Dimension(
					getMapMode().DPtoLP(200), getMapMode().DPtoLP(200)));
			adaptiveProcessBackground0.setMinimumSize(new Dimension(
					getMapMode().DPtoLP(200), getMapMode().DPtoLP(200)));

			this.add(adaptiveProcessBackground0);

			fFigureAdaptiveProcessContent = new RectangleFigure();
			fFigureAdaptiveProcessContent.setFill(false);
			fFigureAdaptiveProcessContent.setOutline(false);
			fFigureAdaptiveProcessContent.setPreferredSize(new Dimension(
					getMapMode().DPtoLP(50), getMapMode().DPtoLP(50)));
			fFigureAdaptiveProcessContent.setMinimumSize(new Dimension(
					getMapMode().DPtoLP(200), getMapMode().DPtoLP(200)));

			this.add(fFigureAdaptiveProcessContent);

		}
		
		@Override
		public void repaint() {

			super.repaint();
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
		public RectangleFigure getFigureAdaptiveProcessContent() {
			return fFigureAdaptiveProcessContent;
		}

	}

	/**
	 * @generated
	 */
	static final Color ADAPTIVEPROCESSBACKGROUND0_BACK = new Color(null, 240,
			240, 240);

}
