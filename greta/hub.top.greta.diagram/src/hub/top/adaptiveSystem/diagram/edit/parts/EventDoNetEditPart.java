package hub.top.adaptiveSystem.diagram.edit.parts;

import hub.top.adaptiveSystem.AdaptiveProcess;
import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.AdaptiveSystemPackage;
import hub.top.adaptiveSystem.ArcToCondition;
import hub.top.adaptiveSystem.ArcToEvent;
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.Oclet;
import hub.top.adaptiveSystem.Temp;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.draw2d.StackLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.emf.common.command.CommandStack;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.impl.EAttributeImpl;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.editpolicies.LayoutEditPolicy;
import org.eclipse.gef.editpolicies.NonResizableEditPolicy;
import org.eclipse.gef.requests.CreateRequest;
import org.eclipse.gmf.runtime.diagram.ui.editparts.AbstractBorderedShapeEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editparts.IBorderItemEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.BorderItemSelectionEditPolicy;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.EditPolicyRoles;
import org.eclipse.gmf.runtime.diagram.ui.figures.BorderItemLocator;
import org.eclipse.gmf.runtime.gef.ui.figures.DefaultSizeNodeFigure;
import org.eclipse.gmf.runtime.gef.ui.figures.NodeFigure;
import org.eclipse.gmf.runtime.notation.Bounds;
import org.eclipse.gmf.runtime.notation.Edge;
import org.eclipse.gmf.runtime.notation.Node;
import org.eclipse.gmf.runtime.notation.View;

/**
 * @generated
 */
public class EventDoNetEditPart extends AbstractBorderedShapeEditPart {

	/**
	 * @generated
	 */
	public static final int VISUAL_ID = 2008;

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
	public EventDoNetEditPart(View view) {
		super(view);
	}

	/**
	 * @generated
	 */
	protected void createDefaultEditPolicies() {
		super.createDefaultEditPolicies();
		installEditPolicy(
				EditPolicyRoles.SEMANTIC_ROLE,
				new hub.top.adaptiveSystem.diagram.edit.policies.EventDoNetItemSemanticEditPolicy());
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
		EventDescriptor figure = new EventDescriptor();
		return primaryShape = figure;
	}

	/**
	 * @generated
	 */
	public EventDescriptor getPrimaryShape() {
		return (EventDescriptor) primaryShape;
	}

	/**
	 * @generated NOT
	 */
	protected void addBorderItem(IFigure borderItemContainer,
			IBorderItemEditPart borderItemEditPart) {
		if (borderItemEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetNameEditPart) {
			BorderItemLocator locator = new BorderItemLocator(getMainFigure(),
					PositionConstants.WEST);
			locator.setBorderItemOffset(new Dimension(-5, 4));
			borderItemContainer.add(borderItemEditPart.getFigure(), locator);
		} else if (borderItemEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetTempEditPart) {
			BorderItemLocator locator = new BorderItemLocator(getMainFigure(),
					PositionConstants.EAST);
			locator.setBorderItemOffset(new Dimension(-5, 1));
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
				.DPtoLP(20), getMapMode().DPtoLP(20));
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
		return getChildBySemanticHint(hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.getType(hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetNameEditPart.VISUAL_ID));
	}

