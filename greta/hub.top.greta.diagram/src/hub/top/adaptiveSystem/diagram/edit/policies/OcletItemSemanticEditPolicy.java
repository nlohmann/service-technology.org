package hub.top.adaptiveSystem.diagram.edit.policies;

import java.util.Iterator;

import org.eclipse.emf.ecore.EAnnotation;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gmf.runtime.emf.type.core.commands.DestroyElementCommand;
import org.eclipse.gmf.runtime.emf.type.core.requests.CreateElementRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.DestroyElementRequest;
import org.eclipse.gmf.runtime.notation.Node;
import org.eclipse.gmf.runtime.notation.View;

/**
 * @generated
 */
public class OcletItemSemanticEditPolicy
		extends
		hub.top.adaptiveSystem.diagram.edit.policies.AdaptiveSystemBaseItemSemanticEditPolicy {

	/**
	 * @generated
	 */
	protected Command getCreateCommand(CreateElementRequest req) {
		if (hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.PreNet_2003 == req
				.getElementType()) {
			if (req.getContainmentFeature() == null) {
				req
						.setContainmentFeature(hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
								.getOclet_PreNet());
			}
			return getGEFWrapper(new hub.top.adaptiveSystem.diagram.edit.commands.PreNetCreateCommand(
					req));
		}
		if (hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.DoNet_2006 == req
				.getElementType()) {
			if (req.getContainmentFeature() == null) {
				req
						.setContainmentFeature(hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
								.getOclet_DoNet());
			}
			return getGEFWrapper(new hub.top.adaptiveSystem.diagram.edit.commands.DoNetCreateCommand(
					req));
		}
		return super.getCreateCommand(req);
	}

	/**
	 * @generated
	 */
	protected Command getDestroyElementCommand(DestroyElementRequest req) {
		CompoundCommand cc = getDestroyEdgesCommand();
		addDestroyChildNodesCommand(cc);
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
	protected void addDestroyChildNodesCommand(CompoundCommand cmd) {
		View view = (View) getHost().getModel();
		EAnnotation annotation = view.getEAnnotation("Shortcut"); //$NON-NLS-1$
		if (annotation != null) {
			return;
		}
		for (Iterator it = view.getChildren().iterator(); it.hasNext();) {
			Node node = (Node) it.next();
			switch (hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getVisualID(node)) {
			case hub.top.adaptiveSystem.diagram.edit.parts.PreNetEditPart.VISUAL_ID:
				cmd.add(getDestroyElementCommand(node));
				break;
			case hub.top.adaptiveSystem.diagram.edit.parts.DoNetEditPart.VISUAL_ID:
				cmd.add(getDestroyElementCommand(node));
				break;
			}
		}
	}

}
