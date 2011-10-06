package hub.top.adaptiveSystem.diagram.edit.commands;

import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.gmf.runtime.common.core.command.CommandResult;
import org.eclipse.gmf.runtime.emf.type.core.commands.EditElementCommand;
import org.eclipse.gmf.runtime.emf.type.core.requests.ReorientRelationshipRequest;

/**
 * @generated
 */
public class ArcToEventReorientCommand extends EditElementCommand {

	/**
	 * @generated
	 */
	private final int reorientDirection;

	/**
	 * @generated
	 */
	private final EObject oldEnd;

	/**
	 * @generated
	 */
	private final EObject newEnd;

	/**
	 * @generated
	 */
	public ArcToEventReorientCommand(ReorientRelationshipRequest request) {
		super(request.getLabel(), request.getRelationship(), request);
		reorientDirection = request.getDirection();
		oldEnd = request.getOldRelationshipEnd();
		newEnd = request.getNewRelationshipEnd();
	}

	/**
	 * @generated
	 */
	public boolean canExecute() {
		if (false == getElementToEdit() instanceof hub.top.adaptiveSystem.ArcToEvent) {
			return false;
		}
		if (reorientDirection == ReorientRelationshipRequest.REORIENT_SOURCE) {
			return canReorientSource();
		}
		if (reorientDirection == ReorientRelationshipRequest.REORIENT_TARGET) {
			return canReorientTarget();
		}
		return false;
	}

	/**
	 * @generated
	 */
	protected boolean canReorientSource() {
		if (!(oldEnd instanceof hub.top.adaptiveSystem.Condition && newEnd instanceof hub.top.adaptiveSystem.Condition)) {
			return false;
		}
		hub.top.adaptiveSystem.Event target = getLink().getDestination();
		if (!(getLink().eContainer() instanceof hub.top.adaptiveSystem.OccurrenceNet)) {
			return false;
		}
		hub.top.adaptiveSystem.OccurrenceNet container = (hub.top.adaptiveSystem.OccurrenceNet) getLink()
				.eContainer();
		return hub.top.adaptiveSystem.diagram.edit.policies.AdaptiveSystemBaseItemSemanticEditPolicy.LinkConstraints
				.canExistArcToEvent_3002(container, getNewSource(), target);
	}

	/**
	 * @generated
	 */
	protected boolean canReorientTarget() {
		if (!(oldEnd instanceof hub.top.adaptiveSystem.Event && newEnd instanceof hub.top.adaptiveSystem.Event)) {
			return false;
		}
		hub.top.adaptiveSystem.Condition source = getLink().getSource();
		if (!(getLink().eContainer() instanceof hub.top.adaptiveSystem.OccurrenceNet)) {
			return false;
		}
		hub.top.adaptiveSystem.OccurrenceNet container = (hub.top.adaptiveSystem.OccurrenceNet) getLink()
				.eContainer();
		return hub.top.adaptiveSystem.diagram.edit.policies.AdaptiveSystemBaseItemSemanticEditPolicy.LinkConstraints
				.canExistArcToEvent_3002(container, source, getNewTarget());
	}

	/**
	 * @generated
	 */
	protected CommandResult doExecuteWithResult(IProgressMonitor monitor,
			IAdaptable info) throws ExecutionException {
		if (!canExecute()) {
			throw new ExecutionException(
					"Invalid arguments in reorient link command"); //$NON-NLS-1$
		}
		if (reorientDirection == ReorientRelationshipRequest.REORIENT_SOURCE) {
			return reorientSource();
		}
		if (reorientDirection == ReorientRelationshipRequest.REORIENT_TARGET) {
			return reorientTarget();
		}
		throw new IllegalStateException();
	}

	/**
	 * @generated
	 */
	protected CommandResult reorientSource() throws ExecutionException {
		getLink().setSource(getNewSource());
		return CommandResult.newOKCommandResult(getLink());
	}

	/**
	 * @generated
	 */
	protected CommandResult reorientTarget() throws ExecutionException {
		getLink().setDestination(getNewTarget());
		return CommandResult.newOKCommandResult(getLink());
	}

	/**
	 * @generated
	 */
	protected hub.top.adaptiveSystem.ArcToEvent getLink() {
		return (hub.top.adaptiveSystem.ArcToEvent) getElementToEdit();
	}

	/**
	 * @generated
	 */
	protected hub.top.adaptiveSystem.Condition getOldSource() {
		return (hub.top.adaptiveSystem.Condition) oldEnd;
	}

	/**
	 * @generated
	 */
	protected hub.top.adaptiveSystem.Condition getNewSource() {
		return (hub.top.adaptiveSystem.Condition) newEnd;
	}

	/**
	 * @generated
	 */
	protected hub.top.adaptiveSystem.Event getOldTarget() {
		return (hub.top.adaptiveSystem.Event) oldEnd;
	}

	/**
	 * @generated
	 */
	protected hub.top.adaptiveSystem.Event getNewTarget() {
		return (hub.top.adaptiveSystem.Event) newEnd;
	}
}
