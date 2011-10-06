package hub.top.editor.ptnetLoLA.diagram.edit.policies;

import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.gef.commands.Command;
import org.eclipse.gmf.runtime.diagram.ui.editparts.IGraphicalEditPart;
import org.eclipse.gmf.runtime.emf.commands.core.commands.DuplicateEObjectsCommand;
import org.eclipse.gmf.runtime.emf.type.core.requests.CreateElementRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.DuplicateElementsRequest;

/**
 * @generated
 */
public class PtNetItemSemanticEditPolicy
		extends
		hub.top.editor.ptnetLoLA.diagram.edit.policies.PtnetLoLABaseItemSemanticEditPolicy {

	/**
	 * @generated
	 */
	protected Command getCreateCommand(CreateElementRequest req) {
		if (hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.Transition_2001 == req
				.getElementType()) {
			if (req.getContainmentFeature() == null) {
				req
						.setContainmentFeature(hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
								.getPtNet_Transitions());
			}
			return getGEFWrapper(new hub.top.editor.ptnetLoLA.diagram.edit.commands.TransitionCreateCommand(
					req));
		}
		if (hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.Place_2002 == req
				.getElementType()) {
			if (req.getContainmentFeature() == null) {
				req
						.setContainmentFeature(hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
								.getPtNet_Places());
			}
			return getGEFWrapper(new hub.top.editor.ptnetLoLA.diagram.edit.commands.PlaceCreateCommand(
					req));
		}
		return super.getCreateCommand(req);
	}

	/**
	 * @generated
	 */
	protected Command getDuplicateCommand(DuplicateElementsRequest req) {
		TransactionalEditingDomain editingDomain = ((IGraphicalEditPart) getHost())
				.getEditingDomain();
		return getGEFWrapper(new DuplicateAnythingCommand(editingDomain, req));
	}

	/**
	 * @generated
	 */
	private static class DuplicateAnythingCommand extends
			DuplicateEObjectsCommand {

		/**
		 * @generated
		 */
		public DuplicateAnythingCommand(TransactionalEditingDomain editingDomain,
				DuplicateElementsRequest req) {
			super(editingDomain, req.getLabel(), req.getElementsToBeDuplicated(), req
					.getAllDuplicatedElementsMap());
		}

	}

}
