package hub.top.adaptiveSystem.diagram.edit.policies;

import java.util.Collection;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;

import org.eclipse.gmf.runtime.diagram.ui.editpolicies.CanonicalEditPolicy;
import org.eclipse.gmf.runtime.notation.View;

/**
 * @generated
 */
public class PreNetCompartmentCanonicalEditPolicy extends CanonicalEditPolicy {

	/**
	 * @generated
	 */
	Set myFeaturesToSynchronize;

	/**
	 * @generated
	 */
	protected List getSemanticChildrenList() {
		View viewObject = (View) getHost().getModel();
		List result = new LinkedList();
		for (Iterator it = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramUpdater
				.getPreNetPreNetCompartment_5002SemanticChildren(viewObject)
				.iterator(); it.hasNext();) {
			result
					.add(((hub.top.adaptiveSystem.diagram.part.AdaptiveSystemNodeDescriptor) it
							.next()).getModelElement());
		}
		return result;
	}

	/**
	 * @generated
	 */
	protected boolean isOrphaned(Collection semanticChildren, final View view) {
		int visualID = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.getVisualID(view);
		switch (visualID) {
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart.VISUAL_ID:
		case hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart.VISUAL_ID:
			if (!semanticChildren.contains(view.getElement())) {
				return true;
			}
		}
		return false;
	}

	/**
	 * @generated
	 */
	protected String getDefaultFactoryHint() {
		return null;
	}

	/**
	 * @generated
	 */
	protected Set getFeaturesToSynchronize() {
		if (myFeaturesToSynchronize == null) {
			myFeaturesToSynchronize = new HashSet();
			myFeaturesToSynchronize
					.add(hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
							.getOccurrenceNet_Nodes());
		}
		return myFeaturesToSynchronize;
	}

}
