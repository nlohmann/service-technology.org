package hub.top.editor.ptnetLoLA.diagram.edit.commands;

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
public class ArcToPlaceReorientCommand extends EditElementCommand {

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
	public ArcToPlaceReorientCommand(ReorientRelationshipRequest request) {
		super(request.getLabel(), request.getRelationship(), request);
		reorientDirection = request.getDirection();
		oldEnd = request.getOldRelationshipEnd();
		newEnd = request.getNewRelationshipEnd();
	}

	/**
	 * @generated
	 */
	public boolean canExecute() {
		if (false == getElementToEdit() instanceof hub.top.editor.ptnetLoLA.ArcToPlace) {
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
		if (!(oldEnd instanceof hub.top.editor.ptnetLoLA.Node && newEnd instanceof hub.top.editor.ptnetLoLA.Node)) {
			return false;
		}
		hub.top.editor.ptnetLoLA.Node target = getLink().getTarget();
		if (!(getLink().eContainer() instanceof hub.top.editor.ptnetLoLA.PtNet)) {
			return false;
		}
		hub.top.editor.ptnetLoLA.PtNet container = (hub.top.editor.ptnetLoLA.PtNet) getLink()
				.eContainer();
		return hub.top.editor.ptnetLoLA.diagram.edit.policies.PtnetLoLABaseItemSemanticEditPolicy.LinkConstraints
				.canExistArcToPlace_4001(container, getNewSource(), target);
	}

	/**
	 * @generated
	 */
	protected boolean canReorientTarget() {
		if (!(oldEnd instanceof hub.top.editor.ptnetLoLA.Node && newEnd instanceof hub.top.editor.ptnetLoLA.Node)) {
			return false;
		}
		hub.top.editor.ptnetLoLA.Node source = getLink().getSource();
		if (!(getLink().eContainer() instanceof hub.top.editor.ptnetLoLA.PtNet)) {
			return false;
		}
		hub.top.editor.ptnetLoLA.PtNet container = (hub.top.editor.ptnetLoLA.PtNet) getLink()
				.eContainer();
		return hub.top.editor.ptnetLoLA.diagram.edit.policies.PtnetLoLABaseItemSemanticEditPolicy.LinkConstraints
				.canExistArcToPlace_4001(container, source, getNewTarget());
	}

	/**
	 * @generated
	 */
	protected CommandResult doExecuteWithResult(IProgressMonitor monitor,
			IAdaptable info) throws ExecutionException {
		if (!canExecute()) {
			throw new ExecutionException("Invalid arguments in reorient link command"); //$NON-NLS-1$
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
		getLink().setTarget(getNewTarget());
		return CommandResult.newOKCommandResult(getLink());
	}

	/**
	 * @generated
	 */
	protected hub.top.editor.ptnetLoLA.ArcToPlace getLink() {
		return (hub.top.editor.ptnetLoLA.ArcToPlace) getElementToEdit();
	}

	/**
	 * @generated
	 */
	protected hub.top.editor.ptnetLoLA.Node getOldSource() {
		return (hub.top.editor.ptnetLoLA.Node) oldEnd;
	}

	/**
	 * @generated
	 */
	protected hub.top.editor.ptnetLoLA.Node getNewSource() {
		return (hub.top.editor.ptnetLoLA.Node) newEnd;
	}

	/**
	 * @generated
	 */
	protected hub.top.editor.ptnetLoLA.Node getOldTarget() {
		return (hub.top.editor.ptnetLoLA.Node) oldEnd;
	}

	/**
	 * @generated
	 */
	protected hub.top.editor.ptnetLoLA.Node getNewTarget() {
		return (hub.top.editor.ptnetLoLA.Node) newEnd;
	}
}
