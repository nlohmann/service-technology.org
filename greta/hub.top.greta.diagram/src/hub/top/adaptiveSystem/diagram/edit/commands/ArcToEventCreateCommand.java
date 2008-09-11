package hub.top.adaptiveSystem.diagram.edit.commands;

import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.gmf.runtime.common.core.command.CommandResult;
import org.eclipse.gmf.runtime.emf.type.core.commands.CreateElementCommand;
import org.eclipse.gmf.runtime.emf.type.core.requests.ConfigureRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.CreateRelationshipRequest;

/**
 * @generated
 */
public class ArcToEventCreateCommand extends CreateElementCommand {

	/**
	 * @generated
	 */
	private final EObject source;

	/**
	 * @generated
	 */
	private final EObject target;

	/**
	 * @generated
	 */
	private hub.top.adaptiveSystem.OccurrenceNet container;

	/**
	 * @generated
	 */
	public ArcToEventCreateCommand(CreateRelationshipRequest request,
			EObject source, EObject target) {
		super(request);
		this.source = source;
		this.target = target;
		if (request.getContainmentFeature() == null) {
			setContainmentFeature(hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
					.getOccurrenceNet_Arcs());
		}

		// Find container element for the new link.
		// Climb up by containment hierarchy starting from the source
		// and return the first element that is instance of the container class.
		for (EObject element = source; element != null; element = element
				.eContainer()) {
			if (element instanceof hub.top.adaptiveSystem.OccurrenceNet) {
				container = (hub.top.adaptiveSystem.OccurrenceNet) element;
				super.setElementToEdit(container);
				break;
			}
		}
	}

	/**
	 * @generated
	 */
	public boolean canExecute() {
		if (source == null && target == null) {
			return false;
		}
		if (source != null
				&& !(source instanceof hub.top.adaptiveSystem.Condition)) {
			return false;
		}
		if (target != null && !(target instanceof hub.top.adaptiveSystem.Event)) {
			return false;
		}
		if (getSource() == null) {
			return true; // link creation is in progress; source is not defined yet
		}
		// target may be null here but it's possible to check constraint
		if (getContainer() == null) {
			return false;
		}
		return hub.top.adaptiveSystem.diagram.edit.policies.AdaptiveSystemBaseItemSemanticEditPolicy.LinkConstraints
				.canCreateArcToEvent_3002(getContainer(), getSource(),
						getTarget());
	}

	/**
	 * @generated
	 */
	protected EObject doDefaultElementCreation() {
		// hub.top.adaptiveSystem.ArcToEvent newElement = (hub.top.adaptiveSystem.ArcToEvent) super.doDefaultElementCreation();
		hub.top.adaptiveSystem.ArcToEvent newElement = hub.top.adaptiveSystem.AdaptiveSystemFactory.eINSTANCE
				.createArcToEvent();
		getContainer().getArcs().add(newElement);
		newElement.setSource(getSource());
		newElement.setDestination(getTarget());
		return newElement;
	}

	/**
	 * @generated
	 */
	protected EClass getEClassToEdit() {
		return hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getOccurrenceNet();
	}

	/**
	 * @generated
	 */
	protected CommandResult doExecuteWithResult(IProgressMonitor monitor,
			IAdaptable info) throws ExecutionException {
		if (!canExecute()) {
			throw new ExecutionException(
					"Invalid arguments in create link command"); //$NON-NLS-1$
		}
		return super.doExecuteWithResult(monitor, info);
	}

	/**
	 * @generated
	 */
	protected ConfigureRequest createConfigureRequest() {
		ConfigureRequest request = super.createConfigureRequest();
		request.setParameter(CreateRelationshipRequest.SOURCE, getSource());
		request.setParameter(CreateRelationshipRequest.TARGET, getTarget());
		return request;
	}

	/**
	 * @generated
	 */
	protected void setElementToEdit(EObject element) {
		throw new UnsupportedOperationException();
	}

	/**
	 * @generated
	 */
	protected hub.top.adaptiveSystem.Condition getSource() {
		return (hub.top.adaptiveSystem.Condition) source;
	}

	/**
	 * @generated
	 */
	protected hub.top.adaptiveSystem.Event getTarget() {
		return (hub.top.adaptiveSystem.Event) target;
	}

	/**
	 * @generated
	 */
	public hub.top.adaptiveSystem.OccurrenceNet getContainer() {
		return container;
	}
}
