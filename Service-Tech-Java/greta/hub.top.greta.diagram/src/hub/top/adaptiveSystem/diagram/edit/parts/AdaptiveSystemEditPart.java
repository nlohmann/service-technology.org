package hub.top.adaptiveSystem.diagram.edit.parts;

import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.AdaptiveSystemPackage;
import hub.top.adaptiveSystem.Oclet;

import org.eclipse.emf.common.command.CommandStack;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.gmf.runtime.diagram.ui.editparts.DiagramEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.EditPolicyRoles;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.gmf.runtime.notation.View;

/**
 * @generated
 */
public class AdaptiveSystemEditPart extends DiagramEditPart {

	/**
	 * @generated
	 */
	public final static String MODEL_ID = "AdaptiveSystem"; //$NON-NLS-1$

	/**
	 * @generated
	 */
	public static final int VISUAL_ID = 79;

	/**
	 * @generated
	 */
	public AdaptiveSystemEditPart(View view) {
		super(view);
	}

	/**
	 * @author Manja Wolf
	 */
	@Override
	protected void handleNotificationEvent(Notification notification) {
		if (notification.getNewValue() != null
				&& notification.getNewValue() instanceof Oclet
				&& notification.getOldValue() == null) {
			//after creating a new oclet, the wellformedness should be checked
			//System.out.println("a new oclet instanciated.");
			AdaptiveSystem adaptiveSystem = (AdaptiveSystem) ((Diagram) this
					.getModel()).getElement();
			if (adaptiveSystem.isSetWellformednessToOclets()) {
				SetCommand cmd = new SetCommand(this.getEditingDomain(),
						adaptiveSystem, AdaptiveSystemPackage.eINSTANCE
								.getAdaptiveSystem_SetWellformednessToOclets(),
						false);
				cmd.setLabel("set adaptive process attribute "
						+ AdaptiveSystemPackage.eINSTANCE
								.getAdaptiveSystem_SetWellformednessToOclets()
								.getName());
				cmd.canExecute();
				((CommandStack) ((EditingDomain) this.getEditingDomain())
						.getCommandStack()).execute(cmd);
			}
		}
		super.handleNotificationEvent(notification);
	}

	/**
	 * @generated
	 */
	protected void createDefaultEditPolicies() {
		super.createDefaultEditPolicies();
		installEditPolicy(
				EditPolicyRoles.SEMANTIC_ROLE,
				new hub.top.adaptiveSystem.diagram.edit.policies.AdaptiveSystemItemSemanticEditPolicy());
		installEditPolicy(
				EditPolicyRoles.CANONICAL_ROLE,
				new hub.top.adaptiveSystem.diagram.edit.policies.AdaptiveSystemCanonicalEditPolicy());
		// removeEditPolicy(org.eclipse.gmf.runtime.diagram.ui.editpolicies.EditPolicyRoles.POPUPBAR_ROLE);
	}

}
