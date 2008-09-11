package hub.top.adaptiveSystem.diagram.edit.policies;

import hub.top.adaptiveSystem.ArcToCondition;
import hub.top.adaptiveSystem.ArcToEvent;
import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.DoNet;
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.PreNet;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gmf.runtime.emf.type.core.commands.DestroyElementCommand;
import org.eclipse.gmf.runtime.emf.type.core.requests.CreateRelationshipRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.DestroyElementRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.ReorientRelationshipRequest;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Shell;

/**
 * @generated
 */
public class EventDoNetItemSemanticEditPolicy
		extends
		hub.top.adaptiveSystem.diagram.edit.policies.AdaptiveSystemBaseItemSemanticEditPolicy {

	/**
	 * @generated
	 */
	protected Command getDestroyElementCommand(DestroyElementRequest req) {
		CompoundCommand cc = getDestroyEdgesCommand();
		addDestroyShortcutsCommand(cc);
		cc.add(getGEFWrapper(new DestroyElementCommand(req)));
		return cc.unwrap();
	}

	/**
	 * @generated
	 */
	protected Command getCreateRelationshipCommand(CreateRelationshipRequest req) {
		Command command = req.getTarget() == null ? getStartCreateRelationshipCommand(req)
				: getCompleteCreateRelationshipCommand(req);
		return command != null ? command : super
				.getCreateRelationshipCommand(req);
	}

	/**
	 * @generated
	 */
	protected Command getStartCreateRelationshipCommand(
			CreateRelationshipRequest req) {
		if (hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001 == req
				.getElementType()) {
			return getGEFWrapper(new hub.top.adaptiveSystem.diagram.edit.commands.ArcToConditionCreateCommand(
					req, req.getSource(), req.getTarget()));
		}
		if (hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002 == req
				.getElementType()) {
			return null;
		}
		return null;
	}

	/**
	 * @generated NOT
	 */
	protected Command getCompleteCreateRelationshipCommand(
			CreateRelationshipRequest req) {
		if (hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001 == req
				.getElementType()) {
			return null;
		}
		if (hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002 == req
				.getElementType()) {
			//START: Manja Wolf
			//forbid the creation of arcs between different nets and from doNet to preNet
			if (!equalParentNet(req)) {
				return null;
			}
			//END: Manja Wolf
			return getGEFWrapper(new hub.top.adaptiveSystem.diagram.edit.commands.ArcToEventCreateCommand(
					req, req.getSource(), req.getTarget()));
		}
		return null;
	}

	/**
	 * @author Manja Wolf
	 * check during creation of an arc that the arc is in the same parentNet (Oclet or AdaptiveProcess) 
	 * 
	 */
	public boolean equalParentNet(CreateRelationshipRequest req) {
		//System.out.println("EventDoNetItemSemanticEditPolicy.equalParentNet(req) START.");
		if (req.getSource() != null && req.getTarget() != null) {
			/*
			if(req.getSource().eContainer() instanceof AdaptiveProcess || req.getTarget().eContainer() instanceof AdaptiveProcess) {
				return req.getSource().eContainer().equals(req.getTarget().eContainer());
			}
			 */
			if (req.getSource().eContainer() instanceof DoNet
					&& req.getTarget().eContainer() instanceof PreNet) {
				System.out
						.println("An arc from DoNet to PreNet is not allowed.");
				Shell shell = new Shell();
				MessageDialog
						.openError(shell, "AdaptiveSystem - create arc",
								"An arc from a node in DoNet to one in PreNet is not allowed.");
				return false;
			}
			if (!req.getSource().eContainer().eContainer().equals(
					req.getTarget().eContainer().eContainer())) {
				System.out
						.println("An arc between nodes in different oclets or adaptive process and oclet is not allowed.");
				Shell shell = new Shell();
				MessageDialog
						.openError(
								shell,
								"AdaptiveSystem - create arc",
								"An arc between nodes in different oclets or adaptive process and oclet is not allowed..");
				return false;
			}
		}

		return true;
	}

	/**
	 * @author Manja Wolf
	 * check during reorientation of an arc that the arc is in the same parentNet (Oclet or AdaptiveProcess) 
	 * 
	 */
	public boolean equalParentNet(ReorientRelationshipRequest req) {
		//System.out.println("EventDoNetItemSemanticEditPolicy.equalParentNet(ReorientReq) START.");
		if (req.getNewRelationshipEnd() != null
				&& req.getOldRelationshipEnd() != null) {
			/*
			if(req.getOldRelationshipEnd().eContainer() instanceof AdaptiveProcess || req.getNewRelationshipEnd().eContainer() instanceof AdaptiveProcess) {
				if(!req.getOldRelationshipEnd().eContainer().equals(req.getNewRelationshipEnd().eContainer())) {
					
				}
			}
			 */
			//an arc from doNet to preNet is bad
			//here: an ArcToCondition
			if (req.getRelationship() instanceof ArcToCondition) {
				ArcToCondition arc = (ArcToCondition) req.getRelationship();
				Event source = (Event) arc.getSource();
				Condition destination = (Condition) arc.getDestination();
				//Fall 1: source.old = preNet && target.old = preNet => !(source.new = doNet)
				if (source.eContainer() instanceof PreNet
						&& destination.eContainer() instanceof PreNet) {
					if (req.getOldRelationshipEnd().equals(source)
							&& ((Event) req.getNewRelationshipEnd())
									.eContainer() instanceof DoNet) {
						System.out
								.println("An arc from DoNet to PreNet is not allowed.");
						Shell shell = new Shell();
						MessageDialog
								.openError(shell,
										"AdaptiveSystem - create arc",
										"An arc from a node in DoNet to one in PreNet is not allowed.");
						return false;
					}
				}
				//Fall 2: source.old = preNet && target.old = doNet => everything is allowed - nothing to do
				//Fall 3: source.old = doNet && target.old = preNet => forbidden - should never occur
				//Fall 4: source.old = doNet && target.old = doNet => !(target.new = preNet)
				if (source.eContainer() instanceof DoNet
						&& destination.eContainer() instanceof DoNet) {
					if (req.getOldRelationshipEnd().equals(destination)
							&& ((Condition) req.getNewRelationshipEnd())
									.eContainer() instanceof PreNet) {
						System.out
								.println("An arc from DoNet to PreNet is not allowed.");
						Shell shell = new Shell();
						MessageDialog
								.openError(shell,
										"AdaptiveSystem - create arc",
										"An arc from a node in DoNet to one in PreNet is not allowed.");
						return false;
					}
				}
			}
			//here: an ArcToEvent-target
			if (req.getRelationship() instanceof ArcToEvent) {
				ArcToEvent arc = (ArcToEvent) req.getRelationship();
				Condition source = (Condition) arc.getSource();
				Event destination = (Event) arc.getDestination();
				//Fall 1: source.old = preNet && target.old = preNet => !(source.new = doNet)
				if (source.eContainer() instanceof PreNet
						&& destination.eContainer() instanceof PreNet) {
					if (req.getOldRelationshipEnd().equals(source)
							&& ((Condition) req.getNewRelationshipEnd())
									.eContainer() instanceof DoNet) {
						System.out
								.println("An arc from DoNet to PreNet is not allowed.");
						Shell shell = new Shell();
						MessageDialog
								.openError(shell,
										"AdaptiveSystem - create arc",
										"An arc from a node in DoNet to one in PreNet is not allowed.");
						return false;
					}
				}
				//Fall 2: source.old = preNet && target.old = doNet => everything is allowed - nothing to do
				//Fall 3: source.old = doNet && target.old = preNet => forbidden - should never occur
				//Fall 4: source.old = doNet && target.old = doNet => !(target.new = preNet)
				if (source.eContainer() instanceof DoNet
						&& destination.eContainer() instanceof DoNet) {
					if (req.getOldRelationshipEnd().equals(destination)
							&& ((Event) req.getNewRelationshipEnd())
									.eContainer() instanceof PreNet) {
						System.out
								.println("An arc from DoNet to PreNet is not allowed.");
						Shell shell = new Shell();
						MessageDialog
								.openError(shell,
										"AdaptiveSystem - create arc",
										"An arc from a node in DoNet to one in PreNet is not allowed.");
						return false;
					}
				}
			}
			//reorientation to another net (oclet or adaptive process) is bad
			if (!req.getOldRelationshipEnd().eContainer().eContainer().equals(
					req.getNewRelationshipEnd().eContainer().eContainer())) {
				System.out
						.println("An arc between nodes in different oclets or adaptive process and oclet is not allowed.");
				Shell shell = new Shell();
				MessageDialog
						.openError(
								shell,
								"AdaptiveSystem - create arc",
								"An arc between nodes in different oclets or adaptive process and oclet is not allowed..");
				return false;
			}
		}

		return true;
	}
	
	/**
	 * Returns command to reorient EClass based link. New link target or source
	 * should be the domain model element associated with this node.
	 * 
	 * @generated
	 */
	protected Command getReorientRelationshipCommand(
			ReorientRelationshipRequest req) {
		switch (getVisualID(req)) {
		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID:
			return getGEFWrapper(new hub.top.adaptiveSystem.diagram.edit.commands.ArcToConditionReorientCommand(
					req));
		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID:
			return getGEFWrapper(new hub.top.adaptiveSystem.diagram.edit.commands.ArcToEventReorientCommand(
					req));
		}
		return super.getReorientRelationshipCommand(req);
	}

}
