package hub.top.editor.ptnetLoLA.transaction.adapt;

import hub.top.editor.ptnetLoLA.Arc;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.PtnetLoLAPackage;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.transaction.ResourceSetChangeEvent;

public class PtnetTAdapter implements TransactionAdapter {

	/**
	 * @see hub.top.editor.ptnetLoLA.transaction.adapt.TransactionAdapter#handleEventForNotifier(org.eclipse.emf.ecore.EObject)
	 */
	public boolean handleEventForNotifier(EObject notifier) {
		if (notifier instanceof PtNet)
			return true;
		return false;
	}

	/**
	 * @see hub.top.editor.ptnetLoLA.transaction.adapt.TransactionAdapter#transactionAboutToCommit(org.eclipse.emf.transaction.ResourceSetChangeEvent, org.eclipse.emf.common.notify.Notification)
	 */
	public Command transactionAboutToCommit(ResourceSetChangeEvent event,
			Notification notification)
	{
		
		if (PtnetLoLAPackage.PT_NET__ARCS == notification.getFeatureID(PtNet.class) &&
				Notification.REMOVE	== notification.getEventType())
    {
			return handle_Arc_remove(event, notification);
		}
		return null;
	}

	/**
	 * handle the removal of arcs from a net, during removal, the source and
	 * the target of the arc must be set to null to remove the arc from the
	 * incoming and outgoing arc lists of source and target node
	 * @param event
	 * @param notification
	 * @return
	 */
	private Command handle_Arc_remove (ResourceSetChangeEvent event,
			Notification notification)
	{
		// find the arc that is removed in the notification
    	Arc arc = (Arc) notification.getOldValue();
    	if (arc == null) arc = (Arc) notification.getNewValue();
    	if (arc == null) return null;
    	
    	// create a compound command of two set commands updating the
    	// arc's source and target
    	CompoundCommand cc = new CompoundCommand();
   		cc.append(new SetCommand(event.getEditingDomain(),
   					// arc.setSource(null);
    				arc, PtnetLoLAPackage.eINSTANCE.getArc_Source(), null));
   		cc.append(new SetCommand(event.getEditingDomain(),
   					// arc.setTarget(null);
    				arc, PtnetLoLAPackage.eINSTANCE.getArc_Target(), null));
    	return cc;
	}
}