	@Override
	protected void handleNotificationEvent(Notification notification) {
		Object feature = notification.getFeature();
		//do something after changing the model
		if (notification.getNotifier() instanceof Event) {
			if (feature instanceof EAttributeImpl) {
				EAttributeImpl attribute = (EAttributeImpl) feature;
				//temp, abstract of event changed => refresh colors and reset oclet check
				if (attribute.getName().equals(
						AdaptiveSystemPackage.eINSTANCE.getNode_Temp()
								.getName())
						|| attribute.getName().equals(
								AdaptiveSystemPackage.eINSTANCE
										.getNode_Abstract().getName())) {
					UserImplUpdateNode.updateEvent(this, primaryShape);
					Oclet oclet;
					//the event is in oclet yet
					if (((Event) ((Node) this.getModel()).getElement())
							.eContainer().eContainer() instanceof Oclet) {
						oclet = (Oclet) ((Event) ((Node) this.getModel())
								.getElement()).eContainer().eContainer();
					} else
					//the condition could be in main process (after moving)
					//than the oclet is in oldContainer of the condition
					if (((Event) notification.getNotifier()).eOldContainer()
							.eContainer() instanceof Oclet) {
						oclet = (Oclet) ((Event) notification.getNotifier())
								.eOldContainer().eContainer();
					} else
						//the condition is not in oclet nor in main process
						//should never occur 
						oclet = null;
					resetCheckOclets(oclet);
				} else
				//disabled of event changed => refresh colors
				if (attribute.getName().equals(
						AdaptiveSystemPackage.eINSTANCE
								.getNode_DisabledByAntiOclet().getName())
						|| attribute.getName()
								.equals(
										AdaptiveSystemPackage.eINSTANCE
												.getNode_DisabledByConflict()
												.getName())) {
					UserImplUpdateNode.updateEvent(this, primaryShape);
				}
				
				// event enabled in simulation view
        if (attribute.getName().equals(
            AdaptiveSystemPackage.eINSTANCE.getEvent_Enabled()
                .getName())) {
          this.getPrimaryShape().recolorActivatedEvent();
        }
			}
		} else
		//do something after moving an event
		if (notification.getNotifier() instanceof Bounds) {
		  
      // layout: resize the containing oclet
      OcletEditPart oep = (OcletEditPart)getParent().getParent().getParent();
      oep.layoutDirty = true;
		  
			//there is at least one arc connected with event as source
			if (this.getSourceConnections() != null
					&& !this.getSourceConnections().isEmpty()) {
				//check every arc, connected with condition whether connected nodes are in different nets
				for (Object object : this.getSourceConnections()) {
					//arc is definitely an ArcToCondition
					ArcToConditionEditPart arcEP = (ArcToConditionEditPart) object;
					ArcToCondition arc = (ArcToCondition) ((Edge) arcEP
							.getModel()).getElement();
					//an arc between main process and an oclet found => remove arc
					if (arc != null
							&& arc.getDestination() != null
							&& arc.getSource() != null
							&& (arc.getDestination().eContainer() instanceof AdaptiveProcess || arc
									.getSource().eContainer() instanceof AdaptiveProcess)
							&& !arc.getDestination().eContainer().equals(
									arc.getSource().eContainer())) {
						//the arc is in net of the target-node (DoNet)
						if ((arcEP.getTarget()).getParent().getParent() instanceof DoNetEditPart) {
							DoNetEditPart doNetEditPart = (DoNetEditPart) (arcEP
									.getTarget()).getParent().getParent();
							doNetEditPart.removeArc(arcEP);
						} else
						//the arc is in net of the target-node (PreNet)
						if ((arcEP.getTarget()).getParent().getParent() instanceof PreNetEditPart) {
							PreNetEditPart preNetEditPart = (PreNetEditPart) (arcEP
									.getTarget()).getParent().getParent();
							preNetEditPart.removeArc(arcEP);
						} else
						//the arc is in net of the target-node (AdaptiveProcess)
						if (arcEP.getTarget().getParent() instanceof AdaptiveProcess) {
							AdaptiveProcessEditPart adaptiveProcessEditPart = (AdaptiveProcessEditPart) (arcEP
									.getTarget()).getParent();
							adaptiveProcessEditPart.removeArc(arcEP);
						}
					} else
					//an arc between different oclets found => remove arc
					if (arc != null
							&& arc.getDestination() != null
							&& arc.getSource() != null
							&& !(arc.getDestination().eContainer() instanceof AdaptiveProcess || arc
									.getSource().eContainer() instanceof AdaptiveProcess)
							&& !(arc.getDestination().eContainer().eContainer()
									.equals(arc.getSource().eContainer()
											.eContainer()))) {
						//the arc is in net of the target-node (DoNet)
						if ((arcEP.getTarget()).getParent().getParent() instanceof DoNetEditPart) {
							DoNetEditPart doNetEditPart = (DoNetEditPart) (arcEP
									.getTarget()).getParent().getParent();
							doNetEditPart.removeArc(arcEP);
						} else
						//the arc is in net of the target-node (PreNet)
						if ((arcEP.getTarget()).getParent().getParent() instanceof PreNetEditPart) {
							PreNetEditPart preNetEditPart = (PreNetEditPart) (arcEP
									.getTarget()).getParent().getParent();
							preNetEditPart.removeArc(arcEP);
						} else
						//the arc is in net of the target-node (AdaptiveProcess)
						if (arcEP.getTarget().getParent() instanceof AdaptiveProcess) {
							AdaptiveProcessEditPart adaptiveProcessEditPart = (AdaptiveProcessEditPart) (arcEP
									.getTarget()).getParent();
							adaptiveProcessEditPart.removeArc(arcEP);
						}
					}
				}
			}
			//there is at least one arc connected with event as target
			if (this.getTargetConnections() != null
					&& !this.getTargetConnections().isEmpty()) {
				//check every arc, connected with condition whether connected nodes are in different nets
				for (Object object : this.getTargetConnections()) {
					//arc is definitely an ArcToEvent 
					ArcToEventEditPart arcEP = (ArcToEventEditPart) object;
					ArcToEvent arc = (ArcToEvent) ((Edge) arcEP.getModel())
							.getElement();
					//an arc between main process and an oclet found => remove arc
					if (arc != null
							&& arc.getDestination() != null
							&& arc.getSource() != null
							&& (arc.getDestination().eContainer() instanceof AdaptiveProcess || arc
									.getSource().eContainer() instanceof AdaptiveProcess)
							&& !arc.getDestination().eContainer().equals(
									arc.getSource().eContainer())) {
						//the arc is in net of the source-node (DoNet)
						if ((arcEP.getSource()).getParent().getParent() instanceof DoNetEditPart) {
							DoNetEditPart doNetEditPart = (DoNetEditPart) (arcEP
									.getSource()).getParent().getParent();
							doNetEditPart.removeArc(arcEP);
						} else
						//the arc is in net of the source-node (PreNet)
						if ((arcEP.getSource()).getParent().getParent() instanceof PreNetEditPart) {
							PreNetEditPart preNetEditPart = (PreNetEditPart) (arcEP
									.getSource()).getParent().getParent();
							preNetEditPart.removeArc(arcEP);
						} else
						//the arc is in net of the source-node (MainPart)
						if (arcEP.getSource().getParent() instanceof AdaptiveProcess) {
							AdaptiveProcessEditPart adaptiveProcessEditPart = (AdaptiveProcessEditPart) (arcEP
									.getSource()).getParent();
							adaptiveProcessEditPart.removeArc(arcEP);
						}
					} else
					//an arc between different oclets found => remove arc
					if (arc != null
							&& arc.getDestination() != null
							&& arc.getSource() != null
							&& !(arc.getDestination().eContainer() instanceof AdaptiveProcess || arc
									.getSource().eContainer() instanceof AdaptiveProcess)
							&& !(arc.getDestination().eContainer().eContainer()
									.equals(arc.getSource().eContainer()
											.eContainer()))) {
						//the arc is in net of the source-node (DoNet)
						if ((arcEP.getSource()).getParent().getParent() instanceof DoNetEditPart) {
							DoNetEditPart doNetEditPart = (DoNetEditPart) (arcEP
									.getSource()).getParent().getParent();
							doNetEditPart.removeArc(arcEP);
						} else
						//the arc is in net of the source-node (PreNet)
						if ((arcEP.getSource()).getParent().getParent() instanceof PreNetEditPart) {
							PreNetEditPart preNetEditPart = (PreNetEditPart) (arcEP
									.getSource()).getParent().getParent();
							preNetEditPart.removeArc(arcEP);
						} else
						//the arc is in net of the source-node (MainPart)
						if (arcEP.getSource().getParent() instanceof AdaptiveProcess) {
							AdaptiveProcessEditPart adaptiveProcessEditPart = (AdaptiveProcessEditPart) (arcEP
									.getSource()).getParent();
							adaptiveProcessEditPart.removeArc(arcEP);
						}
					}
				}
			}
		}
		super.handleNotificationEvent(notification);
	}

