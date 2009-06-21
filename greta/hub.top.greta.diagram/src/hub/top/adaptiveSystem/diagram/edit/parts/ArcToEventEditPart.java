package hub.top.adaptiveSystem.diagram.edit.parts;

import hub.top.adaptiveSystem.AdaptiveProcess;
import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.AdaptiveSystemPackage;
import hub.top.adaptiveSystem.ArcToEvent;
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
import org.eclipse.gef.EditPart;
import org.eclipse.gmf.runtime.diagram.ui.editparts.ConnectionNodeEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editparts.ITreeBranchEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.EditPolicyRoles;
import org.eclipse.gmf.runtime.draw2d.ui.figures.PolylineConnectionEx;
import org.eclipse.gmf.runtime.draw2d.ui.figures.WrappingLabel;
import org.eclipse.gmf.runtime.notation.Edge;
import org.eclipse.gmf.runtime.notation.Node;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.swt.graphics.Color;

/**
 * @generated
 */
public class ArcToEventEditPart extends ConnectionNodeEditPart implements
		ITreeBranchEditPart {

	/**
	 * @generated
	 */
	public static final int VISUAL_ID = 3002;

	/**
	 * @generated
	 */
	public ArcToEventEditPart(View view) {
		super(view);
	}

	/**
	 * @generated
	 */
	protected void createDefaultEditPolicies() {
		super.createDefaultEditPolicies();
		installEditPolicy(
				EditPolicyRoles.SEMANTIC_ROLE,
				new hub.top.adaptiveSystem.diagram.edit.policies.ArcToEventItemSemanticEditPolicy());
	}

	/**
	 * @generated
	 */
	protected boolean addFixedChild(EditPart childEditPart) {
		return false;
	}

	/**
	 * @generated
	 */
	protected void addChildVisual(EditPart childEditPart, int index) {
		if (addFixedChild(childEditPart)) {
			return;
		}
		super.addChildVisual(childEditPart, -1);
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

		if (notification.getNotifier() instanceof ArcToEvent) {

			ArcToEvent arc = (ArcToEvent)notification.getNotifier();

			// see if notifier is still part of the model, 
			if (arc.eContainer() != null && arc.eContainer().eContainer() != null)
			{
				// if an arcToEvent is redirected into different oclet part, than
				// oclets should be checked
				if (arc.eContainer().eContainer() instanceof Oclet) {
					// resetCheckOclets() if there is a reorientation of an arc in oclet 
					resetCheckOclets();
	
				}
				// if disabled to referenced nodes is set, gray the arc
				getPrimaryShape().refreshColor();
				
			} // arc is no longer part of the model (just deleted), so ignore it
		}
		if (notification.getNotifier() instanceof Condition
				|| notification.getNotifier() instanceof Event) {
			getPrimaryShape().refreshColor();
		}
		super.handleNotificationEvent(notification);
	}

	/** 
	 * @author Manja Wolf
	 */
	public void resetCheckOclets() {
		AdaptiveSystem adaptiveSystem = (AdaptiveSystem) ((ArcToEvent) ((Edge) this
				.getModel()).getElement()).eContainer().eContainer()
				.eContainer();
		Oclet oclet = (Oclet) ((ArcToEvent) ((Edge) this.getModel())
				.getElement()).eContainer().eContainer();

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
			Condition source = (Condition) ((Node) ArcToEventEditPart.this
					.getModelSource()).getElement();
			Event destination = (Event) ((Node) ArcToEventEditPart.this
					.getModelTarget()).getElement();
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
