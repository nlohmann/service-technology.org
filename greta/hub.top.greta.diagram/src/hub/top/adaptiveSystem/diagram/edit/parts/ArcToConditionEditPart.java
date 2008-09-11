package hub.top.adaptiveSystem.diagram.edit.parts;

import hub.top.adaptiveSystem.AdaptiveProcess;
import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.AdaptiveSystemPackage;
import hub.top.adaptiveSystem.ArcToCondition;
import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.Oclet;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.PolylineDecoration;
import org.eclipse.draw2d.RotatableDecoration;
import org.eclipse.emf.common.command.CommandStack;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.gmf.runtime.diagram.ui.editparts.ConnectionNodeEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.EditPolicyRoles;
import org.eclipse.gmf.runtime.draw2d.ui.figures.PolylineConnectionEx;
import org.eclipse.gmf.runtime.notation.Edge;
import org.eclipse.gmf.runtime.notation.Node;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.swt.graphics.Color;

/**
 * @generated
 */
public class ArcToConditionEditPart extends ConnectionNodeEditPart {

	/**
	 * @generated
	 */
	public static final int VISUAL_ID = 3001;

	/**
	 * @generated
	 */
	public ArcToConditionEditPart(View view) {
		super(view);
	}

	/**
	 * @generated
	 */
	protected void createDefaultEditPolicies() {
		super.createDefaultEditPolicies();
		installEditPolicy(
				EditPolicyRoles.SEMANTIC_ROLE,
				new hub.top.adaptiveSystem.diagram.edit.policies.ArcToConditionItemSemanticEditPolicy());
	}

	/**
	 * Creates figure for this edit part.
	 * 
	 * Body of this method does not depend on settings in generation model
	 * so you may safely remove <i>generated</i> tag and modify it.
	 * 
	 * @generated
	 */

	protected Connection createConnectionFigure() {
		return new ConnectionDescriptor();
	}

	/**
	 * @generated
	 */
	public ConnectionDescriptor getPrimaryShape() {
		return (ConnectionDescriptor) getFigure();
	}

	@Override
	protected void handleNotificationEvent(Notification notification) {

		if (notification.getNotifier() instanceof ArcToCondition) {
			// if an arcToCondition is redirected into different oclet part,
			// than oclets should be checked
			if (((ArcToCondition) notification.getNotifier()).eContainer()
					.eContainer() instanceof Oclet) {
				// resetCheckOclets() if there is a reorientation of an arc in oclet 
				resetCheckOclets();
			}
			// if disabled to referenced nodes is set, gray the arc
			getPrimaryShape().refreshColor();

		}

		if (notification.getNotifier() instanceof Condition
				|| notification.getNotifier() instanceof Event) {
			getPrimaryShape().refreshColor();
		}
		super.handleNotificationEvent(notification);
	}

	/** 
	 * @author Manja Wolf
	 * @param adaptiveProcess
	 */
	public void resetCheckOclets() {
		AdaptiveSystem adaptiveSystem = (AdaptiveSystem) ((ArcToCondition) ((Edge) this
				.getModel()).getElement()).eContainer().eContainer()
				.eContainer();
		Oclet oclet = (Oclet) ((ArcToCondition) ((Edge) this.getModel())
				.getElement()).eContainer().eContainer();

		if (adaptiveSystem.isSetWellformednessToOclets()) {
			SetCommand cmd = new SetCommand(this.getEditingDomain(),
					adaptiveSystem, AdaptiveSystemPackage.eINSTANCE
							.getAdaptiveSystem_SetWellformednessToOclets(),
					false);
			cmd.setLabel("set adaptive system attribute "
					+ AdaptiveSystemPackage.eINSTANCE
							.getAdaptiveSystem_SetWellformednessToOclets()
							.getName());
			cmd.canExecute();
			((CommandStack) ((EditingDomain) this.getEditingDomain())
					.getCommandStack()).execute(cmd);
		}
		if (oclet.isWellFormed()) {
			SetCommand cmd = new SetCommand(this.getEditingDomain(), oclet,
					AdaptiveSystemPackage.eINSTANCE.getOclet_WellFormed(),
					false);
			cmd.setLabel("set oclet attribute "
					+ AdaptiveSystemPackage.eINSTANCE.getOclet_WellFormed()
							.getName());
			cmd.canExecute();
			((CommandStack) ((EditingDomain) this.getEditingDomain())
					.getCommandStack()).execute(cmd);
		}
		((OcletEditPart) this.getSource().getParent().getParent().getParent())
				.getPrimaryShape().updateFace();
	}
	
	
	/**
	 * @generated
	 */
	public class ConnectionDescriptor extends PolylineConnectionEx {

		/**
		 * @generated NOT
		 */
		public ConnectionDescriptor() {
			this.setForegroundColor(ColorConstants.black);
			this.setBackgroundColor(ColorConstants.black);
			refreshColor();
			setTargetDecoration(createTargetDecoration());
		}

		/**
		 * @generated NOT
		 */
		protected void refreshColor() {

			Condition destination = (Condition) ((Node) ArcToConditionEditPart.this
					.getModelTarget()).getElement();
			Event source = (Event) ((Node) ArcToConditionEditPart.this
					.getModelSource()).getElement();
			if (source.eContainer() instanceof AdaptiveProcess
					|| destination.eContainer() instanceof AdaptiveProcess) {
				if (destination.isSetDisabledByAntiOclet()
						&& destination.isDisabledByAntiOclet()
						|| destination.isSetDisabledByConflict()
						&& destination.isDisabledByConflict()
						|| source.isSetDisabledByAntiOclet()
						&& source.isDisabledByAntiOclet()
						|| source.isSetDisabledByConflict()
						&& source.isDisabledByConflict()) {
					this.setForegroundColor(new Color(null, 190, 190, 190));
					this.setBackgroundColor(new Color(null, 190, 190, 190));
				} else {
					this.setForegroundColor(ColorConstants.black);
					this.setBackgroundColor(ColorConstants.black);
				}
			} else {
				this.setForegroundColor(ColorConstants.black);
				this.setBackgroundColor(ColorConstants.black);
			}

		}
		
		/**
		 * @generated
		 */
		private RotatableDecoration createTargetDecoration() {
			PolylineDecoration df = new PolylineDecoration();
			return df;
		}

	}

}
