package hub.top.adaptiveSystem.diagram.edit.policies;

import org.eclipse.gef.commands.Command;
import org.eclipse.gmf.runtime.emf.type.core.requests.CreateElementRequest;

/**
 * @generated
 */
public class AdaptiveProcessCompartmentItemSemanticEditPolicy
		extends
		hub.top.adaptiveSystem.diagram.edit.policies.AdaptiveSystemBaseItemSemanticEditPolicy {

	/**
	 * @generated
	 */
	protected Command getCreateCommand(CreateElementRequest req) {
		if (hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2001 == req
				.getElementType()) {
			if (req.getContainmentFeature() == null) {
				req
						.setContainmentFeature(hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
								.getOccurrenceNet_Nodes());
			}
			return getGEFWrapper(new hub.top.adaptiveSystem.diagram.edit.commands.ConditionAPCreateCommand(
					req));
		}
		if (hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2002 == req
				.getElementType()) {
			if (req.getContainmentFeature() == null) {
				req
						.setContainmentFeature(hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
								.getOccurrenceNet_Nodes());
			}
			return getGEFWrapper(new hub.top.adaptiveSystem.diagram.edit.commands.EventAPCreateCommand(
					req));
		}
		return super.getCreateCommand(req);
	}

}
