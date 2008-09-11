package hub.top.adaptiveSystem.diagram.edit.policies;

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
public class EventAPItemSemanticEditPolicy
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
				System.out
						.println("An arc from adaptive process to an oclet is not allowed.");
				Shell shell = new Shell();
				MessageDialog
						.openError(shell, "AdaptiveSystem - create arc",
								"An arc from a node in adaptive process to one in an oclet is not allowed.");
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
		//System.out.println("EventAdaptiveProcessItemSemanticEditPolicy.equalParentNet(CreateReq) START.");
		if (req.getSource() != null && req.getTarget() != null) {
			return req.getSource().eContainer().equals(
					req.getTarget().eContainer());
		}

		return true;
	}

	/**
	 * @author Manja Wolf
	 * check during reorientation of an arc that the arc is in the same parentNet (Oclet or AdaptiveProcess) 
	 * 
	 */
	public boolean equalParentNet(ReorientRelationshipRequest req) {
		//System.out.println("EventAdaptiveProcessItemSemanticEditPolicy.equalParentNet(ReorientReq) START.");
		if (req.getOldRelationshipEnd() != null
				&& req.getNewRelationshipEnd() != null) {
			if (!req.getOldRelationshipEnd().eContainer().equals(
					req.getNewRelationshipEnd().eContainer())) {
				System.out
						.println("An arc from adaptive process to an oclet is not allowed.");
				Shell shell = new Shell();
				MessageDialog
						.openError(shell, "AdaptiveSystem - reorient arc",
								"An arc from a node in adaptive process to one in an oclet is not allowed.");
				return false;
			}
		}

		return true;
	}
	
	/**
	 * Returns command to reorient EClass based link. New link target or source
	 * should be the domain model element associated with this node.
	 * 
	 * @generated NOT
	 */
	protected Command getReorientRelationshipCommand(
			ReorientRelationshipRequest req) {
		switch (getVisualID(req)) {
		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID:
			//START: Manja Wolf
			if (!equalParentNet(req))
				return null;
			//END: Manja Wolf
			return getGEFWrapper(new hub.top.adaptiveSystem.diagram.edit.commands.ArcToConditionReorientCommand(
					req));
		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID:
			//START: Manja Wolf
			if (!equalParentNet(req))
				return null;
			//END: Manja Wolf
			return getGEFWrapper(new hub.top.adaptiveSystem.diagram.edit.commands.ArcToEventReorientCommand(
					req));
		}
		return super.getReorientRelationshipCommand(req);
	}

}