	/** 
	 * After editing oclet, it have to be checked for wellformedness.
	 * This method reset the variables for enabling the button.
	 * @author Manja Wolf
	 *
	 */
	public void resetCheckOclets(Oclet oclet) {
		AdaptiveSystem adaptiveSystem;
		if (oclet != null) {
			if (((Event) ((Node) this.getModel()).getElement()).eContainer()
					.eContainer() instanceof AdaptiveSystem) {
				adaptiveSystem = (AdaptiveSystem) ((Event) ((Node) this
						.getModel()).getElement()).eContainer().eContainer();
			} else {
				adaptiveSystem = (AdaptiveSystem) ((Event) ((Node) this
						.getModel()).getElement()).eContainer().eContainer()
						.eContainer();
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
				//recolor border of oclet
				if (this.getParent().getParent().getParent() instanceof OcletEditPart)
					((OcletEditPart) this.getParent().getParent().getParent())
							.getPrimaryShape().updateFace();
			}
		}
	}

	/**
	 * @generated
	 */
	public class EventDescriptor extends RectangleFigure {

		/**
		 * @generated NOT
		 */
		public EventDescriptor() {
			this.setPreferredSize(new Dimension(getMapMode().DPtoLP(20),
					getMapMode().DPtoLP(20)));
			this.setMaximumSize(new Dimension(getMapMode().DPtoLP(20),
					getMapMode().DPtoLP(20)));
			this.setMinimumSize(new Dimension(getMapMode().DPtoLP(20),
					getMapMode().DPtoLP(20)));
			this.setSize(getMapMode().DPtoLP(20), getMapMode().DPtoLP(20));
			UserImplUpdateNode.updateEvent(EventDoNetEditPart.this, this);
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
     * recolor the event if it is an activated one and it should be fired now or
     * set to default after firing
     * @author Manja Wolf
     * 
     */
    protected void recolorActivatedEvent() {
      Event event = (Event) ((Node) (EventDoNetEditPart.this).getModel())
          .getElement();

      if (event.isEnabled()) {
        //this.setPreferredSize(new Dimension(getMapMode().DPtoLP(24),
        //    getMapMode().DPtoLP(24)));
        if (event.getTemp().equals(Temp.COLD)) {
          this.setBorder(new LineBorder(
              EventAPEditPart.ACTIVATED_COLD_EVENT_FOREGROUND, getMapMode().DPtoLP(3)));
          this.setForegroundColor(EventAPEditPart.ACTIVATED_COLD_EVENT_FOREGROUND);
          this.setBackgroundColor(EventAPEditPart.ACTIVATED_COLD_EVENT_BACKGROUND);
        } else if (event.getTemp().equals(Temp.HOT)) {
          this.setBorder(new LineBorder(
              EventAPEditPart.ACTIVATED_HOT_EVENT_FOREGROUND, getMapMode().DPtoLP(3)));
          this.setForegroundColor(EventAPEditPart.ACTIVATED_HOT_EVENT_FOREGROUND);
          this.setBackgroundColor(EventAPEditPart.ACTIVATED_HOT_EVENT_BACKGROUND);
        }
      } else {
        UserImplUpdateNode.updateEvent(EventDoNetEditPart.this, this);
        //this.setPreferredSize(new Dimension(getMapMode().DPtoLP(20),
        //    getMapMode().DPtoLP(20)));
        this.setBorder(null);
      }
    }
	}

}
