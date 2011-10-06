package hub.top.adaptiveSystem.diagram.edit.commands;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.gmf.runtime.emf.type.core.commands.CreateElementCommand;
import org.eclipse.gmf.runtime.emf.type.core.requests.CreateElementRequest;
import org.eclipse.gmf.runtime.notation.View;

/**
 * @generated
 */
public class OcletCreateCommand extends CreateElementCommand {

	/**
	 * @generated
	 */
	public OcletCreateCommand(CreateElementRequest req) {
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
	protected EClass getEClassToEdit() {
		return hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getAdaptiveSystem();
	}

	/**
	 * @generated
	 */
	protected EObject doDefaultElementCreation() {
		hub.top.adaptiveSystem.Oclet newElement = hub.top.adaptiveSystem.AdaptiveSystemFactory.eINSTANCE
				.createOclet();

		hub.top.adaptiveSystem.AdaptiveSystem owner = (hub.top.adaptiveSystem.AdaptiveSystem) getElementToEdit();
		owner.getOclets().add(newElement);

		hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes
				.init_Oclet_1002(newElement);
		return newElement;
	}

}
