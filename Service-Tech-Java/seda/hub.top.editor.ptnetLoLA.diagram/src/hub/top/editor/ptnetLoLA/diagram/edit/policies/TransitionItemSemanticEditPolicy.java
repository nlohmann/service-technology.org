package hub.top.editor.ptnetLoLA.diagram.edit.policies;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gmf.runtime.emf.type.core.commands.DestroyElementCommand;
import org.eclipse.gmf.runtime.emf.type.core.requests.CreateRelationshipRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.DestroyElementRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.ReorientRelationshipRequest;
import org.eclipse.gmf.runtime.notation.View;

/**
 * @generated
 */
public class TransitionItemSemanticEditPolicy
		extends
		hub.top.editor.ptnetLoLA.diagram.edit.policies.PtnetLoLABaseItemSemanticEditPolicy {

	/**
	 * @generated
	 */
	protected Command getDestroyElementCommand(DestroyElementRequest req) {
		CompoundCommand cc = getDestroyEdgesCommand();
		addDestroyShortcutsCommand(cc);
		View view = (View) getHost().getModel();
		if (view.getEAnnotation("Shortcut") != null) { //$NON-NLS-1$
			req.setElementToDestroy(view);
		}
		cc.add(getGEFWrapper(new DestroyElementCommand(req)));
		return cc.unwrap();
	}

	/**
	 * @generated
	 */
	protected Command getCreateRelationshipCommand(CreateRelationshipRequest req) {
		Command command = req.getTarget() == null ? getStartCreateRelationshipCommand(req)
				: getCompleteCreateRelationshipCommand(req);
		return command != null ? command : super.getCreateRelationshipCommand(req);
	}

	/**
	 * @generated
	 */
	protected Command getStartCreateRelationshipCommand(
			CreateRelationshipRequest req) {
		if (hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToPlace_4001 == req
				.getElementType()) {
			return getGEFWrapper(new hub.top.editor.ptnetLoLA.diagram.edit.commands.ArcToPlaceCreateCommand(
					req, req.getSource(), req.getTarget()));
		}
		if (hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToTransition_4002 == req
				.getElementType()) {
			return getGEFWrapper(new hub.top.editor.ptnetLoLA.diagram.edit.commands.ArcToTransitionCreateCommand(
					req, req.getSource(), req.getTarget()));
		}
		return null;
	}

	/**
	 * @generated
	 */
	protected Command getCompleteCreateRelationshipCommand(
			CreateRelationshipRequest req) {
		if (hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToPlace_4001 == req
				.getElementType()) {
			return getGEFWrapper(new hub.top.editor.ptnetLoLA.diagram.edit.commands.ArcToPlaceCreateCommand(
					req, req.getSource(), req.getTarget()));
		}
		if (hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToTransition_4002 == req
				.getElementType()) {
			return getGEFWrapper(new hub.top.editor.ptnetLoLA.diagram.edit.commands.ArcToTransitionCreateCommand(
					req, req.getSource(), req.getTarget()));
		}
		return null;
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
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID:
			return getGEFWrapper(new hub.top.editor.ptnetLoLA.diagram.edit.commands.ArcToPlaceReorientCommand(
					req));
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID:
			return getGEFWrapper(new hub.top.editor.ptnetLoLA.diagram.edit.commands.ArcToTransitionReorientCommand(
					req));
		}
		return super.getReorientRelationshipCommand(req);
	}

}
