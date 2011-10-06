package hub.top.editor.ptnetLoLA.diagram.edit.commands;

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
public class ArcToPlaceCreateCommand extends CreateElementCommand {

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
	private hub.top.editor.ptnetLoLA.PtNet container;

	/**
	 * @generated
	 */
	public ArcToPlaceCreateCommand(CreateRelationshipRequest request,
			EObject source, EObject target) {
		super(request);
		this.source = source;
		this.target = target;
		if (request.getContainmentFeature() == null) {
			setContainmentFeature(hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
					.getPtNet_Arcs());
		}

		// Find container element for the new link.
		// Climb up by containment hierarchy starting from the source
		// and return the first element that is instance of the container class.
		for (EObject element = source; element != null; element = element
				.eContainer()) {
			if (element instanceof hub.top.editor.ptnetLoLA.PtNet) {
				container = (hub.top.editor.ptnetLoLA.PtNet) element;
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
				&& false == source instanceof hub.top.editor.ptnetLoLA.Node) {
			return false;
		}
		if (target != null
				&& false == target instanceof hub.top.editor.ptnetLoLA.Node) {
			return false;
		}
		if (getSource() == null) {
			return true; // link creation is in progress; source is not defined yet
		}
		// target may be null here but it's possible to check constraint
		if (getContainer() == null) {
			return false;
		}
		return hub.top.editor.ptnetLoLA.diagram.edit.policies.PtnetLoLABaseItemSemanticEditPolicy.LinkConstraints
				.canCreateArcToPlace_4001(getContainer(), getSource(), getTarget());
	}

	/**
	 * @generated
	 */
	protected EObject doDefaultElementCreation() {
		hub.top.editor.ptnetLoLA.ArcToPlace newElement = hub.top.editor.ptnetLoLA.PtnetLoLAFactory.eINSTANCE
				.createArcToPlace();
		getContainer().getArcs().add(newElement);
		newElement.setSource(getSource());
		newElement.setTarget(getTarget());
		return newElement;
	}

	/**
	 * @generated
	 */
	protected EClass getEClassToEdit() {
		return hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE.getPtNet();
	}

	/**
	 * @generated
	 */
	protected CommandResult doExecuteWithResult(IProgressMonitor monitor,
			IAdaptable info) throws ExecutionException {
		if (!canExecute()) {
			throw new ExecutionException("Invalid arguments in create link command"); //$NON-NLS-1$
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
	protected hub.top.editor.ptnetLoLA.Node getSource() {
		return (hub.top.editor.ptnetLoLA.Node) source;
	}

	/**
	 * @generated
	 */
	protected hub.top.editor.ptnetLoLA.Node getTarget() {
		return (hub.top.editor.ptnetLoLA.Node) target;
	}

	/**
	 * @generated
	 */
	public hub.top.editor.ptnetLoLA.PtNet getContainer() {
		return container;
	}
}
