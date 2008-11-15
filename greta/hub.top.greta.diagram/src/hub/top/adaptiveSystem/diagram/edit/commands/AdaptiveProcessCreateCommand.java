package hub.top.adaptiveSystem.diagram.edit.commands;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.gmf.runtime.emf.type.core.commands.CreateElementCommand;
import org.eclipse.gmf.runtime.emf.type.core.requests.CreateElementRequest;
import org.eclipse.gmf.runtime.notation.View;

/**
 * @generated
 */
public class AdaptiveProcessCreateCommand extends CreateElementCommand {

	/**
	 * @generated
	 */
	public AdaptiveProcessCreateCommand(CreateElementRequest req) {
		super(req);
	}

	/**
	 * @generated
	 */
	protected EObject getElementToEdit() {
		EObject container = ((CreateElementRequest) getRequest())
				.getContainer();
		if (container instanceof View) {
			container = ((View) container).getElement();
		}
		return container;
	}

	/**
	 * @generated
	 */
	public boolean canExecute() {
		hub.top.adaptiveSystem.AdaptiveSystem container = (hub.top.adaptiveSystem.AdaptiveSystem) getElementToEdit();
		if (container.getAdaptiveProcess() != null) {
			return false;
		}
		return true;
	}

	/**
	 * @generated
	 */
	protected EClass getEClassToEdit() {
		return hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getAdaptiveSystem();
	}

}
